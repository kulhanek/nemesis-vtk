// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2011 Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <CategoryUUID.hpp>
#include <DesktopSystem.hpp>
#include <ProjectList.hpp>
#include <Project.hpp>
#include <ContainerModel.hpp>

#include <RestraintList.hpp>
#include <Structure.hpp>
#include <StructureList.hpp>
#include <WorkPanelList.hpp>

#include "RestraintsManagerWorkPanel.hpp"
#include "StandardWPModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* RestraintsManagerWorkPanelCB(void* p_data);

CExtUUID        RestraintsManagerWorkPanelID(
                    "{RESTRAINTS_MANAGER_WP:e65b89fd-9724-4a33-ab70-e9acccca6d7a}",
                    "Restraints");

CPluginObject   RestraintsManagerWorkPanelObject(&StandardWPPlugin,
                    RestraintsManagerWorkPanelID,WORK_PANEL_CAT,
                    ":/images/StandardWP/RestraintManagement.svg",
                    RestraintsManagerWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* RestraintsManagerWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("Active project required.");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(RestraintsManagerWorkPanelID,p_project);
    if( p_wp == NULL ) {
        p_wp = new CRestraintsManagerWorkPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRestraintsManagerWorkPanel::CRestraintsManagerWorkPanel(CProject* p_project)
    : CWorkPanel(&RestraintsManagerWorkPanelObject,p_project,EWPR_TOOL)
{
    // set up ui file
    WidgetUI.setupUi(this);

    RestraintsModel = NULL;
    ActiveStructure = NULL;

    // connect slots -----------------------------
    connect(GetProject()->GetStructures(),SIGNAL(OnStructureListChanged(void)),
            this,SLOT(UpdateStructureList(void)));
    //--------------
    connect(WidgetUI.structureCB,SIGNAL(currentIndexChanged(int)),
            this,SLOT(StructureIndexChanged(int)));
    //--------------
    connect(WidgetUI.infoStructurePB,SIGNAL(clicked(bool)),
            this,SLOT(StructureInfo(void)));
    //--------------
    connect(WidgetUI.restraintsTV,SIGNAL(clicked(const QModelIndex&)),
            this,SLOT(RestraintsTVClicked(const QModelIndex&)));
    //--------------
    connect(WidgetUI.restraintsTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(RestraintsTVDblClicked(const QModelIndex&)));
    //--------------
    connect(WidgetUI.infoRestraintPB,SIGNAL(clicked(bool)),
            this,SLOT(RestraintInfo(void)));
    //--------------
    connect(WidgetUI.toggleRestraintEnablePB,SIGNAL(clicked(bool)),
            this,SLOT(RestraintToggleEnable(void)));
    //--------------
    connect(WidgetUI.newRestraintPB,SIGNAL(clicked(bool)),
            this,SLOT(RestraintNew(void)));
    //--------------
    connect(WidgetUI.deleteRestraintPB,SIGNAL(clicked(bool)),
            this,SLOT(RestraintDelete(void)));
    //--------------
    connect(WidgetUI.deleteAllRestraintsPB,SIGNAL(clicked(bool)),
            this,SLOT(DeleteAllRestraints(void)));
    //--------------
    connect(WidgetUI.deleteInvalidRestraintsPB,SIGNAL(clicked(bool)),
            this,SLOT(DeleteInvalidRestraints(void)));
    //--------------
    connect(WidgetUI.enableRestraintsCB,SIGNAL(clicked(bool)),
            this,SLOT(EnableRestraints(bool)));
    //--------------
    connect(GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // check for initial locks
    ProjectLockChanged(EHCM_LOCK_LEVEL);

    // populate lists
    UpdateStructureList();

    LoadWorkPanelSetup();  
}

// -----------------------------------------------------------------------------

CRestraintsManagerWorkPanel::~CRestraintsManagerWorkPanel(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRestraintsManagerWorkPanel::UpdateStructureList(void)
{
    WidgetUI.structureCB->clear();

    int active_index = -1;
    int active_structure_index = -1;
    int index = 0;

    // avoid signal StructureIndexChanged(int) emission when structureCB->addItem
    WidgetUI.structureCB->blockSignals(true);

    foreach(QObject *p_obj, GetProject()->GetStructures()->children()){
        CStructure* p_str = static_cast<CStructure*>(p_obj);
        WidgetUI.structureCB->addItem(p_str->GetName());
        if( p_str == ActiveStructure ){
            active_index = index;
        }
        if( p_str == GetProject()->GetActiveStructure() ){
            active_structure_index = index;
        }
        index++;
    }

    WidgetUI.structureCB->blockSignals(false);

    if( active_index >= 0 ){
        // this emits StructureIndexChanged
        WidgetUI.structureCB->setCurrentIndex(active_index);
        StructureIndexChanged(active_index);
    } else {
        if( active_structure_index >= 0 ){
        ActiveStructure = GetProject()->GetActiveStructure();
        // this emits StructureIndexChanged
        WidgetUI.structureCB->setCurrentIndex(active_structure_index);
        StructureIndexChanged(active_structure_index);
        } else {
            ActiveStructure = NULL;
            StructureIndexChanged(-1);
        }
    }
}

//------------------------------------------------------------------------------

void CRestraintsManagerWorkPanel::StructureIndexChanged(int index)
{
    if( RestraintsModel != NULL ){
        delete RestraintsModel;
        RestraintsModel = NULL;
    }

    if( ActiveStructure ){
        ActiveStructure->GetRestraints()->disconnect(this);
    }

    int loc_index = 0;
    foreach(QObject *p_obj, GetProject()->GetStructures()->children()){
        CStructure* p_str = static_cast<CStructure*>(p_obj);
        if( loc_index == index ){
            ActiveStructure = p_str;
        }
        loc_index++;
    }

    WidgetUI.newRestraintPB->setEnabled(ActiveStructure != NULL);
    WidgetUI.enableRestraintsCB->setEnabled(ActiveStructure != NULL);
    WidgetUI.enableRestraintsCB->setChecked(false);
    WidgetUI.deleteAllRestraintsPB->setEnabled(ActiveStructure != NULL);

    WidgetUI.infoStructurePB->setEnabled(ActiveStructure != NULL);

    if( (index < 0) || (ActiveStructure == NULL) ){
        RestraintsTVReset();
        return;
    }

    RestraintsModel = ActiveStructure->GetRestraints()->GetContainerModel(this);
    WidgetUI.restraintsTV->setModel(RestraintsModel);

    connect(ActiveStructure->GetRestraints(),SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(UpdateRestraintListStatus()));

    if( RestraintsModel != NULL ){
        connect(RestraintsModel,SIGNAL(modelReset(void)),
                this,SLOT(RestraintsTVReset(void)));
        for(int i=0; i < RestraintsModel->columnCount(); i++){
            WidgetUI.restraintsTV->resizeColumnToContents(i);
        }
    }

    RestraintsTVReset();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRestraintsManagerWorkPanel::RestraintsTVReset(void)
{
    WidgetUI.infoRestraintPB->setEnabled(false);
    WidgetUI.toggleRestraintEnablePB->setEnabled(false);
    WidgetUI.deleteRestraintPB->setEnabled(false);

    int num = 0;
    if( RestraintsModel != NULL ) {
        num = RestraintsModel->rowCount();
    }
    WidgetUI.deleteAllRestraintsPB->setEnabled(num > 0);    

    WidgetUI.enableRestraintsCB->setEnabled(ActiveStructure != NULL);
    if( ActiveStructure ){
        WidgetUI.enableRestraintsCB->setChecked(ActiveStructure->GetRestraints()->IsEnabled());
    } else {
        WidgetUI.enableRestraintsCB->setChecked(false);
    }

    QString text("%1");
    text = text.arg(num);
    WidgetUI.numOfRestraintsLE->setText(text);
}

//------------------------------------------------------------------------------

void CRestraintsManagerWorkPanel::RestraintsTVClicked(const QModelIndex& index)
{
    CRestraint* p_obj = NULL;
    p_obj = dynamic_cast<CRestraint*>(RestraintsModel->GetItem(index));
    bool obj_enabled = p_obj != NULL;

    WidgetUI.infoRestraintPB->setEnabled(obj_enabled);
    WidgetUI.toggleRestraintEnablePB->setEnabled(obj_enabled);
    WidgetUI.deleteRestraintPB->setEnabled(obj_enabled);
}

//------------------------------------------------------------------------------

void CRestraintsManagerWorkPanel::RestraintsTVDblClicked(const QModelIndex& index)
{
    CRestraint* p_obj = NULL;
    p_obj = dynamic_cast<CRestraint*>(RestraintsModel->GetItem(index));
    if( p_obj == NULL ) return;
    p_obj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CRestraintsManagerWorkPanel::RestraintInfo(void)
{
    const QModelIndex& index = WidgetUI.restraintsTV->currentIndex();
    if( index.isValid() == false ) return;

    CRestraint* p_obj = NULL;
    p_obj = dynamic_cast<CRestraint*>(RestraintsModel->GetItem(index));
    if( p_obj == NULL ) return;

    p_obj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CRestraintsManagerWorkPanel::RestraintToggleEnable(void)
{
    const QModelIndex& index = WidgetUI.restraintsTV->currentIndex();
    if( index.isValid() == false ) return;

    CRestraint* p_obj = NULL;
    p_obj = dynamic_cast<CRestraint*>(RestraintsModel->GetItem(index));
    if( p_obj == NULL ) return;

    p_obj->ToggleEnabledDisabledWH();
}

//------------------------------------------------------------------------------

void CRestraintsManagerWorkPanel::RestraintNew(void)
{
    if( ActiveStructure == NULL ) return;
    ActiveStructure->GetRestraints()->CreateRestraintWH();
}

//------------------------------------------------------------------------------

void CRestraintsManagerWorkPanel::RestraintDelete(void)
{
    const QModelIndex& index = WidgetUI.restraintsTV->currentIndex();
    if( index.isValid() == false ) return;

    CRestraint* p_obj = NULL;
    p_obj = dynamic_cast<CRestraint*>(RestraintsModel->GetItem(index));
    if( p_obj == NULL ) return;

    p_obj->DeleteRestraintWH();
}

//------------------------------------------------------------------------------

void CRestraintsManagerWorkPanel::DeleteAllRestraints(void)
{
    if( ActiveStructure == NULL ) return;
    ActiveStructure->GetRestraints()->DeleteAllRestraintsWH();
}

//------------------------------------------------------------------------------

void CRestraintsManagerWorkPanel::DeleteInvalidRestraints(void)
{
    if( ActiveStructure == NULL ) return;
    ActiveStructure->GetRestraints()->DeleteInvalidRestraintsWH();
}

//------------------------------------------------------------------------------

void CRestraintsManagerWorkPanel::StructureInfo(void)
{
    if( ActiveStructure == NULL ) return;
    ActiveStructure->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CRestraintsManagerWorkPanel::EnableRestraints(bool set)
{
    if( ActiveStructure == NULL ) return;
    ActiveStructure->GetRestraints()->EnableRestraintsWH(set);
}

//------------------------------------------------------------------------------

void CRestraintsManagerWorkPanel::UpdateRestraintListStatus(void)
{
    WidgetUI.enableRestraintsCB->setEnabled(ActiveStructure != NULL);
    if( ActiveStructure ){
        WidgetUI.enableRestraintsCB->setChecked(ActiveStructure->GetRestraints()->IsEnabled());
    } else {
        WidgetUI.enableRestraintsCB->setChecked(false);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRestraintsManagerWorkPanel::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;

    setEnabled( ! (GetProject()->GetHistory()->IsLocked(EHCL_TOPOLOGY) || GetProject()->GetHistory()->IsLocked(EHCL_GEOMETRY)) );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
