// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012        Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <RecentFile.hpp>
#include <Project.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <ProjectList.hpp>
#include <QMessageBox>
#include <ImportJob.hpp>
#include <PluginDatabase.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRecentFile::CRecentFile(void)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

CRecentFile::CRecentFile(const CFileName& name)
{
    FileName = name;
}

//------------------------------------------------------------------------------

CRecentFile::CRecentFile(const CFileName& name,const CExtUUID& processor)
{
    FileName = name;
    Processor = processor;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CRecentFile::OpenFile(CProject* p_project)
{
    if( Processor.IsValidUUID() == false ){
        CProject* p_project = Projects->OpenProject(FileName);
        if( p_project == NULL ){
            ES_ERROR("unable to load project");
            QMessageBox::critical(NULL, tr("Open Project"),
                                  tr("An error occurred during project opening!"),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
            return(false);
        }
        p_project->ShowProject();
        return(true);
    }

    CStructure* p_str = p_project->GetActiveStructure();
    if( p_str == NULL ){
        p_project->TextNotification(ETNT_ERROR,"no active structure",ETNT_ERROR_DELAY);
        return(false);
    }

    // open the file using the processor
    QObject* p_qobj = static_cast<QObject*>(PluginDatabase.CreateObject(Processor,p_project));
    if( p_qobj == NULL ){
        CSmallString error;
        error << "unable to create object " << Processor.GetFullStringForm();
        ES_ERROR(error);
        return(false);
    }

    // is it import job
    CImportJob* p_job = dynamic_cast<CImportJob*>(p_qobj);
    if( p_job == NULL ){
        delete p_qobj;
        ES_ERROR("object is not ImportJob");
        return(false);
    }

    // setup job
    // at this momemnt this object will be deleted do not refer to any local data
    if( p_job->SetImportedStructure(p_str,QString(FileName)) == false ){
        delete p_job;
        ES_ERROR("unable to setup import job");
        return(false);
    }

    if( p_job->SubmitJob() == false ){
        delete p_job;
        ES_ERROR("unable to submit import job");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

const CFileName& CRecentFile::GetName(void) const
{
    return(FileName);
}

//------------------------------------------------------------------------------

const CExtUUID& CRecentFile::GetProcessor(void) const
{
    return(Processor);
}

//------------------------------------------------------------------------------

bool CRecentFile::Load(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    bool result = true;
    result &= p_ele->GetAttribute("name",FileName);
    // this is optional
    Processor.GetValue(p_ele,"processor");

    return(result);
}

//------------------------------------------------------------------------------

void CRecentFile::Save(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    p_ele->SetAttribute("name",FileName);
    if( Processor.IsValidUUID() ){
        Processor.SetValue(p_ele,"processor");
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



