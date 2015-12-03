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

#include <QMessageBox>

#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <XMLDocument.hpp>
#include <XMLPrinter.hpp>
#include <XMLParser.hpp>

#include <StaticIndexes.hpp>

#include <Graphics.hpp>
#include <GraphicsProfile.hpp>
#include <GraphicsObject.hpp>
#include <HistoryList.hpp>

#include "CheminfoProject.hpp"
#include "CheminfoProjectModule.hpp"
#include "CheminfoProjectWindow.hpp"
#include "CheminfoProjectInfo.hpp"
#include "CheminfoProjectStructure.hpp"
#include "CheminfoProjectStructureList.hpp"
#include "CheminfoProjectStructureListProject.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CheminfoProjectCB(void* p_data);

CExtUUID        CheminfoProjectID(
                    "{CHEMINFO_PROJECT:afb4ea80-56f9-4e1d-8121-89b2f277c82c}",
                    "Cheminfo",
                    "Project for Virtual Screening.");

CPluginObject   CheminfoProjectObject(&CheminfoProjectPlugin,
                    CheminfoProjectID,PROJECT_CAT,
                    ":/images/CheminfoProject/CheminfoProject.svg",
                    CheminfoProjectCB);

// -----------------------------------------------------------------------------

QObject* CheminfoProjectCB(void* p_data)
{
    return(new CCheminfoProject(static_cast<CExtComObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProject::CCheminfoProject(CExtComObject* p_owner)
    : CProject(&CheminfoProjectObject,p_owner)
{
    ProjectInfoList = new CCheminfoProjectInfoList(this,true);
    ProjectInfoList->SetIndex(CHEMINFO_INFO_LIST_INDEX);
    StructureListProject = new CCheminfoProjectStructureListProject(this,true);
    StructureListProject->SetIndex(CHEMINFO_STRUCTURE_LIST_PROJECT_INDEX);

    MainWindow = NULL;

    // set top index for dynamical objects
    SetTopObjectIndex(LAST_USER_STATIC_INDEX);

    Database = new CCheminfoProjectDatabase(this);
    DatabaseStructure = new CCheminfoProjectDatabaseStructure(this);
    Transfer = new CCheminfoProjectTransfer(this);
}

//---------------------------------------------------------------------------

CCheminfoProject::~CCheminfoProject(void)
{
    if( MainWindow ){
        // delete visual part of project
        delete MainWindow;
    }
    if( Database ){
        // delete database of project
        delete Database;
    }
    if( DatabaseStructure ){
        // delete structure database of project
        delete DatabaseStructure;
    }
    if( Transfer ){
        // delete transfer structures to new project of project
        delete Transfer;
    }
    if( ProjectInfoList ){
        // delete project info list of project
        delete ProjectInfoList;
    }
    if( StructureListProject ){
        // delete structure info list of project
        delete StructureListProject;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMainWindow* CCheminfoProject::GetMainWindow(void)
{
    return(MainWindow);
}

//------------------------------------------------------------------------------

void CCheminfoProject::CreateMainWindow(void)
{
    if( MainWindow ) {
        ES_ERROR("main window has been already created");
        return;
    }
    MainWindow = new CCheminfoProjectWindow(this);
    MainWindow->RestoreDesktop();
    MainWindow->ConnectAllMenu();
    MainWindow->show();
}

//------------------------------------------------------------------------------

void CCheminfoProject::NewData(void)
{
    // init remaining part
    CProject::NewData();

    if (Database->LoadDatabaseSettings()) {
        Database->ConnectDatabaseFirebird();
        if(Database->IsLogged()) {
            ProjectInfoList->LoadProjectInfoList();
        }
    }

    DatabaseStructure->LoadDatabaseStructureSettings();

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectDatabase* CCheminfoProject::GetDatabase(void)
{
    return(Database);
}

//------------------------------------------------------------------------------

CCheminfoProjectDatabaseStructure* CCheminfoProject::GetDatabaseStructure(void)
{
    return(DatabaseStructure);
}

//------------------------------------------------------------------------------

CCheminfoProjectTransfer* CCheminfoProject::GetTransfer(void)
{
    return(Transfer);
}

//------------------------------------------------------------------------------
CCheminfoProjectInfoList* CCheminfoProject::GetProjectInfoList(void)
{
    return(ProjectInfoList);
}

//------------------------------------------------------------------------------

CCheminfoProjectStructureListProject* CCheminfoProject::GetStructureListProject(void)
{
    return(StructureListProject);
}

//------------------------------------------------------------------------------

QString CCheminfoProject::GetReceptorFile(void)
{
    return(ReceptorFile);
}
//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProject::SetReceptorFile(QString recfile)
{
    ReceptorFile = recfile;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

