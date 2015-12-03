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

#include <AtomSeqSelection.hpp>
#include <SelectionList.hpp>
#include <Atom.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomSeqSelection SH_TwoAtoms(2);
CAtomSeqSelection SH_ThreeAtoms(3);
CAtomSeqSelection SH_FourAtoms(4);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomSeqSelection::CAtomSeqSelection(int numofreqatoms)
{
    NumOfRequestedAtoms = numofreqatoms;
}

//------------------------------------------------------------------------------

const QString CAtomSeqSelection::GetHandlerDescription(void) const
{
    switch(NumOfRequestedAtoms) {
    case 2:
        return("select two atoms");
    case 3:
        return("select three atoms");
    case 4:
        return("select four atoms");
    default: {
        QString descr;
        descr = "select " + QString(NumOfRequestedAtoms) + " atoms";
        return(descr);
    }
    }
}

//------------------------------------------------------------------------------

ESelResult CAtomSeqSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
{
    CProObject* p_obj = obj.GetObject();

    // special keys
    if( p_obj == NULL ){
        if( obj.GetSubID() == SEL_ESC_KEY ){
            // reset list
            p_sel->ResetSelection();
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
        return(ESR_NONE_SELECTED);
    }

    // test structure object selection mode
    if( TestStrObjectSelMode(p_sel,p_obj) == false )  return(ESR_NONE_SELECTED);

    // is it already in selection list?
    if( p_sel->IsInList(obj) == true ) {
        p_sel->RemoveObject(obj);
        return(ESR_SELECTED_OBJECTS_CHANGED);
    }

    // if object is CAtom then select it
    CAtom* p_atom = dynamic_cast<CAtom*>(p_obj);
    if( p_atom != NULL ) {
        p_sel->AddObject(obj);
        if( p_sel->NumOfSelectedObjects() == NumOfRequestedAtoms ) {
            return(ESR_SELECTED_OBJECTS_END);
        } else {
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
    }

    return(ESR_NONE_SELECTED);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
