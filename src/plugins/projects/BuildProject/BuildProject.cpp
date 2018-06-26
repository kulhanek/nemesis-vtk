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
#include <NemesisOptions.hpp>
#include <PluginDatabase.hpp>
#include <QMessageBox>
#include <ImportJob.hpp>

#include "BuildProjectWindow.hpp"
#include "BuildProject.hpp"
#include "BuildProjectModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* BuildProjectCB(void* p_data);

CExtUUID        BuildProjectID(
                    "{BUILD_PROJECT:b64d16f0-b73f-4747-9a13-212ab9a15d38}",
                    "Build Structure",
                    "Basic project for molecule design.");

CPluginObject   BuildProjectObject(&BuildProjectPlugin,
                    BuildProjectID,PROJECT_CAT,
                    ":/images/BuildProject/BuildProject.svg",
                    BuildProjectCB);

// -----------------------------------------------------------------------------

QObject* BuildProjectCB(void* p_data)
{
    return(new CBuildProject(static_cast<CExtComObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBuildProject::CBuildProject(CExtComObject* p_owner)
    : CProject(&BuildProjectObject,p_owner)
{
    // main window is created upon the external request
    MainWindow = NULL;

    // set top index for dynamical objects
    SetTopObjectIndex(LAST_USER_STATIC_INDEX);
}

//---------------------------------------------------------------------------

CBuildProject::~CBuildProject(void)
{
    if( MainWindow ){
        // delete visual part of project
        delete MainWindow;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMainWindow* CBuildProject::GetMainWindow(void)
{
    return(MainWindow);
}

//------------------------------------------------------------------------------

void CBuildProject::CreateMainWindow(void)
{
    if( MainWindow ) {
        ES_ERROR("main window has been already created");
        return;
    }
    MainWindow = new CBuildProjectWindow(this);
    MainWindow->RestoreDesktop();
    MainWindow->ConnectAllMenu();
    MainWindow->show();
}

//------------------------------------------------------------------------------

void CBuildProject::NewData(void)
{
    // create active structure
    GetStructures()->CreateStructure();

    // init remaining part
    CProject::NewData();
}

//------------------------------------------------------------------------------

bool CBuildProject::ProcessArguments(int& narg)
{
    while( narg < NemesisOptions.GetNumberOfProgArgs() ){

        // is it supported import structure format?
        QString arg = QString(NemesisOptions.GetProgArg(narg));
        bool found = false;
        CSimpleIteratorC<CPluginObject>    I(PluginDatabase.GetObjectList());
        CPluginObject*                     p_obj = NULL;
        while( (p_obj = I.Current()) != NULL ) {
            if ( ((p_obj->GetCategoryUUID() == JOB_CAT) && (p_obj->HasAttribute("EPF_IMPORT_STRUCTURE")))
                 || (p_obj->GetCategoryUUID() == IMPORT_STRUCTURE_CAT) ) {
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

        // do we have empty structure?
        CStructure* p_str = GetActiveStructure();
        if( p_str == NULL ){
            p_str = GetStructures()->CreateStructure();
        }
        if( p_str == NULL ){
            QMessageBox::critical(NULL, tr("Build project"),
                                  QString("Unable to create a new structure for data import!"),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
            return(false);
        }
        // is it empty?
        if( p_str->GetAtoms()->GetNumberOfAtoms() > 0 ){
            p_str = GetStructures()->CreateStructure();
        }
        if( p_str == NULL ){
            QMessageBox::critical(NULL, tr("Build project"),
                                  QString("Unable to create a new empty structure for data import!"),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
            return(false);
        }
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

        // disable inject import
        setProperty("impex.inject",false);
        setProperty("impex.direct",true);

        // import via job
        if( p_obj->GetCategoryUUID() == JOB_CAT ){

            CImportJob* p_job = dynamic_cast<CImportJob*>(p_obj->CreateObject(GetProject()));
            if( p_job == NULL ){
                CSmallString error;
                error << "Unable to create an import job for argument: " << arg << "!";
                ES_ERROR(error);
                QMessageBox::critical(NULL, tr("Build project"),
                                      QString(error),
                                      QMessageBox::Ok,
                                      QMessageBox::Ok);
                return(false);
            }

            QString filename = QString(NemesisOptions.GetProgArg(narg));
            narg++;

            // create job and setup job
            p_job->SetImportedStructure(p_str,filename);

            // submit job
            if( p_job->SubmitJob() == false ){
                delete p_job;
            }

            // wait for job termination
            p_job->WaitForEndWithEventLoop();

        } else if( p_obj->GetCategoryUUID() == IMPORT_STRUCTURE_CAT ) {
            // FIXME - TODO

        }

    }
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================





