// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

#include <GraphicsProfileListHistory.hpp>
#include <NemesisCoreModule.hpp>
#include <Project.hpp>
#include <GraphicsProfileList.hpp>
#include <ProObjectHistory.hpp>
#include <ErrorSystem.hpp>
#include <GraphicsProfile.hpp>
#include <Graphics.hpp>
#include <XMLElement.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,SetActiveProfileHI,
                        "{GP_L_SAP:f38d5bca-72e9-4abf-abe2-0c1244906365}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSetActiveProfileHI::CSetActiveProfileHI(CGraphicsProfileList* p_gp,
                                                            CGraphicsProfile* p_prev,
                                                            CGraphicsProfile* p_next,EHistoryItemDirection change)
    : CHistoryItem(&SetActiveProfileHIObject,p_gp->GetProject(),change)
{
    if( p_prev ){
        PrevProfileIndex = p_prev->GetIndex();
    } else {
        PrevProfileIndex = -1;
    }

    if( p_next ){
        NextProfileIndex = p_next->GetIndex();
    } else {
        NextProfileIndex = -1;
    }
}

//------------------------------------------------------------------------------

void CSetActiveProfileHI::Forward(void)
{
    CGraphicsProfileList* p_gpl = GetProject()->GetGraphics()->GetProfiles();
    CGraphicsProfile* p_gpo = dynamic_cast<CGraphicsProfile*>(GetProject()->FindObject(PrevProfileIndex));
    p_gpl->SetActiveProfile(p_gpo);
}

//------------------------------------------------------------------------------

void CSetActiveProfileHI::Backward(void)
{
    CGraphicsProfileList* p_gpl = GetProject()->GetGraphics()->GetProfiles();
    CGraphicsProfile* p_gpo = dynamic_cast<CGraphicsProfile*>(GetProject()->FindObject(NextProfileIndex));
    p_gpl->SetActiveProfile(p_gpo);
}

//------------------------------------------------------------------------------

void CSetActiveProfileHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("pi",PrevProfileIndex);
    p_ele->GetAttribute("ni",NextProfileIndex);
}

//------------------------------------------------------------------------------

void CSetActiveProfileHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("pi",PrevProfileIndex);
    p_ele->SetAttribute("ni",NextProfileIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
