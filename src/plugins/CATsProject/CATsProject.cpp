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
#include <HistoryList.hpp>
#include <StructureList.hpp>
#include <StaticIndexes.hpp>

#include "CATsProjectWindow.hpp"
#include "CATsProject.hpp"
#include "CATsProjectModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CATsProjectCB(void* p_data);

CExtUUID        CATsProjectID(
                    "{CATS_PROJECT:5e8fd5b9-a39d-47ae-9a95-fe4389e83f0a}",
                    "CATs debugger",
                    "bla bla bla.");

CPluginObject   CATsProjectObject(&CATsProjectPlugin,
                    CATsProjectID,PROJECT_CAT,
                    ":/images/CATsProject/CATsProject.svg",
                    CATsProjectCB);

// -----------------------------------------------------------------------------

QObject* CATsProjectCB(void* p_data)
{
    return(new CCATsProject(static_cast<CExtComObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCATsProject::CCATsProject(CExtComObject* p_owner)
    : CProject(&CATsProjectObject,p_owner)
{
    // main window is created upon the external request
    MainWindow = NULL;

    // set top index for dynamical objects
    SetTopObjectIndex(LAST_USER_STATIC_INDEX);
}

//---------------------------------------------------------------------------

CCATsProject::~CCATsProject(void)
{
    if( MainWindow ){
        // delete visual part of project
        delete MainWindow;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMainWindow* CCATsProject::GetMainWindow(void)
{
    return(MainWindow);
}

//------------------------------------------------------------------------------

void CCATsProject::CreateMainWindow(void)
{
    if( MainWindow ) {
        ES_ERROR("main window has been already created");
        return;
    }
    MainWindow = new CCATsProjectWindow(this);
    MainWindow->RestoreDesktop();
    MainWindow->ConnectAllMenu();
    MainWindow->show();
}

//------------------------------------------------------------------------------

void CCATsProject::NewData(void)
{
    // create active structure
    GetStructures()->CreateStructure();

    // init remaining part
    CProject::NewData();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================





