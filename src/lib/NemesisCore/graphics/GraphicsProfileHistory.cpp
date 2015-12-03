// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <GraphicsProfileHistory.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <Graphics.hpp>
#include <GraphicsProfile.hpp>
#include <ProObjectHistory.hpp>
#include <ErrorSystem.hpp>
#include <GraphicsProfileList.hpp>
#include <GraphicsObject.hpp>
#include <XMLElement.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,GraphicsProfileHI,
                        "{GP:c5e8464c-5972-4be4-a0d1-659b24a24367}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,GraphicsProfileAddObjectHI,
                        "{GP_AO:8c450994-1fc7-4dfc-b2d7-e897c3fb1041}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,GraphicsProfileMoveObjectHI,
                        "{GP_MO:83c73aaf-c2ba-4ebf-8984-78fec2ad3da4}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsProfileHI::CGraphicsProfileHI(CGraphicsProfile* p_gp,EHistoryItemDirection change)
    : CHistoryItem(&GraphicsProfileHIObject,p_gp->GetProject(),change)
{
    CXMLElement* p_ele = GraphicsProfileData.CreateChildElement("profile");
    p_gp->SaveData(p_ele);
}

//------------------------------------------------------------------------------

void CGraphicsProfileHI::Forward(void)
{
    CXMLElement* p_ele = GraphicsProfileData.GetFirstChildElement("profile");
    GetProject()->GetGraphics()->GetProfiles()->CreateProfile(p_ele);
}

//------------------------------------------------------------------------------

void CGraphicsProfileHI::Backward(void)
{
    CXMLElement* p_ele = GraphicsProfileData.GetFirstChildElement("profile");
    if( p_ele == NULL ) return;
    int go_index = -1;
    p_ele->GetAttribute("id",go_index);

    CGraphicsProfile* p_go = dynamic_cast<CGraphicsProfile*>(GetProject()->FindObject(go_index));
    if(p_go == NULL) return;

    p_go->RemoveFromBaseList();
}

//------------------------------------------------------------------------------

void CGraphicsProfileHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    CXMLElement* p_sele = p_ele->GetFirstChildElement("profile");
    if( p_sele == NULL ) return;
    GraphicsProfileData.RemoveAllChildNodes();
    CXMLElement* p_dele = GraphicsProfileData.CreateChildElement("profile");
    p_dele->CopyContentsFrom(p_sele);
}

//------------------------------------------------------------------------------

void CGraphicsProfileHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    CXMLElement* p_sele = GraphicsProfileData.GetFirstChildElement("profile");
    if( p_sele == NULL ) return;
    CXMLElement* p_dele = p_ele->CreateChildElement("profile");
    p_dele->CopyContentsFrom(p_sele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsProfileAddObjectHI::CGraphicsProfileAddObjectHI(CGraphicsProfile* p_gp,
                                    CGraphicsObject* p_obj, int position,
                                    EHistoryItemDirection change)
    : CHistoryItem(&GraphicsProfileAddObjectHIObject,p_gp->GetProject(),change)
{
    GraphicsProfileIndex = p_gp->GetIndex();
    GraphicsObjectIndex = p_obj->GetIndex();
    Position = position;
}

//------------------------------------------------------------------------------

void CGraphicsProfileAddObjectHI::Forward(void)
{
    CGraphicsProfile* p_go = dynamic_cast<CGraphicsProfile*>(GetProject()->FindObject(GraphicsProfileIndex));
    if(p_go == NULL) return;
    CGraphicsObject* p_obj = dynamic_cast<CGraphicsObject*>(GetProject()->FindObject(GraphicsObjectIndex));
    if(p_obj == NULL) return;
    p_go->AddObject(p_obj,Position);
}

//------------------------------------------------------------------------------

void CGraphicsProfileAddObjectHI::Backward(void)
{
    CGraphicsProfile* p_go = dynamic_cast<CGraphicsProfile*>(GetProject()->FindObject(GraphicsProfileIndex));
    if(p_go == NULL) return;
    CGraphicsObject* p_obj = dynamic_cast<CGraphicsObject*>(GetProject()->FindObject(GraphicsObjectIndex));
    if(p_obj == NULL) return;
    p_go->RemoveObject(p_obj);
}

//------------------------------------------------------------------------------

void CGraphicsProfileAddObjectHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("pi",GraphicsProfileIndex);
    p_ele->GetAttribute("oi",GraphicsObjectIndex);
    p_ele->GetAttribute("ps",Position);
}

//------------------------------------------------------------------------------

void CGraphicsProfileAddObjectHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("pi",GraphicsProfileIndex);
    p_ele->SetAttribute("oi",GraphicsObjectIndex);
    p_ele->SetAttribute("ps",Position);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsProfileMoveObjectHI::CGraphicsProfileMoveObjectHI(CGraphicsProfile* p_gp,CGraphicsObject* p_obj,
                                                             EHistoryItemDirection change)
    : CHistoryItem(&GraphicsProfileMoveObjectHIObject,p_gp->GetProject(),change)
{
    GraphicsProfileIndex = p_gp->GetIndex();
    GraphicsObjectIndex = p_obj->GetIndex();
}

//------------------------------------------------------------------------------

void CGraphicsProfileMoveObjectHI::Forward(void)
{
    CGraphicsProfile* p_go = dynamic_cast<CGraphicsProfile*>(GetProject()->FindObject(GraphicsProfileIndex));
    if(p_go == NULL) return;
    CGraphicsObject* p_obj = dynamic_cast<CGraphicsObject*>(GetProject()->FindObject(GraphicsObjectIndex));
    if(p_obj == NULL) return;
    p_go->MoveObject(p_obj,EMD_UP);
}

//------------------------------------------------------------------------------

void CGraphicsProfileMoveObjectHI::Backward(void)
{
    CGraphicsProfile* p_go = dynamic_cast<CGraphicsProfile*>(GetProject()->FindObject(GraphicsProfileIndex));
    if(p_go == NULL) return;
    CGraphicsObject* p_obj = dynamic_cast<CGraphicsObject*>(GetProject()->FindObject(GraphicsObjectIndex));
    if(p_obj == NULL) return;
    p_go->MoveObject(p_obj,EMD_DOWN);
}

//------------------------------------------------------------------------------

void CGraphicsProfileMoveObjectHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("pi",GraphicsProfileIndex);
    p_ele->GetAttribute("oi",GraphicsObjectIndex);
}

//------------------------------------------------------------------------------

void CGraphicsProfileMoveObjectHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("pi",GraphicsProfileIndex);
    p_ele->SetAttribute("oi",GraphicsObjectIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
