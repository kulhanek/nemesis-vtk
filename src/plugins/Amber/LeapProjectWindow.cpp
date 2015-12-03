// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include "LeapProjectWindow.hpp"
#include <ProjectList.hpp>
#include <FileName.hpp>
#include <XMLDocument.hpp>
#include <XMLParser.hpp>
#include <XMLPrinter.hpp>
#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <FileSystem.hpp>
#include <XMLIterator.hpp>
#include <XMLElement.hpp>
#include <QCloseEvent>
#include <Project.hpp>
#include <WorkPanelList.hpp>
#include <DesktopSystem.hpp>
#include <TerminalWidget.hpp>

#include "LeapProjectWindow.moc"
#include "LeapProject.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CLeapProjectWindow::CLeapProjectWindow(CLeapProject* p_project)
    : CMainWindow(p_project)
{
    Project = p_project;

    // init ui part
    WidgetUI.setupUi(this);

    HistoryList = Project->GetHistory();

    // setup terminal
    Terminal = new CTerminalWidget(this,"[leap]$ ");
    setCentralWidget(Terminal);
    connect(Terminal,SIGNAL(OnNewLineEntered(QString)),
            this,SLOT(NewLineEntered(QString)));
    Project->Output.SetDevice(Terminal);
}

//------------------------------------------------------------------------------

CLeapProjectWindow::~CLeapProjectWindow(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CLeapProjectWindow::ConnectAllMenu(void)
{
    // restore desktop was already called
    ConnectFileMenu();
    ConnectEditMenu();
    ConnectSetupMenu();
    ConnectHelpMenu();
}

//------------------------------------------------------------------------------

void CLeapProjectWindow::NewLineEntered(const QString& line)
{
    Project->LeapContext.Process(line.toStdString());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



