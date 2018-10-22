// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <ErrorSystem.hpp>
#include <ProjectList.hpp>
#include <GlobalSetup.hpp>
#include <PluginDatabase.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <FileSystem.hpp>
#include <RenderWindow.hpp>
#include <GlobalDesktop.hpp>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <MainWindow.hpp>
#include <GlobalSetup.hpp>
#include <WorkPanelList.hpp>
#include <RecentFileList.hpp>
#include <ImpexFormats.hpp>
#include <list>
#include <QTimer>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ConnectFileMenu(void)
{
    BIND_ACTION(actionNewMolecule);
    BIND_ACTION(actionNewProject);
    BIND_ACTION(actionOpen);
    BIND_ACTION(actionSave);
    BIND_ACTION(actionSaveAs);
    BIND_ACTION(actionSaveAll);
    BIND_ACTION(actionRender);
    BIND_ACTION(actionClose);
    BIND_ACTION(actionCloseAll);
    BIND_ACTION(actionProjectInfo);
    BIND_ACTION(actionQuit);
    BIND_ACTION(actionEmptyRecentList);

    BIND_MENU(menuOpenRecent);
    BIND_MENU(menuImportStructure);
    BIND_MENU(menuInjectCoordinates);
    BIND_MENU(menuExportStructure);

    //====================================
    connect(actionNewMolecule,SIGNAL(triggered(bool)),
            this,SLOT(NewMolecule(void)));
    connect(actionNewProject,SIGNAL(triggered(bool)),
            this,SLOT(NewProject(void)));
    //----
    connect(actionOpen,SIGNAL(triggered(bool)),
            this,SLOT(OpenProject(void)));

    UpdateOpenRecentMenu();
    connect(menuOpenRecent, SIGNAL(triggered(QAction*)),
            SLOT(OpenRecentAction(QAction*)));

    // import/export
    SetupImportExportMenu();

    //----
    connect(actionSave,SIGNAL(triggered(bool)),
            this,SLOT(SaveProject(void)));
    connect(actionSaveAs,SIGNAL(triggered(bool)),
            this,SLOT(SaveProjectAs(void)));
    connect(actionSaveAll,SIGNAL(triggered(bool)),
            this,SLOT(SaveAllProjects(void)));
    //----
    connect(actionRender,SIGNAL(triggered(bool)),
            this,SLOT(RenderProject(void)));
    //----
    connect(actionClose,SIGNAL(triggered(bool)),
            this,SLOT(CloseProject(void)));
    connect(actionCloseAll,SIGNAL(triggered(bool)),
            this,SLOT(CloseAllProjects(void)));
    connect(actionProjectInfo,SIGNAL(triggered(bool)),
            this,SLOT(ProjectInfo(void)));
    //----
    connect(actionQuit,SIGNAL(triggered(bool)),
            this,SLOT(QuitApplication(void)));
    //----
    connect(RecentFiles,SIGNAL(OnRecentFileListUpdate(void)),
            this,SLOT(UpdateOpenRecentMenu(void)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::UpdateOpenRecentMenu(void)
{
    menuOpenRecent->clear();

    unsigned int i = 0;
    QString file,processor;
    while((RecentFiles->GetFileName(i, file, processor)) == true) {
        QString full_caption = file;
        if( ! processor.isEmpty() ){
            full_caption += " (" + processor + ")";
        }
        QAction* p_act = menuOpenRecent->addAction(full_caption);
        p_act->setData(QVariant(i));
        i++;
    }
    if( i == 0 ) {
        menuOpenRecent->addAction(tr("<empty>"));
    }
    menuOpenRecent->addSeparator();
    menuOpenRecent->addAction(actionEmptyRecentList);
}

//------------------------------------------------------------------------------

void CMainWindow::OpenRecentAction(QAction* p_act)
{
    if( p_act == actionEmptyRecentList ) {
        RecentFiles->ClearList();
        return;
    }

    RecentFiles->OpenFile(p_act->data().toInt(),Project);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CMainWindow::SortPluginObjectByName(CPluginObject* p_left,CPluginObject* p_right)
{
    CSmallString fname_l,fname_r,lcategory,rcategory;
    PluginDatabase.FindObjectConfigValue(p_left->GetObjectUUID(),"_category",lcategory,true);
    PluginDatabase.FindObjectConfigValue(p_right->GetObjectUUID(),"_category",rcategory,true);
    fname_l << lcategory << p_left->GetName();
    fname_r << rcategory << p_right->GetName();
    return(fname_l < fname_r);
}

//------------------------------------------------------------------------------

void  CMainWindow::SetupImportExportMenu(void)
{
    list<CPluginObject*>    impitems;
    list<CPluginObject*>    injitems;
    list<CPluginObject*>    expitems;

    CSimpleIteratorC<CPluginObject>    I(PluginDatabase.GetObjectList());
    CPluginObject*                     p_obj;
    QActionGroup*                      p_actGroup = new QActionGroup(this);
    while( (p_obj = I.Current()) != NULL ) {
        if (p_obj->GetCategoryUUID() == IMPORT_STRUCTURE_CAT ) {
            if( p_obj->HasAttribute("EPF_IMPORT_STRUCTURE") ){
                impitems.push_back(p_obj);
            }
            if( p_obj->HasAttribute("EPF_INJECT_COORDINATES") ){
                injitems.push_back(p_obj);
            }
        }
        if (p_obj->GetCategoryUUID() == EXPORT_STRUCTURE_CAT ) {
            expitems.push_back(p_obj);
        }
        I++;
    }

    // sort items
    impitems.sort(SortPluginObjectByName);
    injitems.sort(SortPluginObjectByName);
    expitems.sort(SortPluginObjectByName);

    // build menu items
    list<CPluginObject*>::iterator iit = impitems.begin();
    list<CPluginObject*>::iterator iie = impitems.end();

    while( iit != iie ){
        CPluginObject*  p_obj = *iit;
        CreateImpexAction(p_actGroup,menuImportStructure,p_obj,true,false);
        iit++;
    }

    list<CPluginObject*>::iterator jit = injitems.begin();
    list<CPluginObject*>::iterator jie = injitems.end();

    while( jit != jie ){
        CPluginObject*  p_obj = *jit;
        CreateImpexAction(p_actGroup,menuInjectCoordinates,p_obj,true,true);
        jit++;
    }

    list<CPluginObject*>::iterator eit = expitems.begin();
    list<CPluginObject*>::iterator eie = expitems.end();

    while( eit != eie ){
        CPluginObject*  p_obj = *eit;
        CreateImpexAction(p_actGroup,menuExportStructure,p_obj,false,false);
        eit++;
    }

    // any items?
    if( impitems.size() == 0 ) {
        menuImportStructure->addAction(tr("- not available -"));
    }

    if( injitems.size() == 0 ) {
        menuInjectCoordinates->addAction(tr("- not available -"));
    }

    if( expitems.size() == 0 ) {
        menuExportStructure->addAction(tr("- not available -"));
    }

    connect(p_actGroup,SIGNAL(triggered(QAction *)),this,SLOT(ImportExportAction(QAction*)));
}

//------------------------------------------------------------------------------

void CMainWindow::CreateImpexAction(QActionGroup* p_actGroup,QMenu* p_menu,
                                    CPluginObject* p_pod,bool myimport,bool myinject)
{
    QMenu* p_finmenu = p_menu;

    // do we have category?
    QString category;
    if( PluginDatabase.FindObjectConfigValue(p_pod->GetObjectUUID(),"_category",category,true) == true ){
        QList<QMenu*> menus = p_menu->findChildren<QMenu*>();
        bool found = false;
        foreach(QMenu* p_submenu,menus){
            if( p_submenu->title() == category ){
                p_finmenu = p_submenu;
                found = true;
                break;
            }
        }
        if( found == false ){
            p_finmenu = p_menu->addMenu(category);
        }
    }

    // do we have formats?
    CUUID           idfmts;
    CImpexFormats*  p_formats;
    if( (PluginDatabase.FindObjectConfigValue(p_pod->GetObjectUUID(),"_formats",idfmts,true) == true) &&
        (p_formats = static_cast<CImpexFormats*>(PluginDatabase.CreateObject(idfmts,NULL)))  ){
        QMenu* p_actmenu = p_finmenu->addMenu(p_pod->GetName());     
        p_actmenu->setStyleSheet("* { menu-scrollable: 1 }");
        QStringList formats;
        if( myimport ){
            formats = p_formats->GetImportFormats(true);
        } else {
            formats = p_formats->GetExportFormats(true);
        }
        for(int i=0; i < formats.count(); i++){
            QString format = formats.at(i);
            QAction* p_act = p_actmenu->addAction(format);
            p_act->setProperty("impex.uuid",QVariant(QString(p_pod->GetObjectUUID().GetFullStringForm())));
            p_act->setProperty("impex.format",format);
            p_act->setProperty("impex.inject",myinject);
            p_actGroup->addAction(p_act);
        }
        delete p_formats;
    } else {
        QAction* p_act = p_finmenu->addAction(p_pod->GetName());
        p_act->setProperty("impex.uuid",QVariant(QString(p_pod->GetObjectUUID().GetFullStringForm())));
        p_act->setProperty("impex.format",QVariant());
        p_act->setProperty("impex.inject",myinject);
        p_actGroup->addAction(p_act);
    }
}

//------------------------------------------------------------------------------

void CMainWindow::ImportExportAction(QAction* p_act)
{
    CExtUUID uuid;
    // impex object
    QVariant type = p_act->property("impex.uuid");
    uuid.LoadFromString(type.toString().toLatin1().constData());

    // attach data to project about action
    Project->setProperty("impex.format",p_act->property("impex.format"));
    Project->setProperty("impex.inject",p_act->property("impex.inject"));

    // open dialog
    ExecuteObject(uuid,Project);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::NewMolecule(void)
{
    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString("{BUILD_PROJECT:b64d16f0-b73f-4747-9a13-212ab9a15d38}");
    CProject* p_project = Projects->NewProject(mp_uuid);
    if( p_project == NULL ){
        ES_ERROR("unable to create new molecule");
        QMessageBox::critical(NULL, tr("New Structure"),
                              tr("An error occurred during molecule opening!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        return;
    }
    p_project->ShowProject();
}

//------------------------------------------------------------------------------

void CMainWindow::NewProject(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{NEW_PROJECT_WORK_PANEL:4cf836af-c5ca-4799-b0b7-bd53209d969e}");
    OpenToolPanel(mp_uuid,true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::OpenProject(void)
{
    QString filename = QFileDialog::getOpenFileName(NULL,
                       tr("Open Project"),
                       (const char*)GlobalSetup->GetLastOpenFilePath(GenericProjectID),
                       tr("Nemesis projects (*.npr)"));

    if( filename == NULL ) return; // no file was selected

    // update last open path
    GlobalSetup->SetLastOpenFilePathFromFile(filename,GenericProjectID);

    // load project
    CProject* p_project = Projects->OpenProject(filename);

    if( p_project == NULL ){
        ES_ERROR("unable to load project");
        QMessageBox::critical(NULL, tr("Open Project"),
                              tr("An error occurred during project opening!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        return;
    }

    p_project->ShowProject();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::SaveProject(void)
{
    Projects->ProjectSave(Project);
}

//------------------------------------------------------------------------------

void CMainWindow::SaveProjectAs(void)
{
    Projects->ProjectSaveAs(Project);
}

//------------------------------------------------------------------------------

void CMainWindow::SaveAllProjects(void)
{
    foreach(QObject* p_qobj,Projects->children()) {
        CProject* p_pro = dynamic_cast<CProject*>(p_qobj);
        if( p_pro == NULL ) continue;
        bool result = true;
        if( p_pro->IsFlagSet(EPOF_TMP_NAME) == true ) {
            result = Projects->ProjectSaveAs(p_pro);
        } else {
            result = p_pro->SaveProject();
        }

        if( result == false ) {
            QMessageBox::critical(NULL, tr("Save Project"),
                                  tr("An error occurred during project saving!"),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::RenderProject(void)
{
    CRenderWindow* p_win = new CRenderWindow(Project);
    p_win->exec();
    delete p_win;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ProjectInfo(void)
{
    Project->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CMainWindow::CloseProject(void)
{
    if( Projects->CanCloseProject(Project) == true ){
        Project->CloseProject();
    }

    if( Projects->GetNumberOfProjects() == 0 ){
        if( GlobalDesktop->IsInitialProjectEnabledAfterLastIsClosed() ){
            GlobalDesktop->RestoreInitialProject();
        }
    }

    // close application
    if( Projects->GetNumberOfProjects() == 0 ){
        qApp->quit();
    }
}

//------------------------------------------------------------------------------

void CMainWindow::CloseAllProjects(void)
{
    QList<CProject*>    projects;
    foreach(QObject* p_qobj,Projects->children()) {
        CProject* p_pro = dynamic_cast<CProject*>(p_qobj);
        if( p_pro == NULL ) continue;
        projects.append(p_pro);
    }

    foreach(CProject* p_pro,projects) {
        if( p_pro == NULL ) continue;
        p_pro->BringToFront();
        if( Projects->CanCloseProject(p_pro) == true ) p_pro->CloseProject();
    }

    if( Projects->GetNumberOfProjects() == 0 ){
        if( GlobalDesktop->IsInitialProjectEnabledAfterLastIsClosed() ){
            GlobalDesktop->RestoreInitialProject();
        }
    }

    // close application
    if( Projects->GetNumberOfProjects() == 0 ){
        qApp->quit();
    }
}

//------------------------------------------------------------------------------

void CMainWindow::QuitApplication(void)
{
    QList<CProject*>    projects;
    foreach(QObject* p_qobj,Projects->children()) {
        CProject* p_pro = dynamic_cast<CProject*>(p_qobj);
        if( p_pro == NULL ) continue;
        projects.append(p_pro);
    }

    foreach(CProject* p_pro,projects) {
        if( p_pro == NULL ) continue;
        p_pro->BringToFront();
        if( Projects->CanCloseProject(p_pro) == true ) p_pro->CloseProject();
    }

    // close application
    if( Projects->GetNumberOfProjects() == 0 ){
        qApp->quit();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



