// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Tomas Marek, xmarek11@stud.fit.vutbr.cz,
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

//#include <ContainerModel.hpp>
#include <PluginDatabase.hpp>

#include "CheminfoProjectWindow.hpp"
#include "CheminfoProject.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

using namespace std;

//------------------------------------------------------------------------------

void CCheminfoProjectWindow::ConnectCheminfoMenu(void)
{
    CONNECT_ACTION(CheminfoWP);
    CONNECT_ACTION(CheminfoDatabaseWP);
    CONNECT_ACTION(CheminfoStructuresWP);

    CONNECT_ACTION(SetDatabase);
    CONNECT_ACTION(SetStructureDatabase);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectWindow::CheminfoWP(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{CHEMINFO_PROJECT_WORK_PANEL:22338322-1048-46fd-9424-f3b4be6725a4}");
    OpenToolPanel(mp_uuid,true);
}

//------------------------------------------------------------------------------

void CCheminfoProjectWindow::CheminfoDatabaseWP(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{CHEMINFO_DATABASE_WORK_PANEL:ca096980-262c-423e-8d5f-4bc8667e847f}");
    OpenToolPanel(mp_uuid,true);
}

//------------------------------------------------------------------------------

void CCheminfoProjectWindow::CheminfoStructuresWP(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{CHEMINFO_STRUCTURE_WORK_PANEL:cac186a4-de08-4a6b-b58b-5fc54b0b1c36}");
    OpenToolPanel(mp_uuid,true);
}

//------------------------------------------------------------------------------

void CCheminfoProjectWindow::SetDatabase(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{CHEMINFO_PROJECT_SET_DATABASE:f61eebaf-f15b-4358-8fb7-096d79542fb2}");
    // open dialog
    ExecuteObject(mp_uuid,Project->GetProject());

}

//------------------------------------------------------------------------------

void CCheminfoProjectWindow::SetStructureDatabase(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{CHEMINFO_PROJECT_SET_STRUCTURE_DB:06b310b0-7a82-44c2-ab36-a50d2bf38e18}");
    // open dialog
    ExecuteObject(mp_uuid,Project->GetProject());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



