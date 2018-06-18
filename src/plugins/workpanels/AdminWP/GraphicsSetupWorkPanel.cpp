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

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <GraphicsSetupProfile.hpp>
#include <ContainerModel.hpp>
#include <XMLElement.hpp>
#include <ExtComObjectDesigner.hpp>

#include "GraphicsSetupWorkPanel.hpp"
#include "GlobalWorkPanelsModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GraphicsSetupWorkPanelCB(void* p_data);

CExtUUID        GraphicsSetupWorkPanelID(
                    "{GRAPHICS_SETUP_WORK_PANEL:bff6fd65-eeea-4468-ac92-14b548906511}",
                    "Graphics setup");

CPluginObject   GraphicsSetupWorkPanelObject(&GlobalWorkPanelsPlugin,
                    GraphicsSetupWorkPanelID,WORK_PANEL_CAT,
                    ":images/GlobalWorkPanels/GraphicsSetup.svg",
                    GraphicsSetupWorkPanelCB);

// -----------------------------------------------------------------------------

CGraphicsSetupWorkPanel* p_s3_window = NULL;

// -----------------------------------------------------------------------------

QObject* GraphicsSetupWorkPanelCB(void* p_data)
{
    if( p_s3_window != NULL ) return(p_s3_window);
    p_s3_window = new CGraphicsSetupWorkPanel;
    return(p_s3_window);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsSetupWorkPanel::CGraphicsSetupWorkPanel(void)
    : CWorkPanel(&GraphicsSetupWorkPanelObject,NULL,EWPR_WINDOW)
{
    WidgetUI.setupUi(this);

    // get root enumerator
    Model = GraphicsSetupProfile->GetContainerModel(this);
    Model->sort(0);
    WidgetUI.listView->setModel(Model);

    connect(WidgetUI.listView,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(ListViewDoubleClicked(const QModelIndex&)));

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CGraphicsSetupWorkPanel::~CGraphicsSetupWorkPanel()
{
    SaveWorkPanelSetup();
    p_s3_window = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsSetupWorkPanel::ListViewDoubleClicked(const QModelIndex& index)
{
    QObject* p_obj = Model->GetItem(index);
    if( p_obj == NULL ) return;

    // is it project object?
    CProObject* p_proobj = dynamic_cast<CProObject*>(p_obj);
    if( p_proobj != NULL ) {
        CExtComObjectDesigner* p_designer = p_proobj->OpenObjectDesigner();
        if( p_designer != NULL ){
            p_designer->show();
            p_designer->raise();
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
