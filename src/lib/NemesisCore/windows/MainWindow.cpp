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

#include <MainWindow.hpp>
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
#include <PluginDatabase.hpp>
#include <ProjectDesktop.hpp>
#include <GlobalDesktop.hpp>
#include <SelectionRequest.hpp>
#include <Graphics.hpp>
#include <GraphicsViewList.hpp>
#include <QCoreApplication>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMainWindow::CMainWindow(CProject* p_project)
{
    Project = p_project;

    // helper objects
    StereoGroup = NULL;
    MouseModeGroup = NULL;
    SelRequest = new CSelectionRequest(this,tr("main window"));

    ProfileList = Project->GetGraphics()->GetProfiles();
    HistoryList = Project->GetHistory();

    // enable tooltips
    setAttribute(Qt::WA_AlwaysShowToolTips,true);

    // title
    NEMESIS_TITLE = tr("NEMESIS - Molecular Modelling Package");
    connect(Project,SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(UpdateWindowTitle(void)));

    // setup update
    connect(GlobalDesktop,SIGNAL(OnMainWindowSetupChanged(const QString)),
            this,SLOT(UpdateMainWindowSetup(const QString)),Qt::QueuedConnection);

// init UI mirrors -------------------------------
    // file ----------------------------
    actionNewMolecule = NULL;
    actionNewProject = NULL;
    actionOpen = NULL;
    actionSave = NULL;
    actionSaveAs = NULL;
    actionSaveAll = NULL;
    actionRender = NULL;
    actionClose = NULL;
    actionCloseAll = NULL;
    actionProjectInfo = NULL;
    actionQuit = NULL;
    actionEmptyRecentList = NULL;

    menuOpenRecent = NULL;
    menuImportStructure = NULL;
    menuInjectCoordinates = NULL;
    menuExportStructure = NULL;

    // edit ----------------------------
    LockGroup = NULL;

    actionUndo = NULL;
    actionRedo = NULL;

    actionCopyActiveStructure = NULL;
    actionPasteStructure = NULL;
    actionPasteStructureDoNotOverlap = NULL;

    actionClearHistory = NULL;
    actionClearAllData = NULL;

    actionLockStructures = NULL;
    actionLockTopology = NULL;
    actionLockGeometry = NULL;
    actionLockGraphics = NULL;
    actionLockDescription = NULL;
    actionLockProperties = NULL;

    // selection -----------------------
    actionSelectStructures = NULL;
    actionSelectAtomsAndBonds = NULL;
    actionSelectAtoms = NULL;
    actionSelectBonds = NULL;
    actionSelectResidues = NULL;
    actionSelectRestraints = NULL;
    actionSelectGraphicsObjects = NULL;
    actionSelectProperties = NULL;
    actionInvertSelection = NULL;
    actionClearSelection = NULL;
    actionSelectNone = NULL;
    actionSelectAtomsBySymbol = NULL;
    actionSelectAtomsByName = NULL;
    actionSelectAtomsByType = NULL;
    actionSelectAtomsByMask = NULL;
    actionSelectBondsByName = NULL;
    actionSelectBondsByBondOrder = NULL;
    actionSelectResiduesByName = NULL;
    actionSelectResiduesByMask = NULL;
    actionSelectGraphicsObjectsByType = NULL;

    actionOnlyActiveStructureObjects = NULL;
    actionAlreadySelectedObjectsAsSeed = NULL;
    actionConsiderObjectsOfAllStructures = NULL;

    actionCompleteAll = NULL;
    actionCompleteActiveStructure = NULL;
    actionCompleteMolecules = NULL;
    actionCompleteValences = NULL;
    actionCompleteBonds = NULL;
    actionSelectionListWP = NULL;

    StrObjectSelMode = NULL;

    // structure ----------------------
    // info
    actionActiveStructureInfo = NULL;
    actionAllStructuresInfo = NULL;

    // structure
    actionNewStructure = NULL;
    actionCreateStructureFromSelectedResidues = NULL;
    actionMergeAllStructures = NULL;
    actionSortStructures = NULL;
    actionDeleteSelectedStructures = NULL;
    actionDeleteEmptyStructures = NULL;

    // residue
    actionNewResidue = NULL;
    actionNewResidueFromSelectedAtoms = NULL;
    actionSortResidues = NULL;
    actionDeleteSelectedResidues = NULL;
    actionDeleteEmptyResidues = NULL;

    // atom
    actionNewAtom = NULL;
    actionSortAtoms = NULL;
    actionDeleteSelectedAtoms = NULL;

    // bond
    actionNewBond = NULL;
    actionDeleteSelectedBonds = NULL;
    actionDeleteInvalidBonds = NULL;

    // misc
    actionInsertSMILES = NULL;
    actionInsertInChI = NULL;
    actionConvertTo2D = NULL;
    actionOpenAsBuildProject = NULL;
    actionDuplicateStructure = NULL;
    actionClearStructure = NULL;

    // hydrogens
    actionAddHydrogens = NULL;
    actionAddPolarHydrogens = NULL;
    actionRemoveHydrogens = NULL;

    // single bonds
    actionAddBonds = NULL;
    actionRemoveBonds = NULL;

    // panels
    actionBuildWP = NULL;
    actionManageStructuresWP = NULL;

    // geometry -----------------------
    actionGeometryWP = NULL;
    actionOptimize = NULL;
    actionOptimizerSetup = NULL;
    actionGeoManipWP = NULL;
    actionRestraintManagerWP = NULL;
    actionEnableRestraints = NULL;
    actionNewRestraint = NULL;
    actionDeleteInvalidRestraints = NULL;
    actionDeleteSelectedRestraints = NULL;
    actionDeleteAllRestraints = NULL;
    actionAlignPrincipalAxes = NULL;
    actionCOMToOrigin = NULL;
    actionCOMToBoxCenter = NULL;
    actionRotateAroundXAxisClockwise = NULL;
    actionRotateAroundYAxisClockwise = NULL;
    actionRotateAroundZAxisClockwise = NULL;
    actionRotateAroundXAxisAnticlockwise = NULL;
    actionRotateAroundYAxisAnticlockwise = NULL;
    actionRotateAroundZAxisAnticlockwise = NULL;
    actionMirrorInXYPlane = NULL;
    actionMirrorInXZPlane = NULL;
    actionMirrorInYZPlane = NULL;
    actionFreezeAllAtoms = NULL;
    actionFreezeHeavyAtoms = NULL;
    actionFreezeSelectedAtoms = NULL;
    actionUnfreezeAllAtoms = NULL;
    actionUnfreezeSelectedAtoms = NULL;
    actionImageByMolecules = NULL;
    actionImageByResidues = NULL;
    actionImageByAtoms = NULL;

    actionManipulateAllAtoms = NULL;
    actionManipulateSelectedAtoms = NULL;
    actionManipulateMolecules = NULL;

    actionAroundOrigin = NULL;
    actionFamiliarShape = NULL;

    GeoScopeGroup = NULL;

    // properties ---------------------
    actionNewProperty = NULL;
    actionManagePropertiesWP = NULL;
    actionExportPropertiesWP = NULL;

    actionSelectedProperties = NULL;
    actionDeleteInvalidProperties = NULL;
    actionDeleteAllProperties = NULL;

    menuImportProperty = NULL;

    // graphics -----------------------
    actionAutofitScene = NULL;
    actionPerspective = NULL;
    actionOrthographic = NULL;
    actionStereoSwapEyes = NULL;
    actionDepthCueing = NULL;

    actionNewObjectGlobalScope = NULL;
    actionNewObjectActiveProfile = NULL;
    actionObjectListInfo = NULL;

    actionNewProfile = NULL;
    actionActiveProfileInfo = NULL;
    actionProfileListInfo = NULL;

    actionNewView = NULL;
    actionPrimaryViewInfo = NULL;
    actionViewListInfo = NULL;

    actionGraphicsManagerWP = NULL;
    actionProfileManagerWP = NULL;

    menuStereo = NULL;

    // mouse ---------------------------
    actionSRTMode = NULL;
    actionSelectMode = NULL;
    actionRotateMode = NULL;
    actionTranslateMode = NULL;
    actionScaleMode = NULL;
    actionSelectRotationCentre = NULL;
    actionAtomManipulation = NULL;
    actionDirManipulation = NULL;
    actionMoleculeManipulation = NULL;
    actionObjectManipulation = NULL;
    actionObjectProperties = NULL;

    // tools ---------------------------
    menuTools = NULL;

    // setup ---------------------------
    actionToolbarsConfig = NULL;
    actionSaveLayout = NULL;
    actionUnitSetup = NULL;
    actionGraphicsSetup = NULL;
    actionColorSetup = NULL;
    actionJobSchedulerPreferences = NULL;
    actionGlobalSetup = NULL;

    // help ----------------------------
    actionNemesisHelp = NULL;
    actionAboutNemesis = NULL;
    actionAboutQt = NULL;
}

//------------------------------------------------------------------------------

CMainWindow::~CMainWindow(void)
{

}

//------------------------------------------------------------------------------

void CMainWindow::UpdateWindowTitle(void)
{
    setWindowTitle(Project->GetName() + " : " + NEMESIS_TITLE);
}

//------------------------------------------------------------------------------

CProject* CMainWindow::GetProject(void)
{
    return(Project);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::RestoreDesktop(void)
{
    // update title
    UpdateWindowTitle();

    // load project setup
    // this is called extra in CProject
    // Project->GetDesktop()->LoadDefaultDesktop();

    // restore main window 1
    ConnectToolsMenu(); // we need actions to properly setup toolbars :-(
    LoadToolBars();
    LoadShortcuts();

    // restore project work panels
    WorkPanels->RestoreWorkPanels(Project);

    // restore main window 2
    CXMLElement* p_root = Project->GetDesktop()->GetMainWindowSetup();

    updateGeometry();

    QByteArray array;
    if( p_root->GetAttribute("geom",array) == true ) {
        restoreGeometry(array);
    }
    if( p_root->GetAttribute("state",array) == true ) {
        restoreState(array);
    }
    bool max = true;
    p_root->GetAttribute("maximized",max);
    if( max ){
        showMaximized();
    }

    // restore work panels
    WorkPanels->ShowWorkPanels(Project);
}

//------------------------------------------------------------------------------

void CMainWindow::SaveDesktop(void)
{
    // DO NOT SAVE shortcuts and toolbars here, they are saved on user request
    // by ConfigMainWindow
    //SaveToolBars();
    //SaveShortcuts();

    CXMLElement* p_root = Project->GetDesktop()->GetMainWindowSetup();
    p_root->SetAttribute("geom",saveGeometry());
    p_root->SetAttribute("state",saveState());
    p_root->SetAttribute("maximized",isMaximized());

    // save work panels
    WorkPanels->SaveWorkPanelSetup(Project);
}

//------------------------------------------------------------------------------

void CMainWindow::LoadToolBars(bool system)
{
    // remove previous toolbars
    foreach(QToolBar* p_tb, findChildren<QToolBar*>()) {
        delete p_tb;
    }

    CXMLElement* p_rele = Project->GetDesktop()->GetMainWindowToolbars(system);
    CXMLIterator I(p_rele);
    CXMLElement* p_tele;

    while( (p_tele = I.GetNextChildElement("toolbar")) != NULL ) {
        QString     tbname;
        p_tele->GetAttribute("name",tbname);

        QToolBar* p_tb = new QToolBar(this);
        p_tb->setIconSize(QSize(18,18));
        p_tb->setObjectName(tbname);
        p_tb->setWindowTitle(tbname);
        addToolBar(p_tb);

        CXMLIterator J(p_tele);
        CXMLElement* p_aele;

        while( (p_aele = J.GetNextChildElement()) != NULL ) {
            if(p_aele->GetName() == "action") {
                QAction*    p_act;
                QString     actname;
                p_aele->GetAttribute("name", actname);
                p_act = findChild<QAction*>(actname);
                if(p_act != NULL) {
                    p_tb->addAction(p_act);
                }
            } else {
                p_tb->addSeparator();
            }
        }
    }
}

//------------------------------------------------------------------------------

void CMainWindow::SaveToolBars(void)
{
    CXMLElement* p_rele = Project->GetDesktop()->GetMainWindowToolbars();
    p_rele->RemoveAllChildNodes();

    QList<QToolBar*> toolbars_list = findChildren<QToolBar*>();

    foreach(QToolBar* p_tb,toolbars_list) {
        CXMLElement* p_tele = p_rele->CreateChildElement("toolbar");
        p_tele->SetAttribute("name",p_tb->objectName());

        QList<QAction*> toolbar_actions = p_tb->actions();
        foreach(QAction* p_act,toolbar_actions) {
            if( p_act->isSeparator() ) {
                p_tele->CreateChildElement("separator");
            } else {
                CXMLElement* p_aele = p_tele->CreateChildElement("action");
                p_aele->SetAttribute("name", p_act->objectName());
            }
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::LoadShortcuts(bool system)
{
    CXMLElement* p_ele = Project->GetDesktop()->GetMainWindowShortcuts(system);

    CXMLIterator I(p_ele);
    CXMLElement* p_sele;

    while((p_sele = I.GetNextChildElement("shortcut")) != NULL) {
        QString action, key;
        p_sele->GetAttribute("action",action);
        p_sele->GetAttribute("key",key);

        // find actions
        QAction* p_action = findChild<QAction*>(action);
        if( p_action ){
            p_action->setShortcut(QKeySequence(key));
        }
    }
}

//------------------------------------------------------------------------------

void CMainWindow::SaveShortcuts(void)
{
    CXMLElement* p_ele = Project->GetDesktop()->GetMainWindowShortcuts();
    p_ele->RemoveAllChildNodes();

    foreach(QAction* p_action, findChildren<QAction*>()) {
        // anonymous actions are not used
        if(p_action->objectName().isEmpty() ) continue;

        // actions without shortcut are not used
        if(p_action->shortcut().isEmpty() ) continue;

        CXMLElement* p_sele = p_ele->CreateChildElement("shortcut");
        p_sele->SetAttribute("action",p_action->objectName());
        p_sele->SetAttribute("key",p_action->shortcut().toString());
    }
}

//------------------------------------------------------------------------------

void CMainWindow::UpdateMainWindowSetup(const QString sextuuid)
{
    if( Project->GetType().GetFullStringForm() != sextuuid ) return;

    // main window setup was changed
    Project->GetDesktop()->LoadMainWindowSetup();
    LoadToolBars();
    LoadShortcuts();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::closeEvent(QCloseEvent * event)
{
    if( Projects->CanCloseProject(Project) == false ) {
        // ignore event
        event->ignore();
        return;
    }

    // and remove associated project
    Project->CloseProject();

//    // if no project in the list - restore initial project
//    if( Projects->GetNumberOfProjects() == 0 ){
//        GlobalDesktop->RestoreInitialProject();
//    }

    // if no project in the list - terminate application
    if( Projects->GetNumberOfProjects() == 0 ){
        qApp->quit();
    }

    event->accept();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::OpenToolPanel(const CUUID& object_uuid,bool open)
{
    if( open == true ) {
        CWorkPanel* p_wp = static_cast<CWorkPanel*>(PluginDatabase.CreateObject(object_uuid,Project));
        if( p_wp == NULL ) {
            CSmallString error;
            error << "unable to create tool object " << object_uuid.GetStringForm();
            ES_ERROR(error);
            return;
        }
        p_wp->Show();
    } else {
        CWorkPanel* p_wp;
        // project related
        p_wp = WorkPanels->Find(object_uuid,Project);
        if( p_wp != NULL ) {
            delete p_wp;
        }
        // global ones
        p_wp = WorkPanels->Find(object_uuid,NULL);
        if( p_wp != NULL ) {
            delete p_wp;
        }
    }
}

//------------------------------------------------------------------------------

void CMainWindow::ShowAsDialog(const CUUID& object_uuid,void *p_data)
{
    CWorkPanel* p_wp = static_cast<CWorkPanel*>(PluginDatabase.CreateObject(object_uuid,p_data));
    if( p_wp == NULL ) {
        CSmallString error;
        error << "unable to create object " << object_uuid.GetStringForm();
        ES_ERROR(error);
        return;
    }
    if( p_wp != NULL ) p_wp->ShowAsDialog();
    p_wp->deleteLater();
}

//------------------------------------------------------------------------------

void CMainWindow::ExecuteObject(const CUUID& object_uuid,void *p_data)
{
    PluginDatabase.CreateObject(object_uuid,p_data);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



