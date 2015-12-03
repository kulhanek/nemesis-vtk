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

#include <DirectionSelection.hpp>
#include <SelectionList.hpp>
#include <Bond.hpp>
#include <Atom.hpp>
#include <queue>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDirectionSelection SH_Direction;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CDirectionSelection::GetHandlerDescription(void) const
{
    return("select two atoms or a bond");
}

//------------------------------------------------------------------------------

ESelResult CDirectionSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
{
    CProObject* p_obj = obj.GetObject();

    if( p_obj == NULL ){ 
        if( obj.GetSubID() == SEL_ESC_KEY ){
            UnselectAll(p_sel);
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
        if( obj.GetSubID() == SEL_INVERT ){
            InvertDirection(p_sel);
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
        if( obj.GetSubID() == SEL_COMPLETE_MOLECULES ){
            SelectMolecule(p_sel);
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
    }

    // none object is selected
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // test structure object selection mode
    if( TestStrObjectSelMode(p_sel,p_obj) == false )  return(ESR_NONE_SELECTED);

    ESelResult result = ESR_NONE_SELECTED;
    if( p_sel->NumOfSelectedObjects() >= 2 ){
        UnselectAll(p_sel);
        result = ESR_SELECTED_OBJECTS_CHANGED;
    }

    // if object is CBond then select it
    CBond* p_bond = dynamic_cast<CBond*>(p_obj);
    if( p_bond != NULL ) {
        if( p_sel->IsInList(p_bond->GetFirstAtom()) == false ){
            p_sel->AddObject( CSelObject(p_bond->GetFirstAtom(),1) );
        }
        if( p_sel->IsInList(p_bond->GetSecondAtom()) == false ){
            p_sel->AddObject( CSelObject(p_bond->GetSecondAtom(),1) );
        }
        SelectDirection(p_sel);
        // never emit ESR_SELECTED_OBJECTS_END
        return(ESR_SELECTED_OBJECTS_CHANGED);
    }

    // select Atom
    CAtom* p_atom = dynamic_cast<CAtom*>(p_obj);
    if( p_atom != NULL ){
        if( p_sel->IsInList(p_atom) ){
            p_sel->RemoveObject(obj);
            return(ESR_SELECTED_OBJECTS_CHANGED);
        } else {
            p_sel->AddObject( CSelObject(p_atom,1) );
        }
        if( p_sel->NumOfSelectedObjects() == 2 ){
            SelectDirection(p_sel);
        }
        // never emit ESR_SELECTED_OBJECTS_END
        return(ESR_SELECTED_OBJECTS_CHANGED);
    }

    return(result);
}

//------------------------------------------------------------------------------

void CDirectionSelection::UnselectAll(CSelectionList* p_sel)
{
    while( ! p_sel->SelectedObjects.isEmpty() ) {
        CSelObject obj = p_sel->SelectedObjects.first();
        p_sel->RemoveObject(obj);
    }
}

//------------------------------------------------------------------------------

void CDirectionSelection::InvertDirection(CSelectionList* p_sel)
{
    if( p_sel->NumOfSelectedObjects() < 2 ) return;

    // backup master objects
    CSelObject* p_obj1 = p_sel->GetSelectedSelObject(0);
    if( p_obj1->GetSubID() != 1 ) return;
    CSelObject  obj1(p_obj1->GetObject(),1);

    CSelObject* p_obj2 = p_sel->GetSelectedSelObject(1);
    if( p_obj2->GetSubID() != 1 ) return;
    CSelObject  obj2(p_obj2->GetObject(),1);

    // unselect all
    UnselectAll(p_sel);

    // add objects in oposite dirrection
    p_sel->AddObject( obj2 );
    p_sel->AddObject( obj1 );

    // complete direction
    SelectDirection(p_sel);
}

//------------------------------------------------------------------------------

void CDirectionSelection::SelectDirection(CSelectionList* p_sel)
{
    CSelObject* p_obj1 = p_sel->GetSelectedSelObject(0);
    CSelObject* p_obj2 = p_sel->GetSelectedSelObject(1);

    CAtom* p_at1 = NULL;
    CAtom* p_at2 = NULL;

    if( p_obj1->GetSubID() == 1 ){
        p_at1 = dynamic_cast<CAtom*>(p_obj1->GetObject());
    }
    if( p_obj2->GetSubID() == 1 ){
        p_at2 = dynamic_cast<CAtom*>(p_obj2->GetObject());
    }

    if( (p_at1 == NULL) || (p_at2 == NULL) ) return;

    std::queue<CAtom*>    stack;
    stack.push(p_at2);

    CBond* p_exclude_bond = p_at1->IsBondedWith(p_at2);

    // go through the stack over interconnected atoms
    while( stack.size() > 0 ) {
        CAtom*  p_atom = stack.front();
        stack.pop();

        foreach(CBond* p_bond, p_atom->GetBonds()) {
            CAtom* p_oa = p_bond->GetOppositeAtom(p_atom);
            if( p_exclude_bond != p_bond ){
                if( p_sel->IsInList(p_oa) == false ) {
                    stack.push(p_oa);
                    p_sel->AddObject( CSelObject(p_oa,2) );
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void CDirectionSelection::SelectMolecule(CSelectionList* p_sel)
{
    if( p_sel->NumOfSelectedObjects() < 2 ) return;

    CSelObject* p_obj1 = p_sel->GetSelectedSelObject(0);
    if( p_obj1->GetSubID() != 1 ) return;
    CSelObject  obj1(p_obj1->GetObject(),2);

    CSelObject* p_obj2 = p_sel->GetSelectedSelObject(1);
    if( p_obj2->GetSubID() != 1 ) return;
    CSelObject  obj2(p_obj2->GetObject(),2);

    // unselect all
    UnselectAll(p_sel);

    // add objects back to the list
    p_sel->AddObject( obj1 );
    p_sel->AddObject( obj2 );

    CAtom* p_at1 = NULL;
    CAtom* p_at2 = NULL;

    p_at1 = dynamic_cast<CAtom*>(obj1.GetObject());
    p_at2 = dynamic_cast<CAtom*>(obj2.GetObject());

    std::queue<CAtom*>    stack;

    if( p_at1 != NULL ) stack.push(p_at1);
    if( p_at2 != NULL ) stack.push(p_at2);

    // go through the stack over interconnected atoms
    while( stack.size() > 0 ) {
        CAtom*  p_atom = stack.front();

        stack.pop();

        foreach(CBond* p_bond, p_atom->GetBonds()) {
            CAtom* p_oa = p_bond->GetOppositeAtom(p_atom);
            if( p_sel->IsInList(p_oa) == false ) {
                stack.push(p_oa);
                p_sel->AddObject( CSelObject(p_oa,2) );
            }
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
