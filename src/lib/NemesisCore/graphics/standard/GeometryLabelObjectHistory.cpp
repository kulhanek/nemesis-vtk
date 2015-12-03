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

#include <GeometryLabelObjectHistory.hpp>
#include <GeometryLabelObject.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>
#include <XMLElement.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,GeometryLabelObjectChangeTypeHI,
                        "{GFX_GLAB_TYP:32d72e0a-412a-4cff-b310-43fe80e07f54}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,GeometryLabelObjectChangeOffsetHI,
                        "{GFX_GLAB_OFF:8445c0ea-b419-4a96-b12d-08af34dbaa8e}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,GeometryLabelObjectBackupGeoHI,
                        "{GFX_GLAB_GEO:860ea4e7-aab0-42f1-b610-11f607350b47}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGeometryLabelObjectChangeTypeHI::CGeometryLabelObjectChangeTypeHI(
                        CGeometryLabelObject* p_obj,EGeometryLabelType newtype)
    : CHistoryItem(&GeometryLabelObjectChangeTypeHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldType = p_obj->GetType();
    NewType = newtype;
}

//------------------------------------------------------------------------------

void CGeometryLabelObjectChangeTypeHI::Forward(void)
{
    CGeometryLabelObject* p_go = dynamic_cast<CGeometryLabelObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetType(NewType);
}

//------------------------------------------------------------------------------

void CGeometryLabelObjectChangeTypeHI::Backward(void)
{
    CGeometryLabelObject* p_go = dynamic_cast<CGeometryLabelObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetType(OldType);
}

//------------------------------------------------------------------------------

void CGeometryLabelObjectChangeTypeHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("nt",NewType);
    p_ele->GetAttribute("ot",OldType);
}

//------------------------------------------------------------------------------

void CGeometryLabelObjectChangeTypeHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("nt",NewType);
    p_ele->SetAttribute("ot",OldType);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGeometryLabelObjectChangeOffsetHI::CGeometryLabelObjectChangeOffsetHI(
                        CGeometryLabelObject* p_obj,const CPoint& newoffset)
    : CHistoryItem(&GeometryLabelObjectChangeOffsetHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldOffset = p_obj->GetOffset();
    NewOffset = newoffset;
}

//------------------------------------------------------------------------------

void CGeometryLabelObjectChangeOffsetHI::Forward(void)
{
    CGeometryLabelObject* p_go = dynamic_cast<CGeometryLabelObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetOffset(NewOffset);
}

//------------------------------------------------------------------------------

void CGeometryLabelObjectChangeOffsetHI::Backward(void)
{
    CGeometryLabelObject* p_go = dynamic_cast<CGeometryLabelObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetOffset(OldOffset);
}
//------------------------------------------------------------------------------

void CGeometryLabelObjectChangeOffsetHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("nox",NewOffset.x);
    p_ele->GetAttribute("noy",NewOffset.y);
    p_ele->GetAttribute("noz",NewOffset.z);
    p_ele->GetAttribute("oox",OldOffset.x);
    p_ele->GetAttribute("ooy",OldOffset.y);
    p_ele->GetAttribute("ooz",OldOffset.z);
}

//------------------------------------------------------------------------------

void CGeometryLabelObjectChangeOffsetHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("nox",NewOffset.x);
    p_ele->SetAttribute("noy",NewOffset.y);
    p_ele->SetAttribute("noz",NewOffset.z);
    p_ele->SetAttribute("oox",OldOffset.x);
    p_ele->SetAttribute("ooy",OldOffset.y);
    p_ele->SetAttribute("ooz",OldOffset.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGeometryLabelObjectBackupGeoHI::CGeometryLabelObjectBackupGeoHI(CGeometryLabelObject* p_obj)
    : CHistoryItem(&GeometryLabelObjectBackupGeoHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    Offset = p_obj->GetOffset();
}

//------------------------------------------------------------------------------

void CGeometryLabelObjectBackupGeoHI::Forward(void)
{
    CGeometryLabelObject* p_go = dynamic_cast<CGeometryLabelObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;

    CPoint pos = p_go->GetOffset();
    p_go->SetOffset(Offset);
    Offset = pos;
}

//------------------------------------------------------------------------------

void CGeometryLabelObjectBackupGeoHI::Backward(void)
{
    Forward();
}

//------------------------------------------------------------------------------

void CGeometryLabelObjectBackupGeoHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("ox",Offset.x);
    p_ele->GetAttribute("oy",Offset.y);
    p_ele->GetAttribute("oz",Offset.z);
}

//------------------------------------------------------------------------------

void CGeometryLabelObjectBackupGeoHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("ox",Offset.x);
    p_ele->SetAttribute("oy",Offset.y);
    p_ele->SetAttribute("oz",Offset.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


