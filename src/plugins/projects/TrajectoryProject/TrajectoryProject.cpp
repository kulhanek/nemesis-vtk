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
#include <NemesisOptions.hpp>
#include <PluginDatabase.hpp>
#include <QMessageBox>
#include <ImportTrajectory.hpp>

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
        MainWindow->Project = NULL;
        MainWindow->deleteLater();
        MainWindow = NULL;
    }
}

//------------------------------------------------------------------------------

bool CTrajectoryProject::ProcessArguments(int& narg)
{
    while( narg < NemesisOptions.GetNumberOfProgArgs() ){

        // is it supported import structure format?
        QString arg = QString(NemesisOptions.GetProgArg(narg));
        bool found = false;
        CSimpleIteratorC<CPluginObject>    I(PluginDatabase.GetObjectList());
        CPluginObject*                     p_obj = NULL;
        while( (p_obj = I.Current()) != NULL ) {
            if ( ((p_obj->GetCategoryUUID() == JOB_CAT) && (p_obj->HasAttribute("EPF_IMPORT_TRAJECTORY")))
                 || (p_obj->GetCategoryUUID() == IMPORT_TRAJECTORY_CAT) ) {
                QString format = p_obj->GetAttributeValue("FORMAT");
                if( ! format.isEmpty() ){
                    format = "-" + format;
                    if( format == arg ){
                        found = true;
                        break;
                    }
                }
            }
            I++;
        }

        if( found == false ){
            return(false);
        }

        // enable direct import
        setProperty("impex.direct",true);

        narg++;
        if( narg >= NemesisOptions.GetNumberOfProgArgs() ){
            CSmallString error;
            error << "Unable to process argument: " << arg << " - file name not provided!";
            ES_ERROR(error);
            QMessageBox::critical(NULL, tr("Build project"),
                                  QString(error),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
            return(false);
        }

        QString filename = QString(NemesisOptions.GetProgArg(narg));
        narg++;

        // import via job
        if( p_obj->GetCategoryUUID() == JOB_CAT ){
            // FIXME - TODO


        } else if( p_obj->GetCategoryUUID() == IMPORT_TRAJECTORY_CAT ) {

            CImportTrajectory* p_job = dynamic_cast<CImportTrajectory*>(p_obj->CreateObject(GetProject()));
            if( p_job == NULL ){
                CSmallString error;
                error << "Unable to create an import job for argument: " << arg << "!";
                ES_ERROR(error);
                QMessageBox::critical(NULL, tr("Trajectory project"),
                                      QString(error),
                                      QMessageBox::Ok,
                                      QMessageBox::Ok);
                return(false);
            }



            // create job and setup job
            p_job->LaunchJob(filename);

            // delete job
            delete p_job;
        }

    }
    return(true);
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


