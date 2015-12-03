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

#include <BatchJobList.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <ErrorSystem.hpp>

#include "BatchJobList.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        BatchJobListID(
                    "{BATCH_JOB_LIST:1d92d419-4efe-4fd3-afa6-577a75be2a1a}",
                    "Batch jobs");

CPluginObject   BatchJobListObject(&NemesisCorePlugin,
                    BatchJobListID,JOB_CAT,
                    ":/images/NemesisCore/batchjob/BatchJobList.svg",
                    NULL);

//------------------------------------------------------------------------------

CBatchJobList* BatchJobs = NULL;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBatchJobList::CBatchJobList(CExtComObject* p_owner)
    : CExtComObject(&BatchJobListObject,p_owner)
{
}

//------------------------------------------------------------------------------

CBatchJobList::~CBatchJobList(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBatchJobList::RegisterBatchJob(CBatchJob* p_job)
{
    if( p_job == NULL ) {
        RUNTIME_ERROR("p_job is NULL");
    }

    // insert into the list and emit signal
    p_job->setParent(this);
    emit OnBatchJobAdded(p_job);
}

//------------------------------------------------------------------------------

void CBatchJobList::LoadJobs(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBatchJobList::EmitBatchJobStatusChanged(CBatchJob* p_job)
{
    emit OnBatchJobStatusChanged(p_job);
}

//------------------------------------------------------------------------------

void CBatchJobList::EmitBatchJobReleased(CBatchJob* p_job)
{
    emit OnBatchJobReleased(p_job);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

