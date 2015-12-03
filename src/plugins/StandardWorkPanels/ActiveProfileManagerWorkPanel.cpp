// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include "ActiveProfileManagerWorkPanel.moc"
#include "ActiveProfileManagerWorkPanel.hpp"
#include "StandardWorkPanelsModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* ActiveProfileManagerWorkPanelCB(void* p_data);

CExtUUID        ActiveProfileManagerWorkPanelID(
                    "{ACTIVE_PROFILE_MANAGER_WORK_PANEL:340c9972-f129-4337-a375-e14f11632857}",
                    "Active Profile Manager");

CPluginObject   ActiveProfileManagerWorkPanelObject(&StandardWorkPanelsPlugin,
                    ActiveProfileManagerWorkPanelID,WORK_PANEL_CAT,
                    ":/images/StandardWorkPanels/ActiveProfileInfo.svg",
                    ActiveProfileManagerWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* ActiveProfileManagerWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CActiveProfileManagerWorkPanel requires active project");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(ActiveProfileManagerWorkPanelID,p_project);
    if( p_wp == NULL ) {
        p_wp = new CActiveProfileManagerWorkPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CActiveProfileManagerWorkPanel::CActiveProfileManagerWorkPanel(CProject* p_project)
    : CWorkPanel(&ActiveProfileManagerWorkPanelObject,p_project,EWPR_TOOL)
{
    WidgetUI.setupUi(this);

    ActiveProfile = NULL;
    ProfileModel = NULL;

    // drag and drop support ---------------------
    WidgetUI.profileTV->setSelectionMode(QAbstractItemView::ExtendedSelection);
    WidgetUI.profileTV->setDragDropMode(QAbstractItemView::DragDrop);
    WidgetUI.profileTV->setDragEnabled(true);
    WidgetUI.profileTV->setAcceptDrops(true);
    WidgetUI.profileTV->setDropIndicatorShown(true);

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
            this,SLOT(UpdateProfile(void)));
    //--------------
    connect(WidgetUI.activeProfilePB,SIGNAL(clicked(bool)),
            this,SLOT(OpenProfileDesigner(void)));
    //--------------
    connect(WidgetUI.profileTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(ProfileTVDblClicked(const QModelIndex&)));
    //--------------
    connect(WidgetUI.profileNewObjectPB,SIGNAL(clicked(bool)),
            this,SLOT(ProfileNewObject(void)));
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

    ProjectLockChanged(EHCM_LOCK_LEVEL);
    LoadWorkPanelSetup();
    UpdateProfile();
}

//------------------------------------------------------------------------------

CActiveProfileManagerWorkPanel::~CActiveProfileManagerWorkPanel()
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CActiveProfileManagerWorkPanel::WindowDockStatusChanged(bool docked)
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

void CActiveProfileManagerWorkPanel::ProjectNameChanged(CProject* p_project)
{
    if( p_project != GetProject() ) return;
    WidgetUI.projectPB->setText(GetProject()->GetName());
}

//------------------------------------------------------------------------------

void CActiveProfileManagerWorkPanel::OpenProjectDesigner(void)
{
    GetProject()->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CActiveProfileManagerWorkPanel::UpdateProfile(void)
{
    ActiveProfile = ProfileList->GetActiveProfile();

    // update active profile name
    if( ActiveProfile != NULL ){
        WidgetUI.activeProfilePB->setText(ActiveProfile->GetName());
    } else {
        WidgetUI.activeProfilePB->setText("");
    }

    if( ProfileModel != NULL ){
        delete ProfileModel;
        ProfileModel = NULL;
    }

    WidgetUI.profileObjectUpPB->setEnabled(false);
    WidgetUI.profileObjectDownPB->setEnabled(false);
    WidgetUI.profileObjectShowHidePB->setEnabled(false);
    WidgetUI.removeProfileObjectPB->setEnabled(false);
    WidgetUI.profileObjectInfoPB->setEnabled(false);

    if( ActiveProfile  == NULL ){
        WidgetUI.profileNewObjectPB->setEnabled(false);
        WidgetUI.profileTV->setModel(NULL);
        return;
    }

    WidgetUI.profileNewObjectPB->setEnabled(true);

    ProfileModel = ActiveProfile->GetContainerModel(this);

    WidgetUI.profileTV->setModel(ProfileModel);

    //--------------
    if( WidgetUI.profileTV->selectionModel() ){
        connect(WidgetUI.profileTV->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                this, SLOT(ProfileTVSelectionChanged(void)));
    }

    if( ProfileModel != NULL ){
        connect(ProfileModel, SIGNAL(modelReset(void)),
                this, SLOT(ProfileTVSelectionChanged(void)));
        for(int i=0; i < ProfileModel->columnCount(); i++){
            WidgetUI.profileTV->resizeColumnToContents(i);
        }
    }

    ProfileTVSelectionChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CActiveProfileManagerWorkPanel::OpenProfileDesigner(void)
{
    if( ActiveProfile == NULL ) return;
    ActiveProfile->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CActiveProfileManagerWorkPanel::ProfileTVSelectionChanged(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();
    bool set0 = ActiveProfile != NULL;
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
        set3 = ! ActiveProfile->IsFirstObject(p_obj->GetObject());
        set4 = ! ActiveProfile->IsLastObject(p_obj->GetObject());
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

void CActiveProfileManagerWorkPanel::ProfileTVDblClicked(const QModelIndex& index)
{
    CGraphicsProfileObject* p_gpobj = NULL;
    p_gpobj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));
    if( p_gpobj == NULL ) return;
    CGraphicsObject* p_gobj = p_gpobj->GetObject();
    if( p_gobj == NULL ) return;
    p_gobj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CActiveProfileManagerWorkPanel::ProfileNewObject(void)
{
    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString("{NEW_GRAPHICS_OBJECT_WORK_PANEL:b0568e3a-c78b-44f3-aee5-1e4739cc4be9}");
    CMainWindow::ShowAsDialog(mp_uuid,ActiveProfile);
}

//------------------------------------------------------------------------------

void CActiveProfileManagerWorkPanel::ProfileObjectUp(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();
    if( p_selmodel->selectedRows().count() != 1 ) return;

    CGraphicsProfileObject* p_gpobj = NULL;
    p_gpobj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(p_selmodel->selectedRows().at(0)));

    if( p_gpobj == NULL ) return;
    if( ActiveProfile == NULL ) return;

    ActiveProfile->MoveObjectWH(p_gpobj->GetObject(),EMD_UP);
}

//------------------------------------------------------------------------------

void CActiveProfileManagerWorkPanel::ProfileObjectDown(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();
    if( p_selmodel->selectedRows().count() != 1 ) return;

    CGraphicsProfileObject* p_gpobj = NULL;
    p_gpobj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(p_selmodel->selectedRows().at(0)));

    if( p_gpobj == NULL ) return;
    if( ActiveProfile == NULL ) return;

    ActiveProfile->MoveObjectWH(p_gpobj->GetObject(),EMD_DOWN);
}

//------------------------------------------------------------------------------

void CActiveProfileManagerWorkPanel::ProfileObjectShowHide(void)
{
    if( ActiveProfile == NULL ) return;

    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();

    CProject* p_project = ActiveProfile->GetProject();
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

void CActiveProfileManagerWorkPanel::ProfileObjectDelete(void)
{   
    if( ActiveProfile == NULL ) return;

    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();

    CProject* p_project = ActiveProfile->GetProject();
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

void CActiveProfileManagerWorkPanel::ProfileObjectRemove(void)
{
    if( ActiveProfile == NULL ) return;

    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();

    CProject* p_project = ActiveProfile->GetProject();
    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,tr("remove objects from the profile"));
    if( p_history == NULL ) return;

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsProfileObject* p_obj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));
        if( p_obj ){
            CGraphicsObject* p_gobj = p_obj->GetObject();
            if( p_gobj ){
                ActiveProfile->RemoveObject(p_gobj,p_history);
            }
        }
    }
    p_project->EndChangeWH();
}

//------------------------------------------------------------------------------

void CActiveProfileManagerWorkPanel::ProfileObjectInfo(void)
{
    if( ActiveProfile == NULL ) return;

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

void CActiveProfileManagerWorkPanel::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    WidgetUI.controlW3->setEnabled(! GetProject()->IsHistoryLocked(EHCL_GRAPHICS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


