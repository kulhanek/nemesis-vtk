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

#include "RMSDPropertyHistory.hpp"
#include "RMSDToolsModule.hpp"
#include "RMSDProperty.hpp"
#include <Structure.hpp>
#include <Project.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

REGISTER_HISTORY_OBJECT(RMSDToolsPlugin,RMSDPropertyTemplateHI,
                        "{RMSD_PROP_TEMPLATE:64a3e360-2132-44fb-b4e4-150f1f1d3a5f}")
REGISTER_HISTORY_OBJECT(RMSDToolsPlugin,RMSDPropertyTargetHI,
                        "{RMSD_PROP_TARGET:0bd81f6a-56e5-42d8-a070-81ec07d223b2}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRMSDPropertyTemplateHI::CRMSDPropertyTemplateHI(CRMSDProperty* p_prop,CStructure* p_str)
    : CHistoryItem(&RMSDPropertyTemplateHIObject,p_prop->GetProject(),EHID_FORWARD)
{
    PropertyIndex = p_prop->GetIndex();
    PrevStrIndex = -1;
    if( p_prop->GetTemplateStructure() ){
        PrevStrIndex = p_prop->GetTemplateStructure()->GetIndex();
    }
    NewStrIndex = -1;
    if( p_str ){
        NewStrIndex = p_str->GetIndex();
    }
}

//------------------------------------------------------------------------------

void CRMSDPropertyTemplateHI::Forward(void)
{
    CRMSDProperty* p_prop = dynamic_cast<CRMSDProperty*>(GetProject()->FindObject(PropertyIndex));
    if(p_prop == NULL) return;

    CStructure* p_str = dynamic_cast<CStructure*>(GetProject()->FindObject(NewStrIndex));
    if(p_str == NULL) return;

    p_prop->SetTemplateStructure(p_str);
}

//------------------------------------------------------------------------------

void CRMSDPropertyTemplateHI::Backward(void)
{
    CRMSDProperty* p_prop = dynamic_cast<CRMSDProperty*>(GetProject()->FindObject(PropertyIndex));
    if(p_prop == NULL) return;

    CStructure* p_str = dynamic_cast<CStructure*>(GetProject()->FindObject(PrevStrIndex));
    if(p_str == NULL) return;

    p_prop->SetTemplateStructure(p_str);
}

//------------------------------------------------------------------------------

void CRMSDPropertyTemplateHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("prop",PropertyIndex);
    p_ele->GetAttribute("prev",PrevStrIndex);
    p_ele->GetAttribute("new",NewStrIndex);
}

//------------------------------------------------------------------------------

void CRMSDPropertyTemplateHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("prop",PropertyIndex);
    p_ele->SetAttribute("prev",PrevStrIndex);
    p_ele->SetAttribute("new",NewStrIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRMSDPropertyTargetHI::CRMSDPropertyTargetHI(CRMSDProperty* p_prop,CStructure* p_str)
    : CHistoryItem(&RMSDPropertyTargetHIObject,p_prop->GetProject(),EHID_FORWARD)
{
    PropertyIndex = p_prop->GetIndex();
    PrevStrIndex = -1;
    if( p_prop->GetTargetStructure() ){
        PrevStrIndex = p_prop->GetTargetStructure()->GetIndex();
    }
    NewStrIndex = -1;
    if( p_str ){
        NewStrIndex = p_str->GetIndex();
    }
}

//------------------------------------------------------------------------------

void CRMSDPropertyTargetHI::Forward(void)
{
    CRMSDProperty* p_prop = dynamic_cast<CRMSDProperty*>(GetProject()->FindObject(PropertyIndex));
    if(p_prop == NULL) return;

    CStructure* p_str = dynamic_cast<CStructure*>(GetProject()->FindObject(NewStrIndex));
    if(p_str == NULL) return;

    p_prop->SetTargetStructure(p_str);
}

//------------------------------------------------------------------------------

void CRMSDPropertyTargetHI::Backward(void)
{
    CRMSDProperty* p_prop = dynamic_cast<CRMSDProperty*>(GetProject()->FindObject(PropertyIndex));
    if(p_prop == NULL) return;

    CStructure* p_str = dynamic_cast<CStructure*>(GetProject()->FindObject(PrevStrIndex));
    if(p_str == NULL) return;

    p_prop->SetTargetStructure(p_str);
}

//------------------------------------------------------------------------------

void CRMSDPropertyTargetHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("prop",PropertyIndex);
    p_ele->GetAttribute("prev",PrevStrIndex);
    p_ele->GetAttribute("new",NewStrIndex);
}

//------------------------------------------------------------------------------

void CRMSDPropertyTargetHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("prop",PropertyIndex);
    p_ele->SetAttribute("prev",PrevStrIndex);
    p_ele->SetAttribute("new",NewStrIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



