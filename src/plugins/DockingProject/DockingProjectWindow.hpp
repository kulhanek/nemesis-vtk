#ifndef DockingProjectWindowH
#define DockingProjectWindowH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Tomas Marek, xmarek11@stud.fit.vutbr.cz,
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <MainWindow.hpp>
#include "ui_DockingProjectWindow.h"

//------------------------------------------------------------------------------

class CDockingProject;
class CProjectStatusBar;
class CGraphicsPrimaryView;

//------------------------------------------------------------------------------

class CDockingProjectWindow : public CMainWindow {
    Q_OBJECT
public:
// constructor and destructors -------------------------------------------------
    CDockingProjectWindow(CDockingProject* p_project);
    virtual ~CDockingProjectWindow(void);

    /// connect all menu - do not call it in constructor
    // it requires restored desktop
    void ConnectAllMenu(void);

public slots:
    // panel
    void DockWP(void);

    // panel
    void DockingWP(void);

    // add new structure with trajectory for results
    void AddResults(void);

// section of private data -----------------------------------------------------
private:
    CDockingProject*            Project;
    CGraphicsPrimaryView*       OpenGL;
    CProjectStatusBar*          ProjectStatusBar;
    Ui::DockingProjectWindow    WidgetUI;

    // connect docking menu
    void ConnectDockingMenu(void);

    // dock panel
    QAction* actionDockWP;

    // results panel
    QAction* actionDockingWP;

    // add trajectory for results
    QAction* actionAddResults;


// events -----------------------------------
    // process ESC key to stop project running jobs
    virtual void keyPressEvent(QKeyEvent* p_event);

private slots:
    // handle with change active structure - this will be change active trajectory too
    void ChangeActiveTrajectory(void);

};

//------------------------------------------------------------------------------

#endif
