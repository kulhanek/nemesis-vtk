// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <QtGui>
#include <vector>
#include <string>
#include <utility>
#include <QMessageBox>

#include <HistoryList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <PluginDatabase.hpp>
#include <PluginObject.hpp>
#include <WorkPanel.hpp>
#include <WorkPanelList.hpp>
#include <ErrorSystem.hpp>
#include <QMessageBox>
#include <ContainerModel.hpp>

#include "StandardWorkPanelsModule.hpp"

#include <Structure.hpp>
#include <StructureList.hpp>
#include <Trajectory.hpp>
#include <TrajectoryList.hpp>

#include "DockingWorkPanel.moc"
#include "DockingWorkPanel.hpp"
#include <PODesignerDockingTabsResults.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DockingWorkPanelCB(void* p_data);

CExtUUID        DockingWorkPanelID(
                    "{DOCKING_WORK_PANEL:10ee3f51-9e18-45c0-827c-5d65f161c535}",
                    "Results of Docking Panel");

CPluginObject   DockingWorkPanelObject(&StandardWorkPanelsPlugin,
                     DockingWorkPanelID,WORK_PANEL_CAT,
                     ":images/StandardWorkPanels/StructureBuildStructure.svg",
                     DockingWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* DockingWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CDockingWorkPanel requires active project");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(DockingWorkPanelID,p_project);
    if( p_wp == NULL ) {
        p_wp = new CDockingWorkPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDockingWorkPanel::CDockingWorkPanel(CProject* p_project)
    : CWorkPanel(&DockingWorkPanelObject,p_project,EWPR_TOOL)
{
    // set up ui file
    WidgetUI.setupUi(this);

    //----------------
    connect(GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // handle with new trajectory
    //----------------
    connect(GetProject()->GetTrajectories(), SIGNAL(OnTrajectoryListChanged(void)),
            this,SLOT(InitTabs(void)));
    //----------------
    connect(GetProject()->GetTrajectories(), SIGNAL(OnChildContainerRemoved(QObject*)),
            this,SLOT(InitTabs(void)));
    //----------------
    connect(GetProject()->GetTrajectories(),SIGNAL(OnSnapshotChanged(void)),
            this,SLOT(InitTabs(void)));
    //----------------
    connect(WidgetUI.tabWidgetDocking,SIGNAL(currentChanged(int)),
            this, SLOT(ChangeTab(int)));

    NumberOfTabs = 0;
    OnInitTabChanging = true;
    TabInit = false;
    TabFinding = false;
    TabChanging = false;

    // init tabs
    InitTabs();
    // load work panel setup
    LoadWorkPanelSetup();

    // update locks
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    OnInitTabChanging = false;
}

// -----------------------------------------------------------------------------

CDockingWorkPanel::~CDockingWorkPanel(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingWorkPanel::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;

    setEnabled( ! GetProject()->GetHistory()->IsLocked(EHCL_TOPOLOGY));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingWorkPanel::InitTabs(void)
{
    // init and add tabs for every trajectory which has structure and segment
    if (TabInit) {return;}

    TabInit = true; 
    UpdateTabsAfterChange();

    int n_traj = GetProject()->GetTrajectories()->GetNumberOfTrajectories();

    for (int i = 0; i < n_traj; i++)
    {
        CurrentTrajectory = GetTrajectoryOfIndex(i);
        // init segment model for current trajectory
        SetCurrentSegmentModel();

        // number of segments in trajectory
        int n_segments = CurrentTrajectory->GetNumberOfSegments();
        if (n_segments == 0) {
            continue;
        }
        int n_stored_segments = GetNumberOfStoredSegmentsOfCurrentTraj();
        if (n_stored_segments == n_segments) {
            continue;
        }
        CTrajectorySegment* p_seg = NULL;
        for (int j = 0; j < n_segments; j++) {
            bool have_seg = false;
            p_seg = GetSegmentByRow(j);
            if( p_seg == NULL ){
                continue;
            }

            // search for segment in tabs
            for (int k = 0; k < NumberOfTabs;k++) {
                // check only tabs in current segment model
                CTrajectorySegment* p_seg_stored = GetSegmentFromTab(k);
                if( p_seg_stored == NULL ) continue;
                if (CurrentTrajectory != p_seg_stored->GetTrajectory()) {
                    continue;
                }
                if( p_seg_stored == p_seg){
                    have_seg = true;
                    break;
                }
            }
            if (!have_seg) {
            // add new tab with segment
                AddTab(p_seg);
            }
        }
    }

    TabInit = false;
}

//------------------------------------------------------------------------------

int CDockingWorkPanel::AddTab(CTrajectorySegment* p_seg)
{
    // metod to add new tab for segment of index

    // index is from 1 correspond found not assign segments to tab
    int seg_indx = p_seg->GetSeqIndex();
    // structure must have assign trajectory
    CStructure* curr_str = p_seg->GetTrajectory()->GetStructure();

    CPODesignerDockingTabsResults* resultsTab = new CPODesignerDockingTabsResults (WidgetUI.tabWidgetDocking,p_seg,this);
    resultsTab->InitValues();
    // set name of tab - contain name of trajectory and order of segment
    QString tab_seg_header = tr("%1%2");
    tab_seg_header = tab_seg_header.arg(curr_str->GetName());
    if (seg_indx == 1) {
        tab_seg_header = tab_seg_header.arg("");
    } else {
        QString number_seg = tr(".%1");
        number_seg = number_seg.arg(seg_indx);
        tab_seg_header = tab_seg_header.arg(number_seg);
    }
    NumberOfTabs++;
    // add new tab into tabwidget with new results
    int tab_indx = WidgetUI.tabWidgetDocking->addTab(resultsTab,tab_seg_header);
    return (tab_indx);
}

//------------------------------------------------------------------------------

void CDockingWorkPanel::ChangeTab(int index)
{
    // metod handle with user click on tab
    // change tab view
    if (TabChanging) {return;}
    // protect from init call - and move with segments when we dont want it
    if (OnInitTabChanging) {
        return;
    }

    TabChanging = true;
    // we need activate segment, and his structure is activate when he is save in current index
    if (( 0 <= index) && (index < NumberOfTabs) ) {
        CTrajectorySegment* p_seg = GetSegmentFromTab(index);
        if( p_seg == NULL ) return;
        CTrajectory* p_traj = p_seg->GetTrajectory();
        if( p_traj == NULL ) return;

        // segments are indexed from 1
        p_traj->MoveToSegment(p_seg->GetSeqIndex());
        // we can have more than one trajectory
        // activate moved structure
        GetProject()->GetStructures()->SetActiveStructure(p_traj->GetStructure() );
    }
    TabChanging = false;
}

//------------------------------------------------------------------------------

void CDockingWorkPanel::UpdateTabsAfterChange(void)
{
    // metod find segment which was deleted and handle with this change in Docking panel
    if (TabFinding) {return;}

    // protection from call in call and twice erase
    TabFinding = true;
    int n_tabs = NumberOfTabs;
    std::vector<int> tabs_to_remove;

    // for saved widgets in tabs
    for (int i = 0; i < n_tabs;i++) {
        CTrajectorySegment* p_seg = GetSegmentFromTab(i);
        if( p_seg == NULL )
        {
            tabs_to_remove.push_back(i);
        } else if (p_seg->GetTrajectory() == NULL) {
            tabs_to_remove.push_back(i);
        }
    }
    // QTabWiget has resorting function after remove so i must at first find tabs for
    // remove and than remove from bigger index to lower. Then i use FILO = stack
    while (!tabs_to_remove.empty())
      {
        RemoveTab(tabs_to_remove.back() );
        tabs_to_remove.pop_back();
      }

    TabFinding = false;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingWorkPanel::RemoveTab(int i)
{
    // if we want erase record, we must handle with several problems
    // remove widget and tab
    CPODesignerDockingTabsResults * p_tab_widget =
            static_cast<CPODesignerDockingTabsResults*>
            (WidgetUI.tabWidgetDocking->widget(i) );
    WidgetUI.tabWidgetDocking->removeTab(i);
    p_tab_widget = NULL;
    delete p_tab_widget;
    NumberOfTabs--;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingWorkPanel::SetCurrentSegmentModel(void)
{
    // method set segment model on current trajectory
    // it is important fo change between different results == different trajectories
    SegmentsModel = CurrentTrajectory->GetContainerModel(this,"segments");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingWorkPanel::SetCurrentTrajectoryByID(int ID)
{
    // metod set trajectory of ID index to view
    CContainerModel* p_traj_model = GetProject()->GetTrajectories()->GetContainerModel(this);
    QModelIndexList matches = p_traj_model->match( p_traj_model->index(0,4), Qt::DisplayRole, ID );
    // take first found element - but it cant't find two
    QModelIndex index = matches.at(0);
    CTrajectory* p_traj = dynamic_cast<CTrajectory*>( p_traj_model->GetItem(index) );
    if (CurrentTrajectory == p_traj) {
        return;
    }
    CurrentTrajectory = p_traj;
}

//------------------------------------------------------------------------------

CTrajectory* CDockingWorkPanel::GetTrajectoryOfIndex(int indx)
{
    // metod to get trajectory on index row in trajectory model
    CContainerModel* p_traj_model = GetProject()->GetTrajectories()->GetContainerModel(this);
    QModelIndex index = p_traj_model->index(indx,0);
    CTrajectory* p_traj = dynamic_cast<CTrajectory*>( p_traj_model->GetItem(index) );
    return (p_traj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CDockingWorkPanel::GetNumberOfStoredSegmentsOfCurrentTraj(void)
{
    // method get number of already stored segments in Tabs of current trajectory
    int indx_curr_traj = CurrentTrajectory->GetIndex();

    int n = 0;
    // for saved position in tabs
    for (int i = 0; i < NumberOfTabs;i++) {
        CTrajectorySegment* p_seg = GetSegmentFromTab(i);
        if (p_seg == NULL) continue;
        CTrajectory* p_traj = p_seg->GetTrajectory();
        if (p_traj == NULL) continue;
        if ( indx_curr_traj  == p_traj->GetIndex() ){
            n++;
        }
    }
    return (n);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectorySegment* CDockingWorkPanel::GetSegmentFromTab(int index)
{
    // get segment from tab
    CPODesignerDockingTabsResults * p_tab_widget =
                        static_cast<CPODesignerDockingTabsResults*>
                        (WidgetUI.tabWidgetDocking->widget(index) );
    if (p_tab_widget == NULL) return (NULL);
    CTrajectorySegment* p_seg = p_tab_widget->GetTrajectorySegment();
    return (p_seg);
}

//------------------------------------------------------------------------------

CTrajectorySegment* CDockingWorkPanel::GetSegmentByRow(int row)
{
    // metod for get segment of trajectory in segment view by Row

    // search for segment with indx in segment model
    // indexes are from 1, but rows and comlumns from 0
    if (row < 0) {return (NULL);}
    QModelIndex index = SegmentsModel->index(row,0);
    CTrajectorySegment* p_seg = dynamic_cast<CTrajectorySegment*>( SegmentsModel->GetItem(index) );
    return (p_seg);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectorySegment* CDockingWorkPanel::GetSegmentByID(int traj_id, int seq_id)
{
    // metod for get segment of trajectory in current trajectory by ID

    // if we have set traj_indx for trajectory, we will set view on it
    if (traj_id)
    {
        SetCurrentTrajectoryByID(traj_id);
        SetCurrentSegmentModel();
    }
    // search for segment with indx in segment model
    // indexes are from 1, but rows and comlumns from 0
    // we need 5th column where is saved real ID of segment
    QModelIndexList matches = SegmentsModel->match( SegmentsModel->index(0,5), Qt::DisplayRole, seq_id );

    // if not found then end
    if (matches.isEmpty()) {
        return (NULL);
    }
    // take first found element - but it cant't find two

    QModelIndex index = matches.at(0);

    CTrajectorySegment* p_seg = dynamic_cast<CTrajectorySegment*>( SegmentsModel->GetItem(index) );

    return (p_seg);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CDockingWorkPanel::FindStartID(int n_seg)
{
    // metod finds start ID of segment in segment view
    int start_id = -1;
    for (int i = 0; i < n_seg;i++) {
        CTrajectorySegment* p_seg = GetSegmentByRow(i);
        if (p_seg != NULL) {
            // if segment ID is smaller then ID of current p_seg, i will save new one
            int index = p_seg->GetIndex();
            if (start_id > index) {
                start_id = index;
            }
        } else {
           start_id = -1;
           break;
        }
    }
    return (start_id);
}

//------------------------------------------------------------------------------

int CDockingWorkPanel::FindEndID(int n_seg)
{
    // metod finds start ID of segment in segment view
    int end_id = -1;
    for (int j = 0; j < n_seg;j++) {
        CTrajectorySegment* p_seg = GetSegmentByRow(j);
        if (p_seg != NULL) {
            // if segment ID is bigger then ID of current p_seg, i will save new one
            int index = p_seg->GetIndex();
            if (end_id < index) {
                end_id = index;
            }
        } else {
           end_id = -1;
           break;
        }
    }
    return (end_id);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
