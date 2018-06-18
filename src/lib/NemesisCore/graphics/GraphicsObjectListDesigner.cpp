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
#include <GraphicsObjectList.hpp>
#include <MainWindow.hpp>

#include <PODesignerGeneral.hpp>

#include <GraphicsObjectListDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GraphicsObjectListDesignerCB(void* p_data);

CExtUUID        GraphicsObjectListDesignerID(
                    "{GRAPHICS_OBJECT_LIST_DESIGNER:9a561a4e-0c00-435a-9118-ea2926ff1ef2}",
                    "Objects");

CPluginObject   GraphicsObjectListDesignerObject(&NemesisCorePlugin,
                    GraphicsObjectListDesignerID,WORK_PANEL_CAT,
                    ":/images/NemesisCore/graphics/ObjectList.svg",
                    GraphicsObjectListDesignerCB);

// -----------------------------------------------------------------------------

QObject* GraphicsObjectListDesignerCB(void* p_data)
{
    CGraphicsObjectList* p_gp = static_cast<CGraphicsObjectList*>(p_data);
    QObject* p_object = new CGraphicsObjectListDesigner(p_gp);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsObjectListDesigner::CGraphicsObjectListDesigner(CGraphicsObjectList* p_gp)
    : CProObjectDesigner(&GraphicsObjectListDesignerObject,p_gp)
{
    Object = p_gp;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);

    // profile objects ---------------------------
    ObjectModel = p_gp->GetContainerModel(this);
    WidgetUI.objectTV->setModel(ObjectModel);

    if( ObjectModel != NULL ){
        connect(ObjectModel,SIGNAL(modelReset(void)),
                this,SLOT(ObjectTVSelectionChnaged(void)));
        for(int i=0; i < ObjectModel->columnCount(); i++){
            WidgetUI.objectTV->resizeColumnToContents(i);
        }
    }

    // drag and drop support ---------------------
    WidgetUI.objectTV->setSelectionMode(QAbstractItemView::ExtendedSelection);
    WidgetUI.objectTV->setDragDropMode(QAbstractItemView::DragOnly);
    WidgetUI.objectTV->setDragEnabled(true);

    // connect slots -----------------------------
    if( WidgetUI.objectTV->selectionModel() ){
        connect(WidgetUI.objectTV->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                this,SLOT(ObjectTVSelectionChnaged(void)));
    }
    //--------------
    connect(WidgetUI.objectTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(ObjectTVDblClicked(const QModelIndex&)));
    //--------------
    connect(WidgetUI.newObjectPB,SIGNAL(clicked(bool)),
            this,SLOT(NewObject(void)));
    //--------------
    connect(WidgetUI.objectShowHidePB,SIGNAL(clicked(bool)),
            this,SLOT(ObjectShowHide(void)));
    //--------------
    connect(WidgetUI.objectDeletePB,SIGNAL(clicked(bool)),
            this,SLOT(ObjectDelete(void)));
    //--------------
    connect(WidgetUI.objectInfoPB,SIGNAL(clicked(bool)),
            this,SLOT(ObjectInfo(void)));
    //------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(Object->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // init all values ---------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
    ObjectTVSelectionChnaged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsObjectListDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGraphicsObjectListDesigner::ApplyAllValues(void)
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

void CGraphicsObjectListDesigner::ButtonBoxClicked(QAbstractButton *button)
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

void CGraphicsObjectListDesigner::ObjectTVSelectionChnaged(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.objectTV->selectionModel();
    bool set1 = false;
    if( p_selmodel ) {
        set1 = p_selmodel->selectedRows().count() > 0;
    }

    WidgetUI.newObjectPB->setEnabled(true);
    WidgetUI.objectShowHidePB->setEnabled(set1);
    WidgetUI.objectDeletePB->setEnabled(set1);
    WidgetUI.objectInfoPB->setEnabled(set1);
}

//------------------------------------------------------------------------------

void CGraphicsObjectListDesigner::ObjectTVDblClicked(const QModelIndex& index)
{
    CGraphicsObject* p_go = NULL;
    p_go = dynamic_cast<CGraphicsObject*>(ObjectModel->GetItem(index));
    if( p_go == NULL ) return;
    p_go->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CGraphicsObjectListDesigner::NewObject(void)
{
    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString("{NEW_GRAPHICS_OBJECT_WORK_PANEL:b0568e3a-c78b-44f3-aee5-1e4739cc4be9}");
    CMainWindow::ShowAsDialog(mp_uuid,NULL);
}

//------------------------------------------------------------------------------

void CGraphicsObjectListDesigner::ObjectShowHide(void)
{   
    QItemSelectionModel* p_selmodel = WidgetUI.objectTV->selectionModel();

    CProject* p_project = Object->GetProject();
    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,tr("show/hide graphics objects"));
    if( p_history == NULL ) return;

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsObject* p_obj = dynamic_cast<CGraphicsObject*>(ObjectModel->GetItem(index));
        if( p_obj ){
            p_obj->ToggleShowHideFlagWH();
        }
    }
    p_project->EndChangeWH();
}

//------------------------------------------------------------------------------

void CGraphicsObjectListDesigner::ObjectDelete(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.objectTV->selectionModel();

    CProject* p_project = Object->GetProject();
    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,tr("delete graphics objects"));
    if( p_history == NULL ) return;

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsObject* p_obj = dynamic_cast<CGraphicsObject*>(ObjectModel->GetItem(index));
        if( p_obj ){
            p_obj->RemoveFromBaseList(p_history);
        }
    }
    p_project->EndChangeWH();
}

//------------------------------------------------------------------------------

void CGraphicsObjectListDesigner::ObjectInfo(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.objectTV->selectionModel();

    foreach(QModelIndex index, p_selmodel->selectedRows()){
        if( index.column() > 0 ) continue; // take only first column
        CGraphicsObject* p_obj = dynamic_cast<CGraphicsObject*>(ObjectModel->GetItem(index));
        if( p_obj ){
            p_obj->OpenObjectDesigner();
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsObjectListDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    WidgetUI.controlW->setEnabled(! Object->GetProject()->IsHistoryLocked(EHCL_GRAPHICS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
