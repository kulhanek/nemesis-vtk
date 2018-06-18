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

#include <ErrorSystem.hpp>
#include <ContainerModel.hpp>
#include <GraphicsObject.hpp>
#include <Trajectory.hpp>
#include <ProObjectDesigner.hpp>
#include <Trajectory.hpp>
#include <CategoryUUID.hpp>
#include <TrajectoryList.hpp>
#include <Project.hpp>

#include <PODesignerTrajectories.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPODesignerTrajectories::CPODesignerTrajectories(QWidget* p_owner,CTrajectoryList* p_object,
                                       CProObjectDesigner* p_designer)
    : QWidget(p_owner)
{
    List = p_object;
    if( List == NULL ){
        INVALID_ARGUMENT("p_object is NULL");
    }
    Designer = p_designer;

    // set layout --------------------------------
    if( p_owner->layout() ) delete p_owner->layout();
    QVBoxLayout* p_layout = new QVBoxLayout();
    p_layout->setContentsMargins(0,0,0,0);
    p_layout->addWidget(this);
    p_owner->setLayout(p_layout);

    // load graphics -----------------------------
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
                this,SLOT(ObjectTVReset()));
    }

    // update list status
    ObjectTVReset();

    //------------------
    connect(WidgetUI.objectsTV,SIGNAL(clicked(const QModelIndex&)),
            this,SLOT(ObjectTVClicked(const QModelIndex&)));
    //------------------
    connect(WidgetUI.objectsTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(ObjectTVDblClicked(const QModelIndex&)));
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
    connect(List->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // set initial values ------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    // do not init values here, the owner of this component is
    // responsible to call InitValues explicitly
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPODesignerTrajectories::InitValues(void)
{
    // nothing to do here
}


//------------------------------------------------------------------------------

void CPODesignerTrajectories::ApplyValues(void)
{
    // nothing to do here
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPODesignerTrajectories::ObjectTVReset(void)
{
    WidgetUI.activateTrajectoryPB->setEnabled(false);
    WidgetUI.removeTrajectoryPB->setEnabled(false);

    int num = 0;
    if( ObjectsModel != NULL ) {
        num = ObjectsModel->rowCount();
    }
    QString text("%1");
    text = text.arg(num);
    WidgetUI.numOfObjectsLE->setText(text);
}

//------------------------------------------------------------------------------

void CPODesignerTrajectories::ObjectTVClicked(const QModelIndex& index)
{
    CTrajectory* p_str = dynamic_cast<CTrajectory*>(ObjectsModel->GetItem(WidgetUI.objectsTV->currentIndex()));
    if( p_str == NULL ){
        WidgetUI.activateTrajectoryPB->setEnabled(false);
        WidgetUI.removeTrajectoryPB->setEnabled(false);
    } else {
        WidgetUI.activateTrajectoryPB->setEnabled(p_str != List->GetActiveTrajectory());
        WidgetUI.removeTrajectoryPB->setEnabled(true);
    }
}

//------------------------------------------------------------------------------

void CPODesignerTrajectories::ObjectTVDblClicked(const QModelIndex& index)
{
    CProObject* p_obj = dynamic_cast<CProObject*>(ObjectsModel->GetItem(index));
    if( p_obj == NULL ) return;
    p_obj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CPODesignerTrajectories::NewTrajectory(void)
{
    List->CreateTrajectoryWH(true);
}

//------------------------------------------------------------------------------

void CPODesignerTrajectories::ActivateTrajectory(void)
{
    CTrajectory* p_str = dynamic_cast<CTrajectory*>(ObjectsModel->GetItem(WidgetUI.objectsTV->currentIndex()));
    if( p_str == NULL ) return;
    List->SetActiveTrajectoryWH(p_str);
}

//------------------------------------------------------------------------------

void CPODesignerTrajectories::RemoveTrajectory(void)
{
    CTrajectory* p_str = dynamic_cast<CTrajectory*>(ObjectsModel->GetItem(WidgetUI.objectsTV->currentIndex()));
    if( p_str == NULL ) return;
    p_str->DeleteWH();
}

//------------------------------------------------------------------------------

void CPODesignerTrajectories::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    WidgetUI.controlW->setEnabled(! List->IsHistoryLocked(EHCL_STRUCTURES));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

