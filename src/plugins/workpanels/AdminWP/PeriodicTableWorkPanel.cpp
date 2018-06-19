// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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

#include "AdminWPModule.hpp"
#include "PeriodicTableWorkPanel.hpp"
#include <PeriodicTableWidget.hpp>
#include <QVBoxLayout>

#include <ErrorSystem.hpp>
#include <CategoryUUID.hpp>
#include <DesktopSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PeriodicTableWorkPanelCB(void* p_data);

CExtUUID        PeriodicTableWorkPanelID(
                    "{PERIODIC_TABLE_WORK_PANEL:843f334f-db10-460b-8ea2-bca948d9afd9}",
                    "Periodic Table",
                    "Panel with the periodic table of elements.");

CPluginObject   PeriodicTableWorkPanelObject(&AdminWPPlugin,
                    PeriodicTableWorkPanelID,WORK_PANEL_CAT,
                    ":/images/AdminWP/PeriodicTable.svg",
                    PeriodicTableWorkPanelCB);

// -----------------------------------------------------------------------------

CPeriodicTableWorkPanel* p_periodictab_wp = NULL;

// -----------------------------------------------------------------------------

QObject* PeriodicTableWorkPanelCB(void* p_data)
{
    if (p_periodictab_wp != NULL) return(p_periodictab_wp);
    p_periodictab_wp = new CPeriodicTableWorkPanel;
    return(p_periodictab_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPeriodicTableWorkPanel::CPeriodicTableWorkPanel()
    : CWorkPanel(&PeriodicTableWorkPanelObject,NULL,EWPR_WINDOW)
{
    PeriodicTableWidget = new CPeriodicTableWidget(this);

    setWindowIcon(QIcon(":images/AdminWP/periodic_table.png"));

    QVBoxLayout* p_layout = new QVBoxLayout();
    p_layout->addWidget(PeriodicTableWidget);
    p_layout->setContentsMargins(0,0,0,0);

    setLayout(p_layout);

    // load work panel setup
    LoadWorkPanelSetup();
}

// -----------------------------------------------------------------------------

CPeriodicTableWorkPanel::~CPeriodicTableWorkPanel(void)
{
    SaveWorkPanelSetup();
    p_periodictab_wp = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


