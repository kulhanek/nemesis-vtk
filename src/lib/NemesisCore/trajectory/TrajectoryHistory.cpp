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

#include <TrajectoryHistory.hpp>
#include <Trajectory.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <TrajectoryList.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Structure.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,TrajectoryHI,
                        "{TRJ:a0661923-7df4-4fd3-9d70-a117cb0557c9}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,TrajectorySetStructureHI,
                        "{TRJ_STR:a45ad9a7-4516-4d5e-8434-f7897fb704c5}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,TrajectorySetPlayModeHI,
                        "{TRJ_PM:fb4ed9e0-bae2-4295-95c9-d8f6e7c6580a}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,TrajectorySetPlayTickTimeHI,
                        "{TRJ_PT:596ea5a2-c45a-4a13-bb4d-bc2bc3fcb653}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectoryHI::CTrajectoryHI(CTrajectory* p_traj,EHistoryItemDirection change)
    : CHistoryItem(&TrajectoryHIObject,p_traj->GetProject(),change)
{
    TrajectoryIndex = p_traj->GetIndex();
    CXMLElement* p_ele = TrajectoryData.CreateChildElement("trajectory");
    p_traj->SaveData(p_ele);
}

//------------------------------------------------------------------------------

void CTrajectoryHI::Forward(void)
{
    CXMLElement* p_ele = TrajectoryData.GetFirstChildElement("trajectory");
    GetProject()->GetTrajectories()->CreateTrajectory(p_ele);
}

//------------------------------------------------------------------------------

void CTrajectoryHI::Backward(void)
{
    CXMLElement* p_ele = TrajectoryData.GetFirstChildElement("trajectory");
    if( p_ele == NULL ) return;
    int str_index = -1;
    p_ele->GetAttribute("id",str_index);

    CTrajectory* p_traj = dynamic_cast<CTrajectory*>(GetProject()->FindObject(str_index));
    if(p_traj == NULL) return;

    // save designer setup
    CXMLElement* p_sdele = p_traj->GetDesignerData();
    if( p_sdele ){
        CXMLElement* p_tdele = TrajectoryData.GetChildElementByPath("trajectory/designer",true);
        p_tdele->RemoveAllChildNodes();
        p_tdele->RemoveAllAttributes();
        p_tdele->CopyContentsFrom(p_sdele);
    }

    p_traj->RemoveFromBaseList();
}

//------------------------------------------------------------------------------

void CTrajectoryHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    CXMLElement* p_sele = p_ele->GetFirstChildElement("trajectory");
    if( p_sele == NULL ) return;
    TrajectoryData.RemoveAllChildNodes();
    CXMLElement* p_dele = TrajectoryData.CreateChildElement("trajectory");
    p_dele->CopyContentsFrom(p_sele);
}

//------------------------------------------------------------------------------

void CTrajectoryHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    CXMLElement* p_sele = TrajectoryData.GetFirstChildElement("trajectory");
    if( p_sele == NULL ) return;
    CXMLElement* p_dele = p_ele->CreateChildElement("trajectory");
    p_dele->CopyContentsFrom(p_sele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectorySetStructureHI::CTrajectorySetStructureHI(CTrajectory* p_traj,CStructure* p_str)
    : CHistoryItem(&TrajectorySetStructureHIObject,p_traj->GetProject(),EHID_FORWARD)
{
    TrajectoryIndex = p_traj->GetIndex();
    OldStructureIndex = -1;
    NewStructureIndex = -1;
    if( p_traj->GetStructure() ){
        OldStructureIndex = p_traj->GetStructure()->GetIndex();
    }
    if( p_str ){
        NewStructureIndex = p_str->GetIndex();
    }
}

//------------------------------------------------------------------------------

void CTrajectorySetStructureHI::Forward(void)
{
    CTrajectory* p_traj = dynamic_cast<CTrajectory*>(GetProject()->FindObject(TrajectoryIndex));
    if(p_traj == NULL) return;

    CStructure* p_str = dynamic_cast<CStructure*>(GetProject()->FindObject(NewStructureIndex));
    p_traj->SetStructure(p_str);
}

//------------------------------------------------------------------------------

void CTrajectorySetStructureHI::Backward(void)
{
    CTrajectory* p_traj = dynamic_cast<CTrajectory*>(GetProject()->FindObject(TrajectoryIndex));
    if(p_traj == NULL) return;

    CStructure* p_str = dynamic_cast<CStructure*>(GetProject()->FindObject(OldStructureIndex));
    p_traj->SetStructure(p_str);
}

//------------------------------------------------------------------------------

void CTrajectorySetStructureHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("tri",TrajectoryIndex);
    p_ele->GetAttribute("osi",OldStructureIndex);
    p_ele->GetAttribute("nsi",NewStructureIndex);
}

//------------------------------------------------------------------------------

void CTrajectorySetStructureHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("tri",TrajectoryIndex);
    p_ele->SetAttribute("osi",OldStructureIndex);
    p_ele->SetAttribute("nsi",NewStructureIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectorySetPlayModeHI::CTrajectorySetPlayModeHI(CTrajectory* p_traj,ETrajectoryPlayMode mode)
    : CHistoryItem(&TrajectorySetPlayModeHIObject,p_traj->GetProject(),EHID_FORWARD)
{
    TrajectoryIndex = p_traj->GetIndex();
    OldMode = p_traj->GetPlayMode();
    NewMode = mode;
}

//------------------------------------------------------------------------------

void CTrajectorySetPlayModeHI::Forward(void)
{
    CTrajectory* p_traj = dynamic_cast<CTrajectory*>(GetProject()->FindObject(TrajectoryIndex));
    if(p_traj == NULL) return;
    p_traj->SetPlayMode(NewMode);
}

//------------------------------------------------------------------------------

void CTrajectorySetPlayModeHI::Backward(void)
{
    CTrajectory* p_traj = dynamic_cast<CTrajectory*>(GetProject()->FindObject(TrajectoryIndex));
    if(p_traj == NULL) return;
    p_traj->SetPlayMode(OldMode);
}

//------------------------------------------------------------------------------

void CTrajectorySetPlayModeHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("tri",TrajectoryIndex);
    p_ele->GetAttribute("omd",OldMode);
    p_ele->GetAttribute("nmd",NewMode);
}

//------------------------------------------------------------------------------

void CTrajectorySetPlayModeHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("tri",TrajectoryIndex);
    p_ele->SetAttribute("omd",OldMode);
    p_ele->SetAttribute("nmd",NewMode);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectorySetPlayTickTimeHI::CTrajectorySetPlayTickTimeHI(CTrajectory* p_traj,int time)
    : CHistoryItem(&TrajectorySetPlayTickTimeHIObject,p_traj->GetProject(),EHID_FORWARD)
{
    TrajectoryIndex = p_traj->GetIndex();
    OldTime = p_traj->GetPlayTickTime();
    NewTime = time;
}

//------------------------------------------------------------------------------

void CTrajectorySetPlayTickTimeHI::Forward(void)
{
    CTrajectory* p_traj = dynamic_cast<CTrajectory*>(GetProject()->FindObject(TrajectoryIndex));
    if(p_traj == NULL) return;
    p_traj->SetPlayTickTime(NewTime);
}

//------------------------------------------------------------------------------

void CTrajectorySetPlayTickTimeHI::Backward(void)
{
    CTrajectory* p_traj = dynamic_cast<CTrajectory*>(GetProject()->FindObject(TrajectoryIndex));
    if(p_traj == NULL) return;
    p_traj->SetPlayTickTime(OldTime);
}

//------------------------------------------------------------------------------

void CTrajectorySetPlayTickTimeHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("tri",TrajectoryIndex);
    p_ele->GetAttribute("oti",OldTime);
    p_ele->GetAttribute("nti",NewTime);
}

//------------------------------------------------------------------------------

void CTrajectorySetPlayTickTimeHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("tri",TrajectoryIndex);
    p_ele->SetAttribute("oti",OldTime);
    p_ele->SetAttribute("nti",NewTime);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

