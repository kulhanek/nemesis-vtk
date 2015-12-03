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

#include <BoxObjectHistory.hpp>
#include <BoxObject.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,BoxObjectChangePositionHI,
                        "{GFX_BOX_POS:58e4a67e-30fa-46f4-b99f-217ff3fb20c3}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,BoxObjectChangePositionObjHI,
                        "{GFX_BOX_POS_OBJ:be0a748d-1e2b-41b4-82b0-bfbb910f75fe}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,BoxObjectChangeDirectionHI,
                        "{GFX_BOX_DIR:b850de5d-22fb-410c-9105-f742eca0f4e8}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,BoxObjectChangeDirectionObjHI,
                        "{GFX_BOX_DIR_OBJ:16433492-6330-43f2-bed3-57c08e296d54}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,BoxObjectChangeDimensionsHI,
                        "{GFX_BOX_DIMM:50b1229a-28c6-4c96-99a4-71eb439c23fc}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,BoxObjectChangeRotationHI,
                        "{GFX_BOX_ROT:dfbba04f-509e-4077-9361-93437f1c3926}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,BoxObjectBackupGeoHI,
                        "{GFX_BOX_GEO:692e4461-9e85-4a58-afb0-ec428bb4762b}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBoxObjectChangePositionHI::CBoxObjectChangePositionHI(
                        CBoxObject* p_obj, const CPoint& newPosition)
    : CHistoryItem(&BoxObjectChangePositionHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldPosition = p_obj->GetPosition();
    NewPosition = newPosition;
}

//------------------------------------------------------------------------------

void CBoxObjectChangePositionHI::Forward(void)
{
    CBoxObject* p_go = dynamic_cast<CBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetPosition(NewPosition);
}

//------------------------------------------------------------------------------

void CBoxObjectChangePositionHI::Backward(void)
{
    CBoxObject* p_go = dynamic_cast<CBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetPosition(OldPosition);
}

//------------------------------------------------------------------------------

void CBoxObjectChangePositionHI::LoadData(CXMLElement* p_ele)
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

void CBoxObjectChangePositionHI::SaveData(CXMLElement* p_ele)
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

CBoxObjectChangePositionObjHI::CBoxObjectChangePositionObjHI(
                        CBoxObject* p_obj,CProObject* p_newobj)
    : CHistoryItem(&BoxObjectChangePositionObjHIObject,p_obj->GetProject(),EHID_FORWARD)
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

void CBoxObjectChangePositionObjHI::Forward(void)
{
    CBoxObject* p_go = dynamic_cast<CBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    CProObject* p_pro = GetProject()->FindObject(NewPositionObject);
    p_go->SetPosition(p_pro);
}

//------------------------------------------------------------------------------

void CBoxObjectChangePositionObjHI::Backward(void)
{
    CBoxObject* p_go = dynamic_cast<CBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    CProObject* p_pro = GetProject()->FindObject(OldPositionObject);
    p_go->SetPosition(p_pro);
}

//------------------------------------------------------------------------------

void CBoxObjectChangePositionObjHI::LoadData(CXMLElement* p_ele)
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

void CBoxObjectChangePositionObjHI::SaveData(CXMLElement* p_ele)
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

CBoxObjectChangeDimensionsHI::CBoxObjectChangeDimensionsHI(
                        CBoxObject* p_obj, const CPoint& newDimensions)
    : CHistoryItem(&BoxObjectChangeDimensionsHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldDimensions = p_obj->GetDimensions();
    NewDimensions = newDimensions;
}

//------------------------------------------------------------------------------

void CBoxObjectChangeDimensionsHI::Forward(void)
{
    CBoxObject* p_go = dynamic_cast<CBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetDimensions(NewDimensions);
}

//------------------------------------------------------------------------------

void CBoxObjectChangeDimensionsHI::Backward(void)
{
    CBoxObject* p_go = dynamic_cast<CBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetDimensions(OldDimensions);
}

//------------------------------------------------------------------------------

void CBoxObjectChangeDimensionsHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("ndx",NewDimensions.x);
    p_ele->GetAttribute("ndy",NewDimensions.y);
    p_ele->GetAttribute("ndz",NewDimensions.z);
    p_ele->GetAttribute("odx",OldDimensions.x);
    p_ele->GetAttribute("ody",OldDimensions.y);
    p_ele->GetAttribute("odz",OldDimensions.z);
}

//------------------------------------------------------------------------------

void CBoxObjectChangeDimensionsHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("ndx",NewDimensions.x);
    p_ele->SetAttribute("ndy",NewDimensions.y);
    p_ele->SetAttribute("ndz",NewDimensions.z);
    p_ele->SetAttribute("odx",OldDimensions.x);
    p_ele->SetAttribute("ody",OldDimensions.y);
    p_ele->SetAttribute("odz",OldDimensions.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBoxObjectChangeDirectionHI::CBoxObjectChangeDirectionHI(
                        CBoxObject* p_obj, const CPoint& newDirection)
    : CHistoryItem(&BoxObjectChangeDirectionHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldDirection = p_obj->GetDirection();
    NewDirection = newDirection;
}

//------------------------------------------------------------------------------

void CBoxObjectChangeDirectionHI::Forward(void)
{
    CBoxObject* p_go = dynamic_cast<CBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetDirection(NewDirection);
}

//------------------------------------------------------------------------------

void CBoxObjectChangeDirectionHI::Backward(void)
{
    CBoxObject* p_go = dynamic_cast<CBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetDirection(OldDirection);
}

//------------------------------------------------------------------------------

void CBoxObjectChangeDirectionHI::LoadData(CXMLElement* p_ele)
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

void CBoxObjectChangeDirectionHI::SaveData(CXMLElement* p_ele)
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

CBoxObjectChangeDirectionObjHI::CBoxObjectChangeDirectionObjHI(
                        CBoxObject* p_obj,CProObject* p_newobj)
    : CHistoryItem(&BoxObjectChangeDirectionObjHIObject,p_obj->GetProject(),EHID_FORWARD)
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

void CBoxObjectChangeDirectionObjHI::Forward(void)
{
    CBoxObject* p_go = dynamic_cast<CBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    CProObject* p_pro = GetProject()->FindObject(NewDirectionObject);
    p_go->SetDirection(p_pro);
}

//------------------------------------------------------------------------------

void CBoxObjectChangeDirectionObjHI::Backward(void)
{
    CBoxObject* p_go = dynamic_cast<CBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    CProObject* p_pro = GetProject()->FindObject(OldDirectionObject);
    p_go->SetDirection(p_pro);
}

//------------------------------------------------------------------------------

void CBoxObjectChangeDirectionObjHI::LoadData(CXMLElement* p_ele)
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

void CBoxObjectChangeDirectionObjHI::SaveData(CXMLElement* p_ele)
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

CBoxObjectChangeRotationHI::CBoxObjectChangeRotationHI(
                        CBoxObject* p_obj, double newRotation)
    : CHistoryItem(&BoxObjectChangeRotationHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldRotation = p_obj->GetRotation();
    NewRotation = newRotation;
}

//------------------------------------------------------------------------------

void CBoxObjectChangeRotationHI::Forward(void)
{
    CBoxObject* p_go = dynamic_cast<CBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetRotation(NewRotation);
}

//------------------------------------------------------------------------------

void CBoxObjectChangeRotationHI::Backward(void)
{
    CBoxObject* p_go = dynamic_cast<CBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetRotation(OldRotation);
}

//------------------------------------------------------------------------------

void CBoxObjectChangeRotationHI::LoadData(CXMLElement* p_ele)
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

void CBoxObjectChangeRotationHI::SaveData(CXMLElement* p_ele)
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


CBoxObjectBackupGeoHI::CBoxObjectBackupGeoHI(CBoxObject* p_obj)
    : CHistoryItem(&BoxObjectBackupGeoHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    Position = p_obj->GetPosition();
    Dimensions = p_obj->GetDimensions();
}

//------------------------------------------------------------------------------

void CBoxObjectBackupGeoHI::Forward(void)
{
    CBoxObject* p_go = dynamic_cast<CBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;

    CPoint pos = p_go->GetPosition();
    p_go->SetPosition(Position);
    Position = pos;

    CPoint ori = p_go->GetDimensions();
    p_go->SetDimensions(Dimensions);
    Dimensions = ori;
}

//------------------------------------------------------------------------------

void CBoxObjectBackupGeoHI::Backward(void)
{
    Forward();
}

//------------------------------------------------------------------------------

void CBoxObjectBackupGeoHI::LoadData(CXMLElement* p_ele)
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
    p_ele->GetAttribute("dx",Dimensions.x);
    p_ele->GetAttribute("dy",Dimensions.y);
    p_ele->GetAttribute("dz",Dimensions.z);
}

//------------------------------------------------------------------------------

void CBoxObjectBackupGeoHI::SaveData(CXMLElement* p_ele)
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
    p_ele->SetAttribute("dx",Dimensions.x);
    p_ele->SetAttribute("dy",Dimensions.y);
    p_ele->SetAttribute("dz",Dimensions.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


