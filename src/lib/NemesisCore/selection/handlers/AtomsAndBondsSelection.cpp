// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <AtomsAndBondsSelection.hpp>
#include <SelectionList.hpp>
#include <Atom.hpp>
#include <Bond.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <AtomList.hpp>
#include <BondList.hpp>
#include <StructureList.hpp>
#include <queue>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomsAndBondsSelection SH_AtomsAndBonds;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CAtomsAndBondsSelection::GetHandlerDescription(void) const
{
    return("select atoms and bonds");
}

//------------------------------------------------------------------------------

ESelResult CAtomsAndBondsSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
{
    CProObject* p_obj = obj.GetObject();

    // special keys
    if( p_obj == NULL ){
        if( obj.GetSubID() == SEL_ESC_KEY ){
            // reset list
            p_sel->ResetSelection();
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
        if( obj.GetSubID() == SEL_ENTER_KEY ){
            // complete selection
            return(ESR_SELECTED_OBJECTS_END);
        }
        if( obj.GetSubID() == SEL_INVERT ) {
            return(InvertSelection(p_sel));
        }
        if( obj.GetSubID() == SEL_COMPLETE_ALL ) {
            return(CompleteAllStructures(p_sel));
        }
        if( obj.GetSubID() == SEL_COMPLETE_ACTIVE_STRUCTURE ) {
            return(CompleteActiveStructure(p_sel));
        }
        if( obj.GetSubID() == SEL_COMPLETE_MOLECULES ) {
            return(CompleteMolecules(p_sel));
        }
        if( obj.GetSubID() == SEL_COMPLETE_BONDS ) {
            return(CompleteBonds(p_sel));
        }
        if( obj.GetSubID() == SEL_COMPLETE_VALENCES ) {
            return(CompleteValences(p_sel));
        }
        return(ESR_NONE_SELECTED);
    }

    // none object is selected
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // test structure object selection mode
    if( TestStrObjectSelMode(p_sel,p_obj) == false )  return(ESR_NONE_SELECTED);

    // if object is CAtom then select it
    CAtom* p_atom = dynamic_cast<CAtom*>(p_obj);
    if( p_atom != NULL ) {
        if( p_sel->IsInList(obj) ) {
            p_sel->RemoveObject(obj);
        } else {
            p_sel->AddObject(obj);
        }
        return(ESR_SELECTED_OBJECTS_CHANGED);
    }

    // if object is CBond then select it
    CBond* p_bond = dynamic_cast<CBond*>(p_obj);
    if( p_bond != NULL ) {
        if( p_sel->IsInList(obj) ) {
            p_sel->RemoveObject(obj);
        } else {
            p_sel->AddObject(obj);
        }
        return(ESR_SELECTED_OBJECTS_CHANGED);
    }

    return(ESR_NONE_SELECTED);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

ESelResult CAtomsAndBondsSelection::InvertSelection(CSelectionList* p_sel)
{
    switch( p_sel->GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:{
            // only active structure
            CStructure* p_str = p_sel->GetProject()->GetActiveStructure();
            if( p_str == NULL ) return(ESR_NONE_SELECTED);
            InvertSelection(p_sel,p_str);
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
        case ESOSM_STRUCTURES_FROM_OBJECTS:{
            // extract structures from selected atoms
            QSet<CStructure*> structures;
            for(int i=0; i < p_sel->NumOfSelectedObjects(); i++){
                CAtom* p_atom = dynamic_cast<CAtom*>(p_sel->GetSelectedObject(i));
                if( p_atom == NULL ) continue;
                structures.insert(p_atom->GetStructure());
            }

            foreach(CStructure* p_str, structures){
                if( p_str == NULL ) continue;
                InvertSelection(p_sel,p_str);
            }
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
        case ESOSM_CONSIDER_ALL_STRUCTURES:{
            // all structures
            foreach(QObject* p_qobj,p_sel->GetProject()->GetStructures()->children()) {
                CStructure* p_str = static_cast<CStructure*>(p_qobj);
                InvertSelection(p_sel,p_str);
            }
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
    }

    return(ESR_NONE_SELECTED);
}

//------------------------------------------------------------------------------

void CAtomsAndBondsSelection::InvertSelection(CSelectionList* p_sel,CStructure* p_str)
{
    foreach(QObject* p_qobj,p_str->GetAtoms()->children()) {
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        CSelObject obj(p_atom,0);
        if( p_atom->IsFlagSet(EPOF_SELECTED) ) {
            p_sel->RemoveObject(obj);
        } else {
            p_sel->AddObject(obj);
        }
    }

    foreach(QObject* p_qobj,p_str->GetBonds()->children()) {
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        CSelObject obj(p_bond,0);
        if( p_bond->IsFlagSet(EPOF_SELECTED) ) {
            p_sel->RemoveObject(obj);
        } else {
            p_sel->AddObject(obj);
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

ESelResult CAtomsAndBondsSelection::CompleteAllStructures(CSelectionList* p_sel)
{
    switch( p_sel->GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:{
            // only active structure
            CStructure* p_str = p_sel->GetProject()->GetActiveStructure();
            if( p_str == NULL ) return(ESR_NONE_SELECTED);
            CompleteStructure(p_sel,p_str);
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
        case ESOSM_STRUCTURES_FROM_OBJECTS:{
            // extract structures from selected atoms
            QSet<CStructure*> structures;
            for(int i=0; i < p_sel->NumOfSelectedObjects(); i++){
                CAtom* p_atom = dynamic_cast<CAtom*>(p_sel->GetSelectedObject(i));
                if( p_atom == NULL ) continue;
                structures.insert(p_atom->GetStructure());
            }

            foreach(CStructure* p_str, structures){
                if( p_str == NULL ) continue;
                CompleteStructure(p_sel,p_str);
            }
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
        case ESOSM_CONSIDER_ALL_STRUCTURES: {
            // all structures
            foreach(QObject* p_qobj,p_sel->GetProject()->GetStructures()->children()) {
                CStructure* p_str = static_cast<CStructure*>(p_qobj);
                CompleteStructure(p_sel,p_str);
            }
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
    }

    return(ESR_NONE_SELECTED);
}

//------------------------------------------------------------------------------

ESelResult CAtomsAndBondsSelection::CompleteActiveStructure(CSelectionList* p_sel)
{
    CStructure* p_str = p_sel->GetProject()->GetActiveStructure();
    if( p_str == NULL ) return(ESR_NONE_SELECTED);
    CompleteStructure(p_sel,p_str);
    return(ESR_SELECTED_OBJECTS_CHANGED);
}

//------------------------------------------------------------------------------

void CAtomsAndBondsSelection::CompleteStructure(CSelectionList* p_sel,CStructure* p_str)
{
    foreach(QObject* p_qobj,p_str->GetAtoms()->children()) {
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        CSelObject obj(p_atom,0);
        if( ! p_atom->IsFlagSet(EPOF_SELECTED) ) {
            p_sel->AddObject(obj);
        }
    }

    foreach(QObject* p_qobj,p_str->GetBonds()->children()) {
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        CSelObject obj(p_bond,0);
        if( ! p_bond->IsFlagSet(EPOF_SELECTED) ) {
            p_sel->AddObject(obj);
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

ESelResult CAtomsAndBondsSelection::CompleteMolecules(CSelectionList* p_sel)
{
    std::queue<CAtom*>    stack;

    // For each bond selected put end atoms on stack
    for(int i=0; i < p_sel->NumOfSelectedObjects(); i++){
        CBond* p_bond = dynamic_cast<CBond*>(p_sel->GetSelectedObject(i));
        if( p_bond == NULL ) continue;
        CAtom* p_atom = p_bond->GetFirstAtom();
        if( p_atom != NULL) stack.push(p_atom);
        p_atom = p_bond->GetSecondAtom();
        if( p_atom != NULL) stack.push(p_atom);
    }

    switch( p_sel->GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:{
            CStructure* p_str = p_sel->GetProject()->GetActiveStructure();
            if( p_str == NULL ) return(ESR_NONE_SELECTED);

            // prepare atoms and put selected on stack
            foreach(QObject* p_qobj,p_str->GetAtoms()->children()) {
                CAtom* p_atom = static_cast<CAtom*>(p_qobj);
                if( p_atom->IsFlagSet(EPOF_SELECTED) ) {
                    stack.push(p_atom);
                }
            }
            break;
        }
        case ESOSM_STRUCTURES_FROM_OBJECTS:
        case ESOSM_CONSIDER_ALL_STRUCTURES:  {
            // add atoms from the selection stack
            for(int i=0; i < p_sel->NumOfSelectedObjects(); i++){
                CAtom* p_atom = dynamic_cast<CAtom*>(p_sel->GetSelectedObject(i));
                if( p_atom == NULL ) continue;
                stack.push(p_atom);
            }
            break;
        }
    }

    // go through the stack over interconnected atoms
    while( stack.size() > 0 ) {
        CAtom*                  p_atom = stack.front();
        stack.pop();

        foreach(CBond* p_bond, p_atom->GetBonds()) {
            CAtom* p_atom2 = p_bond->GetOppositeAtom(p_atom);
            if( p_atom2->IsFlagSet(EPOF_SELECTED) == false ) {
                stack.push(p_atom2);
                CSelObject selobj(p_atom2,0);
                p_sel->AddObject(selobj);
            }
        }
    }

    // and complete bonds
    CompleteBonds(p_sel);

    return(ESR_SELECTED_OBJECTS_CHANGED);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

ESelResult CAtomsAndBondsSelection::CompleteBonds(CSelectionList* p_sel)
{
    switch( p_sel->GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:{
            // only active structure
            CStructure* p_str = p_sel->GetProject()->GetActiveStructure();
            if( p_str == NULL ) return(ESR_NONE_SELECTED);
            CompleteBonds(p_sel,p_str);
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
        case ESOSM_STRUCTURES_FROM_OBJECTS:
        case ESOSM_CONSIDER_ALL_STRUCTURES: {
            // all structures
            foreach(QObject* p_qobj,p_sel->GetProject()->GetStructures()->children()) {
                CStructure* p_str = static_cast<CStructure*>(p_qobj);
                CompleteBonds(p_sel,p_str);
            }
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
    }

    return(ESR_NONE_SELECTED);
}

//------------------------------------------------------------------------------

void CAtomsAndBondsSelection::CompleteBonds(CSelectionList* p_sel,CStructure* p_str)
{
    foreach(QObject* p_qobj,p_str->GetBonds()->children()) {
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        if( p_bond->IsFlagSet(EPOF_SELECTED) ) continue; // bond is already selected

        CAtom* p_atom1 = p_bond->GetFirstAtom();
        CAtom* p_atom2 = p_bond->GetSecondAtom();

        if( (p_atom1->IsFlagSet(EPOF_SELECTED) == true) &&
                (p_atom2->IsFlagSet(EPOF_SELECTED) == true) ) {
            CSelObject obj(p_bond,0);
            p_sel->AddObject(obj);
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

ESelResult CAtomsAndBondsSelection::CompleteValences(CSelectionList* p_sel)
{
    switch( p_sel->GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:{
            // only active structure
            CStructure* p_str = p_sel->GetProject()->GetActiveStructure();
            if( p_str == NULL ) return(ESR_NONE_SELECTED);
            CompleteValences(p_sel,p_str);
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
        case ESOSM_STRUCTURES_FROM_OBJECTS:
        case ESOSM_CONSIDER_ALL_STRUCTURES: {
            // all structures
            foreach(QObject* p_qobj,p_sel->GetProject()->GetStructures()->children()) {
                CStructure* p_str = static_cast<CStructure*>(p_qobj);
                CompleteValences(p_sel,p_str);
            }
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
    }

    return(ESR_NONE_SELECTED);
}

//------------------------------------------------------------------------------

void CAtomsAndBondsSelection::CompleteValences(CSelectionList* p_sel,CStructure* p_str)
{
    foreach(QObject* p_qobj,p_str->GetAtoms()->children()) {
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        CBond* p_bond = p_atom->GetConnectBond();
        if( p_atom->IsVirtual() && (p_bond != NULL) ) {
            CAtom* p_satom = p_bond->GetOppositeAtom(p_atom);
            if( p_satom->IsFlagSet(EPOF_SELECTED) == false ) continue;

            if( p_atom->IsFlagSet(EPOF_SELECTED) == false ) {
                CSelObject obj(p_atom,0);
                p_sel->AddObject(obj);
            }
            if( p_bond->IsFlagSet(EPOF_SELECTED) == false ) {
                CSelObject obj(p_bond,0);
                p_sel->AddObject(obj);
            }
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
