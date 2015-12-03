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

#include <TrajectoryListHistory.hpp>
#include <TrajectoryList.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,SetActiveTrajectoryHI,
                        "{TRJ_L:382e143f-1455-4b39-901a-7eaecb3db094}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSetActiveTrajectoryHI::CSetActiveTrajectoryHI(CTrajectoryList* p_sl,
                                CTrajectory* p_prev,CTrajectory* p_next,
                                EHistoryItemDirection change)
    : CHistoryItem(&SetActiveTrajectoryHIObject,p_sl->GetProject(),change)
{
    if( p_prev ){
        PrevTrajectoryIndex = p_prev->GetIndex();
    } else {
        PrevTrajectoryIndex = -1;
    }

    if( p_next ){
        NextTrajectoryIndex = p_next->GetIndex();
    } else {
        NextTrajectoryIndex = -1;
    }
}

//------------------------------------------------------------------------------

void CSetActiveTrajectoryHI::Forward(void)
{
    CTrajectoryList* p_sl = GetProject()->GetTrajectories();
    CTrajectory* p_gpo = dynamic_cast<CTrajectory*>(GetProject()->FindObject(NextTrajectoryIndex));
    p_sl->SetActiveTrajectory(p_gpo);
}

//------------------------------------------------------------------------------

void CSetActiveTrajectoryHI::Backward(void)
{
    CTrajectoryList* p_sl = GetProject()->GetTrajectories();
    CTrajectory* p_gpo = dynamic_cast<CTrajectory*>(GetProject()->FindObject(PrevTrajectoryIndex));
    p_sl->SetActiveTrajectory(p_gpo);
}

//------------------------------------------------------------------------------

void CSetActiveTrajectoryHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("psi",PrevTrajectoryIndex);
    p_ele->GetAttribute("nsi",NextTrajectoryIndex);
}

//------------------------------------------------------------------------------

void CSetActiveTrajectoryHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("psi",PrevTrajectoryIndex);
    p_ele->SetAttribute("nsi",NextTrajectoryIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
