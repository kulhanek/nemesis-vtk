// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <RestraintListSelection.hpp>
#include <SelectionList.hpp>
#include <Restraint.hpp>
#include <RestraintList.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <StructureList.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRestraintListSelection SH_RestraintList;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CRestraintListSelection::GetHandlerDescription(void) const
{
    return("select a restraint");
}

//------------------------------------------------------------------------------

ESelResult CRestraintListSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
{
    CProObject* p_obj = obj.GetObject();

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

    // is object restraint?
    CRestraint* p_rst = GetRestraint(p_sel,p_obj);
    if( p_rst == NULL )return(ESR_NONE_SELECTED);

    // select/deselect restraint
    CSelObject sel(p_rst,0);
    if( p_sel->IsInList(sel) ){
        p_sel->RemoveObject(sel);
    } else {
        p_sel->AddObject(sel);
    }

    return(ESR_SELECTED_OBJECTS_CHANGED);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

ESelResult CRestraintListSelection::InvertSelection(CSelectionList* p_sel)
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
                CRestraint* p_rst = dynamic_cast<CRestraint*>(p_sel->GetSelectedObject(i));
                if( p_rst == NULL ) continue;
                structures.insert(p_rst->GetStructure());
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

void CRestraintListSelection::InvertSelection(CSelectionList* p_sel,CStructure* p_str)
{
    foreach(QObject* p_qobj,p_str->GetRestraints()->children()) {
        CRestraint* p_rst = static_cast<CRestraint*>(p_qobj);
        CSelObject obj(p_rst,0);
        if( p_rst->IsFlagSet(EPOF_SELECTED) ) {
            p_sel->RemoveObject(obj);
        } else {
            p_sel->AddObject(obj);
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

ESelResult CRestraintListSelection::CompleteAllStructures(CSelectionList* p_sel)
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
                CRestraint* p_rst = dynamic_cast<CRestraint*>(p_sel->GetSelectedObject(i));
                if( p_rst == NULL ) continue;
                structures.insert(p_rst->GetStructure());
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

ESelResult CRestraintListSelection::CompleteActiveStructure(CSelectionList* p_sel)
{
    // only active structure
    CStructure* p_str = p_sel->GetProject()->GetActiveStructure();
    if( p_str == NULL ) return(ESR_NONE_SELECTED);
    CompleteStructure(p_sel,p_str);
    return(ESR_SELECTED_OBJECTS_CHANGED);
}

//------------------------------------------------------------------------------

void CRestraintListSelection::CompleteStructure(CSelectionList* p_sel,CStructure* p_str)
{
    foreach(QObject* p_qobj,p_str->GetRestraints()->children()) {
        CRestraint* p_rst = static_cast<CRestraint*>(p_qobj);
        CSelObject obj(p_rst,0);
        if( ! p_rst->IsFlagSet(EPOF_SELECTED) ) {
            p_sel->AddObject(obj);
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
