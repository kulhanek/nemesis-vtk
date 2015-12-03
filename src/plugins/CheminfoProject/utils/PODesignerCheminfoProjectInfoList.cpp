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

#include <ErrorSystem.hpp>
#include <ContainerModel.hpp>
#include <GraphicsObject.hpp>
#include <ProObjectDesigner.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <QMessageBox>

#include "CheminfoProject.hpp"
#include "CheminfoProjectDatabase.hpp"
#include "CheminfoProjectStructureList.hpp"
#include "CheminfoProjectInfoList.hpp"
#include "PODesignerCheminfoProjectInfoList.hpp"
#include "PODesignerCheminfoProjectInfoList.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPODesignerCheminfoProjectInfoList::CPODesignerCheminfoProjectInfoList(QWidget* p_owner, CProObject* p_object,
                                       QWidget *p_widget)
    : QWidget(p_owner)
{
    List = p_object;
    if( List == NULL ){
        INVALID_ARGUMENT("p_object is NULL");
    }
    Widget = p_widget;
    CheminfoProject = dynamic_cast<CCheminfoProject*>( p_object->GetProject() );
    Database = CheminfoProject->GetDatabase();

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
    connect(WidgetUI.loadProjectsPB,SIGNAL(clicked(bool)),
            this,SLOT(LoadProjects(void)));
    //------------------
    connect(WidgetUI.newProjectPB,SIGNAL(clicked(bool)),
            this,SLOT(NewProject(void)));
    //------------------
    connect(WidgetUI.activateProjectPB,SIGNAL(clicked(bool)),
            this,SLOT(ActivateProject(void)));
    //------------------
    connect(WidgetUI.removeProjectPB,SIGNAL(clicked(bool)),
            this,SLOT(RemoveProject(void)));
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

void CPODesignerCheminfoProjectInfoList::InitValues(void)
{
    // nothing to do here
}


//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectInfoList::ApplyValues(void)
{
    // nothing to do here
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPODesignerCheminfoProjectInfoList::ObjectTVReset(void)
{
    WidgetUI.activateProjectPB->setEnabled(false);
    WidgetUI.removeProjectPB->setEnabled(false);

    int num = 0;
    if( ObjectsModel != NULL ) {
        num = ObjectsModel->rowCount();
    }
    QString text("%1");
    text = text.arg(num);
    WidgetUI.numOfObjectsLE->setText(text);
}

//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectInfoList::ObjectTVClicked(const QModelIndex& index)
{
    CCheminfoProjectInfo* p_chemlist = dynamic_cast<CCheminfoProjectInfo*>(ObjectsModel->GetItem(WidgetUI.objectsTV->currentIndex()));
    if( p_chemlist == NULL ){
        WidgetUI.activateProjectPB->setEnabled(false);
        WidgetUI.removeProjectPB->setEnabled(false);
    } else {
     //   WidgetUI.activateProjectPB->setEnabled(p_chemlist != List->GetActiveProject());
        WidgetUI.removeProjectPB->setEnabled(true);
    }
}

//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectInfoList::ObjectTVDblClicked(const QModelIndex& index)
{
    if (Database->IsLogged()) {
        // load clicked project info
        CCheminfoProjectInfo* p_obj = dynamic_cast<CCheminfoProjectInfo*>(ObjectsModel->GetItem(index));
        if (p_obj != NULL) {
            // load project structure list from database
            p_obj->LoadStructureList();
        }
    } else {
        QMessageBox::critical(NULL,"Error","Project has not connected database!\nConnect it first.",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
}

//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectInfoList::LoadProjects(void)
{
    if (Database->IsLogged()) {
        CCheminfoProjectInfoList* p_chemlist = dynamic_cast<CCheminfoProjectInfoList*>(List);
        if (p_chemlist != NULL) {
            // load project info list from database
            p_chemlist->LoadProjectInfoList();
        }
    } else {
        QMessageBox::critical(NULL,"Error","Project has not connected database!\nConnect it first.",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }

}

//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectInfoList::NewProject(void)
{
    //  List->CreateProjectWH(true);
}

//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectInfoList::ActivateProject(void)
{
    //CCheminfoProjectInfo* p_str = dynamic_cast<CCheminfoProjectInfo*>(ObjectsModel->GetItem(WidgetUI.objectsTV->currentIndex()));
   // if( p_str == NULL ) return;
  //  List->SetActiveProjectWH(p_str);
}

//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectInfoList::RemoveProject(void)
{
//    CCheminfoProjectInfo* p_str = dynamic_cast<CCheminfoProjectInfo*>(ObjectsModel->GetItem(WidgetUI.objectsTV->currentIndex()));
//    if( p_str == NULL ) return;
//    p_str->DeleteWH();
}

//------------------------------------------------------------------------------

void CPODesignerCheminfoProjectInfoList::ProjectLockChanged(EHistoryChangeMessage message)
{
    //if( message != EHCM_LOCK_LEVEL ) return;
  //  WidgetUI.controlW->setEnabled(! List->IsHistoryLocked(EHCL_CHEMINFO_PROJECTS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

