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

#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <Project.hpp>
#include <ExtUUID.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <ContainerModel.hpp>

#include <PeriodicTableWidget.hpp>
#include <PeriodicTable.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>
#include <Structure.hpp>
#include <GraphicsProfileList.hpp>
#include <GraphicsProfileObject.hpp>
#include <GraphicsObject.hpp>
#include <MainWindow.hpp>

#include <PODesignerGeneral.hpp>

#include <GraphicsProfileDesigner.hpp>
#include "GraphicsProfileDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GraphicsProfileDesignerCB(void* p_data);

CExtUUID        GraphicsProfileDesignerID(
                    "{GRAPHICS_PROFILE_DESIGNER:310ef994-4099-4772-80e7-cffd2534fcbe}",
                    "Profile");

CPluginObject   GraphicsProfileDesignerObject(&NemesisCorePlugin,
                    GraphicsProfileDesignerID,WORK_PANEL_CAT,
                    GraphicsProfileDesignerCB);

// -----------------------------------------------------------------------------

QObject* GraphicsProfileDesignerCB(void* p_data)
{
    CGraphicsProfile* p_gp = static_cast<CGraphicsProfile*>(p_data);
    QObject* p_object = new CGraphicsProfileDesigner(p_gp);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsProfileDesigner::CGraphicsProfileDesigner(CGraphicsProfile* p_gp)
    : CProObjectDesigner(&GraphicsProfileDesignerObject,p_gp)
{
    Object = p_gp;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);

    // profile objects
    ProfileModel = p_gp->GetContainerModel(this);
    WidgetUI.profileTV->setModel(ProfileModel);

    if( ProfileModel != NULL ){
        connect(ProfileModel,SIGNAL(modelReset(void)),
                this,SLOT(ProfileTVSelectionChanged(void)));
        for(int i=0; i < ProfileModel->columnCount(); i++){
            WidgetUI.profileTV->resizeColumnToContents(i);
        }
    }

    // drag and drop support ---------------------
    WidgetUI.profileTV->setSelectionMode(QAbstractItemView::ExtendedSelection);
    WidgetUI.profileTV->setDragDropMode(QAbstractItemView::DragDrop);
    WidgetUI.profileTV->setDragEnabled(true);
    WidgetUI.profileTV->setAcceptDrops(true);
    WidgetUI.profileTV->setDropIndicatorShown(true);

    //--------------
    if( WidgetUI.profileTV->selectionModel() ){
        connect(WidgetUI.profileTV->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                this,SLOT(ProfileTVSelectionChanged(void)));
    }
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
    //--------------
    connect(WidgetUI.activateProfilePB,SIGNAL(clicked(bool)),
            this,SLOT(ProfileActivate(void)));
    //----------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(Object->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));
    //------------------
    connect(Object,SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));

    // init all values ---------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
    ProfileTVSelectionChanged();
}

//------------------------------------------------------------------------------

CGraphicsProfileDesigner::~CGraphicsProfileDesigner()
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsProfileDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGraphicsProfileDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_COMPOSITE,Object->GetType().GetName()) == false ) return;

    Changing = true;
        General->ApplyValues();
    Changing = false;

    Object->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//------------------------------------------------------------------------------

void CGraphicsProfileDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Reset ) {
        InitAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Apply ) {
        ApplyAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Close ) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsProfileDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.activateProfilePB->setEnabled(!Object->IsProfileActive());

    SetChangedFlag(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsProfileDesigner::ProfileTVSelectionChanged(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();
    bool set0 = true;
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
        set3 = ! Object->IsFirstObject(p_obj->GetObject());
        set4 = ! Object->IsLastObject(p_obj->GetObject());
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

void CGraphicsProfileDesigner::ProfileTVDblClicked(const QModelIndex& index)
{
    CGraphicsProfileObject* p_gpobj = NULL;
    p_gpobj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));
    if( p_gpobj == NULL ) return;
    CGraphicsObject* p_gobj = p_gpobj->GetObject();
    if( p_gobj == NULL ) return;
    p_gobj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CGraphicsProfileDesigner::ProfileNewObject(void)
{
    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString("{NEW_GRAPHICS_OBJECT_WORK_PANEL:b0568e3a-c78b-44f3-aee5-1e4739cc4be9}");
    CMainWindow::ShowAsDialog(mp_uuid,Object);
}

//------------------------------------------------------------------------------

void CGraphicsProfileDesigner::ProfileObjectUp(void)
{
    const QModelIndex& index = WidgetUI.profileTV->currentIndex();
    if( index.isValid() == false ) return;

    CGraphicsProfileObject* p_gpobj = NULL;
    p_gpobj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));
    if( p_gpobj == NULL ) return;

    Object->MoveObjectWH(p_gpobj->GetObject(),EMD_UP);
}

//------------------------------------------------------------------------------

void CGraphicsProfileDesigner::ProfileObjectDown(void)
{
    const QModelIndex& index = WidgetUI.profileTV->currentIndex();
    if( index.isValid() == false ) return;

    CGraphicsProfileObject* p_gpobj = NULL;
    p_gpobj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));
    if( p_gpobj == NULL ) return;

    Object->MoveObjectWH(p_gpobj->GetObject(),EMD_DOWN);
}

//------------------------------------------------------------------------------

void CGraphicsProfileDesigner::ProfileObjectShowHide(void)
{  
    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();

    CProject* p_project = Object->GetProject();
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

void CGraphicsProfileDesigner::ProfileObjectRemove(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();

    CProject* p_project = Object->GetProject();
    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,tr("remove objects from the profile"));
    if( p_history == NULL ) return;

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsProfileObject* p_obj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));
        if( p_obj ){
            CGraphicsObject* p_gobj = p_obj->GetObject();
            Object->RemoveObject(p_gobj,p_history);
        }
    }
    p_project->EndChangeWH();
}

//------------------------------------------------------------------------------

void CGraphicsProfileDesigner::ProfileObjectDelete(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();

    CProject* p_project = Object->GetProject();
    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,tr("delete graphics objects"));
    if( p_history == NULL ) return;

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsProfileObject* p_obj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));
        if( p_obj ){
            CGraphicsObject* p_gobj = p_obj->GetObject();
            p_gobj->RemoveFromBaseList(p_history);
        }
    }
    p_project->EndChangeWH();
}

//------------------------------------------------------------------------------

void CGraphicsProfileDesigner::ProfileObjectInfo(void)
{  
    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsProfileObject* p_obj = dynamic_cast<CGraphicsProfileObject*>(ProfileModel->GetItem(index));
        if( p_obj ){
            CGraphicsObject* p_gobj = p_obj->GetObject();
            p_gobj->OpenObjectDesigner();
        }
    }
}

//------------------------------------------------------------------------------

void CGraphicsProfileDesigner::ProfileActivate(void)
{
    Object->ActivateProfileWH();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsProfileDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    WidgetUI.controlW->setEnabled(! Object->GetProject()->IsHistoryLocked(EHCL_GRAPHICS));
    WidgetUI.activateProfilePB->setEnabled(! Object->GetProject()->IsHistoryLocked(EHCL_GRAPHICS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
