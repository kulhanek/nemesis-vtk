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

#include <GraphicsObjectHistory.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <Graphics.hpp>
#include <GraphicsObject.hpp>
#include <ProObjectHistory.hpp>
#include <ErrorSystem.hpp>
#include <GraphicsObjectList.hpp>
#include <XMLElement.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,GraphicsObjectHI,
                        "{GO:14da0ee4-1ad8-4363-9c68-a90048db898e}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,GraphicsObjectAddProObjectHI,
                        "{GO_AO:0aed4ff5-884b-4896-883b-a153bf4f2c91}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsObjectHI::CGraphicsObjectHI(CGraphicsObject* p_go,EHistoryItemDirection change)
    : CHistoryItem(&GraphicsObjectHIObject,p_go->GetProject(),change)
{
    CXMLElement* p_ele = GraphicsObjectData.CreateChildElement("object");
    p_go->SaveData(p_ele);
}

//------------------------------------------------------------------------------

void CGraphicsObjectHI::Forward(void)
{
    CXMLElement* p_ele = GraphicsObjectData.GetFirstChildElement("object");
    GetProject()->GetGraphics()->GetObjects()->CreateObject(p_ele);
}

//------------------------------------------------------------------------------

void CGraphicsObjectHI::Backward(void)
{
    CXMLElement* p_ele = GraphicsObjectData.GetFirstChildElement("object");
    if( p_ele == NULL ) return;
    int go_index = -1;
    p_ele->GetAttribute("id",go_index);

    CGraphicsObject* p_go = dynamic_cast<CGraphicsObject*>(GetProject()->FindObject(go_index));
    if(p_go == NULL) return;

    // save designer setup
    CXMLElement* p_sdele = p_go->GetDesignerData();
    if( p_sdele ){
        CXMLElement* p_tdele = GraphicsObjectData.GetChildElementByPath("object/designer",true);
        p_tdele->RemoveAllChildNodes();
        p_tdele->RemoveAllAttributes();
        p_tdele->CopyContentsFrom(p_sdele);
    }

    p_go->RemoveFromBaseList();
}

//------------------------------------------------------------------------------

void CGraphicsObjectHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    CXMLElement* p_sele = p_ele->GetFirstChildElement("object");
    if( p_sele == NULL ) return;
    GraphicsObjectData.RemoveAllChildNodes();
    CXMLElement* p_dele = GraphicsObjectData.CreateChildElement("object");
    p_dele->CopyContentsFrom(p_sele);
}

//------------------------------------------------------------------------------

void CGraphicsObjectHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    CXMLElement* p_sele = GraphicsObjectData.GetFirstChildElement("object");
    if( p_sele == NULL ) return;
    CXMLElement* p_dele = p_ele->CreateChildElement("object");
    p_dele->CopyContentsFrom(p_sele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsObjectAddProObjectHI::CGraphicsObjectAddProObjectHI(CGraphicsObject* p_go,CProObject* p_po,
                                                           EHistoryItemDirection change)
    : CHistoryItem(&GraphicsObjectAddProObjectHIObject,p_go->GetProject(),change)
{
    GraphicsObjectIndex = p_go->GetIndex();
    ProObjectIndex = p_po->GetIndex();
}

//------------------------------------------------------------------------------

void CGraphicsObjectAddProObjectHI::Forward(void)
{
    CGraphicsObject* p_go = dynamic_cast<CGraphicsObject*>(GetProject()->FindObject(GraphicsObjectIndex));
    if(p_go == NULL) return;
    CProObject* p_po = dynamic_cast<CProObject*>(GetProject()->FindObject(ProObjectIndex));
    if(p_po == NULL) return;
    p_go->AddObject(p_po);
}

//------------------------------------------------------------------------------

void CGraphicsObjectAddProObjectHI::Backward(void)
{
    CGraphicsObject* p_go = dynamic_cast<CGraphicsObject*>(GetProject()->FindObject(GraphicsObjectIndex));
    if(p_go == NULL) return;
    CProObject* p_po = dynamic_cast<CProObject*>(GetProject()->FindObject(ProObjectIndex));
    if(p_po == NULL) return;
    p_go->RemoveObject(p_po);
}

//------------------------------------------------------------------------------

void CGraphicsObjectAddProObjectHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",GraphicsObjectIndex);
    p_ele->GetAttribute("pi",ProObjectIndex);
}

//------------------------------------------------------------------------------

void CGraphicsObjectAddProObjectHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",GraphicsObjectIndex);
    p_ele->SetAttribute("pi",ProObjectIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
