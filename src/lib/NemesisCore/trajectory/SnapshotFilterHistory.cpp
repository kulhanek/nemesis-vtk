// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <SnapshotFilterHistory.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Trajectory.hpp>
#include <SnapshotFilter.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,SnapshotFilterHI,
                        "{FLT:09f65756-0ba3-4c0f-9388-8b49ea4d23e7}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,SnapshotFilterSeqIdxHI,
                        "{FLT_SI:f2a5e350-0773-4dde-8697-da38cb4c9328}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSnapshotFilterHI::CSnapshotFilterHI(CSnapshotFilter* p_flt,EHistoryItemDirection change)
    : CHistoryItem(&SnapshotFilterHIObject,p_flt->GetProject(),change)
{
    TrajectoryIndex = p_flt->GetTrajectory()->GetIndex();
    CXMLElement* p_ele = FilterData.CreateChildElement("filter");
    p_flt->SaveData(p_ele);
}

//------------------------------------------------------------------------------

void CSnapshotFilterHI::Forward(void)
{
    CXMLElement* p_ele = FilterData.GetFirstChildElement("filter");
    CTrajectory* p_traj = dynamic_cast<CTrajectory*>(GetProject()->FindObject(TrajectoryIndex));
    if(p_traj == NULL) return;
    p_traj->CreateFilter(p_ele);
}

//------------------------------------------------------------------------------

void CSnapshotFilterHI::Backward(void)
{
    CXMLElement* p_ele = FilterData.GetFirstChildElement("filter");
    if( p_ele == NULL ) return;
    int index = -1;
    p_ele->GetAttribute("id",index);

    CSnapshotFilter* p_flt = dynamic_cast<CSnapshotFilter*>(GetProject()->FindObject(index));
    if(p_flt == NULL) return;

    // save designer setup
    CXMLElement* p_sdele = p_flt->GetDesignerData();
    if( p_sdele ){
        CXMLElement* p_tdele = FilterData.GetChildElementByPath("filter/designer",true);
        p_tdele->RemoveAllChildNodes();
        p_tdele->RemoveAllAttributes();
        p_tdele->CopyContentsFrom(p_sdele);
    }

    p_flt->RemoveFromBaseList();
}

//------------------------------------------------------------------------------

void CSnapshotFilterHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    CXMLElement* p_sele = p_ele->GetFirstChildElement("filter");
    if( p_sele == NULL ) return;
    FilterData.RemoveAllChildNodes();
    CXMLElement* p_dele = FilterData.CreateChildElement("filter");
    p_dele->CopyContentsFrom(p_sele);
}

//------------------------------------------------------------------------------

void CSnapshotFilterHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    CXMLElement* p_sele = FilterData.GetFirstChildElement("filter");
    if( p_sele == NULL ) return;
    CXMLElement* p_dele = p_ele->CreateChildElement("filter");
    p_dele->CopyContentsFrom(p_sele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSnapshotFilterSeqIdxHI::CSnapshotFilterSeqIdxHI(CSnapshotFilter* p_flt,int newseqidx)
    : CHistoryItem(&SnapshotFilterSeqIdxHIObject,p_flt->GetProject(),EHID_FORWARD)
{
    FilterIndex = p_flt->GetIndex();
    OldSeqIndex = p_flt->GetSeqIndex();
    NewSeqIndex = newseqidx;
}

//------------------------------------------------------------------------------

void CSnapshotFilterSeqIdxHI::Forward(void)
{
    CSnapshotFilter* p_flt = dynamic_cast<CSnapshotFilter*>(GetProject()->FindObject(FilterIndex));
    if(p_flt == NULL) return;

    p_flt->SetSeqIndex(NewSeqIndex);
}

//------------------------------------------------------------------------------

void CSnapshotFilterSeqIdxHI::Backward(void)
{
    CSnapshotFilter* p_flt = dynamic_cast<CSnapshotFilter*>(GetProject()->FindObject(FilterIndex));
    if(p_flt == NULL) return;

    p_flt->SetSeqIndex(OldSeqIndex);
}

//------------------------------------------------------------------------------

void CSnapshotFilterSeqIdxHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("fi",FilterIndex);
    p_ele->GetAttribute("ns",NewSeqIndex);
    p_ele->GetAttribute("os",OldSeqIndex);
}

//------------------------------------------------------------------------------

void CSnapshotFilterSeqIdxHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("fi",FilterIndex);
    p_ele->SetAttribute("ns",NewSeqIndex);
    p_ele->SetAttribute("os",OldSeqIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

