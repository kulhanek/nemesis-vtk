#ifndef MainWindowH
#define MainWindowH
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

#include <QMainWindow>
#include <QToolBar>
#include <UUID.hpp>
#include <QClipboard>
#include <HistoryList.hpp>
#include <AtomList.hpp>
#include <StandardModelSetup.hpp>
#include <QActionGroup>

//------------------------------------------------------------------------------

class CProject;
class CWorkPanel;
class CSelectionRequest;
class CGraphicsProfileList;
class CMouseHandler;

//------------------------------------------------------------------------------

// # is stringizing operator for macro expansion
// #action -> "action"

#define BIND_ACTION(action) \
    action = findChild<QAction*>(#action); \
    if( ! action ) action = new QAction(this)

#define CONNECT_ACTION(name) \
    action##name = findChild<QAction*>("action"#name); \
    if( ! action##name ) action##name = new QAction(this); \
    connect(action##name, SIGNAL(triggered(bool)), this, SLOT(name(void)))

#define BIND_MENU(menu) \
    menu = findChild<QMenu*>(#menu); \
    if( ! menu ) menu = new QMenu(this)

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CMainWindow : public QMainWindow {
Q_OBJECT
public:
// constructor and destructors -------------------------------------------------
    CMainWindow(CProject* p_project);
    virtual ~CMainWindow(void);

// get associated project ------------------------------------------------------
    /// get project
    CProject* GetProject(void);

// desktop management ----------------------------------------------------------
    /// restore desktop
    // do not call this method in constructor, it is safe
    // to call it in CXXXXProject::CreateMainWindow()
    void RestoreDesktop(void);

    /// save desktop - only update CProjectDesktop
    // results are not saved to file, see CMainWindow::SaveDefaultLayout
    void SaveDesktop(void);

// common methods --------------------------------------------------------------
    /// show work panel as dialog
    static void ShowAsDialog(const CUUID& object_uuid,void *p_data=NULL);

    /// execute object
    static void ExecuteObject(const CUUID& object_uuid,void *p_data=NULL);

    /// open work panel
    void OpenToolPanel(const CUUID& object_uuid,bool open);

// toolbars setup methods ------------------------------------------------------
    /// load toolbars
    void LoadToolBars(bool system=false);

    /// save toolbars
    void SaveToolBars(void);

// shortcuts setup methods -----------------------------------------------------
    /// load shortcuts
    void LoadShortcuts(bool system=false);

    /// save shortcuts
    void SaveShortcuts(void);

public slots:
    // update window title
    void UpdateWindowTitle(void);

// FILE ========================================================================
public:
    void ConnectFileMenu(void);
    void CreateImpexAction(QActionGroup* p_actGroup,QMenu* p_menu,
                           CPluginObject* p_pod,bool myimport,bool myinject);

public slots:
    void NewMolecule(void);
    void NewProject(void);
    //------------
    void OpenProject(void);
    void ImportExportAction(QAction* p_act);
    void OpenRecentAction(QAction* p_act);
    //------------
    void SaveProject(void);
    void SaveProjectAs(void);
    void SaveAllProjects(void);
    //------------
    void RenderProject(void);
    //------------
    void CloseProject(void);
    void CloseAllProjects(void);
    void ProjectInfo(void);
    //------------
    void QuitApplication(void);
    //------------
    void UpdateOpenRecentMenu(void);
    void SetupImportExportMenu(void);

// section of private data -----------------------------------------------------
private:
    QAction* actionNewMolecule;
    QAction* actionNewProject;
    QAction* actionOpen;
    QAction* actionSave;
    QAction* actionSaveAs;
    QAction* actionSaveAll;
    QAction* actionRender;
    QAction* actionClose;
    QAction* actionCloseAll;
    QAction* actionProjectInfo;
    QAction* actionQuit;
    QAction* actionEmptyRecentList;

    QMenu* menuOpenRecent;
    QMenu* menuImportStructure;
    QMenu* menuInjectCoordinates;
    QMenu* menuExportStructure;

// EDIT ========================================================================
public:
    void ConnectEditMenu(void);

public slots:
    void UpdateEditMenu(void);

    void Undo(void);
    void Redo(void);

    void CopyActiveStructure(void);
    void PasteStructure(void);
    void PasteStructureDoNotOverlap(void);

    void ClearHistory(void);
    void ClearAllData(void);

    void LockChanged(QAction* p_act);

// section of private data -----------------------------------------------------
protected:
    QActionGroup* LockGroup;

private:
    QAction* actionUndo;
    QAction* actionRedo;

    QAction* actionCopyActiveStructure;
    QAction* actionPasteStructure;
    QAction* actionPasteStructureDoNotOverlap;

    QAction* actionClearHistory;
    QAction* actionClearAllData;

    QAction* actionLockStructures;
    QAction* actionLockTopology;
    QAction* actionLockGeometry;
    QAction* actionLockGraphics;
    QAction* actionLockDescription;
    QAction* actionLockProperties;

// SELECTION ===================================================================
public:
    void ConnectSelectionMenu(void);

public slots:
    void SelectionListWP(void);

    void SelectStructures(bool checked);
    void SelectAtomsAndBonds(bool checked);
    void SelectAtoms(bool checked);
    void SelectBonds(bool checked);
    void SelectResidues(bool checked);
    void SelectRestraints(bool checked);
    void SelectGraphicsObjects(bool checked);
    void SelectProperties(bool checked);

    void InvertSelection(void);
    void ClearSelection(void);
    void SelectNone(void);

    void SelectAtomsBySymbol(void);
    void SelectAtomsByType(void);
    void SelectAtomsByName(void);
    void SelectAtomsByMask(void);

    void SelectBondsByName(void);
    void SelectBondsByBondOrder(void);

    void SelectResiduesByName(void);
    void SelectResiduesByMask(void);

    void SelectGraphicsObjectsByType(void);

    void StrObjectSelModeChanged(QAction* p_action);

    void CompleteBonds(void);
    void CompleteValences(void);
    void CompleteMolecules(void);
    void CompleteActiveStructure(void);
    void CompleteAll(void);

    void UpdateSelectionMenu(void);
    void UpdateStrObjectSelMode(void);

// section of private data -----------------------------------------------------
private:
    QAction* actionSelectStructures;
    QAction* actionSelectAtomsAndBonds;
    QAction* actionSelectAtoms;
    QAction* actionSelectBonds;
    QAction* actionSelectResidues;
    QAction* actionSelectRestraints;
    QAction* actionSelectGraphicsObjects;
    QAction* actionSelectProperties;
    QAction* actionSelectNone;

    QAction* actionInvertSelection;
    QAction* actionClearSelection;

    QAction* actionSelectAtomsBySymbol;
    QAction* actionSelectAtomsByName;
    QAction* actionSelectAtomsByType;
    QAction* actionSelectAtomsByMask;
    QAction* actionSelectBondsByName;
    QAction* actionSelectBondsByBondOrder;
    QAction* actionSelectResiduesByName;
    QAction* actionSelectResiduesByMask;
    QAction* actionSelectGraphicsObjectsByType;

    QAction* actionOnlyActiveStructureObjects;
    QAction* actionAlreadySelectedObjectsAsSeed;
    QAction* actionConsiderObjectsOfAllStructures;

    QAction* actionCompleteAll;
    QAction* actionCompleteActiveStructure;
    QAction* actionCompleteMolecules;
    QAction* actionCompleteValences;
    QAction* actionCompleteBonds;

    QAction* actionSelectionListWP;

    QActionGroup* StrObjectSelMode;

// STRUCTURE ===================================================================
public:
    void ConnectStructureMenu(void);

public slots:
    void UpdateStructureMenu(void);

    void ActiveStructureInfo(void);
    void AllStructuresInfo(void);

    // structure
    void NewStructure(void);
    void CreateStructureFromSelectedResidues(void);
    void MergeAllStructures(void);
    void SortStructures(void);
    void DeleteSelectedStructures(void);
    void DeleteEmptyStructures(void);

    // residue
    void NewResidue(void);
    void NewResidueFromSelectedAtoms(void);
    void SortResidues(void);
    void DeleteSelectedResidues(void);
    void DeleteEmptyResidues(void);

    // atom
    void NewAtom(void);
    void SortAtoms(void);
    void DeleteSelectedAtoms(void);

    // bond
    void NewBond(void);
    void DeleteSelectedBonds(void);
    void DeleteInvalidBonds(void);

    // misc
    void InsertSMILES(void);
    void InsertInChI(void);
    void ConvertTo2D(void);
    void OpenAsBuildProject(void);
    void DuplicateStructure(void);
    void ClearStructure(void);

    // hydrogens
    void AddHydrogens(void);
    void AddPolarHydrogens(void);
    void RemoveHydrogens(void);

    // single bonds
    void AddBonds(void);
    void RemoveBonds(void);

    // panels
    void BuildWP(void);
    void ManageStructuresWP(void);

// section of private data -----------------------------------------------------
private:
    // info
    QAction* actionActiveStructureInfo;
    QAction* actionAllStructuresInfo;

    // structure
    QAction* actionNewStructure;
    QAction* actionCreateStructureFromSelectedResidues;
    QAction* actionMergeAllStructures;
    QAction* actionSortStructures;
    QAction* actionDeleteSelectedStructures;
    QAction* actionDeleteEmptyStructures;

    // residue
    QAction* actionNewResidue;
    QAction* actionNewResidueFromSelectedAtoms;
    QAction* actionSortResidues;
    QAction* actionDeleteSelectedResidues;
    QAction* actionDeleteEmptyResidues;

    // atom
    QAction* actionNewAtom;
    QAction* actionSortAtoms;
    QAction* actionDeleteSelectedAtoms;

    // bond
    QAction* actionNewBond;
    QAction* actionDeleteSelectedBonds;
    QAction* actionDeleteInvalidBonds;

    // misc
    QAction* actionInsertSMILES;
    QAction* actionInsertInChI;
    QAction* actionConvertTo2D;
    QAction* actionOpenAsBuildProject;
    QAction* actionDuplicateStructure;
    QAction* actionClearStructure;

    // hydrogens
    QAction* actionAddHydrogens;
    QAction* actionAddPolarHydrogens;
    QAction* actionRemoveHydrogens;

    // single bonds
    QAction* actionAddBonds;
    QAction* actionRemoveBonds;

    // panels
    QAction* actionBuildWP;
    QAction* actionManageStructuresWP;

// GEOMETRY ====================================================================
public:
    void ConnectGeometryMenu(void);

public slots:
    void UpdateGeometryMenu(void);

    void GeometryWP(void);
    void GeoManipWP(void);

    void OptimizerSetup(void);

    void Optimize(void);
    void RestraintManagerWP(void);
    void EnableRestraints(bool checked);

    void NewRestraint(void);
    void DeleteInvalidRestraints(void);
    void DeleteSelectedRestraints(void);
    void DeleteAllRestraints(void);

    void AlignPrincipalAxes(void);

    void COMToOrigin(void);
    void COGToOrigin(void);

    void COMToBoxCenter(void);
    void COGToBoxCenter(void);

    void RotateAroundXAxisClockwise(void);
    void RotateAroundYAxisClockwise(void);
    void RotateAroundZAxisClockwise(void);

    void RotateAroundXAxisAnticlockwise(void);
    void RotateAroundYAxisAnticlockwise(void);
    void RotateAroundZAxisAnticlockwise(void);

    void MirrorInXYPlane(void);
    void MirrorInXZPlane(void);
    void MirrorInYZPlane(void);

    void FreezeAllAtoms(void);
    void FreezeHeavyAtoms(void);
    void FreezeSelectedAtoms(void);
    void UnfreezeAllAtoms(void);
    void UnfreezeSelectedAtoms(void);

    void ImageByAtoms(void);
    void ImageByResidues(void);
    void ImageByMolecules(void);

private:
    EGeometryScope GetGeometryScope(void);

    QAction* actionGeometryWP;
    QAction* actionOptimize;
    QAction* actionGeoManipWP;
    QAction* actionOptimizerSetup;
    QAction* actionRestraintManagerWP;

    QAction* actionNewRestraint;
    QAction* actionDeleteInvalidRestraints;
    QAction* actionDeleteSelectedRestraints;
    QAction* actionDeleteAllRestraints;

    QAction* actionEnableRestraints;
    QAction* actionAlignPrincipalAxes;
    QAction* actionCOMToOrigin;
    QAction* actionCOMToBoxCenter;
    QAction* actionRotateAroundXAxisClockwise;
    QAction* actionRotateAroundYAxisClockwise;
    QAction* actionRotateAroundZAxisClockwise;
    QAction* actionRotateAroundXAxisAnticlockwise;
    QAction* actionRotateAroundYAxisAnticlockwise;
    QAction* actionRotateAroundZAxisAnticlockwise;
    QAction* actionMirrorInXYPlane;
    QAction* actionMirrorInXZPlane;
    QAction* actionMirrorInYZPlane;
    QAction* actionFreezeAllAtoms;
    QAction* actionFreezeHeavyAtoms;
    QAction* actionFreezeSelectedAtoms;
    QAction* actionUnfreezeAllAtoms;
    QAction* actionUnfreezeSelectedAtoms;
    QAction* actionImageByMolecules;
    QAction* actionImageByResidues;
    QAction* actionImageByAtoms;

    QAction* actionManipulateAllAtoms;
    QAction* actionManipulateSelectedAtoms;
    QAction* actionManipulateMolecules;

    QAction* actionAroundOrigin;
    QAction* actionFamiliarShape;

    QActionGroup* GeoScopeGroup;

// PROPERTIES ==================================================================
public:
    void ConnectPropertiesMenu(void);
    void SetupImportPropertiesMenu(void);

public slots:
    void NewProperty(void);
    void ManagePropertiesWP(void);
    void SelectedProperties(void);
    void DeleteInvalidProperties(void);
    void DeleteAllProperties(void);
    void ExportPropertiesWP(void);
    void CreateImportPropertyAction(QActionGroup* p_actGroup,QMenu* p_menu,CPluginObject* p_pod);
    void ImportPropertyAction(QAction* p_act);

    void UpdatePropertiesMenu(void);

// section of private data -----------------------------------------------------
private:
    QAction* actionNewProperty;
    QAction* actionManagePropertiesWP;
    QAction* actionExportPropertiesWP;

    QAction* actionSelectedProperties;
    QAction* actionDeleteInvalidProperties;
    QAction* actionDeleteAllProperties;

    QMenu* menuImportProperty;

// GRAPHICS ====================================================================
public:
    void ConnectGraphicsMenu(void);

public slots:

    void AutofitScene(void);
    void ProjectionChanged(QAction* p_action);
    void StereoChanged(QAction* p_action);
    void StereoSwapEyes(void);
    void DepthCueing(void);

    void NewObjectGlobalScope(void);
    void NewObjectActiveProfile(void);
    void ObjectListInfo(void);

    void NewProfile(void);
    void ActiveProfileInfo(void);
    void ProfileListInfo(void);

    void NewView(void);
    void PrimaryViewInfo(void);
    void ViewListInfo(void);

    void GraphicsManagerWP(void);
    void ProfileManagerWP(void);

    void UpdateGraphicsMenu(void);

// section of private data -----------------------------------------------------
private:
    QAction* actionNewObjectGlobalScope;
    QAction* actionNewObjectActiveProfile;
    QAction* actionObjectListInfo;

    QAction* actionNewProfile;
    QAction* actionActiveProfileInfo;
    QAction* actionProfileListInfo;

    QAction* actionAutofitScene;
    QAction* actionPerspective;
    QAction* actionOrthographic;
    QAction* actionStereoSwapEyes;
    QAction* actionDepthCueing;

    QAction* actionNewView;
    QAction* actionPrimaryViewInfo;
    QAction* actionViewListInfo;

    QAction* actionGraphicsManagerWP;
    QAction* actionProfileManagerWP;

    QMenu*              menuStereo;
    QActionGroup*       StereoGroup;
    QActionGroup*       ProjectionGroup;

// MOUSE =======================================================================
public:
    void ConnectMouseMenu(void);

public slots:
    void UpdateMouseMenu(void);

    void MouseModeChanged(QAction* p_act);

    void AtomManipModeChanged(bool checked);
    void DirManipModeChanged(bool checked);
    void MolManipModeChanged(bool checked);
    void ObjManipModeChanged(bool checked);

    void ObjectProperties(void);
    void ObjectPropertiesObjectSelected(void);
    void ObjectPropertiesDetached(void);

    void SelectRotationCentre(void);
    void RotationCentreSelected(void);
    void RotationCentreSelectionDetached(void);

// section of private data -----------------------------------------------------
private:
    QAction* actionSRTMode;
    QAction* actionSelectMode;
    QAction* actionRotateMode;
    QAction* actionTranslateMode;
    QAction* actionScaleMode;
    QAction* actionSelectRotationCentre;
    QAction* actionAtomManipulation;
    QAction* actionDirManipulation;
    QAction* actionMoleculeManipulation;
    QAction* actionObjectManipulation;
    QAction* actionObjectProperties;

    QActionGroup*   MouseModeGroup;

// TOOLS =======================================================================
private:
    // do not call this methods from common ConnectMenu
    // for details see, CMainWindow::RestoreDesktop()
    void ConnectToolsMenu(void);
    void CreateToolActions(QMenu* p_parent,QActionGroup* p_agroup,CXMLElement* p_ele);

public slots:
    void ToolsMenuActionTriggered(QAction* p_a);

// section of private data -----------------------------------------------------
private:
    QMenu*  menuTools;

// SETUP =======================================================================
public:
    void ConnectSetupMenu(void);

public slots:
    void ConfigureUnitSetup(void);
    void ConfigureGraphicsSetup(void);
    void ConfigureColorSetup(void);
    void ConfigureGlobalSetup(void);
    void ConfigureToolbars(void);
    void ConfigureJobScheduler(void);
    void SaveDefaultLayout(void);

// section of private data -----------------------------------------------------
private:
    QAction* actionToolbarsConfig;
    QAction* actionSaveLayout;
    QAction* actionUnitSetup;
    QAction* actionGraphicsSetup;
    QAction* actionColorSetup;
    QAction* actionJobSchedulerPreferences;
    QAction* actionGlobalSetup;

// HELP ========================================================================
public:
    void ConnectHelpMenu(void);

public slots:
    void NemesisHelp(void);
    void AboutNemesis(void);

// section of private data -----------------------------------------------------
private:
    QAction* actionNemesisHelp;
    QAction* actionAboutNemesis;
    QAction* actionAboutQt;

// =============================================================================

// setup -----------------------------------------------------------------------
private slots:
    // update main window setup (toolbars and shortcuts) as a result of
    // setup change in ConfigMainWindow
    void UpdateMainWindowSetup(const QString sextuuid);

// section of private data -----------------------------------------------------
private:
    CProject*           Project;
    QString             NEMESIS_TITLE;
    CSelectionRequest*  SelRequest;

protected:
    // shortcuts
    CHistoryList*           HistoryList;
    CGraphicsProfileList*   ProfileList;

    /// catch and process OnClose event
    virtual void closeEvent(QCloseEvent * event);

    // used to build import/export menu
    static bool SortPluginObjectByName(CPluginObject* p_left,CPluginObject* p_right);
};

//------------------------------------------------------------------------------

#endif
