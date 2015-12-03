// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Tomas Marek, xmarek11@stud.fit.vutbr.cz,
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

#include <ProjectList.hpp>
#include <Project.hpp>
#include <ProjectStatusBar.hpp>
#include <JobScheduler.hpp>
#include <Graphics.hpp>
#include <GraphicsSetupProfile.hpp>
#include <GraphicsPrimaryView.hpp>

#include "BuildProjectWindow.hpp"
#include "BuildProjectWindow.moc"
#include "BuildProject.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBuildProjectWindow::CBuildProjectWindow(CBuildProject* p_project)
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
    setCentralWidget(OpenGL);

    // status bar
    ProjectStatusBar = new CProjectStatusBar(this,Project);
    setStatusBar(ProjectStatusBar);
}

//------------------------------------------------------------------------------

CBuildProjectWindow::~CBuildProjectWindow(void)
{

}

//------------------------------------------------------------------------------

void CBuildProjectWindow::ConnectAllMenu(void)
{
    // restore desktop was already called
    ConnectFileMenu();
    ConnectEditMenu();
    ConnectSelectionMenu();
    ConnectStructureMenu();
    ConnectGeometryMenu();
    ConnectPropertiesMenu();
    ConnectGraphicsMenu();
    ConnectMouseMenu();
    ConnectSetupMenu();
    ConnectHelpMenu();
}

//------------------------------------------------------------------------------

void CBuildProjectWindow::keyPressEvent(QKeyEvent* p_event)
{
    if( p_event->key() == Qt::Key_Escape ){
        JobScheduler->TerminateProjectRunningJobs(Project);
    }
    CMainWindow::keyPressEvent(p_event);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



