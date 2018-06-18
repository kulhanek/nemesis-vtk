// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <stdio.h>
#include <stdlib.h>

#include <ErrorSystem.hpp>
#include <SmallTimeAndDate.hpp>
#include <Terminal.hpp>

#include <NemesisJScript.hpp>

#include <QTextStream>
#include <QFile>
#include <QFileInfo>

#include <WelcomeWindow.hpp>
#include <AboutWindow.hpp>

#include <GlobalSetup.hpp>
#include <PluginDatabase.hpp>

#include <iostream>

#include <JScriptFunctions.hpp>
#include <RootList.hpp>
#include <GlobalObjectList.hpp>
#include <PhysicalQuantities.hpp>
#include <WorkPanelList.hpp>
#include <GlobalDesktop.hpp>
#include <ProjectList.hpp>
#include <GraphicsSetupProfile.hpp>
#include <ElementColorsList.hpp>
#include <InserterList.hpp>
#include <JobScheduler.hpp>
#include <OptimizerSetupList.hpp>
#include <Project.hpp>
#include <RecentFileList.hpp>
#include <QMessageBox>
#include <BatchJobList.hpp>

#include <MouseDriverSetup.hpp>

#include "openbabel/plugin.h"

using namespace std;

//------------------------------------------------------------------------------

CNemesisJScript*    NemesisJScript = NULL;
CTerminalOStream    tout;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

class CNemesisPlugNotification : public OpenBabel::OBPlugNotification {
public:
    CNemesisPlugNotification(CWelcomeWindow* p_pw,const QString& base_text);
    virtual ~CNemesisPlugNotification(){}
    virtual void notify(const std::string& text);
private:
   CWelcomeWindow*  ProgressWindow;
   QString          BaseText;
};

//------------------------------------------------------------------------------

CNemesisPlugNotification::CNemesisPlugNotification(CWelcomeWindow* p_pw,const QString& base_text)
{
    ProgressWindow = p_pw;
    BaseText = base_text;
}

//------------------------------------------------------------------------------

void CNemesisPlugNotification::notify(const std::string& text)
{
    if( ProgressWindow ){
        QString file(text.c_str());
        QFileInfo finfo(file);
        QString msg;
        msg = BaseText.arg(finfo.baseName());
        ProgressWindow->ChangeNotification(msg);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNemesisJScript::CNemesisJScript(void)
{
    NemesisJScript = this;
    InitStatus = SO_CONTINUE;
}

//------------------------------------------------------------------------------

CNemesisJScript::~CNemesisJScript(void)
{
    NemesisJScript = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CNemesisJScript::Init(int argc,char* argv[])
{
    // encode program options
    int result = NemesisOptions.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    PrintWelcomeText();

    qApp->setWindowIcon(QIcon(":/images/NemesisCore/MainIcon.svg"));

    // initialize terminal output device to standard output stream
    tout.SetDevice(&cout);

    // init subsystems
    result = InitSubsystems();

    emit SetupLevelChanged("All done.");

    InitStatus = result;
    return( result );
}

//------------------------------------------------------------------------------

bool CNemesisJScript::Run(void)
{
    // run GUI event loop - only when some project is opened
    bool result = true;
    if( Projects->NumberOfProjects() > 0 ){
        result = QApplication::exec() != 0;
    }
    return(result);
}

//------------------------------------------------------------------------------

void CNemesisJScript::Finalize(void)
{
    if( InitStatus == SO_CONTINUE ){
        // save only if initialization was successfull
        FinalizeSubsystems();
    }

    if( hasUncaughtException() == true ) {
        QScriptValue evalue = uncaughtException();
        QTextStream stream(stderr);
        stream << "nemesis: line " << uncaughtExceptionLineNumber() << " - " << evalue.toString() << endl;
    }

    PrintFinalText();

    if( ErrorSystem.IsError() || NemesisOptions.GetOptVerbose() ){
        ErrorSystem.PrintErrors(stderr);
    }

    cout << endl;
}

//------------------------------------------------------------------------------

void CNemesisJScript::PostCloseEvent(void)
{
    emit OnPostCloseEvent();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CNemesisJScript::PrintWelcomeText(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    CSmallString time_and_date;
    time_and_date = dt.GetSDateAndTime();

    cout << endl;
    cout << "# ==============================================================================" << endl;
    cout << "#                     NEMESIS - Molecular Modelling Package                     " << endl;
    cout << "#                            Graphical User Interface                           " << endl;
    cout << "# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "# Started at : " << time_and_date << endl;
    cout << "# ==============================================================================" << endl;
}

//------------------------------------------------------------------------------

void CNemesisJScript::PrintFinalText(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    CSmallString time_and_date;
    time_and_date = dt.GetSDateAndTime();

    cout << endl;
    cout << "# ==============================================================================" << endl;
    cout << "# Finished at : " << time_and_date << endl;
    cout << "# ==============================================================================" << endl;

    int numofobjects = PluginDatabase.GetNumberOfOpenedObjects();
    cout << "PLUSULA: Opened objects = " << numofobjects << endl;
}

//------------------------------------------------------------------------------

int CNemesisJScript::InitSubsystems(void)
{
    QApplication::setWindowIcon(QIcon(":/images/NemesisCore/MainIcon.svg"));

    CWelcomeWindow* p_ww = NULL;

    // create welcome window
    if( ! NemesisOptions.GetOptNoGUI() ) {
        p_ww = new CWelcomeWindow();
        p_ww->SetMaxProgressSteps(9);
        connect(this,SIGNAL(SetupLevelChanged(const QString&)),p_ww,SLOT(IncrementProgress(const QString&)));
        connect(this,SIGNAL(SetupNotificationChanged(const QString&)),p_ww,SLOT(ChangeNotification(const QString&)));
    }

    emit SetupLevelChanged(tr("Global objects ...."));
    GlobalSetup = new CGlobalSetup();
    GlobalSetup->Init();

    if( NemesisOptions.GetOptVerbose() ){
        GlobalSetup->PrintSetup();
    }

    RootList = new CRootList(NULL);
    Projects = new CProjectList(RootList);
    GlobalObjectList = new CGlobalObjectList(RootList);
    GraphicsSetupProfile = new CGraphicsSetupProfile(GlobalObjectList);
    WorkPanels = new CWorkPanelList(GlobalObjectList);
    PhysicalQuantities = new CPhysicalQuantities(GlobalObjectList);
    Inserters = new CInserterList(GlobalObjectList);
    OptimizerSetups = new COptimizerSetupList(GlobalObjectList);
    JobScheduler = new CJobScheduler(GlobalObjectList);
    GlobalDesktop = new CGlobalDesktop(GlobalObjectList);
    MouseDriverSetup = new CMouseDriverSetup(GlobalObjectList);
    RecentFiles = new CRecentFileList(GlobalObjectList);
    BatchJobs = new CBatchJobList(GlobalObjectList);

    // events
    connect(this,SIGNAL(OnPostCloseEvent(void)),
            this,SLOT(QuitApplication()),Qt::QueuedConnection);

    // register JScript objects
    RegisterObjects();

    emit SetupLevelChanged(tr("Loading Nemesis plugins ...."));
    connect(&PluginDatabase,SIGNAL(PluginLoaded(const QString&)),this,SLOT(PluginLoaded(const QString&)));
    PluginDatabase.SetPluginPath(GlobalSetup->GetPluginsLocation());
    PluginDatabase.LoadPlugins(GlobalSetup->GetPluginsSetup());

    // do we have any plugin loaded?
    if( PluginDatabase.GetNumberOfLoadedModules() == 0 ){
        ES_ERROR("no plugin modules are loaded");
        QMessageBox::critical(NULL,tr("Error"),
                              tr("No Nemesis plugins are loaded! Did you correctly install or activate Nemesis plugins?"),
                              QMessageBox::Ok,QMessageBox::Ok);
        return(SO_USER_ERROR);
    }

    emit SetupLevelChanged(tr("Physical quantities ...."));
    PhysicalQuantities->LoadConfig();

    emit SetupLevelChanged(tr("Graphics setup ...."));
    GraphicsSetupProfile->InitAllSetupObjects();
    GraphicsSetupProfile->LoadDefaultConfig();
    ColorsList.LoadColors();

    emit SetupLevelChanged(tr("Build setup ...."));
    Inserters->LoadConfig();
    OptimizerSetups->LoadConfig();

    emit SetupLevelChanged(tr("Job scheduler ...."));
    JobScheduler->LoadConfig();
    JobScheduler->StartScheduler();

    emit SetupLevelChanged(tr("Init OpenBabel ...."));
    CNemesisPlugNotification* p_obn = new CNemesisPlugNotification(p_ww,tr("Loading OpenBabel plugin %1 ...."));
    OpenBabel::OBPlugNotification::PlugNotification = p_obn;
    OpenBabel::OBPlugin::LoadAllPlugins();    // this will initialize OBPlugin subsystem
    OpenBabel::OBPlugNotification::PlugNotification = NULL;
    delete p_obn;
    // do not remove above line - it is necessary for OB 2.3.0

    emit SetupLevelChanged(tr("Global desktop ...."));
    RecentFiles->Load();
    GlobalDesktop->LoadDefaultDesktop();
    MouseDriverSetup->LoadUserSetup();

    emit SetupLevelChanged("All done.");

    // and finaly create initial project and restore global workpanels
    GlobalDesktop->RestoreInitialProject();
    GlobalDesktop->RestoreWorkPanels();

    return(SO_CONTINUE);
}

//------------------------------------------------------------------------------

void CNemesisJScript::PluginLoaded(const QString& base_name)
{
    QString text;
    text = QString(tr("Loading Nemesis plugin %1 ....")).arg(base_name);
    emit SetupNotificationChanged(text);
}

//------------------------------------------------------------------------------

void CNemesisJScript::FinalizeSubsystems(void)
{
    // ---------------------------------
    // this will remove all setup designers
    GraphicsSetupProfile->CloseAllObjectDesigners();
    GraphicsSetupProfile->SaveDefaultConfig();

    OptimizerSetups->CloseAllObjectDesigners();
    OptimizerSetups->SaveConfig();

    // ---------------------------------
    MouseDriverSetup->SaveUserSetup();
    GlobalSetup->Save();
    RecentFiles->Save();
    GlobalDesktop->SaveDefaultDesktop();

    // ---------------------------------
    // here we should not have any opened project
    // so just to be sure
    Projects->RemoveAllProjects();

    // close all workpanels
    WorkPanels->RemoveAllWorkPanels(true);

    // process all events in the loop
    qApp->processEvents();

    // ---------------------------------
    // some part might be actually in
    // void CMainWindow::closeEvent(QCloseEvent * event)
    // because of order of object destructions
    Inserters->SaveConfig();

    // destroy global objects ---------
    delete RootList;
    RootList = NULL;
    Projects = NULL;
    GlobalObjectList = NULL;
    GraphicsSetupProfile = NULL;
    WorkPanels = NULL;
    PhysicalQuantities = NULL;
    Inserters = NULL;
    OptimizerSetups = NULL;
    JobScheduler = NULL;
    GlobalDesktop = NULL;
    MouseDriverSetup = NULL;

    // process all events in the loop
    qApp->processEvents();

    // close all plugins
    PluginDatabase.UnloadPlugins();
}

//------------------------------------------------------------------------------

void CNemesisJScript::RegisterObjects(void)
{
    // register metaobjects
    ProObjectRegisterMetaObject();
    ProjectRegisterMetaObject();

    // register functions ------------------------
    QScriptValue fce;

    fce = newFunction(NemesisQuit);
    globalObject().setProperty("quit",fce);

    fce = newFunction(NemesisPrint);
    globalObject().setProperty("print",fce);

    // constructable objects ---------------------
    CAboutWindow::Register(this);

    // static objects ----------------------------
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CNemesisJScript::QuitApplication(void)
{
    foreach(QObject* p_qobj,Projects->children()) {
        CProject* p_pro = dynamic_cast<CProject*>(p_qobj);
        if( p_pro == NULL ) continue;
        p_pro->BringToFront();
        if( Projects->CanCloseProject(p_pro) == true ) p_pro->CloseProject();
    }

    if( Projects->GetNumberOfProjects() == 0 ){
        WorkPanels->RemoveAllWorkPanels(false);
    }

    // close application
    if( Projects->GetNumberOfProjects() == 0 ){
        qApp->quit();
    }
}

//------------------------------------------------------------------------------

void CNemesisJScript::ShallQuitApplication(void)
{
    if( Projects->GetNumberOfProjects() != 0 ) return;

    WorkPanels->RemoveAllWorkPanels(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

