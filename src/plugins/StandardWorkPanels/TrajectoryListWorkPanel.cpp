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

#include <QtGui>

#include <TrajectoryList.hpp>
#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <ProjectList.hpp>
#include <TrajectoryList.hpp>
#include <WorkPanelList.hpp>
#include <ContainerModel.hpp>
#include <ErrorSystem.hpp>

#include "StandardWorkPanelsModule.hpp"
#include "TrajectoryListWorkPanel.hpp"
#include "TrajectoryListWorkPanel.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* TrajectoryListWorkPanelCB(void* p_data);

CExtUUID        TrajectoryListWorkPanelID(
                    "{TRAJECTORY_LIST_WORK_PANEL:ff57f787-40f2-4d62-94c8-ce52bfe3257d}",
                    "Manage Trajectories");

CPluginObject   TrajectoryListWorkPanelObject(&StandardWorkPanelsPlugin,
                    TrajectoryListWorkPanelID,WORK_PANEL_CAT,
                    ":/images/StandardWorkPanels/ManageTrajectories.svg",
                    TrajectoryListWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* TrajectoryListWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CTrajectoryListWorkPanel requires active project");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(TrajectoryListWorkPanelID,p_project);
    if( p_wp == NULL ) {
        p_wp = new CTrajectoryListWorkPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectoryListWorkPanel::CTrajectoryListWorkPanel(CProject* p_owner)
    : CWorkPanel(&TrajectoryListWorkPanelObject,p_owner,EWPR_TOOL)
{   
    List = p_owner->GetTrajectories();

    WidgetUI.setupUi(this);

    // init model --------------------------------
    ObjectsModel = List->GetContainerModel(this);
    WidgetUI.objectsTV->setModel(ObjectsModel);

    WidgetUI.objectsTV->setDragDropMode(QAbstractItemView::DragOnly);
    WidgetUI.objectsTV->setDragEnabled(true);

    if( ObjectsModel != NULL ){
        for(int i=0; i < ObjectsModel->columnCount(); i++){
            WidgetUI.objectsTV->resizeColumnToContents(i);
        }
        // connect signals ---------------------------
        connect(ObjectsModel,SIGNAL(modelReset()),
                this,SLOT(ObjectsTVSelectionChanged(void)));
    }


    // signals -----------------------------------
    connect(Projects,SIGNAL(OnProjectNameChanged(CProject*)),
            this,SLOT(ProjectNameChanged(CProject*)));
    //--------------
    connect(WidgetUI.projectPB,SIGNAL(clicked(bool)),
            this,SLOT(OpenProjectDesigner(void)));

    //------------------
    connect(WidgetUI.objectsTV->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this,SLOT(ObjectsTVSelectionChanged(void)));
    //------------------
    connect(WidgetUI.objectsTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(ObjectsTVDblClicked(const QModelIndex&)));
    //------------------
    connect(WidgetUI.newTrajectoryPB,SIGNAL(clicked(bool)),
            this,SLOT(NewTrajectory(void)));
    //------------------
    connect(WidgetUI.activateTrajectoryPB,SIGNAL(clicked(bool)),
            this,SLOT(ActivateTrajectory(void)));
    //------------------
    connect(WidgetUI.removeTrajectoryPB,SIGNAL(clicked(bool)),
            this,SLOT(RemoveTrajectory(void)));
    //------------------
    connect(WidgetUI.infoTrajectoryPB,SIGNAL(clicked(bool)),
            this,SLOT(TrajectoryInfo(void)));

    //--------------
    connect(GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // finalization
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    ObjectsTVSelectionChanged();
    ProjectNameChanged(GetProject());
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CTrajectoryListWorkPanel::~CTrajectoryListWorkPanel(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectoryListWorkPanel::ProjectNameChanged(CProject* p_project)
{
    if( p_project != GetProject() ) return;
    WidgetUI.projectPB->setText(p_project->GetName());
}

//------------------------------------------------------------------------------

void CTrajectoryListWorkPanel::OpenProjectDesigner(void)
{
    if( GetProject() == NULL ) return;
    GetProject()->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CTrajectoryListWorkPanel::WindowDockStatusChanged(bool docked)
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

void CTrajectoryListWorkPanel::ObjectsTVSelectionChanged(void)
{
    QModelIndexList selected_rows = WidgetUI.objectsTV->selectionModel()->selectedRows();

    WidgetUI.activateTrajectoryPB->setEnabled(selected_rows.count() == 1);
    WidgetUI.removeTrajectoryPB->setEnabled(selected_rows.count() > 0);
    WidgetUI.infoTrajectoryPB->setEnabled(selected_rows.count() > 0);

    int num = 0;
    if( ObjectsModel != NULL ) {
        num = ObjectsModel->rowCount();
    }
    QString text("%1");
    text = text.arg(num);
    WidgetUI.numOfObjectsLE->setText(text);
}

//------------------------------------------------------------------------------

void CTrajectoryListWorkPanel::ObjectsTVDblClicked(const QModelIndex& index)
{
    CProObject* p_obj = dynamic_cast<CProObject*>(ObjectsModel->GetItem(index));
    if( p_obj == NULL ) return;
    p_obj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CTrajectoryListWorkPanel::NewTrajectory(void)
{
    List->CreateTrajectoryWH(true);
}

//------------------------------------------------------------------------------

void CTrajectoryListWorkPanel::ActivateTrajectory(void)
{
    QModelIndexList selected_rows = WidgetUI.objectsTV->selectionModel()->selectedRows();
    if(selected_rows.count() != 1 ) return;
    CTrajectory* p_str = dynamic_cast<CTrajectory*>(ObjectsModel->GetItem(selected_rows[0]));
    if( p_str == NULL ) return;
    List->SetActiveTrajectoryWH(p_str);
}

//------------------------------------------------------------------------------

void CTrajectoryListWorkPanel::RemoveTrajectory(void)
{
    QModelIndexList selected_rows = WidgetUI.objectsTV->selectionModel()->selectedRows();
    if( selected_rows.count() > 1 ){
        CHistoryNode* p_history = List->BeginChangeWH(EHCL_TRAJECTORIES,"delete trajectories");
        if( p_history == NULL ) return;
        for(int i=0; i < selected_rows.count(); i++){
            CTrajectory* p_str = dynamic_cast<CTrajectory*>(ObjectsModel->GetItem(selected_rows[i]));
            if( p_str != NULL ){
                p_str->RemoveFromBaseList(p_history);
            }
        }
        List->EndChangeWH();
    } else {
        CTrajectory* p_str = dynamic_cast<CTrajectory*>(ObjectsModel->GetItem(selected_rows[0]));
        if( p_str == NULL ) return;
        p_str->DeleteWH();
    }
}

//------------------------------------------------------------------------------

void CTrajectoryListWorkPanel::TrajectoryInfo(void)
{
    QModelIndexList selected_rows = WidgetUI.objectsTV->selectionModel()->selectedRows();
    for(int i=0; i < selected_rows.count(); i++){
        CTrajectory* p_str = dynamic_cast<CTrajectory*>(ObjectsModel->GetItem(selected_rows[i]));
        if( p_str != NULL ){
            p_str->OpenObjectDesigner();
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectoryListWorkPanel::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    bool set1;
    set1 = ! List->IsHistoryLocked(EHCL_STRUCTURES);

    WidgetUI.tools1W->setEnabled(set1);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================










