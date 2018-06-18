// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2014 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <BatchJob.hpp>
#include <BatchJobList.hpp>
#include <BatchJobThread.hpp>
#include <BatchJobScheduler.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBatchJob::CBatchJob(CPluginObject* p_objectinfo,CProject* p_project)
    : CProObject(p_objectinfo,NULL,p_project)
{
    BatchJobStatus = EJS_NEW;
    Terminated = false;
    Thread = new CBatchJobThread(this);

    // this bridge connects two threads (job and main thread)
    connect(this,SIGNAL(FinalizeBatchJobSignal(void)),
            this,SLOT(FinalizeBatchJobSlot(void)));
    connect(this,SIGNAL(BatchJobStatusSignal(void)),
            this,SLOT(BatchJobStatusSlot(void)));
}

//------------------------------------------------------------------------------

CBatchJob::~CBatchJob(void)
{
    if( Thread != NULL ) {
        // wait until thread completely stops
        // this is really important due to possible race condition
        // QThread: Destroyed while thread is still running
        Thread->wait();
        delete Thread;
        Thread = NULL;
    }

    CBatchJobList* p_list = GetBatchJobList();
    setParent(NULL);
    if( p_list != NULL ) p_list->EmitBatchJobRemoved(this);

    if( BatchJobScheduler != NULL ) {
        BatchJobScheduler->BatchJobDeleted(this);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

EBatchJobStatus CBatchJob::GetBatchJobStatus(void)
{
    return(BatchJobStatus);
}

//------------------------------------------------------------------------------

QString CBatchJob::GetBatchJobStatusString(void)
{
    QString text;
    switch(BatchJobStatus) {
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
    }

    if( Terminated == true ) {
        text += "T";
    }

    return(text);
}

//------------------------------------------------------------------------------

void CBatchJob::SetBatchJobStatus(EBatchJobStatus status)
{
    BatchJobStatus = status;
    emit BatchJobStatusSignal();
}

//------------------------------------------------------------------------------

CBatchJobList* CBatchJob::GetBatchJobList(void)
{
    return(static_cast<CBatchJobList*>(parent()));
}

//------------------------------------------------------------------------------

QThread* CBatchJob::GetBatchJobThread(void)
{
    return(Thread);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CBatchJob::SubmitBatchJob(void)
{
    if( BatchJobScheduler == NULL ) return(false);

    if( BatchJobAboutToBeSubmitted() == true ) {
        BatchJobScheduler->AddBatchJob(this);
        return(true);
    }

    return(false);
}

//------------------------------------------------------------------------------

void CBatchJob::TerminateBatchJob(void)
{
    Terminated = true;
    emit BatchJobStatusSignal();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBatchJob::StartBatchJob(void)
{
    // preinitialize job in main thread
    if( InitializeBatchJob() == false ) return;

    // lunch job thread
    Thread->start();
}

//------------------------------------------------------------------------------

void CBatchJob::run(void)
{
    SetBatchJobStatus(EJS_RUNNING);

    bool result = ExecuteBatchJob();

    if( result ) {
        SetBatchJobStatus(EJS_FINISHED);
    } else {
        SetBatchJobStatus(EJS_ABORTED);
    }

    emit FinalizeBatchJobSignal();
}

//------------------------------------------------------------------------------

bool CBatchJob::BatchJobAboutToBeSubmitted(void)
{
    return(true);
}

//------------------------------------------------------------------------------

bool CBatchJob::InitializeBatchJob(void)
{
    return(true);
}

//------------------------------------------------------------------------------

bool CBatchJob::ExecuteBatchJob(void)
{
    return(true);
}

//------------------------------------------------------------------------------

bool CBatchJob::FinalizeBatchJob(void)
{
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBatchJob::BatchJobStatusSlot(void)
{
    emit OnBatchJobStatusChanged(this);

    CBatchJobList* p_list = GetBatchJobList();
    if( p_list != NULL ) {
        p_list->EmitBatchJobStatusChanged(this);
    }

    if( BatchJobScheduler != NULL ) {
        BatchJobScheduler->EmitBatchJobStatusChanged(this);
    }
}

//------------------------------------------------------------------------------

void CBatchJob::FinalizeBatchJobSlot(void)
{
    FinalizeBatchJob();
    deleteLater();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
