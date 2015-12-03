// =============================================================================
// NEMESIS - Molecular Modeling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <SpecAxesObjectHistory.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <Graphics.hpp>
#include <ProObjectHistory.hpp>
#include <ErrorSystem.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,SpecAxesObjectChangeModeHI,
                        "{GFX_SAO_MOD:1bbedc5f-8e76-4227-bc64-a47bc26af31b}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSpecAxesObjectChangeModeHI::CSpecAxesObjectChangeModeHI(CSpecAxesObject* p_obj,ESpecAxesObjectMode newmodel)
    : CHistoryItem(&SpecAxesObjectChangeModeHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldMode = p_obj->GetMode();
    NewMode = newmodel;
}

//------------------------------------------------------------------------------

void CSpecAxesObjectChangeModeHI::Forward(void)
{
    CSpecAxesObject* p_go = dynamic_cast<CSpecAxesObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetMode(NewMode);
}

//------------------------------------------------------------------------------

void CSpecAxesObjectChangeModeHI::Backward(void)
{
    CSpecAxesObject* p_go = dynamic_cast<CSpecAxesObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetMode(OldMode);
}

//------------------------------------------------------------------------------

void CSpecAxesObjectChangeModeHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute<ESpecAxesObjectMode>("nm",NewMode);
    p_ele->GetAttribute<ESpecAxesObjectMode>("om",OldMode);
}

//------------------------------------------------------------------------------

void CSpecAxesObjectChangeModeHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("nm",NewMode);
    p_ele->SetAttribute("om",OldMode);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
