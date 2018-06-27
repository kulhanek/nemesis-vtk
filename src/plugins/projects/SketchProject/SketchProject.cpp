// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QWebChannel>

#include "SketchProjectWindow.hpp"
#include "SketchProject.hpp"
#include "SketchProjectModule.hpp"
#include "SketchProjectJSObject.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* SketchProjectCB(void* p_data);

CExtUUID        SketchProjectID(
                    "{SKETCH_PROJECT:6b047926-9a91-4e5f-884a-500db358ddb2}",
                    "Sketch Structure",
                    "Sketch project for molecule design in 2D.");

CPluginObject   SketchProjectObject(&SketchProjectPlugin,
                    SketchProjectID,PROJECT_CAT,
                    ":/images/SketchProject/SketchProject.svg",
                    SketchProjectCB);

// -----------------------------------------------------------------------------

QObject* SketchProjectCB(void* p_data)
{
    return(new CSketchProject(static_cast<CExtComObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSketchProject::CSketchProject(CExtComObject* p_owner)
    : CProject(&SketchProjectObject,p_owner)
{
    // disable subcontainer enumeration
    ConFlags &= ~EECOF_SUB_CONTAINERS;

    // main window is created upon the external request
    MainWindow = NULL;

    // set top index for dynamical objects
    SetTopObjectIndex(LAST_USER_STATIC_INDEX);
}

//---------------------------------------------------------------------------

CSketchProject::~CSketchProject(void)
{
    if( MainWindow ){
        // delete visual part of project
        MainWindow->Project = NULL;
        MainWindow->deleteLater();  // need to be as it crash otherwise
        MainWindow = NULL;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMainWindow* CSketchProject::GetMainWindow(void)
{
    return(MainWindow);
}

//------------------------------------------------------------------------------

void CSketchProject::CreateMainWindow(void)
{
    if( MainWindow ) {
        ES_ERROR("main window has been already created");
        return;
    }
    MainWindow = new CSketchProjectWindow(this);
    MainWindow->RestoreDesktop();
    MainWindow->ConnectAllMenu();
    MainWindow->show();
}

//------------------------------------------------------------------------------

void CSketchProject::NewData(void)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CSketchProject::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core project data
    CProObject::LoadData(p_ele);

    // load structure
    p_ele->GetAttribute("smiles",LoadedSmiles);
    // data load is delayed because MainWindow is not available yet
    // see const QString CSketchProjectJSObject::getInitialStructure(void)
    // see ui.js init() function
}

//------------------------------------------------------------------------------

void CSketchProject::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core project data
    CProObject::SaveData(p_ele);

    // save structure
    QString smiles;
    if( MainWindow ){
        smiles = MainWindow->SaveData();
    }
    p_ele->SetAttribute("smiles",smiles);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================





