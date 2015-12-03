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

#include <PropertyHistory.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <Property.hpp>
#include <ProObjectHistory.hpp>
#include <ErrorSystem.hpp>
#include <PropertyList.hpp>
#include <XMLElement.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,PropertyHI,
                        "{PRP:5da86453-6824-41b2-9050-1f35c3963fdd}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPropertyHI::CPropertyHI(CProperty* p_prop,EHistoryItemDirection change)
    : CHistoryItem(&PropertyHIObject,p_prop->GetProject(),change)
{
    CXMLElement* p_ele = PropertyData.CreateChildElement("property");
    p_prop->SaveData(p_ele);
}

//------------------------------------------------------------------------------

void CPropertyHI::Forward(void)
{
    CXMLElement* p_ele = PropertyData.GetFirstChildElement("property");
    GetProject()->GetProperties()->CreateProperty(p_ele);
}

//------------------------------------------------------------------------------

void CPropertyHI::Backward(void)
{
    CXMLElement* p_ele = PropertyData.GetFirstChildElement("property");
    if( p_ele == NULL ) return;
    int go_index = -1;
    p_ele->GetAttribute("id",go_index);

    CProperty* p_prop = dynamic_cast<CProperty*>(GetProject()->FindObject(go_index));
    if(p_prop == NULL) return;

    // save designer setup
    CXMLElement* p_sdele = p_prop->GetDesignerData();
    if( p_sdele ){
        CXMLElement* p_tdele = PropertyData.GetChildElementByPath("property/designer",true);
        p_tdele->RemoveAllChildNodes();
        p_tdele->RemoveAllAttributes();
        p_tdele->CopyContentsFrom(p_sdele);
    }

    p_prop->RemoveFromBaseList();
}

//------------------------------------------------------------------------------

void CPropertyHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    CXMLElement* p_sele = p_ele->GetFirstChildElement("property");
    if( p_sele == NULL ) return;
    PropertyData.RemoveAllChildNodes();
    CXMLElement* p_dele = PropertyData.CreateChildElement("property");
    p_dele->CopyContentsFrom(p_sele);
}

//------------------------------------------------------------------------------

void CPropertyHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    CXMLElement* p_sele = PropertyData.GetFirstChildElement("property");
    if( p_sele == NULL ) return;
    CXMLElement* p_dele = p_ele->CreateChildElement("property");
    p_dele->CopyContentsFrom(p_sele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
