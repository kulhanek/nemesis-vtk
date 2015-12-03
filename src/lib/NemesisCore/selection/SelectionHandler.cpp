// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <SelectionHandler.hpp>
#include <Atom.hpp>
#include <Bond.hpp>
#include <Residue.hpp>
#include <Restraint.hpp>
#include <Structure.hpp>
#include <StructureList.hpp>
#include <SelectionList.hpp>
#include <Project.hpp>
#include <Property.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSelectionHandler::CSelectionHandler(void)
{

}

//------------------------------------------------------------------------------

CSelectionHandler::~CSelectionHandler(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

ESelResult CSelectionHandler::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
{
    return(ESR_NONE_SELECTED);
}

//------------------------------------------------------------------------------

const QString CSelectionHandler::GetHandlerDescription(void) const
{
    return("not redefined");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CStructure* CSelectionHandler::GetStructure(CSelectionList* p_sel,CProObject* p_obj)
{
    // no object
    if( p_obj == NULL ) return(NULL);

    // if object is CAtom
    CAtom* p_atom = dynamic_cast<CAtom*>(p_obj);
    if( p_atom != NULL ) {
        return(p_atom->GetStructure());
    }

    // if object is CBond
    CBond* p_bond = dynamic_cast<CBond*>(p_obj);
    if( p_bond != NULL ) {
        return(p_bond->GetStructure());
    }

    // if object is CResidue
    CResidue* p_res = dynamic_cast<CResidue*>(p_obj);
    if( p_res != NULL ) {
        return(p_res->GetStructure());
    }

    // if object is CRestraint
    CRestraint* p_rst = GetRestraint(p_sel,p_obj);
    if( p_rst != NULL ) {
        return(p_rst->GetStructure());
    }

    // if it is structure
    CStructure* p_str = dynamic_cast<CStructure*>(p_obj);
    if( p_str != NULL ) {
        return(p_str);
    }

    // object is not associated with structure
    return(NULL);
}

//------------------------------------------------------------------------------

bool CSelectionHandler::TestStrObjectSelMode(CSelectionList* p_sel,CProObject* p_obj)
{
    CStructure* p_str = GetStructure(p_sel,p_obj);
    if( p_str == NULL ) return(true);

    if( p_sel->GetStrObjectSelMode() == ESOSM_ACTIVE_STRUCTURE_ONLY ){
        if( p_sel->GetProject()->GetStructures()->GetActiveStructure() == p_str ){
            return(true);
        } else {
            p_sel->GetProject()->TextNotification(ETNT_WARNING,"selected object is not from the active structure",ETNT_WARNING_DELAY);
            return(false);
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

CRestraint* CSelectionHandler::GetRestraint(CSelectionList* p_sel,CProObject* p_obj)
{
    if( p_obj == NULL ) return(NULL);

    // if object is CRestraint
    CRestraint* p_rst = dynamic_cast<CRestraint*>(p_obj);
    if( p_rst != NULL ) {
        return(p_rst);
    }

    // if object is CProperty
    CProperty* p_prop = dynamic_cast<CProperty*>(p_obj);
    if( p_prop != NULL ){
        QSet<CRestraint*> rsts;
        foreach(CProObject* p_ro, p_prop->GetROList()){
            CRestraint* p_rst = dynamic_cast<CRestraint*>(p_ro);
            if( p_rst != NULL ) rsts.insert(p_rst);
        }
        if( rsts.count() == 0 ) return(NULL);
        if( rsts.count() == 1 ){
            return( *rsts.begin() );
        }
        p_sel->GetProject()->TextNotification(ETNT_WARNING,"selected property is associated with more than one restraint",ETNT_WARNING_DELAY);
    }

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

