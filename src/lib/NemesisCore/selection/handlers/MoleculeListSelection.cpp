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

#include <MoleculeListSelection.hpp>
#include <SelectionList.hpp>
#include <Bond.hpp>
#include <Atom.hpp>
#include <queue>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMoleculeListSelection SH_MoleculeList;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CMoleculeListSelection::GetHandlerDescription(void) const
{
    return("select molecules");
}

//------------------------------------------------------------------------------

ESelResult CMoleculeListSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
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

    // if object is CBond then select it
    CBond* p_bond = dynamic_cast<CBond*>(p_obj);
    if( (p_bond != NULL) && (p_bond->GetFirstAtom() != NULL) ) {
        SelectMolecule(p_sel,p_bond->GetFirstAtom());
        return(ESR_SELECTED_OBJECTS_CHANGED);
    }

    // select Atom
    CAtom* p_atom = dynamic_cast<CAtom*>(p_obj);
    if( p_atom != NULL ){
        SelectMolecule(p_sel,p_atom);
        return(ESR_SELECTED_OBJECTS_CHANGED);
    }

    return(ESR_NONE_SELECTED);
}

//------------------------------------------------------------------------------

void CMoleculeListSelection::SelectMolecule(CSelectionList* p_sel,CAtom* p_at1)
{
    std::queue<CAtom*>    stack;

    if( p_sel->IsInList(p_at1) == false ) {
        p_sel->AddObject( CSelObject(p_at1,0) );
    }

    stack.push(p_at1);

    // go through the stack over interconnected atoms
    while( stack.size() > 0 ) {
        CAtom*  p_atom = stack.front();
        stack.pop();

        foreach(CBond* p_bond, p_atom->GetBonds()) {
            CAtom* p_oa = p_bond->GetOppositeAtom(p_atom);
            if( p_sel->IsInList(p_oa) == false ) {
                stack.push(p_oa);
                p_sel->AddObject( CSelObject(p_oa,0) );
            }
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
