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

#include <SnapshotFilter.hpp>
#include <Trajectory.hpp>
#include <SnapshotFilterHistory.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>

#include "SnapshotFilter.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSnapshotFilter::CSnapshotFilter(CPluginObject* p_po,CTrajectory* p_traj,bool no_index)
    : CProObject(p_po,p_traj,p_traj->GetProject(),no_index)
{
    SeqIndex = 0;
}

//------------------------------------------------------------------------------

CSnapshotFilter::~CSnapshotFilter(void)
{
    CTrajectory* p_list = GetTrajectory();

    setParent(NULL);    // remove object from the list

    if( p_list ){
        p_list->Filters.removeOne(this);
        p_list->EmitOnSnapshotFiltersChanged();
    }
}

// ----------------------------------------------------------------------------

void CSnapshotFilter::RemoveFromBaseList(CHistoryNode* p_history)
{
    CProObject* p_obj;

    // unregister other objects
    while( (p_obj = RemoveFirstFromROList()) !=NULL ) {
        p_obj->RemoveFromRegistered(this,p_history);
    }

    // register elementary change to history list
    if( p_history != NULL ) {
        CSnapshotFilterHI* p_strdata = new CSnapshotFilterHI(this,EHID_BACKWARD);
        p_history->Register(p_strdata);
    }

    // --------------------------------
    delete this; // destroy object
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CSnapshotFilter::DeleteWH(void)
{
    // object will be deleted - use parent for all operation
    CTrajectory* p_sl = GetTrajectory();

    CHistoryNode* p_history = p_sl->BeginChangeWH(EHCL_TRAJECTORIES,tr("delete snapshot filter"));
    if( p_history == NULL ) return (false);

    RemoveFromBaseList(p_history);

    p_sl->EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CSnapshotFilter::MoveUpWH(void)
{
    if( IsFirstFilter() ) return(true);
    if( GetTrajectory()->GetNumberOfFilters() <= 1 ) return(false);

    CHistoryNode* p_history = BeginChangeWH(EHCL_TRAJECTORIES,tr("move snapshot filter up"));
    if( p_history == NULL ) return (false);

    CSnapshotFilter* p_flt = GetNextFilter();
    if( p_flt != NULL ){
        int seqindex = GetSeqIndex();
        SetSeqIndex(p_flt->GetSeqIndex(),p_history);
        p_flt->SetSeqIndex(seqindex);
    }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CSnapshotFilter::MoveDownWH(void)
{
    if( IsFirstFilter() ) return(true);
    if( GetTrajectory()->GetNumberOfFilters() <= 1 ) return(false);

    CHistoryNode* p_history = BeginChangeWH(EHCL_TRAJECTORIES,tr("move snapshot filter down"));
    if( p_history == NULL ) return (false);

    CSnapshotFilter* p_flt = GetPrevFilter();
    if( p_flt != NULL ){
        int seqindex = GetSeqIndex();
        SetSeqIndex(p_flt->GetSeqIndex(),p_history);
        p_flt->SetSeqIndex(seqindex);
    }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CSnapshotFilter::SetSeqIndexWH(int index)
{
    if( index == SeqIndex ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_TRAJECTORIES,tr("snapshot filter sequence index"));
    if( p_history == NULL ) return (false);

    SetSeqIndex(index,p_history);

    EndChangeWH();
    return(true);
}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSnapshotFilter::SetSeqIndex(int index,CHistoryNode* p_history)
{
    if( SeqIndex == index ) return;

    // register elementary change to history list
    if( p_history != NULL ) {
        CSnapshotFilterSeqIdxHI* p_strdata = new CSnapshotFilterSeqIdxHI(this,index);
        p_history->Register(p_strdata);
    }

    SeqIndex = index;
    GetTrajectory()->SegmentCounter.SetTopIndex(SeqIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectory* CSnapshotFilter::GetTrajectory(void) const
{
    return(static_cast<CTrajectory*>(parent()));
}

//------------------------------------------------------------------------------

bool CSnapshotFilter::IsFirstFilter(void)
{
    return( this == GetTrajectory()->Filters.first() );
}

//------------------------------------------------------------------------------

bool CSnapshotFilter::IsLastFilter(void)
{
    return( this == GetTrajectory()->Filters.last() );
}

//------------------------------------------------------------------------------

int CSnapshotFilter::GetSeqIndex(void)
{
    return(SeqIndex);
}

//------------------------------------------------------------------------------

CSnapshotFilter* CSnapshotFilter::GetPrevFilter(void)
{
    CSnapshotFilter* p_prev = NULL;
    foreach(CSnapshotFilter* p_flt,GetTrajectory()->Filters){
        if( p_flt == this ) return(p_prev);
        p_prev = p_flt;
    }
    return(NULL);
}

//------------------------------------------------------------------------------

CSnapshotFilter* CSnapshotFilter::GetNextFilter(void)
{
    CSnapshotFilter* p_prev = NULL;
    foreach(CSnapshotFilter* p_flt,GetTrajectory()->Filters){
        if( p_prev == this ) return(p_flt);
        p_prev = p_flt;
    }

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSnapshotFilter::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        RUNTIME_ERROR("p_ele is NULL");
    }
    if( p_ele->GetName() != "filter" ){
        RUNTIME_ERROR("p_ele is not filter");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    // specific data --------------------------------
    // p_ele->GetAttribute("uuid",GetType()); // it is not needed

    int si = -1;
    p_ele->GetAttribute("si",si);
    SetSeqIndex(si); // it sets correctly top index in parent trajectory
}

//------------------------------------------------------------------------------

void CSnapshotFilter::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        RUNTIME_ERROR("p_ele is NULL");
    }
    if( p_ele->GetName() != "filter" ){
        RUNTIME_ERROR("p_ele is not filter");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    // specific data --------------------------------
    p_ele->SetAttribute("uuid",GetType().GetFullStringForm());
    p_ele->SetAttribute("si",GetSeqIndex());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

