#ifndef TrajectoryProjectWindowH
#define TrajectoryProjectWindowH
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

#include <MainWindow.hpp>
#include "ui_TrajectoryProjectWindow.h"

//------------------------------------------------------------------------------

class CTrajectoryProject;
class CProjectStatusBar;
class CGraphicsPrimaryView;

//------------------------------------------------------------------------------

class CTrajectoryProjectWindow : public CMainWindow {
    Q_OBJECT
public:
// constructor and destructors -------------------------------------------------
    CTrajectoryProjectWindow(CTrajectoryProject* p_project);
    virtual ~CTrajectoryProjectWindow(void);

    /// connect all menu - do not call it in constructor
    // it requires restored desktop
    void ConnectAllMenu(void);

// TRAJECTORY ==================================================================
private:
    bool SliderUpdating;
    bool SpinBoxUpdating;

    void ConnectTrajectoryMenu(void);

private slots:
    void UpdateTrajectoryMenu(void);
    void SetupImportTrajectoryMenu(void);
    void CreateTrajImpexAction(QActionGroup* p_actGroup,QMenu* p_menu,CPluginObject* p_pod);
    void ImportTrajectoryAction(QAction* p_act);

    void ActiveTrajectoryInfo(void);
    void AllTrajectoriesInfo(void);
    void FirstSnapshot(void);
    void PrevSnapshot(void);
    void NextSnapshot(void);
    void LastSnapshot(void);
    void FirstSegment(void);
    void PrevSegment(void);
    void NextSegment(void);
    void LastSegment(void);
    void Play(void);
    void Pause(void);
    void Stop(void);
    void ManageTrajectories(void);

    void SliderValueChanged(int pos);
    void SpinBoxValueChanged(int pos);

private:
    QAction* actionActiveTrajectoryInfo;
    QAction* actionAllTrajectoriesInfo;
    QAction* actionFirstSnapshot;
    QAction* actionPrevSnapshot;
    QAction* actionNextSnapshot;
    QAction* actionLastSnapshot;
    QAction* actionFirstSegment;
    QAction* actionPrevSegment;
    QAction* actionNextSegment;
    QAction* actionLastSegment;
    QAction* actionPlay;
    QAction* actionPause;
    QAction* actionStop;
    QAction* actionManageTrajectories;

    QMenu* menuImportTrajectory;

// =============================================================================
// section of private data -----------------------------------------------------
private:
    CTrajectoryProject*             Project;
    CGraphicsPrimaryView*           OpenGL;
    CProjectStatusBar*              ProjectStatusBar;
    Ui::TrajectoryProjectWindow     WidgetUI;

    friend class CTrajectoryProject;
};

//------------------------------------------------------------------------------

#endif
