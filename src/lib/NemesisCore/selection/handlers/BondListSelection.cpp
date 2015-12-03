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

#include <BondListSelection.hpp>
#include <SelectionList.hpp>
#include <Bond.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <BondList.hpp>
#include <StructureList.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBondsSelection SH_BondList;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CBondsSelection::GetHandlerDescription(void) const
{
    return("select bonds");
}

//------------------------------------------------------------------------------

ESelResult CBondsSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
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
            // invert selected objects
            return(InvertSelection(p_sel));
        }
        if( obj.GetSubID() == SEL_COMPLETE_ALL ) {
            return(CompleteAllStructures(p_sel));
        }
        if( obj.GetSubID() == SEL_COMPLETE_ACTIVE_STRUCTURE ) {
            return(CompleteActiveStructure(p_sel));
        }
        return(ESR_NONE_SELECTED);
    }

    // none object is selected
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // test structure object selection mode
    if( TestStrObjectSelMode(p_sel,p_obj) == false )  return(ESR_NONE_SELECTED);

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

ESelResult CBondsSelection::InvertSelection(CSelectionList* p_sel)
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
                CBond* p_bond = dynamic_cast<CBond*>(p_sel->GetSelectedObject(i));
                if( p_bond == NULL ) continue;
                structures.insert(p_bond->GetStructure());
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

void CBondsSelection::InvertSelection(CSelectionList* p_sel,CStructure* p_str)
{
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

ESelResult CBondsSelection::CompleteAllStructures(CSelectionList* p_sel)
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
                CBond* p_bond = dynamic_cast<CBond*>(p_sel->GetSelectedObject(i));
                if( p_bond == NULL ) continue;
                structures.insert(p_bond->GetStructure());
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

ESelResult CBondsSelection::CompleteActiveStructure(CSelectionList* p_sel)
{
    CStructure* p_str = p_sel->GetProject()->GetActiveStructure();
    if( p_str == NULL ) return(ESR_NONE_SELECTED);
    CompleteStructure(p_sel,p_str);
    return(ESR_SELECTED_OBJECTS_CHANGED);
}

//------------------------------------------------------------------------------

void CBondsSelection::CompleteStructure(CSelectionList* p_sel,CStructure* p_str)
{
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
