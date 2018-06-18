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

#include <Project.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <ContainerModel.hpp>

#include <Property.hpp>
#include <PropertyList.hpp>
#include <WorkPanelList.hpp>
#include <MainWindow.hpp>
#include <ProjectList.hpp>

#include "PropertyManagerWorkPanel.hpp"
#include "PropertyManagerWorkPanel.moc"
#include "StandardWorkPanelsModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PropertyManagerWorkPanelCB(void* p_data);

CExtUUID        PropertyManagerWorkPanelID(
                    "{PROPERTIES_MANAGER_WORK_PANEL:25843821-dab1-4b5a-a594-d1c0603f0dfd}",
                    "Property Manager Work Panel");

CPluginObject   PropertyManagerWorkPanelObject(&StandardWorkPanelsPlugin,
                    PropertyManagerWorkPanelID,WORK_PANEL_CAT,
                    ":/images/StandardWorkPanels/PropertiesManagement.svg",
                    PropertyManagerWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* PropertyManagerWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("Active project required!");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(PropertyManagerWorkPanelID,p_project);
    if( p_wp == NULL ) {
        p_wp = new CPropertyManagerWorkPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPropertyManagerWorkPanel::CPropertyManagerWorkPanel(CProject* p_proj)
    : CWorkPanel(&PropertyManagerWorkPanelObject, p_proj,EWPR_TOOL)
{
    WidgetUI.setupUi(this);

    // profile objects
    PropertiesModel = GetProject()->GetProperties()->GetContainerModel(this);
    WidgetUI.propertiesTV->setModel(PropertiesModel);

    WidgetUI.propertiesTV->setSelectionMode(QAbstractItemView::ExtendedSelection);
    WidgetUI.propertiesTV->setDragDropMode(QAbstractItemView::DragOnly);
    WidgetUI.propertiesTV->setDragEnabled(true);

    if( PropertiesModel != NULL ){
        connect(PropertiesModel,SIGNAL(modelReset(void)),
                this,SLOT(PropertiesTVSelectionChanged(void)));
        for(int i=0; i < PropertiesModel->columnCount(); i++){
            WidgetUI.propertiesTV->resizeColumnToContents(i);
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

    // connect slots
    if( WidgetUI.propertiesTV->selectionModel() ){
        connect(WidgetUI.propertiesTV->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                this,SLOT(PropertiesTVSelectionChanged(void)));
    }
    //--------------
    connect(WidgetUI.propertiesTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(PropertiesTVDblClicked(const QModelIndex&)));
    //--------------
    connect(WidgetUI.propertyNewPB,SIGNAL(clicked(bool)),
            this,SLOT(PropertyNew(void)));
    //--------------
    connect(WidgetUI.propertyDeletePB,SIGNAL(clicked(bool)),
            this,SLOT(PropertyDelete(void)));
    //--------------
    connect(WidgetUI.propertyInfoPB,SIGNAL(clicked(bool)),
            this,SLOT(PropertyInfo(void)));

    //------------------
    connect(GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // check for initial locks
    ProjectLockChanged(EHCM_LOCK_LEVEL);

    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CPropertyManagerWorkPanel::~CPropertyManagerWorkPanel()
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPropertyManagerWorkPanel::WindowDockStatusChanged(bool docked)
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

void CPropertyManagerWorkPanel::ProjectNameChanged(CProject* p_project)
{
    if( p_project != GetProject() ) return;
    WidgetUI.projectPB->setText(GetProject()->GetName());
}

//------------------------------------------------------------------------------

void CPropertyManagerWorkPanel::OpenProjectDesigner(void)
{
    GetProject()->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPropertyManagerWorkPanel::PropertiesTVSelectionChanged(void)
{   
    QItemSelectionModel* p_selmodel = WidgetUI.propertiesTV->selectionModel();
    bool set1 = false;
    if( p_selmodel ) {
        set1 = p_selmodel->selectedRows().count() > 0;
    }

    WidgetUI.propertyNewPB->setEnabled(true);
    WidgetUI.propertyDeletePB->setEnabled(set1);
    WidgetUI.propertyInfoPB->setEnabled(set1);
}

//------------------------------------------------------------------------------

void CPropertyManagerWorkPanel::PropertiesTVDblClicked(const QModelIndex& index)
{
    CProperty* p_obj = NULL;
    p_obj = dynamic_cast<CProperty*>(PropertiesModel->GetItem(index));
    if( p_obj == NULL ) return;
    p_obj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CPropertyManagerWorkPanel::PropertyNew(void)
{
    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString("{NEW_PROPERTY_WORK_PANEL:4a0b7079-006c-4c7b-a26a-2fec50413de5}");
    CMainWindow* p_mw = static_cast<CMainWindow*>(GetProject()->GetMainWindow());
    p_mw->ShowAsDialog(mp_uuid,GetProject());
}

//------------------------------------------------------------------------------

void CPropertyManagerWorkPanel::PropertyDelete(void)
{
    const QModelIndex& index = WidgetUI.propertiesTV->currentIndex();
    if( index.isValid() == false ) return;

    CProperty* p_obj = NULL;
    p_obj = dynamic_cast<CProperty*>(PropertiesModel->GetItem(index));
    if( p_obj == NULL ) return;

    p_obj->DeletePropertyWH();
}

//------------------------------------------------------------------------------

void CPropertyManagerWorkPanel::PropertyInfo(void)
{
    const QModelIndex& index = WidgetUI.propertiesTV->currentIndex();
    if( index.isValid() == false ) return;

    CProperty* p_obj = NULL;
    p_obj = dynamic_cast<CProperty*>(PropertiesModel->GetItem(index));
    if( p_obj == NULL ) return;

    p_obj->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPropertyManagerWorkPanel::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;

    setEnabled( ! GetProject()->GetHistory()->IsLocked(EHCL_PROPERTY));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
