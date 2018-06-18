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

#include "MapCoupleHistory.hpp"
#include "RMSDToolsModule.hpp"
#include "MapCouple.hpp"
#include "RMSDProperty.hpp"
#include <XMLElement.hpp>
#include <Project.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

REGISTER_HISTORY_OBJECT(RMSDToolsPlugin,MapCoupleHI,
                        "{MAP_COUPLE:f8fb0132-63f9-46e1-967e-8f3d452232e7}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMapCoupleHI::CMapCoupleHI(CMapCouple* p_couple,EHistoryItemDirection change)
    : CHistoryItem(&MapCoupleHIObject,p_couple->GetProject(),change)
{
    PropertyIndex = p_couple->GetMapList()->GetIndex();
    CXMLElement* p_ele = CoupleData.CreateChildElement("couple");
    p_couple->SaveData(p_ele);
}

//------------------------------------------------------------------------------

void CMapCoupleHI::Forward(void)
{
    CRMSDProperty* p_prop = dynamic_cast<CRMSDProperty*>(GetProject()->FindObject(PropertyIndex));
    if(p_prop == NULL) return;

    CXMLElement* p_ele = CoupleData.GetFirstChildElement("couple");
    p_prop->AddCouple(p_ele);
}

//------------------------------------------------------------------------------

void CMapCoupleHI::Backward(void)
{
    CXMLElement* p_ele = CoupleData.GetFirstChildElement("couple");
    if( p_ele == NULL ) return;
    int map_couple_index = -1;
    p_ele->GetAttribute("id",map_couple_index);

    CMapCouple* p_map = dynamic_cast<CMapCouple*>(GetProject()->FindObject(map_couple_index));
    if(p_map == NULL) return;

    p_map->RemoveFromBaseList();
}

//------------------------------------------------------------------------------

void CMapCoupleHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("pi",PropertyIndex);
    CXMLElement* p_sele = p_ele->GetFirstChildElement("couple");
    if( p_sele == NULL ) return;
    CoupleData.RemoveAllChildNodes();
    CXMLElement* p_dele = CoupleData.CreateChildElement("couple");
    p_dele->CopyContentsFrom(p_sele);
}

//------------------------------------------------------------------------------

void CMapCoupleHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("pi",PropertyIndex);
    CXMLElement* p_sele = CoupleData.GetFirstChildElement("couple");
    if( p_sele == NULL ) return;
    CXMLElement* p_dele = p_ele->CreateChildElement("couple");
    p_dele->CopyContentsFrom(p_sele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




