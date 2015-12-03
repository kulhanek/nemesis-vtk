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

#include <StructureListSelection.hpp>
#include <SelectionList.hpp>
#include <Structure.hpp>
#include <StructureList.hpp>
#include <Project.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CStructureListSelection SH_StructureList;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CStructureListSelection::GetHandlerDescription(void) const
{
    return("select structures");
}

//------------------------------------------------------------------------------

ESelResult CStructureListSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
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
            // select all structures
            return(CompleteAllStructures(p_sel));
        }
        return(ESR_NONE_SELECTED);
    }

    // none object is selected
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // test structure object selection mode
    if( TestStrObjectSelMode(p_sel,p_obj) == false )  return(ESR_NONE_SELECTED);

    CStructure* p_str = GetStructure(p_sel,p_obj);
    // object is not related to structure
    if( p_str == NULL ) return(ESR_NONE_SELECTED);

    CSelObject sel(p_str,0);
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

ESelResult CStructureListSelection::InvertSelection(CSelectionList* p_sel)
{
    CStructureList* p_strlist = p_sel->GetProject()->GetStructures();
    if( p_strlist == NULL ) return(ESR_NONE_SELECTED);

    foreach(QObject* p_qobj,p_strlist->children()) {
        CStructure* p_str = static_cast<CStructure*>(p_qobj);
        CSelObject obj(p_str,0);
        if( p_str->IsFlagSet(EPOF_SELECTED) ) {
            p_sel->RemoveObject(obj);
        } else {
            p_sel->AddObject(obj);
        }
    }

    return(ESR_SELECTED_OBJECTS_CHANGED);
}

//------------------------------------------------------------------------------

ESelResult CStructureListSelection::CompleteAllStructures(CSelectionList* p_sel)
{
    CStructureList* p_strlist = p_sel->GetProject()->GetStructures();
    if( p_strlist == NULL ) return(ESR_NONE_SELECTED);

    foreach(QObject* p_qobj,p_strlist->children()) {
        CStructure* p_str = static_cast<CStructure*>(p_qobj);
        CSelObject obj(p_str,0);
        if( ! p_str->IsFlagSet(EPOF_SELECTED) ) {
            p_sel->AddObject(obj);
        }
    }

    return(ESR_SELECTED_OBJECTS_CHANGED);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
