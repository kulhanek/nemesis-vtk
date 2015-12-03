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

#include <Job.hpp>
#include <JobList.hpp>
#include <JobThread.hpp>
#include <JobScheduler.hpp>

#include "Job.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CJob::CJob(CPluginObject* p_objectinfo,CProject* p_project)
    : CProObject(p_objectinfo,NULL,p_project)
{
    JobStatus = EJS_NEW;
    Terminated = false;
    Thread = new CJobThread(this);

    // this bridge connects two threads (job and main thread)
    connect(this,SIGNAL(FinalizeJobSignal(void)),
            this,SLOT(FinalizeJobSlot(void)));
    connect(this,SIGNAL(JobStatusSignal(void)),
            this,SLOT(JobStatusSlot(void)));
}

//------------------------------------------------------------------------------

CJob::~CJob(void)
{
    if( Thread != NULL ) {
        // wait until thread completely stops
        // this is really important due to possible race condition
        // QThread: Destroyed while thread is still running
        Thread->wait();
        delete Thread;
        Thread = NULL;
    }

    CJobList* p_list = GetJobList();
    setParent(NULL);
    if( p_list != NULL ) p_list->EmitJobRemoved(this);

    if( JobScheduler != NULL ) {
        JobScheduler->JobDeleted(this);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

EJobStatus CJob::GetJobStatus(void)
{
    return(JobStatus);
}

//------------------------------------------------------------------------------

QString CJob::GetJobStatusString(void)
{
    QString text;
    switch(JobStatus) {
    case EJS_NEW:
        text = "N";
        break;
    case EJS_QUEUED:
        text = "Q";
        break;
    case EJS_RUNNING:
        text = "R";
        break;
    case EJS_FINISHED:
        text = "F";
        break;
    case EJS_ABORTED:
        text = "A";
        break;
    case EJS_ENDED:
        text = "E";
        break;
    }

    if( Terminated == true ) {
        text += "T";
    }

    return(text);
}

//------------------------------------------------------------------------------

void CJob::SetJobStatus(EJobStatus status)
{
    JobStatus = status;
    emit JobStatusSignal();
}

//------------------------------------------------------------------------------

CJobList* CJob::GetJobList(void)
{
    return(static_cast<CJobList*>(parent()));
}

//------------------------------------------------------------------------------

QThread* CJob::GetJobThread(void)
{
    return(Thread);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CJob::SubmitJob(void)
{
    if( JobScheduler == NULL ) return(false);

    if( JobAboutToBeSubmitted() == true ) {
        JobScheduler->AddJob(this);
        return(true);
    }

    return(false);
}

//------------------------------------------------------------------------------

void CJob::TerminateJob(void)
{
    Terminated = true;
    emit JobStatusSignal();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CJob::StartJob(void)
{
    // preinitialize job in main thread
    if( InitializeJob() == false ) return;

    // lunch job thread
    Thread->start();
}

//------------------------------------------------------------------------------

void CJob::run(void)
{
    SetJobStatus(EJS_RUNNING);

    bool result = ExecuteJob();

    if( result ) {
        SetJobStatus(EJS_FINISHED);
    } else {
        SetJobStatus(EJS_ABORTED);
    }

    emit FinalizeJobSignal();
}

//------------------------------------------------------------------------------

bool CJob::JobAboutToBeSubmitted(void)
{
    return(true);
}

//------------------------------------------------------------------------------

bool CJob::InitializeJob(void)
{
    return(true);
}

//------------------------------------------------------------------------------

bool CJob::ExecuteJob(void)
{
    return(true);
}

//------------------------------------------------------------------------------

bool CJob::FinalizeJob(void)
{
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CJob::JobStatusSlot(void)
{
    emit OnJobStatusChanged(this);

    CJobList* p_list = GetJobList();
    if( p_list != NULL ) {
        p_list->EmitJobStatusChanged(this);
    }

    if( JobScheduler != NULL ) {
        JobScheduler->EmitJobStatusChanged(this);
    }
}

//------------------------------------------------------------------------------

void CJob::FinalizeJobSlot(void)
{
    FinalizeJob();
    SetJobStatus(EJS_ENDED);
    deleteLater();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
