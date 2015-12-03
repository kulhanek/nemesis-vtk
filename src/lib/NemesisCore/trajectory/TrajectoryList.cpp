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

#include <TrajectoryList.hpp>
#include <Trajectory.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <TrajectoryListHistory.hpp>
#include <TrajectoryHistory.hpp>

#include "TrajectoryList.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        TrajectoryListID(
                    "{TRAJECTORY_LIST:2b2a4e86-223a-433a-bcb9-93c5d2b3c717}",
                    "Trajectories");

CPluginObject   TrajectoryListObject(&NemesisCorePlugin,
                    TrajectoryListID,TRAJECTORY_CAT,
                    ":/images/NemesisCore/trajectory/TrajectoryList.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectoryList::CTrajectoryList(CProject* p_project)
    : CProObject(&TrajectoryListObject,p_project,p_project)
{
    ConFlags |= EECOF_SUB_CONTAINERS;
    ConFlags |= EECOF_DYNAMICAL_CONTENTS;
    ActiveTrajectory = NULL;
    Changed = false;
    UpdateLevel = 0;
}

//------------------------------------------------------------------------------

CTrajectoryList::CTrajectoryList(CProject* p_project,bool no_index)
    : CProObject(&TrajectoryListObject,p_project,p_project,no_index)
{
    ConFlags |= EECOF_SUB_CONTAINERS;
    ConFlags |= EECOF_DYNAMICAL_CONTENTS;
    ActiveTrajectory = NULL;
    Changed = false;
    UpdateLevel = 0;
}

//------------------------------------------------------------------------------

CTrajectoryList::~CTrajectoryList(void)
{
    foreach(QObject* p_qobj,children()) {
        delete p_qobj;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectory* CTrajectoryList::CreateTrajectoryWH(bool activate,const QString& name)
{
    CHistoryNode*    p_history;

    p_history = BeginChangeWH(EHCL_TRAJECTORIES,tr("new trajectory"));
    if( p_history == NULL ) return (NULL);


    CTrajectory* p_str = CreateTrajectory(name,p_history);

    if( activate ){
        SetActiveTrajectory(p_str,p_history);
    }

    EndChangeWH();

    return(p_str);
}

//------------------------------------------------------------------------------

bool CTrajectoryList::SetActiveTrajectoryWH(CTrajectory* p_str)
{
    CHistoryNode*    p_history;

    p_history = BeginChangeWH(EHCL_TRAJECTORIES,tr("activate trajectory"));
    if( p_history == NULL ) return (false);

    SetActiveTrajectory(p_str,p_history);

    EndChangeWH();

    return(true);
}

//------------------------------------------------------------------------------

bool CTrajectoryList::DeleteAllTrajectoriesWH(void)
{
    if( children().count() == 0 ){
        GetProject()->TextNotification(ETNT_WARNING,"no trajectory is available to be deleted",ETNT_WARNING_DELAY);
        return(false);
    }

    CHistoryNode*    p_history;
    p_history = BeginChangeWH(EHCL_TRAJECTORIES,tr("delete all trajectories"));
    if( p_history == NULL ) return (false);

        DeleteAllTrajectories(p_history);

    EndChangeWH();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectory* CTrajectoryList::CreateTrajectory(const QString& name,CHistoryNode* p_history)
{
    BeginUpdate();

    QString str_name = name;

    if( str_name.isEmpty() ){
        str_name = GenerateName(tr("Trajectory %1"));
    }

    CTrajectory* p_str = new CTrajectory(this);
    p_str->SetName(str_name);

    if( p_history ){
        CTrajectoryHI* p_item = new CTrajectoryHI(p_str,EHID_FORWARD);
        p_history->Register(p_item);
    }

    // do we have active trajectory?
    if( ActiveTrajectory == NULL ){
        SetActiveTrajectory(p_str,p_history);
    }

    EndUpdate();

    emit OnChildContainerAdded(this,p_str);

    return(p_str);
}

//------------------------------------------------------------------------------

CTrajectory* CTrajectoryList::CreateTrajectory(CXMLElement* p_ele,CHistoryNode* p_history)
{
    BeginUpdate();

    CTrajectory* p_str = new CTrajectory(this,true);

    GetProject()->BeginLinkProcedure(0);
    p_str->LoadData(p_ele);
    GetProject()->EndLinkProcedure();

    if( p_history ){
        CTrajectoryHI* p_item = new CTrajectoryHI(p_str,EHID_FORWARD);
        p_history->Register(p_item);
    }

    // do we have active trajectory?
    if( ActiveTrajectory == NULL ){
        SetActiveTrajectory(p_str,p_history);
    }

    EndUpdate();

    emit OnChildContainerAdded(this,p_str);

    return(p_str);
}

//------------------------------------------------------------------------------

void CTrajectoryList::SetActiveTrajectory(CTrajectory* p_str,CHistoryNode* p_history)
{
    // no change
    if( ActiveTrajectory == p_str ) return;

    // record change if necessary
    if( p_history ){
        CSetActiveTrajectoryHI* p_item = new CSetActiveTrajectoryHI(this,
                                                    ActiveTrajectory,p_str,EHID_FORWARD);
        p_history->Register(p_item);
    }

    // change the active trajectory
    ActiveTrajectory = p_str;

    EmitOnTrajectoryListChanged();
}

//------------------------------------------------------------------------------

void CTrajectoryList::DeleteAllTrajectories(CHistoryNode* p_history)
{
    BeginUpdate();

    foreach(QObject* p_qobj,children()) {
        CTrajectory* p_str = static_cast<CTrajectory*>(p_qobj);
        p_str->RemoveFromBaseList(p_history);
    }

    EndUpdate();
}

//------------------------------------------------------------------------------

void CTrajectoryList::ClearTrajectories(void)
{
    BeginUpdate();

    foreach(QObject* p_object,children()){
        delete p_object;
    }

    EndUpdate();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectory* CTrajectoryList::GetActiveTrajectory(void) const
{
    return(ActiveTrajectory);
}

//------------------------------------------------------------------------------

int CTrajectoryList::GetNumberOfTrajectories(void) const
{
    return(children().size());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectoryList::LoadData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "trajectories" ) {
        LOGIC_ERROR("p_ele is not trajectories");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    // load data ------------------------------------
    CXMLElement* p_ael;
    p_ael = p_ele->GetFirstChildElement("trajectory");
    while( p_ael != NULL ) {
        CreateTrajectory(p_ael);
        p_ael = p_ael->GetNextSiblingElement("trajectory");
    }
}

//------------------------------------------------------------------------------

void CTrajectoryList::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "trajectories" ) {
        LOGIC_ERROR("p_ele is not trajectories");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    // save data ------------------------------------
    foreach(QObject* p_qobj,children()) {
        CTrajectory* p_str = static_cast<CTrajectory*>(p_qobj);
        CXMLElement* p_sel = p_ele->CreateChildElement("trajectory");
        p_str->SaveData(p_sel);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectoryList::BeginUpdate()
{
    if( UpdateLevel == 0 ){
        blockSignals(true);
    }
    UpdateLevel++;
}

//------------------------------------------------------------------------------

void CTrajectoryList::EndUpdate(void)
{
    if( UpdateLevel == 0 ) return;
    UpdateLevel--;
    if( UpdateLevel == 0 ){
        blockSignals(false);
        if( Changed ){
            emit OnTrajectoryListChanged();
            Changed = false;
        }
    }
}

//------------------------------------------------------------------------------

void CTrajectoryList::EmitOnTrajectoryListChanged(void)
{
    if( UpdateLevel > 0 ) Changed = true;
    emit OnTrajectoryListChanged();
}

//------------------------------------------------------------------------------

void CTrajectoryList::EmitOnSnapshotChanged(void)
{
    emit OnSnapshotChanged();
}

//------------------------------------------------------------------------------

void CTrajectoryList::ListSizeChanged(void)
{
    if( UpdateLevel > 0 ){
        Changed = true;
        return;
    }
    EmitOnTrajectoryListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
