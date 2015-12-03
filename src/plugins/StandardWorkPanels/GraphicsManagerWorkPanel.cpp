// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <QtGui>

#include <ErrorSystem.hpp>
#include <SmallString.hpp>
#include <SmallTime.hpp>
#include <XMLElement.hpp>

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <Graphics.hpp>
#include <Project.hpp>
#include <ProjectList.hpp>
#include <ContainerModel.hpp>
#include <MainWindow.hpp>
#include <GraphicsProfileObject.hpp>
#include <GraphicsObjectList.hpp>
#include <GraphicsProfileList.hpp>
#include <GraphicsObject.hpp>
#include <WorkPanelList.hpp>

#include "GraphicsManagerWorkPanel.moc"
#include "GraphicsManagerWorkPanel.hpp"
#include "StandardWorkPanelsModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GraphicsManagerWorkPanelCB(void* p_data);

CExtUUID        GraphicsManagerWorkPanelID(
                    "{GRAPHICS_MANAGER_WORK_PANEL:b202f126-3322-4cbe-8400-01899b71a818}",
                    "Graphics Manager");

CPluginObject   GraphicsManagerWorkPanelObject(&StandardWorkPanelsPlugin,
                    GraphicsManagerWorkPanelID,WORK_PANEL_CAT,
                    ":/images/StandardWorkPanels/GraphicsManager.svg",
                    GraphicsManagerWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* GraphicsManagerWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CGraphicsManagerWorkPanel requires active project");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(GraphicsManagerWorkPanelID,p_project);
    if( p_wp == NULL ) {
        p_wp = new CGraphicsManagerWorkPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsManagerWorkPanel::CGraphicsManagerWorkPanel(CProject* p_project)
    : CWorkPanel(&GraphicsManagerWorkPanelObject,p_project,EWPR_TOOL)
{
    WidgetUI.setupUi(this);

    ObjectsModel = NULL;
    ProfileList = NULL;
    SelectedProfile = NULL;
    ProfileModel = NULL;

    // drag and drop support ---------------------
    WidgetUI.objectsListTV->setSelectionMode(QAbstractItemView::ExtendedSelection);
    WidgetUI.objectsListTV->setDragDropMode(QAbstractItemView::DragOnly);
    WidgetUI.objectsListTV->setDragEnabled(true);

    WidgetUI.profileTV->setSelectionMode(QAbstractItemView::ExtendedSelection);
    WidgetUI.profileTV->setDragDropMode(QAbstractItemView::DragDrop);
    WidgetUI.profileTV->setDragEnabled(true);
    WidgetUI.profileTV->setAcceptDrops(true);
    WidgetUI.profileTV->setDropIndicatorShown(true);

    // objects list ------------------------------
    ObjectsModel = GetProject()->GetGraphics()->GetObjects()->GetContainerModel(this);
    WidgetUI.objectsListTV->setModel(ObjectsModel);

    if( ObjectsModel != NULL ){
        connect(ObjectsModel,SIGNAL(modelReset(void)),
                this,SLOT(ObjectsListTVSelectionChanged(void)));
        for(int i=0; i < ObjectsModel->columnCount(); i++){
            WidgetUI.objectsListTV->resizeColumnToContents(i);
        }
    }

    // project name management -------------------
    ProjectNameChanged(GetProject());
    //--------------
    connect(Projects,SIGNAL(OnProjectNameChanged(CProject*)),
            this,SLOT(ProjectNameChanged(CProject*)));
    //--------------
    connect(WidgetUI.projectPB,SIGNAL(clicked(bool)),
            this,SLOT(OpenProjectDesigner(void)));

    // profiles setup ----------------------------
    ProfileList = GetProject()->GetGraphics()->GetProfiles();
    connect(ProfileList,SIGNAL(OnGraphicsProfileListChanged(void)),
            this,SLOT(UpdateProfileList(void)));

    //--------------
    // GraphicsObjectList
    if( WidgetUI.objectsListTV->selectionModel() ){
        connect(WidgetUI.objectsListTV->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                this,SLOT(ObjectsListTVSelectionChanged(void)));
    }
    //--------------
    connect(WidgetUI.objectsListTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(ObjectsListTVDblClicked(const QModelIndex&)));
    //--------------
    connect(WidgetUI.newGraphicsObjectPB,SIGNAL(clicked(bool)),
            this,SLOT(NewGraphicsObject(void)));
    //--------------
    connect(WidgetUI.objectShowHidePB,SIGNAL(clicked(bool)),
            this,SLOT(ObjectShowHide(void)));
    //--------------
    connect(WidgetUI.deleteObjectPB,SIGNAL(clicked(bool)),
            this,SLOT(DeleteObject(void)));
    //--------------
    connect(WidgetUI.objectInfoPB,SIGNAL(clicked(bool)),
            this,SLOT(ObjectInfo(void)));

    //--------------
    connect(WidgetUI.activeProfilePB,SIGNAL(clicked(bool)),
            this,SLOT(OpenProfileDesigner(void)));
    //--------------
    connect(WidgetUI.profilesCB,SIGNAL(activated(int)),
            this,SLOT(UpdateProfile(int)));
    //--------------
    connect(WidgetUI.activateProfilePB,SIGNAL(clicked(bool)),
            this,SLOT(ActivateProfile(void)));
    //--------------
    connect(WidgetUI.newProfilePB,SIGNAL(clicked(bool)),
            this,SLOT(NewProfile(void)));
    //--------------
    connect(WidgetUI.deleteProfilePB,SIGNAL(clicked(bool)),
            this,SLOT(DeleteProfile(void)));
    //--------------
    connect(WidgetUI.profileInfoPB,SIGNAL(clicked(bool)),
            this,SLOT(ProfileInfo(void)));

    //--------------
    connect(WidgetUI.profileTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(ProfileTVDblClicked(const QModelIndex&)));
    //--------------
    connect(WidgetUI.profileNewObjectPB,SIGNAL(clicked(bool)),
            this,SLOT(ProfileNewObject(void)));
    //--------------
    connect(WidgetUI.profileObjectAddPB,SIGNAL(clicked(bool)),
            this,SLOT(ProfileObjectAdd(void)));
    //--------------
    connect(WidgetUI.profileObjectUpPB,SIGNAL(clicked(bool)),
            this,SLOT(ProfileObjectUp(void)));
    //--------------
    connect(WidgetUI.profileObjectDownPB,SIGNAL(clicked(bool)),
            this,SLOT(ProfileObjectDown(void)));
    //--------------
    connect(WidgetUI.profileObjectShowHidePB,SIGNAL(clicked(bool)),
            this,SLOT(ProfileObjectShowHide(void)));
    //--------------
    connect(WidgetUI.removeProfileObjectPB,SIGNAL(clicked(bool)),
            this,SLOT(ProfileObjectRemove(void)));
    //--------------
    connect(WidgetUI.deleteProfileObjectPB,SIGNAL(clicked(bool)),
            this,SLOT(ProfileObjectDelete(void)));
    //--------------
    connect(WidgetUI.profileObjectInfoPB,SIGNAL(clicked(bool)),
            this,SLOT(ProfileObjectInfo(void)));
    //------------------
    connect(GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // final initialization
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    LoadWorkPanelSetup();
    UpdateProfileList();
    ObjectsListTVSelectionChanged();
}

//------------------------------------------------------------------------------

CGraphicsManagerWorkPanel::~CGraphicsManagerWorkPanel()
{
    SaveWorkPanelSetup();
}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsManagerWorkPanel::WindowDockStatusChanged(bool docked)
{
    if( docked ){
        WidgetUI.projectNameW->hide();
    } else {
        WidgetUI.projectNameW->show();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsManagerWorkPanel::HideObjectList(bool set)
{
    if( set ){
        WidgetUI.objectsGB->hide();
    } else {
        WidgetUI.objectsGB->show();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsManagerWorkPanel::ProjectNameChanged(CProject* p_project)
{
    if( p_project != GetProject() ) return;
    WidgetUI.projectPB->setText(GetProject()->GetName());
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::OpenProjectDesigner(void)
{
    GetProject()->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsManagerWorkPanel::ObjectsListTVSelectionChanged(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.objectsListTV->selectionModel();
    bool set1 = false;
    if( p_selmodel ) {
        set1 = p_selmodel->selectedRows().count() > 0;
    }

    WidgetUI.newGraphicsObjectPB->setEnabled(true);
    WidgetUI.objectShowHidePB->setEnabled(set1);
    WidgetUI.deleteObjectPB->setEnabled(set1);
    WidgetUI.objectInfoPB->setEnabled(set1);

    WidgetUI.profileObjectAddPB->setEnabled(set1 && (SelectedProfile != NULL));
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ObjectsListTVDblClicked(const QModelIndex& index)
{
    CProObject* p_obj = NULL;
    p_obj = dynamic_cast<CProObject*>(ObjectsModel->GetItem(index));
    if( p_obj == NULL ) return;
    p_obj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::NewGraphicsObject(void)
{
    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString("{NEW_GRAPHICS_OBJECT_WORK_PANEL:b0568e3a-c78b-44f3-aee5-1e4739cc4be9}");
    CMainWindow::ShowAsDialog(mp_uuid,GetProject()->GetGraphics()->GetObjects());
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ObjectShowHide(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.objectsListTV->selectionModel();

    CProject* p_project = ProfileList->GetProject();
    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,tr("show/hide objects"));
    if( p_history == NULL ) return;

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsObject* p_obj = dynamic_cast<CGraphicsObject*>(ObjectsModel->GetItem(index));
        if( p_obj ){
            p_obj->ToggleShowHideFlagWH();
        }
    }
    p_project->EndChangeWH();
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::DeleteObject(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.objectsListTV->selectionModel();

    CProject* p_project = ProfileList->GetProject();
    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,tr("delete graphics objects"));
    if( p_history == NULL ) return;

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsObject* p_obj = dynamic_cast<CGraphicsObject*>(ObjectsModel->GetItem(index));
        if( p_obj ){
            p_obj->RemoveFromBaseList(p_history);
        }
    }
    p_project->EndChangeWH();
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ObjectInfo(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.objectsListTV->selectionModel();

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsObject* p_obj = dynamic_cast<CGraphicsObject*>(ObjectsModel->GetItem(index));
        if( p_obj ){
            p_obj->OpenObjectDesigner();
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsManagerWorkPanel::UpdateProfileList(void)
{
    WidgetUI.profilesCB->clear();

    // populate items
    int  index = 0;
    int  sel_index = -1;
    foreach(QObject* p_qobj,ProfileList->children()) {
        CGraphicsProfile* p_gp = static_cast<CGraphicsProfile*>(p_qobj);
        if( p_gp == SelectedProfile ){
            sel_index = index;
        }
        WidgetUI.profilesCB->addItem(p_gp->GetName());
        index++;
    }

    if( sel_index < 0 ){
        index = 0;
        SelectedProfile = ProfileList->GetActiveProfile();
        if( SelectedProfile == NULL ){
            if( ProfileList->children().count() > 0 ){
                SelectedProfile =  static_cast<CGraphicsProfile*>(ProfileList->children().first());
            }
        }
        foreach(QObject* p_qobj,ProfileList->children()) {
            CGraphicsProfile* p_gp = static_cast<CGraphicsProfile*>(p_qobj);
            if( p_gp == SelectedProfile ){
                sel_index = index;
            }
            index++;
        }
    }

    // update selected item
    WidgetUI.profilesCB->setCurrentIndex(sel_index);

    // update active profile name
    CGraphicsProfile* p_actpro = ProfileList->GetActiveProfile();
    if( p_actpro != NULL ){
        WidgetUI.activeProfilePB->setText(p_actpro->GetName());
    } else {
        WidgetUI.activeProfilePB->setText("");
    }

    bool enable = WidgetUI.profilesCB->currentIndex() >= 0;

    WidgetUI.activateProfilePB->setEnabled(enable);
    WidgetUI.newProfilePB->setEnabled(true);
    WidgetUI.deleteProfilePB->setEnabled(enable);
    WidgetUI.profileInfoPB->setEnabled(enable);

    WidgetUI.profileObjectAddPB->setEnabled((SelectedProfile!=NULL)&&(WidgetUI.objectsListTV->currentIndex().isValid()));

    UpdateProfile(WidgetUI.profilesCB->currentIndex());
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::OpenProfileDesigner(void)
{
    CGraphicsProfile* p_acpro = GetProject()->GetGraphics()->GetProfiles()->GetActiveProfile();
    if( p_acpro == NULL ) return;
    p_acpro->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ActivateProfile(void)
{
    int index = WidgetUI.profilesCB->currentIndex();
    if( index < 0 ) return;

    CGraphicsProfile* p_gp = static_cast<CGraphicsProfile*>(GetProject()->GetGraphics()->GetProfiles()->children()[index]);
    p_gp->ActivateProfileWH();
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::NewProfile(void)
{
    GetProject()->GetGraphics()->GetProfiles()->CreateProfileWH();
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::DeleteProfile(void)
{
    int index = WidgetUI.profilesCB->currentIndex();
    if( index < 0 ) return;

    CGraphicsProfile* p_gp = static_cast<CGraphicsProfile*>(GetProject()->GetGraphics()->GetProfiles()->children()[index]);
    p_gp->DeleteProfileWH();
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ProfileInfo(void)
{
    int index = WidgetUI.profilesCB->currentIndex();
    if( index < 0 ) return;

    CGraphicsProfile* p_gp = static_cast<CGraphicsProfile*>(GetProject()->GetGraphics()->GetProfiles()->children()[index]);
    p_gp->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsManagerWorkPanel::UpdateProfile(int index)
{
    if( ProfileModel != NULL ){
        delete ProfileModel;
        ProfileModel = NULL;
    }

    WidgetUI.profileObjectUpPB->setEnabled(false);
    WidgetUI.profileObjectDownPB->setEnabled(false);
    WidgetUI.profileObjectShowHidePB->setEnabled(false);
    WidgetUI.removeProfileObjectPB->setEnabled(false);
    WidgetUI.profileObjectInfoPB->setEnabled(false);

    if( index < 0 ){
        SelectedProfile = NULL;

        WidgetUI.profileNewObjectPB->setEnabled(false);

        WidgetUI.activateProfilePB->setEnabled(false);
        WidgetUI.newProfilePB->setEnabled(true);
        WidgetUI.deleteProfilePB->setEnabled(false);
        WidgetUI.profileInfoPB->setEnabled(false);

        WidgetUI.profileTV->setModel(NULL);
        return;
    }

    WidgetUI.profileNewObjectPB->setEnabled(true);

    WidgetUI.activateProfilePB->setEnabled(true);
    WidgetUI.newProfilePB->setEnabled(true);
    WidgetUI.deleteProfilePB->setEnabled(true);
    WidgetUI.profileInfoPB->setEnabled(true);

    SelectedProfile = static_cast<CGraphicsProfile*>(GetProject()->GetGraphics()->GetProfiles()->children()[index]);
    ProfileModel = SelectedProfile->GetContainerModel(this);

    WidgetUI.profileTV->setModel(ProfileModel);

    if( WidgetUI.profileTV->selectionModel() ){
        connect(WidgetUI.profileTV->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                this,SLOT(ProfileTVSelectionChanged(void)));
    }

    if( ProfileModel != NULL ){
        connect(ProfileModel,SIGNAL(modelReset(void)),
                this,SLOT(ProfileTVSelectionChanged(void)));
        for(int i=0; i < ProfileModel->columnCount(); i++){
            WidgetUI.profileTV->resizeColumnToContents(i);
        }
    }

    ProfileTVSelectionChanged();
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ProfileTVSelectionChanged(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();
    bool set0 = SelectedProfile != NULL;
    bool set1 = false;
    bool set2 = false;
    if( p_selmodel ) {
        set1 = p_selmodel->selectedRows().count() > 0;
        set2 = p_selmodel->selectedRows().count() == 1;
    }

    bool set3 = true;
    bool set4 = true;
    if( set2 && set0 ){
        CGraphicsProfileObject* p_obj = NULL;
        p_obj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(p_selmodel->selectedRows().at(0)));
        set3 = ! SelectedProfile->IsFirstObject(p_obj->GetObject());
        set4 = ! SelectedProfile->IsLastObject(p_obj->GetObject());
    }

    WidgetUI.profileNewObjectPB->setEnabled(set0);
    WidgetUI.profileObjectUpPB->setEnabled(set2 && set3);
    WidgetUI.profileObjectDownPB->setEnabled(set2 && set4);
    WidgetUI.profileObjectShowHidePB->setEnabled(set1);
    WidgetUI.removeProfileObjectPB->setEnabled(set1);
    WidgetUI.deleteProfileObjectPB->setEnabled(set1);
    WidgetUI.profileObjectInfoPB->setEnabled(set1);
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ProfileTVDblClicked(const QModelIndex& index)
{
    CGraphicsProfileObject* p_gpobj = NULL;
    p_gpobj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));
    if( p_gpobj == NULL ) return;
    CGraphicsObject* p_gobj = p_gpobj->GetObject();
    if( p_gobj == NULL ) return;
    p_gobj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ProfileNewObject(void)
{
    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString("{NEW_GRAPHICS_OBJECT_WORK_PANEL:b0568e3a-c78b-44f3-aee5-1e4739cc4be9}");
    CMainWindow::ShowAsDialog(mp_uuid,SelectedProfile);
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ProfileObjectAdd(void)
{
    if( SelectedProfile == NULL ) return;

    QItemSelectionModel* p_selmodel = WidgetUI.objectsListTV->selectionModel();

    CProject* p_project = SelectedProfile->GetProject();
    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,tr("add objects to the profile"));
    if( p_history == NULL ) return;

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsObject* p_obj = dynamic_cast<CGraphicsObject*>(ObjectsModel->GetItem(index));
        if( p_obj ){
            SelectedProfile->AddObject(p_obj,-1,p_history);
        }
    }
    p_project->EndChangeWH();
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ProfileObjectUp(void)
{
    const QModelIndex& index = WidgetUI.profileTV->currentIndex();
    if( index.isValid() == false ) return;

    CGraphicsProfileObject* p_gpobj = NULL;
    p_gpobj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));

    if( p_gpobj == NULL ) return;
    if( SelectedProfile == NULL ) return;

    SelectedProfile->MoveObjectWH(p_gpobj->GetObject(),EMD_UP);
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ProfileObjectDown(void)
{
    const QModelIndex& index = WidgetUI.profileTV->currentIndex();
    if( index.isValid() == false ) return;

    CGraphicsProfileObject* p_gpobj = NULL;
    p_gpobj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));

    if( p_gpobj == NULL ) return;
    if( SelectedProfile == NULL ) return;

    SelectedProfile->MoveObjectWH(p_gpobj->GetObject(),EMD_DOWN);
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ProfileObjectShowHide(void)
{
    if( SelectedProfile == NULL ) return;

    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();

    CProject* p_project = SelectedProfile->GetProject();
    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,tr("show/hide objects"));
    if( p_history == NULL ) return;

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsProfileObject* p_obj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));
        if( p_obj ){
            p_obj->ToggleShowHideFlagWH();
        }
    }
    p_project->EndChangeWH();
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ProfileObjectDelete(void)
{   
    if( SelectedProfile == NULL ) return;

    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();

    CProject* p_project = SelectedProfile->GetProject();
    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,tr("delete objects"));
    if( p_history == NULL ) return;

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsProfileObject* p_obj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));
        if( p_obj ){
            CGraphicsObject* p_gobj = p_obj->GetObject();
            if( p_gobj ){
                p_gobj->RemoveFromBaseList(p_history);
            }
        }
    }
    p_project->EndChangeWH();
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ProfileObjectRemove(void)
{
    if( SelectedProfile == NULL ) return;

    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();

    CProject* p_project = SelectedProfile->GetProject();
    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,tr("remove objects from the profile"));
    if( p_history == NULL ) return;

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsProfileObject* p_obj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));
        if( p_obj ){
            CGraphicsObject* p_gobj = p_obj->GetObject();
            if( p_gobj ){
                SelectedProfile->RemoveObject(p_gobj,p_history);
            }
        }
    }
    p_project->EndChangeWH();
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ProfileObjectInfo(void)
{
    if( SelectedProfile == NULL ) return;

    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsProfileObject* p_obj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));
        if( p_obj ){
            CGraphicsObject* p_gobj = p_obj->GetObject();
            if( p_gobj ){
                p_gobj->OpenObjectDesigner();
            }
        }
    }
}

//------------------------------------------------------------------------------

void CGraphicsManagerWorkPanel::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    WidgetUI.controlW1->setEnabled(! GetProject()->IsHistoryLocked(EHCL_GRAPHICS));
    WidgetUI.controlW2->setEnabled(! GetProject()->IsHistoryLocked(EHCL_GRAPHICS));
    WidgetUI.controlW3->setEnabled(! GetProject()->IsHistoryLocked(EHCL_GRAPHICS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


