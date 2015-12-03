// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <CategoryUUID.hpp>
#include <WorkPanel.hpp>
#include <WorkPanelList.hpp>
#include <ErrorSystem.hpp>
#include <ContainerModel.hpp>
#include <MainWindow.hpp>

#include "CheminfoProjectModule.hpp"
#include "CheminfoProject.hpp"

#include "utils/PODesignerCheminfoProjectStructureList.hpp"

#include "CheminfoProjectStructure.hpp"
#include "CheminfoProjectStructureList.hpp"
#include "CheminfoProjectStructureListProject.hpp"

#include "CheminfoProjectTransfer.hpp"

#include "CheminfoStructureWorkPanel.moc"
#include "CheminfoStructureWorkPanel.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CheminfoStructureWorkPanelCB(void* p_data);

CExtUUID        CheminfoStructureWorkPanelID(
                    "{CHEMINFO_STRUCTURE_WORK_PANEL:cac186a4-de08-4a6b-b58b-5fc54b0b1c36}",
                    "Cheminfo Project Structures Panel");

CPluginObject   CheminfoStructureWorkPanelObject(&CheminfoProjectPlugin,
                     CheminfoStructureWorkPanelID,WORK_PANEL_CAT,
                     ":../images/CheminfoProject/CheminfoProject.svg",
                     CheminfoStructureWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* CheminfoStructureWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CCheminfoStructureWorkPanel requires active project");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(CheminfoStructureWorkPanelID,p_project);
    if( p_wp == NULL ) {
        p_wp = new CCheminfoStructureWorkPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoStructureWorkPanel::CCheminfoStructureWorkPanel(CProject* p_project)
    : CWorkPanel(&CheminfoStructureWorkPanelObject,p_project,EWPR_TOOL)
{
    // set up ui file
    WidgetUI.setupUi(this);

    //ResultsList = new CStructureList(p_project);
    CheminfoProject = dynamic_cast<CCheminfoProject*>(p_project);

    if (CheminfoProject == NULL) {
        QMessageBox::critical(NULL,"Error","Project is not Cheminfo Project!",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
    // handle with new trajectory
    //----------------
    connect(CheminfoProject->GetStructureListProject(), SIGNAL(OnCheminfoProjectStructureListProjectChanged(void) ),
            this,SLOT(InitTabs(void)));

    InitTabs();
    // load work panel setup
    LoadWorkPanelSetup();

}

// -----------------------------------------------------------------------------

void CCheminfoStructureWorkPanel::UpdateValues(void)
{

}

// -----------------------------------------------------------------------------

void CCheminfoStructureWorkPanel::InitTabs(void)
{
    CheminfoStructureListProject = CheminfoProject->GetStructureListProject();
    CheminfoStructureList = CheminfoStructureListProject->GetActiveStructureList();
    if (CheminfoStructureList != NULL) {
        AddTab();
    }

}

// -----------------------------------------------------------------------------

void CCheminfoStructureWorkPanel::AddTab(void)
{
    CPODesignerCheminfoProjectStructureList* resultsTab = new CPODesignerCheminfoProjectStructureList (WidgetUI.tabsStructures,CheminfoStructureList,this);
    QString dialog_label;
    dialog_label = "Project ";
    dialog_label += CheminfoStructureList->GetProjectInfo()->GetName().GetBuffer();
    dialog_label += " structures";
    // add new tab into tabwidget with new results
    WidgetUI.tabsStructures->addTab(resultsTab,dialog_label);

}

// -----------------------------------------------------------------------------

CCheminfoStructureWorkPanel::~CCheminfoStructureWorkPanel(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoStructureWorkPanel::ClickedOnProjectStructure(CCheminfoProjectStructure* p_chemstr)
{
    CheminfoProject->GetTransfer()->LoadStructureToNewProject(p_chemstr);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
