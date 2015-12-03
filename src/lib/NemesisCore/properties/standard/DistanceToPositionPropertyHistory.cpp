// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <DistanceToPositionPropertyHistory.hpp>
#include <DistanceToPositionProperty.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>
#include <XMLElement.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,DistanceToPositionPropertyChangePointBHI,
                        "{DTP_CHPB:d650e263-1575-4a85-bb86-71836c820d3f}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDistanceToPositionPropertyChangePointBHI::CDistanceToPositionPropertyChangePointBHI(
                        CDistanceToPositionProperty* p_obj,const CPoint& newPointB)
    : CHistoryItem(&DistanceToPositionPropertyChangePointBHIObject,p_obj->GetProject(),EHID_FORWARD)
{
    ObjectID = p_obj->GetIndex();
    OldPointB = p_obj->GetPointB();
    NewPointB = newPointB;
}

//------------------------------------------------------------------------------

void CDistanceToPositionPropertyChangePointBHI::Forward(void)
{
    CDistanceToPositionProperty* p_go = dynamic_cast<CDistanceToPositionProperty*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetPointB(NewPointB);
}

//------------------------------------------------------------------------------

void CDistanceToPositionPropertyChangePointBHI::Backward(void)
{
    CDistanceToPositionProperty* p_go = dynamic_cast<CDistanceToPositionProperty*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetPointB(OldPointB);
}
//------------------------------------------------------------------------------

void CDistanceToPositionPropertyChangePointBHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("gi",ObjectID);
    p_ele->GetAttribute("npx",NewPointB.x);
    p_ele->GetAttribute("npy",NewPointB.y);
    p_ele->GetAttribute("npz",NewPointB.z);
    p_ele->GetAttribute("opx",OldPointB.x);
    p_ele->GetAttribute("opy",OldPointB.y);
    p_ele->GetAttribute("opz",OldPointB.z);
}

//------------------------------------------------------------------------------

void CDistanceToPositionPropertyChangePointBHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("gi",ObjectID);
    p_ele->SetAttribute("npx",NewPointB.x);
    p_ele->SetAttribute("npy",NewPointB.y);
    p_ele->SetAttribute("npz",NewPointB.z);
    p_ele->SetAttribute("opx",OldPointB.x);
    p_ele->SetAttribute("opy",OldPointB.y);
    p_ele->SetAttribute("opz",OldPointB.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


