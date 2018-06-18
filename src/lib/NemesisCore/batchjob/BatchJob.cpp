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

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBatchJob::CBatchJob(CPluginObject* p_objectinfo)
    : CProObject(p_objectinfo,NULL,NULL)
{
    BatchJobStatus = EBJS_NEW;
    Terminated = false;

    // this bridge connects two threads (job and main thread)
    connect(this,SIGNAL(FinalizeBatchJobSignal(void)),
            this,SLOT(FinalizeBatchJobSlot(void)));
    connect(this,SIGNAL(BatchJobStatusSignal(void)),
            this,SLOT(BatchJobStatusSlot(void)));
}

//------------------------------------------------------------------------------

CBatchJob::~CBatchJob(void)
{
    CBatchJobList* p_list = GetBatchJobList();
    setParent(NULL);
    if( p_list != NULL ) p_list->EmitBatchJobReleased(this);
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
        case EBJS_NEW:
            text = "N";
            break;
        case EBJS_QUEUED:
            text = "Q";
            break;
        case EBJS_RUNNING:
            text = "R";
            break;
        case EBJS_FINISHED:
            text = "F";
            break;
        case EBJS_ABORTED:
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

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CBatchJob::SubmitBatchJob(void)
{
    return(false);
}

//------------------------------------------------------------------------------

bool CBatchJob::ReleaseBatchJob(void)
{
    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBatchJob::StartBatchJob(void)
{
    // preinitialize job in main thread
    if( InitializeBatchJob() == false ) return;
}

//------------------------------------------------------------------------------

void CBatchJob::run(void)
{
    SetBatchJobStatus(EBJS_RUNNING);

    bool result = ExecuteBatchJob();

    if( result ) {
        SetBatchJobStatus(EBJS_FINISHED);
    } else {
        SetBatchJobStatus(EBJS_ABORTED);
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
