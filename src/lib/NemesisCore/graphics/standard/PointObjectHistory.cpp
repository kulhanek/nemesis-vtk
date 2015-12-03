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

#include <PointObjectHistory.hpp>
#include <PointObject.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>
#include <XMLElement.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,PointObjectChangePosHI,
                        "{GFX_POINT_POS:59426dfd-c6d7-4238-bd74-61f906bf341b}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,PointObjectBackupGeoHI,
                        "{GFX_POINT_GEO:e594dd4c-4d2b-46db-ab28-1884e5c6a173}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPointObjectChangePosHI::CPointObjectChangePosHI(
                        CPointObject* p_obj, const CPoint& newpos)
    : CHistoryItem(&PointObjectChangePosHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldPos = p_obj->GetPosition();
    NewPos = newpos;
}

//------------------------------------------------------------------------------

void CPointObjectChangePosHI::Forward(void)
{
    CPointObject* p_go = dynamic_cast<CPointObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetPosition(NewPos);
}

//------------------------------------------------------------------------------

void CPointObjectChangePosHI::Backward(void)
{
    CPointObject* p_go = dynamic_cast<CPointObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetPosition(OldPos);
}

//------------------------------------------------------------------------------

void CPointObjectChangePosHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("npx",NewPos.x);
    p_ele->GetAttribute("npy",NewPos.y);
    p_ele->GetAttribute("npz",NewPos.z);
    p_ele->GetAttribute("opx",OldPos.x);
    p_ele->GetAttribute("opy",OldPos.y);
    p_ele->GetAttribute("opz",OldPos.z);
}

//------------------------------------------------------------------------------

void CPointObjectChangePosHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("npx",NewPos.x);
    p_ele->SetAttribute("npy",NewPos.y);
    p_ele->SetAttribute("npz",NewPos.z);
    p_ele->SetAttribute("opx",OldPos.x);
    p_ele->SetAttribute("opy",OldPos.y);
    p_ele->SetAttribute("opz",OldPos.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPointObjectBackupGeoHI::CPointObjectBackupGeoHI(CPointObject* p_obj)
    : CHistoryItem(&PointObjectBackupGeoHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    Point = p_obj->GetPosition();
}

//------------------------------------------------------------------------------

void CPointObjectBackupGeoHI::Forward(void)
{
    CPointObject* p_go = dynamic_cast<CPointObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;

    CPoint pos = p_go->GetPosition();
    p_go->SetPosition(Point);
    Point = pos;
}

//------------------------------------------------------------------------------

void CPointObjectBackupGeoHI::Backward(void)
{
    Forward();
}

//------------------------------------------------------------------------------

void CPointObjectBackupGeoHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("ox",Point.x);
    p_ele->GetAttribute("oy",Point.y);
    p_ele->GetAttribute("oz",Point.z);
}

//------------------------------------------------------------------------------

void CPointObjectBackupGeoHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("ox",Point.x);
    p_ele->SetAttribute("oy",Point.y);
    p_ele->SetAttribute("oz",Point.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


