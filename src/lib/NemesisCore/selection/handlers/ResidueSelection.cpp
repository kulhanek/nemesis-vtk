// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <ResidueSelection.hpp>
#include <SelectionList.hpp>
#include <Residue.hpp>
#include <Atom.hpp>
#include <Bond.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <Residue.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CResidueSelection SH_Residue;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CResidueSelection::GetHandlerDescription(void) const
{
    return("select a residue");
}

//------------------------------------------------------------------------------

ESelResult CResidueSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
{
    CProObject* p_obj = obj.GetObject();

    // special keys
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // test structure object selection mode
    if( TestStrObjectSelMode(p_sel,p_obj) == false )  return(ESR_NONE_SELECTED);

    // if object is CAtom then select its residue
    CAtom* p_atom = dynamic_cast<CAtom*>(p_obj);
    if( p_atom != NULL ) {
        CResidue* p_res = p_atom->GetResidue();
        if( p_res == NULL ) return(ESR_NONE_SELECTED);
        CSelObject sel(p_res,0);
        if( p_sel->IsInList(sel) ){
            p_sel->RemoveObject(sel);
        } else {
            p_sel->AddObject(sel);
        }
        return(ESR_SELECTED_OBJECTS_END);
    }

    // if object is CBond then select its residues (for both atoms)
    CBond* p_bond = dynamic_cast<CBond*>(p_obj);
    if( p_bond != NULL ) {
        CResidue* p_res1 = NULL;
        CResidue* p_res2 = NULL;
        CAtom* p_at1 = p_bond->GetFirstAtom();
        if( p_at1 != NULL ){
            p_res1 = p_at1->GetResidue();
        }
        CAtom* p_at2 = p_bond->GetSecondAtom();
        if( p_at2 != NULL ){
            p_res2 = p_at2->GetResidue();
        }
        if( p_res1 != p_res2 ){
            p_sel->GetProject()->TextNotification(ETNT_WARNING,"selected bond is between two residues",ETNT_WARNING_DELAY);
            return(ESR_NONE_SELECTED);
        }
        if( p_res1 != NULL ){
            CSelObject sel(p_res1,0);
            if( p_sel->IsInList(sel) ){
                p_sel->RemoveObject(sel);
            } else {
                p_sel->AddObject(sel);
            }
            return(ESR_SELECTED_OBJECTS_END);
        }
        return(ESR_NONE_SELECTED);
    }

    // if object is CResidue then select it
    CResidue* p_res = dynamic_cast<CResidue*>(p_obj);
    if( p_res != NULL ) {
        CSelObject sel(p_res,0);
        if( p_sel->IsInList(sel) ){
            p_sel->RemoveObject(sel);
        } else {
            p_sel->AddObject(sel);
        }
        return(ESR_SELECTED_OBJECTS_END);
    }

    return(ESR_NONE_SELECTED);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
