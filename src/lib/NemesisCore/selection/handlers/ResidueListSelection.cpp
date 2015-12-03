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

#include <ResidueListSelection.hpp>
#include <SelectionList.hpp>
#include <Residue.hpp>
#include <Atom.hpp>
#include <Bond.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <ResidueList.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CResidueListSelection SH_ResidueList;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CResidueListSelection::GetHandlerDescription(void) const
{
    return("select residues");
}

//------------------------------------------------------------------------------

ESelResult CResidueListSelection::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
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
            return(CompleteAllStructures(p_sel));
        }
        return(ESR_NONE_SELECTED);
    }

    // none object is selected
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // test structure object selection mode
    if( TestStrObjectSelMode(p_sel,p_obj) == false )  return(ESR_NONE_SELECTED);

    // if object is CAtom then select its residue
    CAtom* p_atom = dynamic_cast<CAtom*>(p_obj);
    if( p_atom != NULL ) {
        CResidue* p_res = p_atom->GetResidue();
        if( p_res == NULL ) return(ESR_NONE_SELECTED);
        CSelObject sel(p_res,0);
        if( p_sel->IsInList(sel) ){
            p_sel->RemoveObject(sel);
        } else {
            p_sel->AddObject(sel);
        }
        return(ESR_SELECTED_OBJECTS_CHANGED);
    }

    // if object is CBond then select its residues (for both atoms)
    CBond* p_bond = dynamic_cast<CBond*>(p_obj);
    if( p_bond != NULL ) {
        CResidue* p_res1 = NULL;
        CResidue* p_res2 = NULL;
        CAtom* p_at1 = p_bond->GetFirstAtom();
        if( p_at1 != NULL ){
            p_res1 = p_at1->GetResidue();
        }
        CAtom* p_at2 = p_bond->GetSecondAtom();
        if( p_at2 != NULL ){
            p_res2 = p_at2->GetResidue();
        }
        bool changed = false;

        if( p_res1 != NULL ){
            CSelObject sel(p_res1,0);
            if( p_sel->IsInList(sel) ){
                p_sel->RemoveObject(sel);
            } else {
                p_sel->AddObject(sel);
            }
            changed = true;
        }
        if( (p_res2 != p_res1) && (p_res2 != NULL) ){
            CSelObject sel(p_res2,0);
            if( p_sel->IsInList(sel) ){
                p_sel->RemoveObject(sel);
            } else {
                p_sel->AddObject(sel);
            }
            changed = true;
        }
        if( changed ) return(ESR_SELECTED_OBJECTS_CHANGED);
        return(ESR_NONE_SELECTED);
    }

    // if object is CResidue then select it
    CResidue* p_res = dynamic_cast<CResidue*>(p_obj);
    if( p_res != NULL ) {
        CSelObject sel(p_res,0);
        if( p_sel->IsInList(sel) ){
            p_sel->RemoveObject(sel);
        } else {
            p_sel->AddObject(sel);
        }
        return(ESR_SELECTED_OBJECTS_CHANGED);
    }

    return(ESR_NONE_SELECTED);
}

//------------------------------------------------------------------------------

ESelResult CResidueListSelection::InvertSelection(CSelectionList* p_sel)
{
    CStructure* p_mol = p_sel->GetProject()->GetActiveStructure();
    if( p_mol == NULL ) return(ESR_NONE_SELECTED);

    foreach(QObject* p_qobj,p_mol->GetResidues()->children()) {
        CResidue* p_res = static_cast<CResidue*>(p_qobj);
        CSelObject obj(p_res,0);
        if( p_res->IsFlagSet(EPOF_SELECTED) ) {
            p_sel->RemoveObject(obj);
        } else {
            p_sel->AddObject(obj);
        }
    }

    return(ESR_SELECTED_OBJECTS_CHANGED);
}

//------------------------------------------------------------------------------

ESelResult CResidueListSelection::CompleteAllStructures(CSelectionList* p_sel)
{
    // FIXME
    return(ESR_NONE_SELECTED);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
