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

#include <TextObjectHistory.hpp>
#include <TextObject.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>
#include <XMLElement.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,TextObjectChangeTextHI,
                        "{GFX_TEXT_TXT:23c4838c-bbe0-440b-ac81-1dc2413383b6}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,TextObjectChangePositionHI,
                        "{GFX_TEXT_POS:0bb157bd-2cad-42e9-aadd-0c217e3c0e2f}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,TextObjectChangeOrientationHI,
                        "{GFX_TEXT_DIR:61aba7d6-b105-4d86-9bf1-0bd83fbfa9b6}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,TextObjectBackupGeoHI,
                        "{GFX_TEXT_BACK:61152463-bcc5-4ef7-a517-aa9c38c08e48}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTextObjectChangeTextHI::CTextObjectChangeTextHI(
                        CTextObject* p_obj,const QString& newtext)
    : CHistoryItem(&TextObjectChangeTextHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldText = p_obj->GetText();
    NewText = newtext;
}

//------------------------------------------------------------------------------

void CTextObjectChangeTextHI::Forward(void)
{
    CTextObject* p_go = dynamic_cast<CTextObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetText(NewText);
}

//------------------------------------------------------------------------------

void CTextObjectChangeTextHI::Backward(void)
{
    CTextObject* p_go = dynamic_cast<CTextObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetText(OldText);
}

//------------------------------------------------------------------------------

void CTextObjectChangeTextHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("nt",NewText);
    p_ele->GetAttribute("ot",OldText);
}

//------------------------------------------------------------------------------

void CTextObjectChangeTextHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("nt",NewText);
    p_ele->SetAttribute("ot",OldText);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTextObjectChangePositionHI::CTextObjectChangePositionHI(
                        CTextObject* p_obj,const CPoint& newpos)
    : CHistoryItem(&TextObjectChangePositionHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldPosition = p_obj->GetPosition();
    NewPosition = newpos;
}

//------------------------------------------------------------------------------

void CTextObjectChangePositionHI::Forward(void)
{
    CTextObject* p_go = dynamic_cast<CTextObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetPosition(NewPosition);
}

//------------------------------------------------------------------------------

void CTextObjectChangePositionHI::Backward(void)
{
    CTextObject* p_go = dynamic_cast<CTextObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetPosition(OldPosition);
}

//------------------------------------------------------------------------------

void CTextObjectChangePositionHI::LoadData(CXMLElement* p_ele)
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

void CTextObjectChangePositionHI::SaveData(CXMLElement* p_ele)
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

CTextObjectChangeOrientationHI::CTextObjectChangeOrientationHI(
                        CTextObject* p_obj,const CPoint& newpos)
    : CHistoryItem(&TextObjectChangeOrientationHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldOrientation = p_obj->GetOrientation();
    NewOrientation = newpos;
}

//------------------------------------------------------------------------------

void CTextObjectChangeOrientationHI::Forward(void)
{
    CTextObject* p_go = dynamic_cast<CTextObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetOrientation(NewOrientation);
}

//------------------------------------------------------------------------------

void CTextObjectChangeOrientationHI::Backward(void)
{
    CTextObject* p_go = dynamic_cast<CTextObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetOrientation(OldOrientation);
}

//------------------------------------------------------------------------------

void CTextObjectChangeOrientationHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("nox",NewOrientation.x);
    p_ele->GetAttribute("noy",NewOrientation.y);
    p_ele->GetAttribute("noz",NewOrientation.z);
    p_ele->GetAttribute("oox",OldOrientation.x);
    p_ele->GetAttribute("ooy",OldOrientation.y);
    p_ele->GetAttribute("ooz",OldOrientation.z);
}

//------------------------------------------------------------------------------

void CTextObjectChangeOrientationHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("nox",NewOrientation.x);
    p_ele->SetAttribute("noy",NewOrientation.y);
    p_ele->SetAttribute("noz",NewOrientation.z);
    p_ele->SetAttribute("oox",OldOrientation.x);
    p_ele->SetAttribute("ooy",OldOrientation.y);
    p_ele->SetAttribute("ooz",OldOrientation.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTextObjectBackupGeoHI::CTextObjectBackupGeoHI(CTextObject* p_obj)
    : CHistoryItem(&TextObjectBackupGeoHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    Position = p_obj->GetPosition();
    Orientation = p_obj->GetOrientation();
}

//------------------------------------------------------------------------------

void CTextObjectBackupGeoHI::Forward(void)
{
    CTextObject* p_go = dynamic_cast<CTextObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;

    CPoint pos = p_go->GetPosition();
    p_go->SetPosition(Position);
    Position = pos;

    CPoint ori = p_go->GetPosition();
    p_go->SetOrientation(Orientation);
    Orientation = ori;
}

//------------------------------------------------------------------------------

void CTextObjectBackupGeoHI::Backward(void)
{
    Forward();
}

//------------------------------------------------------------------------------

void CTextObjectBackupGeoHI::LoadData(CXMLElement* p_ele)
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
    p_ele->GetAttribute("ox",Orientation.x);
    p_ele->GetAttribute("oy",Orientation.y);
    p_ele->GetAttribute("oz",Orientation.z);
}

//------------------------------------------------------------------------------

void CTextObjectBackupGeoHI::SaveData(CXMLElement* p_ele)
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
    p_ele->SetAttribute("ox",Orientation.x);
    p_ele->SetAttribute("oy",Orientation.y);
    p_ele->SetAttribute("oz",Orientation.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
