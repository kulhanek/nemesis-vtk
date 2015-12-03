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

#include <ConnectAtomSelection.hpp>
#include <SelectionList.hpp>
#include <Atom.hpp>
#include <Bond.hpp>
#include <Structure.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CConnectAtomSelection SH_ConnectAtom;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CConnectAtomSelection::GetHandlerDescription(void) const
{
    return("select a connect atom (open valence)");
}

//------------------------------------------------------------------------------

ESelResult CConnectAtomSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
{
    CProObject* p_obj = obj.GetObject();

    // none object is selected
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // test structure object selection mode
    if( TestStrObjectSelMode(p_sel,p_obj) == false )  return(ESR_NONE_SELECTED);

    // molecule can be connect object
    CStructure* p_mol = dynamic_cast<CStructure*>(p_obj);
    if( p_mol != NULL ) {
        CSelObject sel(p_mol,0);
        p_sel->AddObject(sel);
        return(ESR_SELECTED_OBJECTS_END);
    }

    CAtom* p_atom = dynamic_cast<CAtom*>(p_obj);
    if( p_atom != NULL ) {
        // atom can be connect only there is a connect bond
        if( p_atom->GetConnectBond() != NULL ) {
            CSelObject sel(p_atom,0);
            p_sel->AddObject(sel);
            return(ESR_SELECTED_OBJECTS_END);
        }
        return(ESR_NONE_SELECTED);
    }

    CBond* p_bond = dynamic_cast<CBond*>(p_obj);
    if( p_bond != NULL ) {
        // bond can be selected only if there is a connect atom
        if( (p_atom = p_bond->GetConnectAtom()) != NULL) {
            CSelObject sel(p_atom,0);
            p_sel->AddObject(sel);
            return(ESR_SELECTED_OBJECTS_END);
        }
        return(ESR_NONE_SELECTED);
    }

    return(ESR_NONE_SELECTED);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
