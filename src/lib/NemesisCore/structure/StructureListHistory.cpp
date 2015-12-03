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

#include <StructureListHistory.hpp>
#include <StructureList.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,SetActiveStructureHI,
                        "{STR_L:e33386ce-f8c5-446a-8293-0eb969d9572b}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSetActiveStructureHI::CSetActiveStructureHI(CStructureList* p_sl,
                                CStructure* p_prev,CStructure* p_next,
                                EHistoryItemDirection change)
    : CHistoryItem(&SetActiveStructureHIObject,p_sl->GetProject(),change)
{
    if( p_prev ){
        PrevStructureIndex = p_prev->GetIndex();
    } else {
        PrevStructureIndex = -1;
    }

    if( p_next ){
        NextStructureIndex = p_next->GetIndex();
    } else {
        NextStructureIndex = -1;
    }
}

//------------------------------------------------------------------------------

void CSetActiveStructureHI::Forward(void)
{
    CStructureList* p_sl = GetProject()->GetStructures();
    CStructure* p_gpo = dynamic_cast<CStructure*>(GetProject()->FindObject(NextStructureIndex));
    p_sl->SetActiveStructure(p_gpo);
}

//------------------------------------------------------------------------------

void CSetActiveStructureHI::Backward(void)
{
    CStructureList* p_sl = GetProject()->GetStructures();
    CStructure* p_gpo = dynamic_cast<CStructure*>(GetProject()->FindObject(PrevStructureIndex));
    p_sl->SetActiveStructure(p_gpo);
}

//------------------------------------------------------------------------------

void CSetActiveStructureHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("psi",PrevStructureIndex);
    p_ele->GetAttribute("nsi",NextStructureIndex);
}

//------------------------------------------------------------------------------

void CSetActiveStructureHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("psi",PrevStructureIndex);
    p_ele->SetAttribute("nsi",NextStructureIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
