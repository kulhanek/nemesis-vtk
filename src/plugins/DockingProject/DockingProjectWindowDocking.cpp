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

#include <ContainerModel.hpp>

#include <StructureList.hpp>
#include <TrajectoryList.hpp>

#include "DockingProjectWindow.hpp"
#include "DockingProject.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

using namespace std;

//------------------------------------------------------------------------------

void CDockingProjectWindow::ConnectDockingMenu(void)
{
    CONNECT_ACTION(DockWP);

    CONNECT_ACTION(DockingWP);

    CONNECT_ACTION(AddResults);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectWindow::DockWP(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{DOCKING_DOCK_PANEL:71573814-d613-4664-a6cd-beb8b4ee9229}");
    OpenToolPanel(mp_uuid,true);
}

//------------------------------------------------------------------------------

void CDockingProjectWindow::DockingWP(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{DOCKING_WORK_PANEL:10ee3f51-9e18-45c0-827c-5d65f161c535}");
    OpenToolPanel(mp_uuid,true);
}

//------------------------------------------------------------------------------

void CDockingProjectWindow::AddResults(void)
{
    // metod to add new structure with trajectory for results

    CDockingProject* p_docproj = dynamic_cast<CDockingProject*>(GetProject());
    p_docproj->CreateResults();
}

//------------------------------------------------------------------------------

void  CDockingProjectWindow::ChangeActiveTrajectory(void)
{
    // slot handle with change active structure - this will be change active trajectory too

    // if havent structure, cant change trajectory
    if (!GetProject()->GetStructures()->GetActiveStructure())
    {
        return;
    }

    // for active structure i change her trajectory to active state
    CTrajectory* p_results_traj = GetProject()->GetStructures()->GetActiveStructure()->GetTrajectory();
    if (p_results_traj == NULL)
        return;
    // activation
    GetProject()->GetTrajectories()->SetActiveTrajectory(p_results_traj);

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



