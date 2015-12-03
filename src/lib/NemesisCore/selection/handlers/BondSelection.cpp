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

#include <BondSelection.hpp>
#include <SelectionList.hpp>
#include <Bond.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBondSelection SH_Bond;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CBondSelection::GetHandlerDescription(void) const
{
    return("select a bond");
}

//------------------------------------------------------------------------------

ESelResult CBondSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
{
    CProObject* p_obj = obj.GetObject();

    // none object is selected
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // test structure object selection mode
    if( TestStrObjectSelMode(p_sel,p_obj) == false )  return(ESR_NONE_SELECTED);

    // if object is CBond then select it
    CBond* p_bond = dynamic_cast<CBond*>(p_obj);
    if( p_bond != NULL ) {
        p_sel->AddObject(obj);
        return(ESR_SELECTED_OBJECTS_END);
    }

    return(ESR_NONE_SELECTED);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
