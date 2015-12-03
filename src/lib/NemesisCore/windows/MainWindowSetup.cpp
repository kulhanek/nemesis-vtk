// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <DesktopSystem.hpp>
#include <ExtUUID.hpp>
#include <ErrorSystem.hpp>
#include <PluginDatabase.hpp>
#include <WorkPanel.hpp>
#include <ConfigMainWindow.hpp>
#include <MainWindow.hpp>
#include <Project.hpp>
#include <ProjectDesktop.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ConnectSetupMenu(void)
{
    BIND_ACTION(actionToolbarsConfig);
    BIND_ACTION(actionSaveLayout);
    BIND_ACTION(actionUnitSetup);
    BIND_ACTION(actionGraphicsSetup);
    BIND_ACTION(actionColorSetup);
    BIND_ACTION(actionJobSchedulerPreferences);
    BIND_ACTION(actionGlobalSetup);

    //====================================
    connect(actionToolbarsConfig, SIGNAL(triggered(bool)),
            this, SLOT(ConfigureToolbars(void)));
    //-----------------
    connect(actionSaveLayout, SIGNAL(triggered(bool)),
            this, SLOT(SaveDefaultLayout(void)));
    //-----------------
    connect(actionUnitSetup, SIGNAL(triggered(bool)),
            this, SLOT(ConfigureUnitSetup(void)));
    //-----------------
    connect(actionGraphicsSetup, SIGNAL(triggered(bool)),
            this, SLOT(ConfigureGraphicsSetup(void)));
    //-----------------
    connect(actionColorSetup, SIGNAL(triggered(bool)),
            this, SLOT(ConfigureColorSetup(void)));
    //-----------------
    connect(actionJobSchedulerPreferences, SIGNAL(triggered(bool)),
            this, SLOT(ConfigureJobScheduler(void)));
    //-----------------
    connect(actionGlobalSetup, SIGNAL(triggered(bool)),
            this, SLOT(ConfigureGlobalSetup(void)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::SaveDefaultLayout(void)
{
    SaveDesktop();

    // to default file
    Project->GetDesktop()->SaveDefaultDesktop();
}

//------------------------------------------------------------------------------

void CMainWindow::ConfigureUnitSetup(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{UNIT_SETUP_WORK_PANEL:98627045-b118-49e1-ab08-e779c19e2423}");

    CWorkPanel* p_wp = static_cast<CWorkPanel*>(PluginDatabase.CreateObject(mp_uuid));
    if (p_wp == NULL) {
        ES_ERROR("unable to create setup work panel");
        return;
    }

    p_wp->Show();
}

//------------------------------------------------------------------------------

void CMainWindow::ConfigureGraphicsSetup(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{GRAPHICS_SETUP_WORK_PANEL:bff6fd65-eeea-4468-ac92-14b548906511}");

    CWorkPanel* p_wp = static_cast<CWorkPanel*>(PluginDatabase.CreateObject(mp_uuid));
    if (p_wp == NULL) {
        ES_ERROR("unable to create setup work panel");
        return;
    }

    p_wp->Show();
}

//------------------------------------------------------------------------------

void CMainWindow::ConfigureColorSetup(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{COLOR_SETUP_WORK_PANEL:17c59331-de64-40a1-8854-2b6298cb5a9e}");

    CWorkPanel* p_wp = static_cast<CWorkPanel*>(PluginDatabase.CreateObject(mp_uuid));
    if (p_wp == NULL) {
        ES_ERROR("unable to create setup work panel");
        return;
    }

    p_wp->Show();
}

//------------------------------------------------------------------------------

void CMainWindow::ConfigureJobScheduler(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{JOB_SCHEDULER_SETUP:ae11c09c-1b53-4ac7-826e-2a19a3471681}");

    CWorkPanel* p_wp = static_cast<CWorkPanel*>(PluginDatabase.CreateObject(mp_uuid));
    if (p_wp == NULL) {
        ES_ERROR("unable to create setup work panel");
        return;
    }

    p_wp->Show();
}

//------------------------------------------------------------------------------

void CMainWindow::ConfigureGlobalSetup(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{GLOBAL_SETUP_WORK_PANEL:6b9610e6-1c81-4b1c-9c2b-60f6af9a45be}");
    ShowAsDialog(mp_uuid);
}

//------------------------------------------------------------------------------

void CMainWindow::ConfigureToolbars(void)
{
    CConfigMainWindow* p_dialog = new CConfigMainWindow(this);
    p_dialog->ShowAsDialog();
    delete p_dialog;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



