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

#include <DistanceSelection.hpp>
#include <SelectionList.hpp>
#include <Atom.hpp>
#include <Bond.hpp>
#include <Structure.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDistanceSelection SH_Distance;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CDistanceSelection::GetHandlerDescription(void) const
{
    return("select a distance");
}

//------------------------------------------------------------------------------

ESelResult CDistanceSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
{
    CProObject* p_obj = obj.GetObject();

    if( p_obj == NULL ){
        if( obj.GetSubID() == SEL_ESC_KEY ){
            // reset list
            p_sel->ResetSelection();
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
    }

    // none object is selected
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // test structure object selection mode
    if( TestStrObjectSelMode(p_sel,p_obj) == false )  return(ESR_NONE_SELECTED);

    // is it already in selection list?
    if( p_sel->IsInList(obj) == true ) {
        p_sel->RemoveObject(obj);
        return(ESR_SELECTED_OBJECTS_CHANGED);
    }

    CGeoDescriptor descrip1;

    if( p_sel->NumOfSelectedObjects() == 0 ) {
        // no object selected so far
        descrip1 = p_obj->GetGeoDescriptor();
        switch(descrip1.GetType()) {
        case EGDT_TWO_POINT:
        case EGDT_TWO_POINT_ORIENTED:
            p_sel->AddObject(obj);
            return(ESR_SELECTED_OBJECTS_END);

        case EGDT_ONE_POINT:
        case EGDT_LINE:
        case EGDT_PLANE:
            p_sel->AddObject(obj);
            return(ESR_SELECTED_OBJECTS_CHANGED);

        default:
            return(ESR_NONE_SELECTED);
        }
    } else {
        // some object is already selected
        descrip1 = p_sel->GetSelectedObject(0)->GetGeoDescriptor();

        CGeoDescriptor descrip2;
        descrip2 = p_obj->GetGeoDescriptor();

        if( descrip1.GetType() == EGDT_ONE_POINT ) {
            switch(descrip2.GetType()) {
            case EGDT_ONE_POINT:
            case EGDT_LINE:
            case EGDT_PLANE:
                p_sel->AddObject(obj);
                return(ESR_SELECTED_OBJECTS_END);

            default:
                return(ESR_NONE_SELECTED);
            }
        } else {
            if( descrip2.GetType() != EGDT_ONE_POINT) return(ESR_NONE_SELECTED);
            p_sel->AddObject(obj);
            return(ESR_SELECTED_OBJECTS_END);
        }
    }

    return(ESR_NONE_SELECTED);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
