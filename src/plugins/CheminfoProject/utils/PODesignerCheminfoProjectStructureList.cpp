// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <QMessageBox>
#include <QSortFilterProxyModel>

#include <ErrorSystem.hpp>
#include <ContainerModel.hpp>
#include <GraphicsObject.hpp>
#include <ProObjectDesigner.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>

#include "../workpanels/CheminfoStructureWorkPanel.hpp"

#include "CheminfoProjectStructureList.hpp"
#include "CheminfoProjectInfoList.hpp"
#include "PODesignerCheminfoProjectStructureList.hpp"
#include "PODesignerCheminfoProjectStructureList.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPODesignerCheminfoProjectStructureList::CPODesignerCheminfoProjectStructureList(QWidget* p_owner, CCheminfoProjectStructureList* p_object,
                                       QWidget *p_widget)
    : QWidget(p_owner)
{
    List = p_object;
    if( List == NULL ){
        INVALID_ARGUMENT("p_object is NULL");
    }
    Widget = p_widget;

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

    // enable sorting on columns
    SortFilter = new QSortFilterProxyModel(this);
    SortFilter->setSourceModel(ObjectsModel);
    SortFilter->sort (0);

    WidgetUI.objectsTV->setModel(SortFilter);

    WidgetUI.objectsTV->setSortingEnabled(true);
    WidgetUI.objectsTV->setAlternatingRowColors(true);
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
    connect(WidgetUI.showStructuresPB,SIGNAL(clicked(bool)),
            this,SLOT(LoadStructures(void)));
    //------------------
    connect(WidgetUI.newStructurePB,SIGNAL(clicked(bool)),
            this,SLOT(NewStructure(void)));
    //------------------
    connect(WidgetUI.activateStructurePB,SIGNAL(clicked(bool)),
            this,SLOT(ActivateStructure(void)));
    //------------------
    connect(WidgetUI.removeStructurePB,SIGNAL(clicked(bool)),
            this,SLOT(RemoveStructure(void)));
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

void CPODesignerCheminfoProjectStructureList::InitValues(void)
{
    // nothing to do here
}


//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectStructureList::ApplyValues(void)
{
    // nothing to do here
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPODesignerCheminfoProjectStructureList::ObjectTVReset(void)
{
    WidgetUI.activateStructurePB->setEnabled(false);
    WidgetUI.removeStructurePB->setEnabled(false);

    int num = 0;
    if( ObjectsModel != NULL ) {
        num = ObjectsModel->rowCount();
    }
    QString text("%1");
    text = text.arg(num);
    WidgetUI.numOfObjectsLE->setText(text);
}

//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectStructureList::ObjectTVClicked(const QModelIndex& index)
{
    CCheminfoProjectStructure* p_chemstr = dynamic_cast<CCheminfoProjectStructure*>(ObjectsModel->GetItem(SortFilter->mapToSource(index)));
    if( p_chemstr == NULL ){
        WidgetUI.activateStructurePB->setEnabled(false);
        WidgetUI.removeStructurePB->setEnabled(false);
    } else {
     //   WidgetUI.activateStructurePB->setEnabled(p_chemlist != List->GetActiveProject());
        WidgetUI.removeStructurePB->setEnabled(true);
    }
}

//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectStructureList::ObjectTVDblClicked(const QModelIndex& index)
{
    CCheminfoProjectStructure* p_obj = dynamic_cast<CCheminfoProjectStructure*>(ObjectsModel->GetItem(SortFilter->mapToSource(index)));
    if( p_obj == NULL ) return;
    CCheminfoStructureWorkPanel* p_workpanel = dynamic_cast<CCheminfoStructureWorkPanel*>(Widget);
    if( p_workpanel == NULL ) return;
    p_workpanel->ClickedOnProjectStructure(p_obj);

}

//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectStructureList::LoadStructures(void)
{
    CCheminfoProjectInfoList* p_obj = dynamic_cast<CCheminfoProjectInfoList*>(List);
    if (p_obj != NULL) {
        p_obj->LoadProjectInfoList();
    }
}

//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectStructureList::NewStructure(void)
{
    //  List->CreateProjectWH(true);
}

//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectStructureList::ActivateStructure(void)
{
    //CCheminfoProjectInfo* p_str = dynamic_cast<CCheminfoProjectInfo*>(ObjectsModel->GetItem(WidgetUI.objectsTV->currentIndex()));
   // if( p_str == NULL ) return;
  //  List->SetActiveProjectWH(p_str);
}

//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectStructureList::RemoveStructure(void)
{
//    CCheminfoProjectInfo* p_str = dynamic_cast<CCheminfoProjectInfo*>(ObjectsModel->GetItem(WidgetUI.objectsTV->currentIndex()));
//    if( p_str == NULL ) return;
//    p_str->DeleteWH();
}

//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectStructureList::ProjectLockChanged(EHistoryChangeMessage message)
{
    //if( message != EHCM_LOCK_LEVEL ) return;
  //  WidgetUI.controlW->setEnabled(! List->IsHistoryLocked(EHCL_CHEMINFO_PROJECTS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

