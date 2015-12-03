// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <XMLDocument.hpp>
#include <XMLPrinter.hpp>
#include <XMLParser.hpp>

#include <Structure.hpp>
#include <Graphics.hpp>
#include <GraphicsProfile.hpp>
#include <GraphicsObject.hpp>
#include <MainWindow.hpp>
#include <HistoryList.hpp>
#include <StructureList.hpp>
#include <TrajectoryList.hpp>
#include <StaticIndexes.hpp>

#include "TrajectoryProject.hpp"
#include "TrajectoryProjectModule.hpp"
#include "TrajectoryProjectWindow.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* TrajectoryProjectCB(void* p_data);

CExtUUID        TrajectoryProjectID(
                    "{TRAJECTORY_PROJECT:c4dd64d9-da20-413d-a2df-6f38697e4d2d}",
                    "Trajectory",
                    "Trajectory project.");

CPluginObject   TrajectoryProjectObject(&TrajectoryProjectPlugin,
                    TrajectoryProjectID,PROJECT_CAT,
                    ":/images/TrajectoryProject/TrajectoryProject.svg",
                    TrajectoryProjectCB);

// -----------------------------------------------------------------------------

QObject* TrajectoryProjectCB(void* p_data)
{
    return(new CTrajectoryProject(static_cast<CExtComObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectoryProject::CTrajectoryProject(CExtComObject* p_owner)
    : CProject(&TrajectoryProjectObject,p_owner)
{
    SetFlag(EPF_ALLOW_IMP_TO_EMPTY_STR,true);

    // main window is created upon the external request
    MainWindow = NULL;

    // setup trajectory object
    Trajectories = new CTrajectoryList(this,true);
    Trajectories->SetIndex(TRAJECTORY_LIST_INDEX);

    // lock TOPOLOGY and GEOMETRY subsystems
    GetHistory()->SetMinLockModeLevels(CLockLevels(EHCL_TOPOLOGY|EHCL_GEOMETRY));

    // set top index for dynamical objects
    SetTopObjectIndex(LAST_USER_STATIC_INDEX);
}

//---------------------------------------------------------------------------

CTrajectoryProject::~CTrajectoryProject(void)
{
    if( MainWindow ){
        // delete visual part of project
        delete MainWindow;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMainWindow* CTrajectoryProject::GetMainWindow(void)
{
    return(MainWindow);
}

//------------------------------------------------------------------------------

void CTrajectoryProject::CreateMainWindow(void)
{
    if( MainWindow ) {
        ES_ERROR("main window has been already created");
        return;
    }
    MainWindow = new CTrajectoryProjectWindow(this);
    MainWindow->RestoreDesktop();
    MainWindow->ConnectAllMenu();
    MainWindow->show();
}

//------------------------------------------------------------------------------

CTrajectoryList* CTrajectoryProject::GetTrajectories(void)
{
    return(Trajectories);
}

//------------------------------------------------------------------------------

void CTrajectoryProject::NewData(void)
{
    // create active structure
    CStructure*     p_str = GetStructures()->CreateStructure();
    CTrajectory*    p_trj = GetTrajectories()->CreateTrajectory();
    p_trj->SetStructure(p_str);

    // init remaining part
    CProject::NewData();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


