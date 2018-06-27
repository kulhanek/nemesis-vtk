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

#include <TrajectorySegment.hpp>
#include <Trajectory.hpp>
#include <TrajectorySegmentHistory.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <QFileInfo>
#include <Structure.hpp>
#include <AtomList.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectorySegment::CTrajectorySegment(CPluginObject* p_po,CTrajectory* p_traj,bool no_index)
    : CProObject(p_po,p_traj,p_traj->GetProject(),no_index)
{
    SeqIndex = 0;
    SnapshotIndex = 0;
}

//------------------------------------------------------------------------------

CTrajectorySegment::~CTrajectorySegment(void)
{
    CTrajectory* p_list = GetTrajectory();

    setParent(NULL);    // remove object from the list

    if( p_list ){
        p_list->Segments.removeOne(this);
        p_list->EmitOnTrajectorySegmentsChanged();
    }
}

// ----------------------------------------------------------------------------

void CTrajectorySegment::RemoveFromBaseList(CHistoryNode* p_history)
{
    CProObject* p_obj;

    // unregister other objects
    while( (p_obj = RemoveFirstFromROList()) !=NULL ) {
        p_obj->RemoveFromRegistered(this,p_history);
    }

    // register elementary change to history list
    if( p_history != NULL ) {
        CTrajectorySegmentHI* p_strdata = new CTrajectorySegmentHI(this,EHID_BACKWARD);
        p_history->Register(p_strdata);
    }

    // --------------------------------
    delete this; // destroy object
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTrajectorySegment::DeleteWH(void)
{
    // object will be deleted - use parent for all operation
    CTrajectory* p_sl = GetTrajectory();

    CHistoryNode* p_history = p_sl->BeginChangeWH(EHCL_TRAJECTORIES,tr("delete trajectory segment"));
    if( p_history == NULL ) return (false);

    RemoveFromBaseList(p_history);

    p_sl->EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CTrajectorySegment::MoveUpWH(void)
{
    if( IsFirstSegment() ) return(true);
    if( GetTrajectory()->GetNumberOfSegments() <= 1 ) return(false);

    CHistoryNode* p_history = BeginChangeWH(EHCL_TRAJECTORIES,tr("move trajectory segment up"));
    if( p_history == NULL ) return (false);

    CTrajectorySegment* p_seg = GetPrevSegment();
    if( p_seg != NULL ){
        int seqindex = GetSeqIndex();
        SetSeqIndex(p_seg->GetSeqIndex(),false,p_history);
        p_seg->SetSeqIndex(seqindex,true,p_history);
    }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CTrajectorySegment::MoveDownWH(void)
{
    if( IsLastSegment() ) return(true);
    if( GetTrajectory()->GetNumberOfSegments() <= 1 ) return(false);

    CHistoryNode* p_history = BeginChangeWH(EHCL_TRAJECTORIES,tr("move trajectory segment down"));
    if( p_history == NULL ) return (false);

    CTrajectorySegment* p_seg = GetNextSegment();
    if( p_seg != NULL ){
        int seqindex = GetSeqIndex();
        SetSeqIndex(p_seg->GetSeqIndex(),false,p_history);
        p_seg->SetSeqIndex(seqindex,true,p_history);
    }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CTrajectorySegment::MoveInTheFirstWH(void)
{
    // metod take segment and move with its to the top = activate it and set history checkpoint
    if( IsFirstSegment() ) return(true);
    if( GetTrajectory()->GetNumberOfSegments() <= 1 ) return(false);

    CHistoryNode* p_history = BeginChangeWH(EHCL_TRAJECTORIES,tr("move trajectory segment in the first place"));
    if( p_history == NULL ) return (false);

    int seqindex = GetSeqIndex();
    while (!IsFirstSegment())
    {
        CTrajectorySegment* p_seg = GetPrevSegment();
        if( p_seg != NULL ){
            seqindex = GetSeqIndex();
            SetSeqIndex(p_seg->GetSeqIndex(),false,p_history);
            p_seg->SetSeqIndex(seqindex,true,p_history);
        }
    }
    if( GetTrajectory() ){
        GetTrajectory()->EmitOnTrajectorySegmentsChanged();
    }
    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CTrajectorySegment::MoveInTheFirst(void)
{
    // metod take segment and move with its to the top = activate it
    if( IsFirstSegment() ) return(true);
    if( GetTrajectory()->GetNumberOfSegments() <= 1 ) return(false);
    int seqindex = GetSeqIndex();
    while (!IsFirstSegment())
    {
        CTrajectorySegment* p_seg = GetPrevSegment();
        if( p_seg != NULL ){
            seqindex = GetSeqIndex();
            SetSeqIndex(p_seg->GetSeqIndex(),false,NULL);
            p_seg->SetSeqIndex(seqindex,true,NULL);
        }
    }
    if( GetTrajectory() ){
        GetTrajectory()->EmitOnTrajectorySegmentsChanged();
    }
    return(true);
}

//------------------------------------------------------------------------------

bool CTrajectorySegment::SetSeqIndexWH(int index)
{
    if( index == SeqIndex ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_TRAJECTORIES,tr("trajectory segment sequence index"));
    if( p_history == NULL ) return (false);

    SetSeqIndex(index,true,p_history);

    EndChangeWH();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectorySegment::SetSeqIndex(int index,bool signal,CHistoryNode* p_history)
{
    if( SeqIndex == index ) return;

    // register elementary change to history list
    if( p_history != NULL ) {
        CTrajectorySegmentSeqIdxHI* p_strdata = new CTrajectorySegmentSeqIdxHI(this,index);
        p_history->Register(p_strdata);
    }

    SeqIndex = index;
    GetTrajectory()->SegmentCounter.SetTopIndex(SeqIndex);

    if( signal ){
        GetTrajectory()->EmitOnTrajectorySegmentsChanged();
    }

    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CTrajectorySegment::SetFileName(const QString& name)
{
    FileName = name;
    SetName(QFileInfo(FileName).baseName());
}

//------------------------------------------------------------------------------

bool CTrajectorySegment::FirstSnapshot(void)
{
    if( GetNumberOfSnapshots() > 0 ){
            SnapshotIndex = 1;
            SnapshotChanged();
            return(true);
    }
    SnapshotIndex = 0;
    SnapshotChanged();
    return(false);
}

//------------------------------------------------------------------------------

bool CTrajectorySegment::NextSnapshot(void)
{
    if( SnapshotIndex + 1 > GetNumberOfSnapshots() ) return(false);
    SnapshotIndex++;
    SnapshotChanged();
    return(true);
}

//------------------------------------------------------------------------------

bool CTrajectorySegment::PrevSnapshot(void)
{
    if( SnapshotIndex - 1 < 1 ) return(false);
    SnapshotIndex--;
    SnapshotChanged();
    return(true);
}

//------------------------------------------------------------------------------

bool CTrajectorySegment::LastSnapshot(void)
{
    if( GetNumberOfSnapshots() > 0 ){
            SnapshotIndex = GetNumberOfSnapshots();
            SnapshotChanged();
            return(true);
    }
    SnapshotIndex = 0;
    SnapshotChanged();
    return(false);
}

//------------------------------------------------------------------------------

bool CTrajectorySegment::MoveToSnapshot(long int index)
{
    if( (index < 1) || (index > GetNumberOfSnapshots()) ) return(false);
    SnapshotIndex = index;
    SnapshotChanged();
    return(true);
}

//------------------------------------------------------------------------------

long int CTrajectorySegment::GetBaseSnapshopIndex(void)
{
    CTrajectory* p_trj = GetTrajectory();
    if( p_trj == NULL ) return(-1);
    long int baseidx = 0;
    for(long int i = 1; i <= p_trj->GetNumberOfSegments(); i++){
        CTrajectorySegment* p_sgm = p_trj->GetSegment(i);
        if( p_sgm == this ) return(baseidx);
        baseidx += p_sgm->GetNumberOfSnapshots();
    }
    return(-1);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectory* CTrajectorySegment::GetTrajectory(void) const
{
    return(static_cast<CTrajectory*>(parent()));
}

//------------------------------------------------------------------------------

CStructure* CTrajectorySegment::GetStructure(void) const
{
    if( GetTrajectory() == NULL ) return(NULL);
    return( GetTrajectory()->GetStructure() );
}

//------------------------------------------------------------------------------

int CTrajectorySegment::GetNumberOfAtoms(void) const
{
    if( GetTrajectory() == NULL ) return(0);
    if( GetTrajectory()->GetStructure() == NULL ) return(0);
    return( GetTrajectory()->GetStructure()->GetAtoms()->GetNumberOfAtoms() );
}

//------------------------------------------------------------------------------

long int CTrajectorySegment::GetNumberOfSnapshots(void)
{
    return(0);
}

//------------------------------------------------------------------------------

const QString& CTrajectorySegment::GetFileName(void) const
{
    return(FileName);
}

//------------------------------------------------------------------------------

int CTrajectorySegment::GetSeqIndex(void)
{
    return(SeqIndex);
}

//------------------------------------------------------------------------------

bool CTrajectorySegment::IsFirstSegment(void)
{
    return( this == GetTrajectory()->Segments.first() );
}

//------------------------------------------------------------------------------

bool CTrajectorySegment::IsLastSegment(void)
{
    return( this == GetTrajectory()->Segments.last() );
}

//------------------------------------------------------------------------------

CTrajectorySegment* CTrajectorySegment::GetPrevSegment(void)
{
    CTrajectorySegment* p_prev = NULL;
    foreach(CTrajectorySegment* p_seg,GetTrajectory()->Segments){
        if( p_seg == this ) return(p_prev);
        p_prev = p_seg;
    }
    return(NULL);
}

//------------------------------------------------------------------------------

CTrajectorySegment* CTrajectorySegment::GetNextSegment(void)
{
    CTrajectorySegment* p_prev = NULL;
    foreach(CTrajectorySegment* p_seg,GetTrajectory()->Segments){
        if( p_prev == this ) return(p_seg);
        p_prev = p_seg;
    }

    return(NULL);
}

//------------------------------------------------------------------------------

CSnapshot* CTrajectorySegment::GetCurrentSnapshot(void)
{
    return(NULL);
}

//------------------------------------------------------------------------------

long int CTrajectorySegment::GetCurrentSnapshotIndex(void)
{
    return(SnapshotIndex);
}

//------------------------------------------------------------------------------

void CTrajectorySegment::SnapshotChanged(void)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CTrajectorySegment::EmitOnSnapshotChanged(void)
{
   if( GetTrajectory() == NULL ) return;
   if( GetTrajectory()->IsSegmentActive(this) == false ) return;
   GetTrajectory()->EmitOnSnapshotChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectorySegment::SetName(const QString& name,CHistoryNode* p_history)
{
    CProObject::SetName(name,p_history);
    if( GetTrajectory() ){
        GetTrajectory()->EmitOnTrajectorySegmentsChanged();
    }
}

//------------------------------------------------------------------------------

void CTrajectorySegment::SetDescription(const QString& descrip,CHistoryNode* p_history)
{
    CProObject::SetDescription(descrip,p_history);
    if( GetTrajectory() ){
        GetTrajectory()->EmitOnTrajectorySegmentsChanged();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectorySegment::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        RUNTIME_ERROR("p_ele is NULL");
    }
    if( p_ele->GetName() != "segment" ){
        RUNTIME_ERROR("p_ele is not segement");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    // specific data --------------------------------
    p_ele->GetAttribute("traj",FileName);
    // p_ele->GetAttribute("uuid",GetType()); // it is not needed

    int si = -1;
    p_ele->GetAttribute("si",si);
    SetSeqIndex(si,false); // it sets correctly top index in parent trajectory
}

//------------------------------------------------------------------------------

void CTrajectorySegment::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        RUNTIME_ERROR("p_ele is NULL");
    }
    if( p_ele->GetName() != "segment" ){
        RUNTIME_ERROR("p_ele is not segement");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    // specific data --------------------------------
    p_ele->SetAttribute("traj",FileName);
    p_ele->SetAttribute("uuid",GetType().GetFullStringForm());
    p_ele->SetAttribute("si",GetSeqIndex());
}

//------------------------------------------------------------------------------

void CTrajectorySegment::LoadTrajectoryData(void)
{
    // nothing to be here
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

