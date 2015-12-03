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

#include <VectorObjectHistory.hpp>
#include <VectorObject.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,VectorObjectChangePositionHI,
                        "{GFX_VEC_POS:f251d099-b17c-4a21-8de8-fcec10eb2034}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,VectorObjectChangeDirectionHI,
                        "{GFX_VEC_DIR:6efb2928-8cf3-4412-9efd-2258d7fada5c}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,VectorObjectChangeOffsetHI,
                        "{GFX_VEC_OFF:341b4613-3e49-4aca-9a0d-3d29504e2d17}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,VectorObjectChangeLengthHI,
                        "{GFX_VEC_LEN:218fcf48-a07d-4b86-9e2b-00123ff06b87}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,VectorObjectChangePositionObjHI,
                        "{GFX_VEC_POSO:09616990-9010-47c1-8337-38ab4faec9c0}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,VectorObjectChangeDirectionObjHI,
                        "{GFX_VEC_DIRO:9f8f58c8-9493-455f-836b-d95af9453c2a}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,VectorObjectBackupGeoHI,
                        "{GFX_VEC_GEO:13b2a727-03e8-4241-b701-9687acabaeb0}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CVectorObjectChangePositionHI::CVectorObjectChangePositionHI(
                        CVectorObject* p_obj, const CPoint& newPosition)
    : CHistoryItem(&VectorObjectChangePositionHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldPosition = p_obj->GetPosition();
    NewPosition = newPosition;
}

//------------------------------------------------------------------------------

void CVectorObjectChangePositionHI::Forward(void)
{
    CVectorObject* p_go = dynamic_cast<CVectorObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetPosition(NewPosition);
}

//------------------------------------------------------------------------------

void CVectorObjectChangePositionHI::Backward(void)
{
    CVectorObject* p_go = dynamic_cast<CVectorObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetPosition(OldPosition);
}

//------------------------------------------------------------------------------

void CVectorObjectChangePositionHI::LoadData(CXMLElement* p_ele)
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

void CVectorObjectChangePositionHI::SaveData(CXMLElement* p_ele)
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

CVectorObjectChangeDirectionHI::CVectorObjectChangeDirectionHI(
                        CVectorObject* p_obj, const CPoint& newDirection)
    : CHistoryItem(&VectorObjectChangeDirectionHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldDirection = p_obj->GetDirection();
    NewDirection = newDirection;
}

//------------------------------------------------------------------------------

void CVectorObjectChangeDirectionHI::Forward(void)
{
    CVectorObject* p_go = dynamic_cast<CVectorObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetDirection(NewDirection);
}

//------------------------------------------------------------------------------

void CVectorObjectChangeDirectionHI::Backward(void)
{
    CVectorObject* p_go = dynamic_cast<CVectorObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetDirection(OldDirection);
}

//------------------------------------------------------------------------------

void CVectorObjectChangeDirectionHI::LoadData(CXMLElement* p_ele)
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

void CVectorObjectChangeDirectionHI::SaveData(CXMLElement* p_ele)
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

CVectorObjectChangeOffsetHI::CVectorObjectChangeOffsetHI(
                        CVectorObject* p_obj, double newOffset)
    : CHistoryItem(&VectorObjectChangeOffsetHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldOffset = p_obj->GetOffset();
    NewOffset = newOffset;
}

//------------------------------------------------------------------------------

void CVectorObjectChangeOffsetHI::Forward(void)
{
    CVectorObject* p_go = dynamic_cast<CVectorObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetOffset(NewOffset);
}

//------------------------------------------------------------------------------

void CVectorObjectChangeOffsetHI::Backward(void)
{
    CVectorObject* p_go = dynamic_cast<CVectorObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetOffset(OldOffset);
}

//------------------------------------------------------------------------------

void CVectorObjectChangeOffsetHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("no",NewOffset);
    p_ele->GetAttribute("oo",OldOffset);
}

//------------------------------------------------------------------------------

void CVectorObjectChangeOffsetHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("no",NewOffset);
    p_ele->SetAttribute("oo",OldOffset);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CVectorObjectChangeLengthHI::CVectorObjectChangeLengthHI(
                        CVectorObject* p_obj, double newLength)
    : CHistoryItem(&VectorObjectChangeLengthHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldLength = p_obj->GetLength();
    NewLength = newLength;
}

//------------------------------------------------------------------------------

void CVectorObjectChangeLengthHI::Forward(void)
{
    CVectorObject* p_go = dynamic_cast<CVectorObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetLength(NewLength);
}

//------------------------------------------------------------------------------

void CVectorObjectChangeLengthHI::Backward(void)
{
    CVectorObject* p_go = dynamic_cast<CVectorObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetLength(OldLength);
}

//------------------------------------------------------------------------------

void CVectorObjectChangeLengthHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("nl",NewLength);
    p_ele->GetAttribute("ol",OldLength);
}

//------------------------------------------------------------------------------

void CVectorObjectChangeLengthHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("nl",NewLength);
    p_ele->SetAttribute("ol",OldLength);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CVectorObjectChangePositionObjHI::CVectorObjectChangePositionObjHI(
                        CVectorObject* p_obj,CProObject* p_newobj)
    : CHistoryItem(&VectorObjectChangePositionObjHIObject,p_obj->GetProject(),EHID_FORWARD)
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

void CVectorObjectChangePositionObjHI::Forward(void)
{
    CVectorObject* p_go = dynamic_cast<CVectorObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    CProObject* p_pro = GetProject()->FindObject(NewPositionObject);
    p_go->SetPosition(p_pro);
}

//------------------------------------------------------------------------------

void CVectorObjectChangePositionObjHI::Backward(void)
{
    CVectorObject* p_go = dynamic_cast<CVectorObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    CProObject* p_pro = GetProject()->FindObject(OldPositionObject);
    p_go->SetPosition(p_pro);
}

//------------------------------------------------------------------------------

void CVectorObjectChangePositionObjHI::LoadData(CXMLElement* p_ele)
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

void CVectorObjectChangePositionObjHI::SaveData(CXMLElement* p_ele)
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

CVectorObjectChangeDirectionObjHI::CVectorObjectChangeDirectionObjHI(
                        CVectorObject* p_obj,CProObject* p_newobj)
    : CHistoryItem(&VectorObjectChangeDirectionObjHIObject,p_obj->GetProject(),EHID_FORWARD)
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

void CVectorObjectChangeDirectionObjHI::Forward(void)
{
    CVectorObject* p_go = dynamic_cast<CVectorObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    CProObject* p_pro = GetProject()->FindObject(NewDirectionObject);
    p_go->SetDirection(p_pro);
}

//------------------------------------------------------------------------------

void CVectorObjectChangeDirectionObjHI::Backward(void)
{
    CVectorObject* p_go = dynamic_cast<CVectorObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    CProObject* p_pro = GetProject()->FindObject(OldDirectionObject);
    p_go->SetDirection(p_pro);
}

//------------------------------------------------------------------------------

void CVectorObjectChangeDirectionObjHI::LoadData(CXMLElement* p_ele)
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

void CVectorObjectChangeDirectionObjHI::SaveData(CXMLElement* p_ele)
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

CVectorObjectBackupGeoHI::CVectorObjectBackupGeoHI(CVectorObject* p_obj)
    : CHistoryItem(&VectorObjectBackupGeoHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    Position = p_obj->GetPosition();
    Direction = p_obj->GetDirection();
    Offset = p_obj->GetOffset();
    Length = p_obj->GetLength();
}

//------------------------------------------------------------------------------

void CVectorObjectBackupGeoHI::Forward(void)
{
    CVectorObject* p_go = dynamic_cast<CVectorObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;

    CPoint pos = p_go->GetPosition();
    p_go->SetPosition(Position);
    Position = pos;

    CPoint ori = p_go->GetDirection();
    p_go->SetDirection(Direction);
    Direction = ori;

    double off = p_go->GetOffset();
    p_go->SetOffset(Offset);
    Offset = off;

    double len = p_go->GetLength();
    p_go->SetLength(len);
    Length = len;
}

//------------------------------------------------------------------------------

void CVectorObjectBackupGeoHI::Backward(void)
{
    Forward();
}

//------------------------------------------------------------------------------

void CVectorObjectBackupGeoHI::LoadData(CXMLElement* p_ele)
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
    p_ele->GetAttribute("of",Offset);
    p_ele->GetAttribute("le",Length);
}

//------------------------------------------------------------------------------

void CVectorObjectBackupGeoHI::SaveData(CXMLElement* p_ele)
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
    p_ele->SetAttribute("rt",Offset);
    p_ele->SetAttribute("le",Length);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


