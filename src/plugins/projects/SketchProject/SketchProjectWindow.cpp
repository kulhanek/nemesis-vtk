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

#include "SketchProjectWindow.hpp"
#include <ProjectList.hpp>
#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <QCloseEvent>
#include <QWebFrame>
#include <Project.hpp>
#include <WorkPanelList.hpp>
#include <DesktopSystem.hpp>
#include <ProjectStatusBar.hpp>
#include <JobScheduler.hpp>

#include "SketchProjectJSObject.hpp"
#include "SketchProject.hpp"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------

#define KETCHER_VERSION "1.0.b4"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSketchProjectWindow::CSketchProjectWindow(CSketchProject* p_project)
    : CMainWindow(p_project)
{
    // init ui part
    WidgetUI.setupUi(this);

    // shortcuts
    Project = p_project;
    HistoryList = Project->GetHistory();

    // setup Ketcher area
    WebView = new QWebView(this);
    // load page
    CFileName ketcher_index = GlobalSetup->GetSystemSharePath() / "ketcher" / KETCHER_VERSION / "ketcher.html";
    QUrl url = QUrl::fromLocalFile(QString(ketcher_index));

    if( ! url.isValid() ){
        ES_ERROR("url is not valid");
    }

    setCentralWidget(WebView);
    WebView->load(url);
    // attach nemesis JS object
    JSObject = new CSketchProjectJSObject(this);
    WebView->page()->mainFrame()->addToJavaScriptWindowObject("nemesis",JSObject);
    // no scrollbars
    WebView->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    WebView->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    WebView->setContextMenuPolicy(Qt::CustomContextMenu);

    // status bar
    ProjectStatusBar = new CProjectStatusBar(this,Project);
    setStatusBar(ProjectStatusBar);
}

//------------------------------------------------------------------------------

CSketchProjectWindow::~CSketchProjectWindow(void)
{

}

//------------------------------------------------------------------------------

void CSketchProjectWindow::ConnectAllMenu(void)
{
    // restore desktop was already called
    ConnectFileMenu();
    ConnectEditMenu();
    ConnectStructureMenu();
    ConnectSetupMenu();
    ConnectHelpMenu();
}

//------------------------------------------------------------------------------

void CSketchProjectWindow::LoadData(const QString& smiles)
{
    // layout data
    QString cleaned_mol = JSObject->cleanProject(smiles);

    // update view
    QStringList script;
    script << "var data=[";

    QStringList molsep = cleaned_mol.split("\n");
    for(int i =0; i < molsep.size(); i++){
        if( i+1 != molsep.size() ){
            script << "'" + molsep[i] + "',";
        } else {
            script << "'" + molsep[i] + "'";
        }
    }
    script << "];";
    script << "ui.loadMolecule(data.join('\\n'),false);";

    QWebFrame* p_mf = WebView->page()->mainFrame();
    p_mf->evaluateJavaScript(script.join("\n"));
}

//------------------------------------------------------------------------------

const QString CSketchProjectWindow::SaveData(void)
{
    // save structure as SMILES
    QWebFrame* p_mf = WebView->page()->mainFrame();
    QVariant result = p_mf->evaluateJavaScript("ui.saveAsSMILES();");

    return(result.toString());
}

//------------------------------------------------------------------------------

void CSketchProjectWindow::keyPressEvent(QKeyEvent* p_event)
{
    if( p_event->key() == Qt::Key_Escape ){
        JobScheduler->TerminateProjectRunningJobs(Project);
    }
    CMainWindow::keyPressEvent(p_event);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



