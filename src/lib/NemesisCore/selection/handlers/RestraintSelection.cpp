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

#include <RestraintSelection.hpp>
#include <SelectionList.hpp>
#include <Restraint.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRestraintSelection SH_Restraint;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CRestraintSelection::GetHandlerDescription(void) const
{
    return("select a restraint");
}

//------------------------------------------------------------------------------

ESelResult CRestraintSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
{
    CProObject* p_obj = obj.GetObject();

    // none object is selected
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // test structure object selection mode
    if( TestStrObjectSelMode(p_sel,p_obj) == false )  return(ESR_NONE_SELECTED);

    // is object restraint?
    CRestraint* p_rst = GetRestraint(p_sel,p_obj);
    if( p_rst == NULL )return(ESR_NONE_SELECTED);

    // select restraint
    CSelObject sel(p_rst,0);
    p_sel->AddObject(sel);

    return(ESR_SELECTED_OBJECTS_END);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
