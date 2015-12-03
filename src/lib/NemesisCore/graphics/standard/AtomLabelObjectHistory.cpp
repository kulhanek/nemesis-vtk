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

#include <AtomLabelObjectHistory.hpp>
#include <AtomLabelObject.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>
#include <XMLElement.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,AtomLabelObjectChangeFormatHI,
                        "{GFX_ALAB_FMT:1250ee46-820d-428d-b1c7-af8f7b744089}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,AtomLabelObjectChangeOffsetHI,
                        "{GFX_ALAB_OFF:5c27e977-1e2b-487e-9a95-74ce89fc7b5f}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,AtomLabelObjectBackupGeoHI,
                        "{GFX_ALAB_GEO:f1d6d9b4-3ed5-43ba-a701-41c670b86606}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomLabelObjectChangeFormatHI::CAtomLabelObjectChangeFormatHI(
                        CAtomLabelObject* p_obj,const QString& newformat)
    : CHistoryItem(&AtomLabelObjectChangeFormatHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldFormat = p_obj->GetCustomFormat();
    NewFormat = newformat;
}

//------------------------------------------------------------------------------

void CAtomLabelObjectChangeFormatHI::Forward(void)
{
    CAtomLabelObject* p_go = GetProject()->FindObject<CAtomLabelObject*>(ObjectID);
    if(p_go == NULL) return;
    p_go->SetCustomFormat(NewFormat);
}

//------------------------------------------------------------------------------

void CAtomLabelObjectChangeFormatHI::Backward(void)
{
    CAtomLabelObject* p_go = GetProject()->FindObject<CAtomLabelObject*>(ObjectID);
    if(p_go == NULL) return;
    p_go->SetCustomFormat(OldFormat);
}

//------------------------------------------------------------------------------

void CAtomLabelObjectChangeFormatHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("nf",NewFormat);
    p_ele->GetAttribute("of",OldFormat);
}

//------------------------------------------------------------------------------

void CAtomLabelObjectChangeFormatHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("nf",NewFormat);
    p_ele->SetAttribute("of",OldFormat);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomLabelObjectChangeOffsetHI::CAtomLabelObjectChangeOffsetHI(
                        CAtomLabelObject* p_obj,const CPoint& newoffset)
    : CHistoryItem(&AtomLabelObjectChangeOffsetHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldOffset = p_obj->GetOffset();
    NewOffset = newoffset;
}

//------------------------------------------------------------------------------

void CAtomLabelObjectChangeOffsetHI::Forward(void)
{
    CAtomLabelObject* p_go = GetProject()->FindObject<CAtomLabelObject*>(ObjectID);
    if(p_go == NULL) return;
    p_go->SetOffset(NewOffset);
}

//------------------------------------------------------------------------------

void CAtomLabelObjectChangeOffsetHI::Backward(void)
{
    CAtomLabelObject* p_go = GetProject()->FindObject<CAtomLabelObject*>(ObjectID);
    if(p_go == NULL) return;
    p_go->SetOffset(OldOffset);
}
//------------------------------------------------------------------------------

void CAtomLabelObjectChangeOffsetHI::LoadData(CXMLElement* p_ele)
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

void CAtomLabelObjectChangeOffsetHI::SaveData(CXMLElement* p_ele)
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

CAtomLabelObjectBackupGeoHI::CAtomLabelObjectBackupGeoHI(CAtomLabelObject* p_obj)
    : CHistoryItem(&AtomLabelObjectBackupGeoHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    Offset = p_obj->GetOffset();
}

//------------------------------------------------------------------------------

void CAtomLabelObjectBackupGeoHI::Forward(void)
{
    CAtomLabelObject* p_go = GetProject()->FindObject<CAtomLabelObject*>(ObjectID);
    if(p_go == NULL) return;

    CPoint pos = p_go->GetOffset();
    p_go->SetOffset(Offset);
    Offset = pos;
}

//------------------------------------------------------------------------------

void CAtomLabelObjectBackupGeoHI::Backward(void)
{
    Forward();
}

//------------------------------------------------------------------------------

void CAtomLabelObjectBackupGeoHI::LoadData(CXMLElement* p_ele)
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

void CAtomLabelObjectBackupGeoHI::SaveData(CXMLElement* p_ele)
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


