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

#include <PointSelection.hpp>
#include <SelectionList.hpp>
#include <Atom.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPointSelection SH_Point(false);
CPointSelection SH_Point_ESC(true);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPointSelection::CPointSelection(bool esc_transmit)
{
    TransmitESCObject = esc_transmit;
}

//------------------------------------------------------------------------------

const QString CPointSelection::GetHandlerDescription(void) const
{
    return("select a point");
}

//------------------------------------------------------------------------------

ESelResult CPointSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
{
    CProObject* p_obj = obj.GetObject();

    if( p_obj == NULL ){
        if( TransmitESCObject ){
            if( obj.GetSubID() == SEL_ESC_KEY ){
                p_sel->AddObject(obj);
                return(ESR_SELECTED_OBJECTS_END);
            }
        }
    }

    // none object is selected
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // test structure object selection mode
    if( TestStrObjectSelMode(p_sel,p_obj) == false )  return(ESR_NONE_SELECTED);

    CGeoDescriptor desc;

    desc = p_obj->GetGeoDescriptor();
    switch(desc.GetType()) {

        case EGDT_ONE_POINT:
            p_sel->AddObject(obj);
            return(ESR_SELECTED_OBJECTS_END);

        default:
            return(ESR_NONE_SELECTED);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
