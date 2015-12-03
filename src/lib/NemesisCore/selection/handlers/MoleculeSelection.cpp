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

#include <MoleculeSelection.hpp>
#include <SelectionList.hpp>
#include <Bond.hpp>
#include <Atom.hpp>
#include <queue>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMoleculeSelection SH_Molecule;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CMoleculeSelection::GetHandlerDescription(void) const
{
    return("select a molecule");
}

//------------------------------------------------------------------------------

ESelResult CMoleculeSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
{
    CProObject* p_obj = obj.GetObject();

    // none object is selected
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // test structure object selection mode
    if( TestStrObjectSelMode(p_sel,p_obj) == false )  return(ESR_NONE_SELECTED);

    // if object is CBond then select it
    CBond* p_bond = dynamic_cast<CBond*>(p_obj);
    if( p_bond != NULL ) {
        if( p_sel->IsInList(p_bond->GetFirstAtom()) == false ){
            p_sel->AddObject( CSelObject(p_bond->GetFirstAtom(),0) );
        }
        SelectMolecule(p_sel);
        return(ESR_SELECTED_OBJECTS_END);
    }

    // select Atom
    CAtom* p_atom = dynamic_cast<CAtom*>(p_obj);
    if( p_atom != NULL ){
        if( p_sel->IsInList(p_atom) == false ){
            p_sel->AddObject( CSelObject(p_atom,0) );
        }
        SelectMolecule(p_sel);
        return(ESR_SELECTED_OBJECTS_END);
    }

    return(ESR_NONE_SELECTED);
}

//------------------------------------------------------------------------------

void CMoleculeSelection::SelectMolecule(CSelectionList* p_sel)
{
    if( p_sel->NumOfSelectedObjects() < 1 ) return;

    CSelObject* p_obj1 = p_sel->GetSelectedSelObject(0);

    CAtom* p_at1 = NULL;

    p_at1 = dynamic_cast<CAtom*>(p_obj1->GetObject());

    std::queue<CAtom*>    stack;

    if( p_at1 != NULL ) stack.push(p_at1);

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
