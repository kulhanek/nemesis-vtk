// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <StaticIndexes.hpp>

#include "AmberModule.hpp"
#include "LeapProject.hpp"
#include "LeapProjectWindow.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* LeapProjectCB(void* p_data);

CExtUUID        LeapProjectID(
                    "{LEAP_PROJECT:ebe3c3d4-f600-4f19-83a2-05f15e22e21a}",
                    "nLEaP",
                    "nLEaP project");

CPluginObject   LeapProjectObject(&AmberPlugin,
                    LeapProjectID,PROJECT_CAT,
                    ":/images/AmberPlugin/nLEaPProject.svg",
                    QStringList() << "incubator",
                    LeapProjectCB);

// -----------------------------------------------------------------------------

QObject* LeapProjectCB(void* p_data)
{
    return(new CLeapProject(static_cast<CExtComObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CLeapProject::CLeapProject(CExtComObject* p_owner)
    : CProject(&LeapProjectObject,p_owner)
{
    MainWindow = NULL;

    // set output stream
    LeapContext.SetOut(&Output);

    // set default PATH
    std::string amberhome = LeapContext.GetPrefix();
    std::string path = "./:";

    path += amberhome + std::string( "/share/leap/1.4/parm:" );
    path += amberhome + std::string( "/share/leap/1.4/lib:" );
    path += amberhome + std::string( "/share/leap/1.4/cmd"  );

    //LeapContext.mortenv().set_s( "path", path.c_str() );

    // set top index for dynamical objects
    SetTopObjectIndex(LAST_USER_STATIC_INDEX);
}

//---------------------------------------------------------------------------

CLeapProject::~CLeapProject(void)
{
    if( MainWindow ){
        // delete visual part of project
        delete MainWindow;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMainWindow* CLeapProject::GetMainWindow(void)
{
    return(MainWindow);
}

//------------------------------------------------------------------------------

void CLeapProject::CreateMainWindow(void)
{
    if( MainWindow ) {
        ES_ERROR("main window has been already created");
        return;
    }
    MainWindow = new CLeapProjectWindow(this);
    MainWindow->RestoreDesktop();
    MainWindow->ConnectAllMenu();
    MainWindow->show();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CLeapProject::LoadData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }
    if( p_ele->GetName() != "data" ) {
        LOGIC_ERROR("p_ele is not 'data'");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);
}

//------------------------------------------------------------------------------

void CLeapProject::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "data" ) {
        LOGIC_ERROR("p_ele is not data");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



