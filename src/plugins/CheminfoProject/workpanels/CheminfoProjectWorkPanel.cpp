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


#include <QtGui>
#include <vector>
#include <string>
#include <utility>
#include <QMessageBox>

#include <HistoryList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <PluginDatabase.hpp>
#include <PluginObject.hpp>
#include <WorkPanel.hpp>
#include <WorkPanelList.hpp>
#include <ErrorSystem.hpp>
#include <QMessageBox>
#include <ContainerModel.hpp>




#include <QtGui>
#include <QMessageBox>

#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <WorkPanel.hpp>
#include <WorkPanelList.hpp>
#include <ErrorSystem.hpp>
#include <QMessageBox>
#include <ContainerModel.hpp>

#include "utils/PODesignerCheminfoProjectInfoList.hpp"

#include "CheminfoProject.hpp"
#include "CheminfoProjectModule.hpp"
#include "CheminfoProjectDatabase.hpp"

#include "CheminfoProjectWorkPanel.hpp"
#include "CheminfoProjectWorkPanel.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CheminfoProjectWorkPanelCB(void* p_data);

CExtUUID        CheminfoProjectWorkPanelID(
                    "{CHEMINFO_PROJECT_WORK_PANEL:22338322-1048-46fd-9424-f3b4be6725a4}",
                    "Cheminfo Panel");

CPluginObject   CheminfoProjectWorkPanelObject(&CheminfoProjectPlugin,
                     CheminfoProjectWorkPanelID,WORK_PANEL_CAT,
                     ":../images/CheminfoProject/CheminfoProject.svg",
                     CheminfoProjectWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* CheminfoProjectWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CCheminfoProjectWorkPanel requires active project");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(CheminfoProjectWorkPanelID,p_project);
    if( p_wp == NULL ) {
        p_wp = new CCheminfoProjectWorkPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectWorkPanel::CCheminfoProjectWorkPanel(CProject* p_project)
    : CWorkPanel(&CheminfoProjectWorkPanelObject,p_project,EWPR_TOOL)
{
    // set up ui file
    WidgetUI.setupUi(this);
    CheminfoProject = dynamic_cast<CCheminfoProject*>(p_project);
    List = CheminfoProject->GetProjectInfoList();
    CPODesignerCheminfoProjectInfoList* resultsTab = new CPODesignerCheminfoProjectInfoList (WidgetUI.tabWidgetCH,List,this);

    // add new tab into tabwidget with new results
    WidgetUI.tabWidgetCH->addTab(resultsTab,"Projects");

    // load work panel setup
    LoadWorkPanelSetup();

}

// -----------------------------------------------------------------------------

CCheminfoProjectWorkPanel::~CCheminfoProjectWorkPanel(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
