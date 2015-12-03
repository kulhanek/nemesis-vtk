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

#include <LightObjectHistory.hpp>
#include <LightObject.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,LightObjectChangePositionHI,
                        "{GFX_LIGHT_POS:3f43c02f-a284-4b2b-a27c-1bc019cd9992}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,LightObjectChangeOrientationHI,
                        "{GFX_LIGHT_DIR:6de2a719-6f7d-4758-82f6-6a2d3dc7f008}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CLightObjectChangePositionHI::CLightObjectChangePositionHI(
                        CLightObject* p_obj,const CPoint& newpos)
    : CHistoryItem(&LightObjectChangePositionHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldPosition = p_obj->GetPosition();
    NewPosition = newpos;
}

//------------------------------------------------------------------------------

void CLightObjectChangePositionHI::Forward(void)
{
    CLightObject* p_go = dynamic_cast<CLightObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetPosition(NewPosition);
}

//------------------------------------------------------------------------------

void CLightObjectChangePositionHI::Backward(void)
{
    CLightObject* p_go = dynamic_cast<CLightObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetPosition(OldPosition);
}

//------------------------------------------------------------------------------

void CLightObjectChangePositionHI::LoadData(CXMLElement* p_ele)
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

void CLightObjectChangePositionHI::SaveData(CXMLElement* p_ele)
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

CLightObjectChangeOrientationHI::CLightObjectChangeOrientationHI(
                        CLightObject* p_obj,const CPoint& newpos)
    : CHistoryItem(&LightObjectChangeOrientationHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldOrientation = p_obj->GetOrientation();
    NewOrientation = newpos;
}

//------------------------------------------------------------------------------

void CLightObjectChangeOrientationHI::Forward(void)
{
    CLightObject* p_go = dynamic_cast<CLightObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetOrientation(NewOrientation);
}

//------------------------------------------------------------------------------

void CLightObjectChangeOrientationHI::Backward(void)
{
    CLightObject* p_go = dynamic_cast<CLightObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetOrientation(OldOrientation);
}

//------------------------------------------------------------------------------

void CLightObjectChangeOrientationHI::LoadData(CXMLElement* p_ele)
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

void CLightObjectChangeOrientationHI::SaveData(CXMLElement* p_ele)
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


