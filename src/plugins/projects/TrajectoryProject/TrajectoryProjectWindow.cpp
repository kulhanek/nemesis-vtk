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

#include <ProjectList.hpp>
#include <FileName.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <ProjectStatusBar.hpp>
#include <JobScheduler.hpp>
#include <Graphics.hpp>
#include <GraphicsSetupProfile.hpp>
#include <GraphicsPrimaryView.hpp>

#include "TrajectoryProjectWindow.hpp"
#include "TrajectoryProject.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectoryProjectWindow::CTrajectoryProjectWindow(CTrajectoryProject* p_project)
    : CMainWindow(p_project)
{
    // init ui part
    WidgetUI.setupUi(this);

    // shortcuts
    Project = p_project;

    // setup OpenGL area
    OpenGL = new CGraphicsPrimaryView(this);
    OpenGL->setContextMenuPolicy(Qt::PreventContextMenu);
    OpenGL->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    OpenGL->SetGraphicsView(Project->GetGraphics()->GetPrimaryView());
    QVBoxLayout* p_layout = new QVBoxLayout;
    p_layout->setContentsMargins(0,0,0,0);
    p_layout->addWidget(OpenGL);
    WidgetUI.centralW->setLayout(p_layout);

    // status bar
    ProjectStatusBar = new CProjectStatusBar(this,Project);
    setStatusBar(ProjectStatusBar);

    // trajectory action
    actionActiveTrajectoryInfo = NULL;
    actionAllTrajectoriesInfo = NULL;
    actionFirstSnapshot = NULL;
    actionPrevSnapshot = NULL;
    actionNextSnapshot = NULL;
    actionLastSnapshot = NULL;
    actionFirstSegment = NULL;
    actionPrevSegment = NULL;
    actionNextSegment = NULL;
    actionLastSegment = NULL;
    actionPlay = NULL;
    actionPause = NULL;
    actionStop = NULL;
    actionManageTrajectories = NULL;

    menuImportTrajectory = NULL;

    SliderUpdating = false;
    SpinBoxUpdating = false;
}

//------------------------------------------------------------------------------

CTrajectoryProjectWindow::~CTrajectoryProjectWindow(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectoryProjectWindow::ConnectAllMenu(void)
{
    // restore desktop was already called
    ConnectFileMenu();
    ConnectEditMenu();
    ConnectSelectionMenu();
    ConnectStructureMenu();
    ConnectGeometryMenu();
    ConnectTrajectoryMenu();
    ConnectPropertiesMenu();
    ConnectGraphicsMenu();
    ConnectMouseMenu();
    ConnectSetupMenu();
    ConnectHelpMenu();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



