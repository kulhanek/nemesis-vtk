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

#include <PBCBoxObjectHistory.hpp>
#include <PBCBoxObject.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,PBCBoxObjectChangePosHI,
                        "{GFX_PBC_POS:c844d2ed-5683-4055-a694-f41dbaa8abcd}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPBCBoxObjectChangePosHI::CPBCBoxObjectChangePosHI(
                        CPBCBoxObject* p_obj, int ka,int kb,int kc)
    : CHistoryItem(&PBCBoxObjectChangePosHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldKA = p_obj->GetKAOffset();
    OldKB = p_obj->GetKBOffset();
    OldKC = p_obj->GetKCOffset();
    NewKA = ka;
    NewKB = kb;
    NewKC = kc;
}

//------------------------------------------------------------------------------

void CPBCBoxObjectChangePosHI::Forward(void)
{
    CPBCBoxObject* p_go = dynamic_cast<CPBCBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetKOffsets(NewKA,NewKB,NewKC);
}

//------------------------------------------------------------------------------

void CPBCBoxObjectChangePosHI::Backward(void)
{
    CPBCBoxObject* p_go = dynamic_cast<CPBCBoxObject*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetKOffsets(OldKA,OldKB,OldKC);
}

//------------------------------------------------------------------------------

void CPBCBoxObjectChangePosHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("oi",ObjectID);
    p_ele->GetAttribute("nka",NewKA);
    p_ele->GetAttribute("nkb",NewKB);
    p_ele->GetAttribute("nkc",NewKC);
    p_ele->GetAttribute("oka",OldKA);
    p_ele->GetAttribute("okb",OldKB);
    p_ele->GetAttribute("okc",OldKC);
}

//------------------------------------------------------------------------------

void CPBCBoxObjectChangePosHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("oi",ObjectID);
    p_ele->SetAttribute("nka",NewKA);
    p_ele->SetAttribute("nkb",NewKB);
    p_ele->SetAttribute("nkc",NewKC);
    p_ele->SetAttribute("oka",OldKA);
    p_ele->SetAttribute("okb",OldKB);
    p_ele->SetAttribute("okc",OldKC);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


