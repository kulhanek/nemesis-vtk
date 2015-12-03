// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//                  This code is based on ASL mask implementation in
//                  Amber Support Library
//
//     This program is free software; you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation; either version 2 of the License, or
//     (at your option) any later version.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License along
//     with this program; if not, write to the Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// =============================================================================

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <ErrorSystem.hpp>
#include <ASLSelection.hpp>
#include <ASLMask.hpp>
#include <Structure.hpp>
#include <AtomList.hpp>
#include <ResidueList.hpp>
#include <Atom.hpp>
#include <Residue.hpp>
#include <PeriodicTable.hpp>

#include "maskparser/AmberMaskParser.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CASLSelection::CASLSelection(CASLMask* p_mask,QObject* p_owner)
    : QObject(p_owner)
{
    Mask = p_mask;
    foreach(QObject* p_qobj, Mask->Structure->GetAtoms()->children()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        Selection[p_atom] = false;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CASLSelection::ExpandAndReduceTree(struct SExpression* p_expr)
{
    if( p_expr == NULL ) {
        INVALID_ARGUMENT("p_expr is NULL");
    }

    // is it selection?
    if( p_expr->Selection != NULL ) {
        if( Select(p_expr->Selection) == false ) {
            ES_ERROR("unable to select");
            return(false);
        }
        return(true);
    }

    CASLSelection* p_left  = NULL;
    CASLSelection* p_right = NULL;

    // it is operator - allocate left operand
    switch(p_expr->Operator) {
        case O_NOT:
            // no left operand
            break;
        case O_AND:
        case O_OR:
            // alocate expression
            p_left = new CASLSelection(Mask,this);
            // expand and reduce
            if( p_left->ExpandAndReduceTree(p_expr->LeftExpression) == false ) {
                ES_ERROR("cannot expand and reduce left operand");
                return(false);
            }
            break;
        case O_RLT:
        case O_RGT:
        case O_ALT:
        case O_AGT:
            switch(p_expr->Modificator) {
            case D_ORIGIN:
            case D_CBOX:
                // nothing to do
                break;
            case D_LIST:
            case D_COM:
            case D_PLANE:
                // alocate expression
                p_left = new CASLSelection(Mask,this);
                // expand and reduce
                if( p_left->ExpandAndReduceTree(p_expr->LeftExpression) == false ) {
                    ES_ERROR("cannot expand and reduce left operand");
                    return(false);
                }
                break;
            default:
                ES_ERROR("not implemented distance modificator");
                return(false);
            }
            break;
        default:
            ES_ERROR("not implemented operator");
            return(false);
    }

    // it is operator - allocate right operand
    switch(p_expr->Operator) {
        case O_NOT:
        case O_AND:
        case O_OR:
            p_right = new CASLSelection(Mask,this);
            break;
        case O_RLT:
        case O_RGT:
        case O_ALT:
        case O_AGT:
            // nothing to do
            break;
        default:
            ES_ERROR("not implemented");
            return(false);
    }

    // expand and reduce right operand
    if( p_right != NULL ) {
        if( p_right->ExpandAndReduceTree(p_expr->RightExpression) == false ) {
            ES_ERROR("cannot expand and reduce right operand");
            return(false);
        }
    }

    // do arithemetic
    switch(p_expr->Operator) {
        case O_NOT:
            foreach(QObject* p_qobj, Mask->GetAtoms()){
                CAtom* p_atom = static_cast<CAtom*>(p_qobj);
                Selection[p_atom] = ! p_right->Selection[p_atom];
            }
            break;
        case O_AND:
            foreach(QObject* p_qobj, Mask->GetAtoms()){
                CAtom* p_atom = static_cast<CAtom*>(p_qobj);
                Selection[p_atom] = p_left->Selection[p_atom] && p_right->Selection[p_atom];
            }
            break;
        case O_OR:
            foreach(QObject* p_qobj, Mask->GetAtoms()){
                CAtom* p_atom = static_cast<CAtom*>(p_qobj);
                Selection[p_atom] = p_left->Selection[p_atom] || p_right->Selection[p_atom];
            }
            break;
        case O_RLT:
        case O_RGT: {
            bool result = false;
            switch(p_expr->Modificator) {
            case D_ORIGIN:
                result = SelectResidueByDistanceFromOrigin(p_expr->Operator,p_expr->Distance);
                break;
            case D_CBOX:
                result = SelectResidueByDistanceFromCentreOfBox(p_expr->Operator,p_expr->Distance);
                break;
            case D_LIST:
                result = SelectResidueByDistanceFromList(p_left,p_expr->Operator,p_expr->Distance);
                break;
            case D_COM:
                result = SelectResidueByDistanceFromCOM(p_left,p_expr->Operator,p_expr->Distance);
                break;
            case D_PLANE:
                result = SelectResidueByDistanceFromPlane(p_left,p_expr->Operator,p_expr->Distance);
                break;
            default:
                ES_ERROR("not implemented");
                return(false);
            }
            if( result == false ) return(false);
        }
        break;
        case O_AGT:
        case O_ALT: {
            bool result = false;
            switch(p_expr->Modificator) {
            case D_ORIGIN:
                result = SelectAtomByDistanceFromOrigin(p_expr->Operator,p_expr->Distance);
                break;
            case D_CBOX:
                result = SelectAtomByDistanceFromCentreOfBox(p_expr->Operator,p_expr->Distance);
                break;
            case D_LIST:
                result = SelectAtomByDistanceFromList(p_left,p_expr->Operator,p_expr->Distance);
                break;
            case D_COM:
                result = SelectAtomByDistanceFromCOM(p_left,p_expr->Operator,p_expr->Distance);
                break;
            case D_PLANE:
                result = SelectAtomByDistanceFromPlane(p_left,p_expr->Operator,p_expr->Distance);
                break;
            default:
                ES_ERROR("not implemented");
                return(false);
            }
            if( result == false ) return(false);
        }
        break;
        default:
            ES_ERROR("not implemented");
            return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CASLSelection::Select(struct SSelection* p_sel)
{
    if( p_sel == NULL ) {
        INVALID_ARGUMENT("p_sel is NULL");
    }

    // get selection list
    struct SList* p_list = p_sel->Items;
    if( p_list == NULL ) {
        ES_ERROR("p_list is NULL");
        return(false);
    }

    struct SListItem* p_item = p_list->FirstItem;

    while( p_item != NULL ) {
        if( p_item->Index < 0 ) {
            // no matter of selector - this always means all atoms
            foreach(QObject* p_qobj, Mask->GetAtoms()){
                CAtom* p_atom = static_cast<CAtom*>(p_qobj);
                Selection[p_atom] = true;
            }
        }
        if( p_item->Index > 0 ) {
            if( p_item->Length >= 1 ) {
                switch(p_sel->Type) {
                    case T_RSELECTOR:
                        SelectResidueByIndex(p_item->Index,p_item->Length);
                        break;
                    case T_ASELECTOR:
                        SelectAtomByIndex(p_item->Index,p_item->Length);
                        break;
                    case T_TSELECTOR:
                        ES_ERROR("index or range for type selector is meaningless");
                        return(false);
                    default:
                        ES_ERROR("unknown selector");
                        return(false);
                }
            } else {
                ES_ERROR("illegal range");
                return(false);
            }
        }

        if( p_item->Index == 0 ) {
            switch(p_sel->Type) {
                case T_RSELECTOR:
                    SelectResidueByName(p_item->Name);
                    break;
                case T_ASELECTOR:
                    SelectAtomByName(p_item->Name);
                    break;
                case T_TSELECTOR:
                    SelectAtomByType(p_item->Name);
                    break;
                default:
                    ES_ERROR("unknown selector");
                    return(false);
            }
        }

        p_item = p_item->NextItem;
    }

    return(true);
}

//------------------------------------------------------------------------------

void CASLSelection::SelectAtomByIndex(int index,int length)
{
    int start = index;
    int end   = index + length - 1;

    foreach(QObject* p_qobj, Mask->GetAtoms()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        if( (p_atom->GetSerIndex() < start) || (p_atom->GetSerIndex() > end) ) continue;
        Selection[p_atom] = true;
    }
}

//------------------------------------------------------------------------------

void CASLSelection::SelectAtomByName(const char* p_name)
{
    // wild card '*' is treated on parser level!
    // see bool Select(struct SSelection* p_sel) method

    // here we need to manage only '=' wild card
    // this wild card can be only at the end of p_name

    int search_len = 4;
    // try to find wild card
    for(int i=3; i >= 0; i--) {
        if( p_name[i] == '=' ) {
            search_len = i;
            break;
        }
    }

    foreach(QObject* p_qobj, Mask->GetAtoms()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        if( strncmp(p_atom->GetName().toLatin1(),p_name,search_len) != 0 ) continue;
        Selection[p_atom] = true;
    }
}

//------------------------------------------------------------------------------

void CASLSelection::SelectAtomByType(const char* p_name)
{
    // wild card '*' is treated on parser level!
    // see bool Select(struct SSelection* p_sel) method

    // here we need to manage only '=' wild card
    // this wild card can be only at the end of p_name

    int search_len = 4;
    // try to find wild card
    for(int i=3; i >= 0; i--) {
        if( p_name[i] == '=' ) {
            search_len = i;
            break;
        }
    }

    foreach(QObject* p_qobj, Mask->GetAtoms()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        if( strncmp(p_atom->GetType().toLatin1(),p_name,search_len) != 0 ) continue;
        Selection[p_atom] = true;
    }
}

//------------------------------------------------------------------------------

void CASLSelection::SelectResidueByIndex(int index,int length)
{
    int start = index;
    int end   = index + length - 1;

    foreach(QObject* p_qobj, Mask->GetResidues()){
        CResidue* p_res = static_cast<CResidue*>(p_qobj);
        if( (p_res->GetSeqIndex() < start) || (p_res->GetSeqIndex() > end) ) continue;

        foreach(CAtom* p_atom, p_res->GetAtoms()){
            Selection[p_atom] = true;
        }
    }
}

//------------------------------------------------------------------------------

void CASLSelection::SelectResidueByName(const char* p_name)
{
    // wild card '*' is treated on parser level!
    // see bool Select(struct SSelection* p_sel) method

    // here we need to manage only '=' wild card
    // this wild card can be only at the end

    int search_len = 4;
    // try to find wild card
    for(int i=3; i >= 0; i--) {
        if( p_name[i] == '=' ) {
            search_len = i;
            break;
        }
    }

    foreach(QObject* p_qobj, Mask->GetResidues()){
        CResidue* p_res = static_cast<CResidue*>(p_qobj);
        if( strncmp(p_res->GetName().toLatin1(),p_name,search_len) != 0 ) continue;

        foreach(CAtom* p_atom, p_res->GetAtoms()){
            Selection[p_atom] = true;
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CASLSelection::SelectAtomByDistanceFromOrigin(SOperator dist_oper,double dist)
{
    double dist2 = dist*dist;

    foreach(QObject* p_qobj, Mask->GetAtoms()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);

        CPoint pos = p_atom->GetPos();
        double ldist2 = Square(pos);

        switch(dist_oper) {
            case O_ALT:
                if( ldist2 < dist2 ) Selection[p_atom] = true;
                break;
            case O_AGT:
                if( ldist2 > dist2 ) Selection[p_atom] = true;
                break;
            case O_RLT:
            case O_RGT:
            default:
                ES_ERROR("incorrect operator");
                return(false);
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CASLSelection::SelectAtomByDistanceFromCentreOfBox(SOperator dist_oper,double dist)
{
    if( ! Mask->Structure->PBCInfo.IsValid() ) {
        ES_ERROR("cbox requires box");
        return(false);
    }

    double dist2 = dist*dist;
    CPoint cbox = Mask->Structure->PBCInfo.GetBoxCenter();

    foreach(QObject* p_qobj, Mask->GetAtoms()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);

        CPoint pos = p_atom->GetPos();
        double ldist2 = Square(pos-cbox);

        switch(dist_oper) {
            case O_ALT:
                if( ldist2 < dist2 ) Selection[p_atom] = true;
                break;
            case O_AGT:
                if( ldist2 > dist2 ) Selection[p_atom] = true;
                break;
            case O_RLT:
            case O_RGT:
            default:
                ES_ERROR("incorrect operator");
                return(false);
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CASLSelection::SelectAtomByDistanceFromList(CASLSelection* p_left,
                                                 SOperator dist_oper,double dist)
{
    double dist2 = dist*dist;

    foreach(QObject* p_qobj1, Mask->GetAtoms()){
        CAtom* p_atom1 = static_cast<CAtom*>(p_qobj1);

        CPoint pos1 = p_atom1->GetPos();

        foreach(QObject* p_qobj2, Mask->GetAtoms()){
            CAtom* p_atom2 = static_cast<CAtom*>(p_qobj2);
            if( p_left->Selection[p_atom2] == false ) continue;

            CPoint pos2 = p_atom2->GetPos();
            double ldist2 = Square(pos2-pos1);

            bool set = false;
            switch(dist_oper) {
                case O_ALT:
                    if( ldist2 < dist2 ) {
                        Selection[p_atom1] = true;
                        set = true;
                    }
                    break;
                case O_AGT:
                    if( ldist2 > dist2 ) {
                        Selection[p_atom1] = true;
                        set = true;
                    }
                    break;
                case O_RLT:
                case O_RGT:
                default:
                    ES_ERROR("incorrect operator");
                    return(false);
            }
            if( set == true ) break;
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CASLSelection::SelectAtomByDistanceFromCOM(CASLSelection* p_left,
                                                SOperator dist_oper,double dist)
{
    // calculate centre of mass
    CPoint     com;
    double     tmass = 0.0;

    foreach(QObject* p_qobj, Mask->GetAtoms()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        if( p_left->Selection[p_atom] == false ) continue;
        double mass = PeriodicTable.GetMass(p_atom->GetZ());
        com += p_atom->GetPos()*mass;
        tmass += mass;
    }

    if( tmass <= 0.0 ) return(true);
    com /= tmass;

    double dist2 = dist*dist;

    // select atoms
    foreach(QObject* p_qobj, Mask->GetAtoms()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);

        CPoint pos = p_atom->GetPos();
        double ldist2 = Square(pos-com);

        switch(dist_oper) {
            case O_ALT:
                if( ldist2 < dist2 ) Selection[p_atom] = true;
                break;
            case O_AGT:
                if( ldist2 > dist2 ) Selection[p_atom] = true;
                break;
            case O_RLT:
            case O_RGT:
            default:
                ES_ERROR("incorrect operator");
                return(false);
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CASLSelection::SelectAtomByDistanceFromPlane(CASLSelection* p_left,
                                                  SOperator dist_oper,double dist)
{
    ES_ERROR("not implemented");
    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CASLSelection::SelectResidueByDistanceFromOrigin(SOperator dist_oper,double dist)
{
    double dist2 = dist*dist;

    foreach(QObject* p_qobj, Mask->GetResidues() ){
        CResidue* p_res = static_cast<CResidue*>(p_qobj);

        bool set = false;

        // check any distance
        foreach(CAtom* p_atom,p_res->GetAtoms()) {

            CPoint pos = p_atom->GetPos();
            double ldist2 = Square(pos);

            switch(dist_oper) {
                case O_RLT:
                    if( ldist2 < dist2 ) set = true;
                    break;
                case O_RGT:
                    if( ldist2 > dist2 ) set = true;
                    break;
                case O_ALT:
                case O_AGT:
                default:
                    ES_ERROR("incorrect operator");
                    return(false);
            }
            if( set == true ) break;
        }

        // select residue
        if( set ) {
            foreach(CAtom* p_atom,p_res->GetAtoms()) {
                Selection[p_atom] = true;
            }
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CASLSelection::SelectResidueByDistanceFromCentreOfBox(SOperator dist_oper,double dist)
{
    if( ! Mask->Structure->PBCInfo.IsValid() ) {
        ES_ERROR("cbox requires box");
        return(false);
    }

    double dist2 = dist*dist;
    CPoint cbox = Mask->Structure->PBCInfo.GetBoxCenter();

    foreach(QObject* p_qobj, Mask->GetResidues() ){
        CResidue* p_res = static_cast<CResidue*>(p_qobj);

        bool set = false;

        // check any distance
        foreach(CAtom* p_atom,p_res->GetAtoms()) {

            CPoint pos = p_atom->GetPos();
            double ldist2 = Square(pos-cbox);

            switch(dist_oper) {
                case O_RLT:
                    if( ldist2 < dist2 ) set = true;
                    break;
                case O_RGT:
                    if( ldist2 > dist2 ) set = true;
                    break;
                case O_ALT:
                case O_AGT:
                default:
                    ES_ERROR("incorrect operator");
                    return(false);
            }
            if( set == true ) break;
        }

        // select residue
        if( set ) {
            foreach(CAtom* p_atom,p_res->GetAtoms()) {
                Selection[p_atom] = true;
            }
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CASLSelection::SelectResidueByDistanceFromList(CASLSelection* p_left,
                                                    SOperator dist_oper,double dist)
{
    double dist2 = dist*dist;

    foreach(QObject* p_qobj, Mask->GetResidues() ){
        CResidue* p_res = static_cast<CResidue*>(p_qobj);

        bool set = false;

        // check any distance
        foreach(CAtom* p_atom1,p_res->GetAtoms()) {
            CPoint pos1 = p_atom1->GetPos();

            foreach(QObject* p_qobj2, Mask->GetAtoms()){
                CAtom* p_atom2 = static_cast<CAtom*>(p_qobj2);
                if( p_left->Selection[p_atom2] == false ) continue;

                CPoint pos2 = p_atom2->GetPos();
                double ldist2 = Square(pos2-pos1);

                switch(dist_oper) {
                    case O_RLT:
                        if( ldist2 < dist2 ) {
                            set = true;
                        }
                        break;
                    case O_RGT:
                        if( ldist2 > dist2 ) {
                            set = true;
                        }
                        break;
                    case O_ALT:
                    case O_AGT:
                    default:
                        ES_ERROR("incorrect operator");
                        return(false);
                }
                if( set == true ) break;
            }

            if( set == true ) break;
        }

        // select residue
        if( set ) {
            foreach(CAtom* p_atom,p_res->GetAtoms()) {
                Selection[p_atom] = true;
            }
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CASLSelection::SelectResidueByDistanceFromCOM(CASLSelection* p_left,
                                                   SOperator dist_oper,double dist)
{
    // calculate centre of mass
    CPoint     com;
    double     tmass = 0.0;

    foreach(QObject* p_qobj, Mask->GetAtoms()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        if( p_left->Selection[p_atom] == false ) continue;
        double mass = PeriodicTable.GetMass(p_atom->GetZ());
        com += p_atom->GetPos()*mass;
        tmass += mass;
    }

    if( tmass <= 0.0 ) return(true);
    com /= tmass;

    double dist2 = dist*dist;

    foreach(QObject* p_qobj, Mask->GetResidues() ){
        CResidue* p_res = static_cast<CResidue*>(p_qobj);

        bool set = false;

        // check any distance
        foreach(CAtom* p_atom,p_res->GetAtoms()) {

            CPoint pos = p_atom->GetPos();
            double ldist2 = Square(pos-com);

            switch(dist_oper) {
                case O_RLT:
                    if( ldist2 < dist2 ) set = true;
                    break;
                case O_RGT:
                    if( ldist2 > dist2 ) set = true;
                    break;
                case O_ALT:
                case O_AGT:
                default:
                    ES_ERROR("incorrect operator");
                    return(false);
            }
            if( set == true ) break;
        }

        // select residue
        if( set ) {
            foreach(CAtom* p_atom,p_res->GetAtoms()) {
                Selection[p_atom] = true;
            }
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CASLSelection::SelectResidueByDistanceFromPlane(CASLSelection* p_left,
        SOperator dist_oper,double dist)
{
    ES_ERROR("not implemented");
    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


