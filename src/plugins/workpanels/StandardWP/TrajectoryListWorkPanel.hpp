#ifndef TrajectoryListWorkPanelH
#define TrajectoryListWorkPanelH
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

#include <WorkPanel.hpp>
#include <HistoryList.hpp>
#include "ui_TrajectoryListWorkPanel.h"

//------------------------------------------------------------------------------

class CTrajectoryList;

//------------------------------------------------------------------------------

class CTrajectoryListWorkPanel : public CWorkPanel {
Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CTrajectoryListWorkPanel(CProject* p_owner);
    ~CTrajectoryListWorkPanel(void);

// section of private data ----------------------------------------------------
private:
    Ui::TrajectoryListWorkPanel WidgetUI;
    CTrajectoryList*            List;
    CContainerModel*            ObjectsModel;

    /// raised when window is docked/undocked from the main window
    virtual void WindowDockStatusChanged(bool docked);

private slots:
    void ProjectNameChanged(CProject* p_project);
    void OpenProjectDesigner(void);
    void ProjectLockChanged(EHistoryChangeMessage message);

    void ObjectsTVSelectionChanged(void);
    void ObjectsTVDblClicked(const QModelIndex& index);

    void NewTrajectory(void);
    void ActivateTrajectory(void);
    void RemoveTrajectory(void);
    void TrajectoryInfo(void);
};

//------------------------------------------------------------------------------

#endif
