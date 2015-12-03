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

#include <PlaneObjectHistory.hpp>
#include <PlaneObject.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,PlaneObjectChangePositionHI,
                        "{GFX_PLANE_POS:4798c398-f322-4fa8-979c-e4a88eda6da0}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,PlaneObjectChangeDirectionHI,
                        "{GFX_PLANE_DIR:b7e52c21-94e2-458b-a9f4-265edf43c878}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,PlaneObjectChangeRotationHI,
                        "{GFX_PLANE_ROT:420e0a0c-c1f8-4d5d-b9fa-9d70d19ec3b1}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,PlaneObjectChangeSizeHI,
                        "{GFX_PLANE_SIZ:84898350-7056-41b5-81c2-cc932d9aea6c}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,PlaneObjectChangeClippingIDHI,
                        "{GFX_PLANE_CID:f1bed0ff-b257-45e4-8020-24ce6b680c0a}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,PlaneObjectChangePositionObjHI,
                        "{GFX_PLANE_POSO:ad955d51-a138-4526-81c1-100139789afd}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,PlaneObjectChangeDirectionObjHI,
                        "{GFX_PLANE_DIRO:d3bc908a-3b53-4d6a-a814-17d67ae73fc8}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,PlaneObjectBackupGeoHI,
                        "{GFX_PLANE_BACK:0ceb52f9-968d-430a-a9cf-7f118f4037c9}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPlaneObjectChangePositionHI::CPlaneObjectChangePositionHI(
                        CPlaneObject* p_obj, const CPoint& newPosition)
    : CHistoryItem(&PlaneObjectChangePositionHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldPosition = p_obj->GetPosition();
    NewPosition = newPosition;
}

//------------------------------------------------------------------------------

void CPlaneObjectChangePositionHI::Forward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetPosition(NewPosition);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangePositionHI::Backward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetPosition(OldPosition);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangePositionHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("npx",NewPosition.x);
    p_ele->GetAttribute("npy",NewPosition.y);
    p_ele->GetAttribute("npz",NewPosition.z);
    p_ele->GetAttribute("opx",OldPosition.x);
    p_ele->GetAttribute("opy",OldPosition.y);
    p_ele->GetAttribute("opz",OldPosition.z);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangePositionHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("npx",NewPosition.x);
    p_ele->SetAttribute("npy",NewPosition.y);
    p_ele->SetAttribute("npz",NewPosition.z);
    p_ele->SetAttribute("opx",OldPosition.x);
    p_ele->SetAttribute("opy",OldPosition.y);
    p_ele->SetAttribute("opz",OldPosition.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPlaneObjectChangeDirectionHI::CPlaneObjectChangeDirectionHI(
                        CPlaneObject* p_obj, const CPoint& newDirection)
    : CHistoryItem(&PlaneObjectChangeDirectionHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldDirection = p_obj->GetDirection();
    NewDirection = newDirection;
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeDirectionHI::Forward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetDirection(NewDirection);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeDirectionHI::Backward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetDirection(OldDirection);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeDirectionHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("ndx",NewDirection.x);
    p_ele->GetAttribute("ndy",NewDirection.y);
    p_ele->GetAttribute("ndz",NewDirection.z);
    p_ele->GetAttribute("odx",OldDirection.x);
    p_ele->GetAttribute("ody",OldDirection.y);
    p_ele->GetAttribute("odz",OldDirection.z);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeDirectionHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("ndx",NewDirection.x);
    p_ele->SetAttribute("ndy",NewDirection.y);
    p_ele->SetAttribute("ndz",NewDirection.z);
    p_ele->SetAttribute("odx",OldDirection.x);
    p_ele->SetAttribute("ody",OldDirection.y);
    p_ele->SetAttribute("odz",OldDirection.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPlaneObjectChangeRotationHI::CPlaneObjectChangeRotationHI(
                        CPlaneObject* p_obj, double newRotation)
    : CHistoryItem(&PlaneObjectChangeRotationHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldRotation = p_obj->GetRotation();
    NewRotation = newRotation;
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeRotationHI::Forward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetRotation(NewRotation);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeRotationHI::Backward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetRotation(OldRotation);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeRotationHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("nr",NewRotation);
    p_ele->GetAttribute("or",OldRotation);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeRotationHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("nr",NewRotation);
    p_ele->SetAttribute("or",OldRotation);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPlaneObjectChangeSizeHI::CPlaneObjectChangeSizeHI(
                        CPlaneObject* p_obj, double newSize)
    : CHistoryItem(&PlaneObjectChangeSizeHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldSize = p_obj->GetSize();
    NewSize = newSize;
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeSizeHI::Forward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetSize(NewSize);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeSizeHI::Backward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetSize(OldSize);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeSizeHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("ns",NewSize);
    p_ele->GetAttribute("os",OldSize);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeSizeHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("ns",NewSize);
    p_ele->SetAttribute("os",OldSize);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPlaneObjectChangeClippingIDHI::CPlaneObjectChangeClippingIDHI(CPlaneObject* p_obj, int newID)
    : CHistoryItem(&PlaneObjectChangeClippingIDHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldID = p_obj->GetClippingPlaneID();
    NewID = newID;
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeClippingIDHI::Forward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetClippingPlaneID(NewID);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeClippingIDHI::Backward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetClippingPlaneID(OldID);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeClippingIDHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("ni",NewID);
    p_ele->GetAttribute("oi",OldID);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeClippingIDHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("ni",NewID);
    p_ele->SetAttribute("oi",OldID);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPlaneObjectChangePositionObjHI::CPlaneObjectChangePositionObjHI(
                        CPlaneObject* p_obj,CProObject* p_newobj)
    : CHistoryItem(&PlaneObjectChangePositionObjHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();

    if( p_obj->GetPositionObject() ){
        OldPositionObject = p_obj->GetPositionObject()->GetIndex();
    } else {
        OldPositionObject = -1;
    }

    if( p_newobj ){
        NewPositionObject = p_newobj->GetIndex();
    } else {
        NewPositionObject = -1;
    }
}

//------------------------------------------------------------------------------

void CPlaneObjectChangePositionObjHI::Forward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    CProObject* p_pro = GetProject()->FindObject(NewPositionObject);
    p_go->SetPosition(p_pro);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangePositionObjHI::Backward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    CProObject* p_pro = GetProject()->FindObject(OldPositionObject);
    p_go->SetPosition(p_pro);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangePositionObjHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("npo",NewPositionObject);
    p_ele->GetAttribute("opo",OldPositionObject);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangePositionObjHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("npo",NewPositionObject);
    p_ele->SetAttribute("opo",OldPositionObject);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPlaneObjectChangeDirectionObjHI::CPlaneObjectChangeDirectionObjHI(
                        CPlaneObject* p_obj,CProObject* p_newobj)
    : CHistoryItem(&PlaneObjectChangeDirectionObjHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();

    if( p_obj->GetDirectionObject() ){
        OldDirectionObject = p_obj->GetDirectionObject()->GetIndex();
    } else {
        OldDirectionObject = -1;
    }

    if( p_newobj ){
        NewDirectionObject = p_newobj->GetIndex();
    } else {
        NewDirectionObject = -1;
    }
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeDirectionObjHI::Forward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    CProObject* p_pro = GetProject()->FindObject(NewDirectionObject);
    p_go->SetDirection(p_pro);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeDirectionObjHI::Backward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    CProObject* p_pro = GetProject()->FindObject(OldDirectionObject);
    p_go->SetDirection(p_pro);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeDirectionObjHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("ndo",NewDirectionObject);
    p_ele->GetAttribute("odo",OldDirectionObject);
}

//------------------------------------------------------------------------------

void CPlaneObjectChangeDirectionObjHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("ndo",NewDirectionObject);
    p_ele->SetAttribute("odo",OldDirectionObject);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPlaneObjectBackupGeoHI::CPlaneObjectBackupGeoHI(CPlaneObject* p_obj)
    : CHistoryItem(&PlaneObjectBackupGeoHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    Position = p_obj->GetPosition();
    Direction = p_obj->GetDirection();
    Rotation = p_obj->GetRotation();
    Size = p_obj->GetSize();
}

//------------------------------------------------------------------------------

void CPlaneObjectBackupGeoHI::Forward(void)
{
    CPlaneObject* p_go = dynamic_cast<CPlaneObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;

    CPoint pos = p_go->GetPosition();
    p_go->SetPosition(Position);
    Position = pos;

    CPoint ori = p_go->GetDirection();
    p_go->SetDirection(Direction);
    Direction = ori;

    double rot = p_go->GetRotation();
    p_go->SetRotation(Rotation);
    Rotation = rot;

    double siz = p_go->GetSize();
    p_go->SetSize(Size);
    Size = siz;
}

//------------------------------------------------------------------------------

void CPlaneObjectBackupGeoHI::Backward(void)
{
    Forward();
}

//------------------------------------------------------------------------------

void CPlaneObjectBackupGeoHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("px",Position.x);
    p_ele->GetAttribute("py",Position.y);
    p_ele->GetAttribute("pz",Position.z);
    p_ele->GetAttribute("ox",Direction.x);
    p_ele->GetAttribute("oy",Direction.y);
    p_ele->GetAttribute("oz",Direction.z);
    p_ele->GetAttribute("rt",Rotation);
    p_ele->GetAttribute("sz",Size);
}

//------------------------------------------------------------------------------

void CPlaneObjectBackupGeoHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("px",Position.x);
    p_ele->SetAttribute("py",Position.y);
    p_ele->SetAttribute("pz",Position.z);
    p_ele->SetAttribute("ox",Direction.x);
    p_ele->SetAttribute("oy",Direction.y);
    p_ele->SetAttribute("oz",Direction.z);
    p_ele->SetAttribute("rt",Rotation);
    p_ele->SetAttribute("sz",Size);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


