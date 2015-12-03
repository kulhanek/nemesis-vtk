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

#include <PropertyListSelection.hpp>
#include <SelectionList.hpp>
#include <Property.hpp>
#include <PropertyList.hpp>
#include <Project.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPropertyListSelection SH_PropertyList;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CPropertyListSelection::GetHandlerDescription(void) const
{
    return("select properties");
}

//------------------------------------------------------------------------------

ESelResult CPropertyListSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
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
            return(CompleteAllProperties(p_sel));
        }
        return(ESR_NONE_SELECTED);
    }

    // none object is selected
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // if object is CProperty then select it
    CProperty* p_prop = dynamic_cast<CProperty*>(p_obj);
    if( p_prop != NULL ) {
        CSelObject sel(p_prop,0);
        if( p_sel->IsInList(sel) ){
            p_sel->RemoveObject(sel);
        } else {
            p_sel->AddObject(sel);
        }
        return(ESR_SELECTED_OBJECTS_CHANGED);
    }

    return(ESR_NONE_SELECTED);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

ESelResult CPropertyListSelection::InvertSelection(CSelectionList* p_sel)
{
    CPropertyList* p_proplist = p_sel->GetProject()->GetProperties();
    if( p_proplist == NULL ) return(ESR_NONE_SELECTED);

    foreach(QObject* p_qobj,p_proplist->children()) {
        CProperty* p_prop = static_cast<CProperty*>(p_qobj);
        CSelObject obj(p_prop,0);
        if( p_prop->IsFlagSet(EPOF_SELECTED) ) {
            p_sel->RemoveObject(obj);
        } else {
            p_sel->AddObject(obj);
        }
    }

    return(ESR_SELECTED_OBJECTS_CHANGED);
}

//------------------------------------------------------------------------------

ESelResult CPropertyListSelection::CompleteAllProperties(CSelectionList* p_sel)
{
    CPropertyList* p_proplist = p_sel->GetProject()->GetProperties();
    if( p_proplist == NULL ) return(ESR_NONE_SELECTED);

    foreach(QObject* p_qobj,p_proplist->children()) {
        CProperty* p_prop = static_cast<CProperty*>(p_qobj);
        CSelObject obj(p_prop,0);
        if( ! p_prop->IsFlagSet(EPOF_SELECTED) ) {
            p_sel->AddObject(obj);
        }
    }

    return(ESR_SELECTED_OBJECTS_CHANGED);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
