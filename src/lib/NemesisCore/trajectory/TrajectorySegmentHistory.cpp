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

#include <TrajectorySegmentHistory.hpp>
#include <Trajectory.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <TrajectorySegment.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,TrajectorySegmentHI,
                        "{SEG:b80362a1-5480-41d8-9f1a-86fa4b283a52}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,TrajectorySegmentSeqIdxHI,
                        "{SEG_SI:fbe474c3-bfa2-4dba-bd7a-5768bb576b29}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectorySegmentHI::CTrajectorySegmentHI(CTrajectorySegment* p_seg,EHistoryItemDirection change)
    : CHistoryItem(&TrajectorySegmentHIObject,p_seg->GetProject(),change)
{
    TrajectoryIndex = p_seg->GetTrajectory()->GetIndex();
    CXMLElement* p_ele = SegmentData.CreateChildElement("segment");
    p_seg->SaveData(p_ele);
}

//------------------------------------------------------------------------------

void CTrajectorySegmentHI::Forward(void)
{
    CXMLElement* p_ele = SegmentData.GetFirstChildElement("segment");
    CTrajectory* p_traj = dynamic_cast<CTrajectory*>(GetProject()->FindObject(TrajectoryIndex));
    if(p_traj == NULL) return;
    p_traj->BeginUpdate();
    CTrajectorySegment* p_seg = p_traj->CreateSegment(p_ele);
    if( p_seg ){
        p_seg->LoadTrajectoryData();
    }
    p_traj->EndUpdate();
}

//------------------------------------------------------------------------------

void CTrajectorySegmentHI::Backward(void)
{
    CXMLElement* p_ele = SegmentData.GetFirstChildElement("segment");
    if( p_ele == NULL ) return;
    int index = -1;
    p_ele->GetAttribute("id",index);

    CTrajectorySegment* p_seg = dynamic_cast<CTrajectorySegment*>(GetProject()->FindObject(index));
    if(p_seg == NULL) return;

    // save designer setup
    CXMLElement* p_sdele = p_seg->GetDesignerData();
    if( p_sdele ){
        CXMLElement* p_tdele = SegmentData.GetChildElementByPath("segment/designer",true);
        p_tdele->RemoveAllChildNodes();
        p_tdele->RemoveAllAttributes();
        p_tdele->CopyContentsFrom(p_sdele);
    }

    p_seg->RemoveFromBaseList();
}

//------------------------------------------------------------------------------

void CTrajectorySegmentHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    CXMLElement* p_sele = p_ele->GetFirstChildElement("segment");
    if( p_sele == NULL ) return;
    SegmentData.RemoveAllChildNodes();
    CXMLElement* p_dele = SegmentData.CreateChildElement("segment");
    p_dele->CopyContentsFrom(p_sele);
}

//------------------------------------------------------------------------------

void CTrajectorySegmentHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    CXMLElement* p_sele = SegmentData.GetFirstChildElement("segment");
    if( p_sele == NULL ) return;
    CXMLElement* p_dele = p_ele->CreateChildElement("segment");
    p_dele->CopyContentsFrom(p_sele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectorySegmentSeqIdxHI::CTrajectorySegmentSeqIdxHI(CTrajectorySegment* p_seg,int newseqidx)
    : CHistoryItem(&TrajectorySegmentSeqIdxHIObject,p_seg->GetProject(),EHID_FORWARD)
{
    SegmentIndex = p_seg->GetIndex();
    OldSeqIndex = p_seg->GetSeqIndex();
    NewSeqIndex = newseqidx;
}

//------------------------------------------------------------------------------

void CTrajectorySegmentSeqIdxHI::Forward(void)
{
    CTrajectorySegment* p_seg = dynamic_cast<CTrajectorySegment*>(GetProject()->FindObject(SegmentIndex));
    if(p_seg == NULL) return;

    p_seg->SetSeqIndex(NewSeqIndex,true);
}

//------------------------------------------------------------------------------

void CTrajectorySegmentSeqIdxHI::Backward(void)
{
    CTrajectorySegment* p_seg = dynamic_cast<CTrajectorySegment*>(GetProject()->FindObject(SegmentIndex));
    if(p_seg == NULL) return;

    p_seg->SetSeqIndex(OldSeqIndex,true);
}

//------------------------------------------------------------------------------

void CTrajectorySegmentSeqIdxHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("fi",SegmentIndex);
    p_ele->GetAttribute("ns",NewSeqIndex);
    p_ele->GetAttribute("os",OldSeqIndex);
}

//------------------------------------------------------------------------------

void CTrajectorySegmentSeqIdxHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("fi",SegmentIndex);
    p_ele->SetAttribute("ns",NewSeqIndex);
    p_ele->SetAttribute("os",OldSeqIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

