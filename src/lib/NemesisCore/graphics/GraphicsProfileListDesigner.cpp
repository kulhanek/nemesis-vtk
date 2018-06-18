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

#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <Project.hpp>
#include <ExtUUID.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <ContainerModel.hpp>
#include <GraphicsProfileList.hpp>
#include <MainWindow.hpp>

#include <PODesignerGeneral.hpp>

#include <GraphicsProfileListDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GraphicsProfileListDesignerCB(void* p_data);

CExtUUID        GraphicsProfileListDesignerID(
                    "{GRAPHICS_PROFILE_LIST_DESIGNER:172e56fa-1375-4e26-adb4-a5b229f288c6}",
                    "Profiles");

CPluginObject   GraphicsProfileListDesignerObject(&NemesisCorePlugin,
                    GraphicsProfileListDesignerID,WORK_PANEL_CAT,
                    ":/images/NemesisCore/graphics/ProfileList.svg",
                    GraphicsProfileListDesignerCB);

// -----------------------------------------------------------------------------

QObject* GraphicsProfileListDesignerCB(void* p_data)
{
    CGraphicsProfileList* p_gp = static_cast<CGraphicsProfileList*>(p_data);
    QObject* p_object = new CGraphicsProfileListDesigner(p_gp);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsProfileListDesigner::CGraphicsProfileListDesigner(CGraphicsProfileList* p_gp)
    : CProObjectDesigner(&GraphicsProfileListDesignerObject,p_gp)
{
    Object = p_gp;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);

    // profile objects ---------------------------
    ProfileModel = p_gp->GetContainerModel(this);
    WidgetUI.profileTV->setModel(ProfileModel);
    WidgetUI.profileTV->setSelectionMode(QAbstractItemView::ExtendedSelection);

    if( ProfileModel != NULL ){
        connect(ProfileModel,SIGNAL(modelReset(void)),
                this,SLOT(ProfileTVSelectionChanged(void)));
        for(int i=0; i < ProfileModel->columnCount(); i++){
            WidgetUI.profileTV->resizeColumnToContents(i);
        }
    }

    // connect slots -----------------------------
    if( WidgetUI.profileTV->selectionModel() ){
        connect(WidgetUI.profileTV->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                this,SLOT(ProfileTVSelectionChanged(void)));
    }
    //--------------
    connect(WidgetUI.profileTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(ProfileTVDblClicked(const QModelIndex&)));
    //--------------
    connect(WidgetUI.newProfilePB,SIGNAL(clicked(bool)),
            this,SLOT(NewProfile(void)));
    //--------------
    connect(WidgetUI.deleteProfilePB,SIGNAL(clicked(bool)),
            this,SLOT(DeleteProfile(void)));
    //--------------
    connect(WidgetUI.activateProfilePB,SIGNAL(clicked(bool)),
            this,SLOT(ActivateProfile(void)));
    //--------------
    connect(WidgetUI.profileInfoPB,SIGNAL(clicked(bool)),
            this,SLOT(ProfileInfo(void)));
    //------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(Object->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // init all values ---------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
    ProfileTVSelectionChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsProfileListDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGraphicsProfileListDesigner::ApplyAllValues(void)
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

void CGraphicsProfileListDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Reset ) {
        InitAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Apply  ) {
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

void CGraphicsProfileListDesigner::ProfileTVSelectionChanged(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();
    bool set1 = false;
    if( p_selmodel ) {
        set1 = p_selmodel->selectedRows().count() > 0;
    }

    WidgetUI.newProfilePB->setEnabled(true);
    WidgetUI.deleteProfilePB->setEnabled(set1);
    WidgetUI.activateProfilePB->setEnabled(set1);
    WidgetUI.profileInfoPB->setEnabled(set1);
}

//------------------------------------------------------------------------------

void CGraphicsProfileListDesigner::ProfileTVDblClicked(const QModelIndex& index)
{
    CGraphicsProfile* p_go = NULL;
    p_go = dynamic_cast<CGraphicsProfile*>(ProfileModel->GetItem(index));
    if( p_go == NULL ) return;
    p_go->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CGraphicsProfileListDesigner::NewProfile(void)
{
    Object->CreateProfileWH();
}

//------------------------------------------------------------------------------

void CGraphicsProfileListDesigner::DeleteProfile(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();

    CProject* p_project = Object->GetProject();
    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,tr("delete profiles"));
    if( p_history == NULL ) return;

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsProfile* p_obj = dynamic_cast<CGraphicsProfile*>(ProfileModel->GetItem(index));
        if( p_obj ){
            p_obj->RemoveFromBaseList(p_history);
        }
    }
    p_project->EndChangeWH();
}

//------------------------------------------------------------------------------

void CGraphicsProfileListDesigner::ActivateProfile(void)
{
    const QModelIndex& index = WidgetUI.profileTV->currentIndex();
    if( index.isValid() == false ) return;

    CGraphicsProfile* p_go = NULL;
    p_go = dynamic_cast<CGraphicsProfile*>(ProfileModel->GetItem(index));
    if( p_go == NULL ) return;

    p_go->ActivateProfileWH();
}

//------------------------------------------------------------------------------

void CGraphicsProfileListDesigner::ProfileInfo(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.profileTV->selectionModel();

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsProfile* p_obj = dynamic_cast<CGraphicsProfile*>(ProfileModel->GetItem(index));
        if( p_obj ){
            p_obj->OpenObjectDesigner();
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsProfileListDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    WidgetUI.controlW->setEnabled(! Object->GetProject()->IsHistoryLocked(EHCL_GRAPHICS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
