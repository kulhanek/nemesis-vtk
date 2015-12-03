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

#include <QtGui>

#include "RMSDToolsModule.hpp"
#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <Atom.hpp>

#include "RMSDProperty.hpp"
#include "MapCouple.hpp"
#include "MapCoupleHistory.hpp"
#include "MapCouple.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        MapCoupleID(
                    "{MAP_COUPLE:9e683db6-ad78-402f-a51f-d298c64babcb}",
                    "Map Couple");

CPluginObject   MapCoupleObject(&RMSDToolsPlugin,
                    MapCoupleID,WORK_PANEL_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMapCouple::CMapCouple(CRMSDProperty* p_map_list,CAtom* p_at1,CAtom* p_at2)
    : CProObject(&MapCoupleObject,p_map_list,p_map_list->GetProject())
{
    if( p_at1 == NULL ){
        INVALID_ARGUMENT("p_at1 is NULL");
    }
    if( p_at2 == NULL ){
        INVALID_ARGUMENT("p_at2 is NULL");
    }

    if( p_at1->GetStructure() == p_map_list->GetTemplateStructure() ){
        TemplateAtom = p_at1;
        TargetAtom = p_at2;
    } else {
        TemplateAtom = p_at2;
        TargetAtom = p_at1;
    }
    if( TemplateAtom == TargetAtom ){
        INVALID_ARGUMENT("template is the same as target");
    }

    TemplateAtom->RegisterObject(this);
    TargetAtom->RegisterObject(this);
}

// -----------------------------------------------------------------------------

CMapCouple::CMapCouple(CRMSDProperty* p_map_list)
    : CProObject(&MapCoupleObject,p_map_list,p_map_list->GetProject())
{
    TemplateAtom = NULL;
    TargetAtom = NULL;

    // default name is empty
    SetName("");
}

// -----------------------------------------------------------------------------

CMapCouple::~CMapCouple(void)
{
    CRMSDProperty* p_list = GetMapList();
    setParent(NULL);    // remove object from the list
    if( p_list ) p_list->EmitOnRMSDPropertyMapChanged();
}

// -----------------------------------------------------------------------------

void CMapCouple::RemoveFromBaseList(CHistoryNode* p_history)
{
    if( TemplateAtom ){
        TemplateAtom->RemoveFromRegistered(this,p_history);
    }
    if( TargetAtom ){
        TargetAtom->RemoveFromRegistered(this,p_history);
    }

    // register elementary change to history list
    if( p_history != NULL ) {
        CHistoryItem* p_hi = new CMapCoupleHI(this,EHID_BACKWARD);
        p_history->Register(p_hi);
    }

    delete this;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CMapCouple::DeleteWH(void)
{
    CRMSDProperty*  p_list = GetMapList(); // object will be destroyed - use baselist instead
    CHistoryNode*   p_history = NULL;

    p_history = p_list->BeginChangeWH(EHCL_PROPERTY,"delete map couple");
    if( p_history == NULL ) return (false);

    RemoveFromBaseList(p_history);

    p_list->EndChangeWH();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtom* CMapCouple::GetTemplateAtom(void)
{
    return(TemplateAtom);
}

// -----------------------------------------------------------------------------

CAtom* CMapCouple::GetTargetAtom(void)
{
    return(TargetAtom);
}

// -----------------------------------------------------------------------------

bool CMapCouple::IsHydrogenCouple(void)
{
    if( TemplateAtom ){
        if( TemplateAtom->IsVirtual() ) return(true);
    }
    if( TargetAtom ){
        if( TargetAtom->IsVirtual() ) return(true);
    }
    return(false);
}

// -----------------------------------------------------------------------------

CRMSDProperty* CMapCouple::GetMapList(void)
{
    return(dynamic_cast<CRMSDProperty*>(parent()));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMapCouple::LoadData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "couple" ) {
        LOGIC_ERROR("p_ele is not couple");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    // load data ------------------------------------
    int tei = -1;
    int tai = -1;

    p_ele->GetAttribute("tei",tei);
    TemplateAtom = static_cast<CAtom*>(GetProject()->FindObject(tei));
    if( TemplateAtom ){
        TemplateAtom->RegisterObject(this);
    }

    p_ele->GetAttribute("tai",tai);
    TargetAtom = static_cast<CAtom*>(GetProject()->FindObject(tai));
    if( TargetAtom ){
        TargetAtom->RegisterObject(this);
    }
}

//------------------------------------------------------------------------------

void CMapCouple::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "couple" ) {
        LOGIC_ERROR("p_ele is not couple");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    // save data ------------------------------------
    if( TemplateAtom ){
        p_ele->SetAttribute("tei",TemplateAtom->GetIndex());
    }
    if( TargetAtom ){
        p_ele->SetAttribute("tai",TargetAtom->GetIndex());
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMapCouple::RemoveFromRegistered(CProObject* p_object,CHistoryNode* p_history)
{
    if( p_object == TemplateAtom ){
        RemoveFromBaseList(p_history);
    }
    if( p_object == TargetAtom ){
        RemoveFromBaseList(p_history);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




