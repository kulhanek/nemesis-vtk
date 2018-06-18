// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <JobScheduler.hpp>
#include <GlobalSetup.hpp>
#include <XMLDocument.hpp>
#include <XMLParser.hpp>
#include <XMLPrinter.hpp>
#include <FileSystem.hpp>
#include <ErrorSystem.hpp>
#include <SimpleIterator.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Job.hpp>
#include <XMLElement.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        JobSchedulerID(
                    "{JOB_SCHEDULER:c39f642e-7729-4dec-8d12-ce1d665d46ed}",
                    "Job scheduler");

CPluginObject   JobSchedulerObject(&NemesisCorePlugin,
                    JobSchedulerID,JOB_CAT,
                    ":/images/NemesisCore/job/JobList.svg",
                    NULL);

//------------------------------------------------------------------------------

CJobScheduler* JobScheduler = NULL;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CJobScheduler::CJobScheduler(CExtComObject* p_parent)
    : CExtComObject(&JobSchedulerObject,p_parent)
{
    MaxRunningJobs = 2;
    ScheduleTime = 100;

    Timer = new QTimer(this);
    Timer->setInterval(ScheduleTime);

    connect(Timer,SIGNAL(timeout(void)),
            this,SLOT(MainLoopTick(void)));
}

//------------------------------------------------------------------------------

CJobScheduler::~CJobScheduler(void)
{
    delete Timer;
    Timer = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CJobScheduler::LoadConfig(void)
{
    // config name
    CFileName      config_name;

    config_name = GlobalSetup->GetUserSetupPath() / "job_scheduler.xml"; // first user setup
    if( CFileSystem::IsFile(config_name) == false ) {
        // use system config
        config_name = GlobalSetup->GetSystemSetupPath() / "job_scheduler.xml";
    }

    if( CFileSystem::IsFile(config_name) == false ) {
        // no setup is available - use default
        return(true);
    }

    CXMLDocument  scheduler_setup;

    // open config file
    CXMLParser xml_parser;
    xml_parser.SetOutputXMLNode(&scheduler_setup);

    if( xml_parser.Parse(config_name) == false ) {
        CSmallString error;
        error <<  "unable to load configuration file (" << config_name << ")";
        ES_ERROR(error);
        return(false);
    }

    CXMLElement* p_ele;
    p_ele = scheduler_setup.GetFirstChildElement("scheduler");
    if( p_ele ){

        // setup goes here
        p_ele->GetAttribute("mrj",MaxRunningJobs);
        p_ele->GetAttribute("sct",ScheduleTime);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CJobScheduler::SaveUserConfig(void)
{
    // root document
    CXMLDocument  scheduler_setup;
    scheduler_setup.CreateChildDeclaration();
    scheduler_setup.CreateChildComment("NEMESIS job scheduler setup");

    CXMLElement* p_ele;
    p_ele = scheduler_setup.CreateChildElement("scheduler");

    // setup goes here
    p_ele->SetAttribute("mrj",MaxRunningJobs);
    p_ele->SetAttribute("sct",ScheduleTime);

    // config name
    CFileName config_name;
    config_name = GlobalSetup->GetUserSetupPath() / "job_scheduler.xml";

    // open config file
    CXMLPrinter xml_printer;
    xml_printer.SetPrintedXMLNode(&scheduler_setup);

    if( xml_printer.Print(config_name) == false ) {
        CSmallString error;
        error <<  "unable to save configuration file (" << config_name << ")";
        ES_ERROR(error);
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CJobScheduler::GetNumberOfJobs(void)
{
    int njobs = 0;
    njobs = Jobs.NumOfMembers();
    return(njobs);
}

//------------------------------------------------------------------------------

CJob* CJobScheduler::GetJob(unsigned int index)
{
    CJob* p_job = NULL;
    p_job = Jobs.Get(index);
    return(p_job);
}

//------------------------------------------------------------------------------

int  CJobScheduler::GetMaxNumberOfRunningJobs(void)
{
    return(MaxRunningJobs);
}

//------------------------------------------------------------------------------

void CJobScheduler::SetMaxNumberOfRunningJobs(int setup)
{
    MaxRunningJobs = setup;
}

//------------------------------------------------------------------------------

void CJobScheduler::TerminateProjectRunningJobs(CProject* p_project)
{
    CSimpleIterator<CJob>   I(Jobs);
    CJob*                   p_job;

    I.SetToBegin();
    while( (p_job = I.Current()) != NULL ) {
        if( p_job->GetProject() == p_project ){
            if( p_job->GetJobStatus() == EJS_RUNNING ){
                p_job->TerminateJob();
            }
        }
        I++;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CJobScheduler::StartScheduler(void)
{
    Timer->start();
}

//------------------------------------------------------------------------------

void CJobScheduler::StopScheduler(void)
{
    Timer->stop();
}

//------------------------------------------------------------------------------

bool CJobScheduler::AddJob(CJob* p_job)
{
    if( p_job == NULL ) return(false);

    Jobs.InsertToEnd(p_job);
    emit OnJobAdded(p_job);

    p_job->SetJobStatus(EJS_QUEUED);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CJobScheduler::MainLoopTick(void)
{
    CSimpleIterator<CJob>   I(Jobs);
    CJob*                   p_job;

    // how many jobs are running?
    int  running = 0;
    I.SetToBegin();
    while( (p_job = I.Current()) != NULL ) {
        if( p_job->GetJobStatus() == EJS_RUNNING ) running++;
        I++;
    }

    if( running >= MaxRunningJobs ) { // no more jobs can be run
        return;
    }

    // start new jobs
    I.SetToBegin();
    while( ((p_job = I.Current()) != NULL) && (running < MaxRunningJobs) ) {
        if( p_job->GetJobStatus() == EJS_QUEUED ) {
            p_job->StartJob();
            running++;
        }
        I++;
    }
}

//------------------------------------------------------------------------------

void CJobScheduler::JobDeleted(CJob* p_job)
{
    if( p_job == NULL ) return;

    Jobs.Remove(p_job);
    emit OnJobRemoved(p_job);

    return;
}

//------------------------------------------------------------------------------

void CJobScheduler::EmitJobStatusChanged(CJob* p_job)
{
    emit OnJobStatusChanged(p_job);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

