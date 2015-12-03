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

#include <GraphicsObjectListSelection.hpp>
#include <SelectionList.hpp>
#include <GraphicsObject.hpp>
#include <Project.hpp>
#include <Graphics.hpp>
#include <GraphicsObjectList.hpp>
#include <GraphicsObjectListSelection.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsObjectListSelection SH_GraphicsObjectList;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CGraphicsObjectListSelection::GetHandlerDescription(void) const
{
    return("select graphics objects");
}

//------------------------------------------------------------------------------

ESelResult CGraphicsObjectListSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
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
            return(CompleteAllObjects(p_sel));
        }
        return(ESR_NONE_SELECTED);
    }

    // if object is CAtom then select it
    CGraphicsObject* p_go = dynamic_cast<CGraphicsObject*>(p_obj);
    if( p_go != NULL ) {
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

ESelResult CGraphicsObjectListSelection::InvertSelection(CSelectionList* p_sel)
{
    CGraphics* p_grph = p_sel->GetProject()->GetGraphics();
    if( p_grph == NULL ) return(ESR_NONE_SELECTED);

    foreach(QObject* p_qobj,p_grph->GetObjects()->children()) {
        CGraphicsObject* p_gobj = static_cast<CGraphicsObject*>(p_qobj);
        CSelObject obj(p_gobj,0);
        if( p_gobj->IsFlagSet(EPOF_SELECTED) ) {
            p_sel->RemoveObject(obj);
        } else {
            p_sel->AddObject(obj);
        }
    }

    return(ESR_SELECTED_OBJECTS_CHANGED);
}

//------------------------------------------------------------------------------

ESelResult CGraphicsObjectListSelection::CompleteAllObjects(CSelectionList* p_sel)
{
    CGraphics* p_grph = p_sel->GetProject()->GetGraphics();
    if( p_grph == NULL ) return(ESR_NONE_SELECTED);

    foreach(QObject* p_qobj,p_grph->GetObjects()->children()) {
        CGraphicsObject* p_gobj = static_cast<CGraphicsObject*>(p_qobj);
        CSelObject obj(p_gobj,0);
        if( ! p_gobj->IsFlagSet(EPOF_SELECTED) ) {
            p_sel->AddObject(obj);
        }
    }

    return(ESR_SELECTED_OBJECTS_CHANGED);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
