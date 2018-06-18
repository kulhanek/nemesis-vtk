#ifndef DockingWorkPanelH
#define DockingWorkPanelH
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

#include <WorkPanel.hpp>
#include <Trajectory.hpp>
#include <TrajectorySegment.hpp>
#include "ui_DockingWorkPanel.h"

// -----------------------------------------------------------------------------

class CTrajectorySegment;
class CContainerModel;
class CPODesignerDockingTabsResults;

//------------------------------------------------------------------------------

class CDockingWorkPanel : public CWorkPanel {
    Q_OBJECT
public:
    // constructor and destructors ------------------------------------------------
    CDockingWorkPanel(CProject* p_project);
    ~CDockingWorkPanel();

    // section of private data ----------------------------------------------------
private:
    CHistoryNode*             History;
    Ui::DockingWorkPanel      WidgetUI;

    int                       NumberOfTabs;

    CTrajectory*              CurrentTrajectory;
    CContainerModel*          SegmentsModel;

    bool                      OnInitTabChanging;
    bool                      TabInit;
    bool                      TabFinding;
    bool                      TabChanging;

    // add new tab for segment
    int                AddTab(CTrajectorySegment* p_seg);
    // remove tab for segment on index of tab
    void               RemoveTab(int i);
    // activate segment model on current trajectory
    void               SetCurrentSegmentModel(void);
    // set trajectory to view
    void               SetCurrentTrajectoryByID(int ID);
    // get number of stored segment of current trajectory
    int                GetNumberOfStoredSegmentsOfCurrentTraj(void);
    // get trajectory on index row in trajectory model
    CTrajectory*       GetTrajectoryOfIndex(int indx);
    // get segment from tab
    CTrajectorySegment* GetSegmentFromTab (int indx);
    // get segment of trajectory segment view by SI
    CTrajectorySegment* GetSegmentByRow(int row);
    // get segment of trajectory in current trajectory by ID
    CTrajectorySegment* GetSegmentByID(int traj_id, int seq_id);
    // finds start ID of segment in segment view for n segments
    int                FindStartID(int n_seg);
    // finds end ID of segment in segment view for n segments
    int                FindEndID(int n_seg);

private slots:    
    void ProjectLockChanged(EHistoryChangeMessage message);
    // init and add tabs
    void InitTabs(void);
    // change tab view after click
    void ChangeTab(int index);
    // update tabs after change in trajectories and segments
    // will delete tab if was delete trajectory
    void UpdateTabsAfterChange(void);

};

// -----------------------------------------------------------------------------

#endif
