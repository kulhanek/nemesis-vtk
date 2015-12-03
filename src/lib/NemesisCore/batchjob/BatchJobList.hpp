#ifndef BatchJobListH
#define BatchJobListH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2014 Petr Kulhanek, kulhanek@chemi.muni.cz
//
//     This prog // JOBLIST_Hram is free software; you can redistribute it and/or modify
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

#include <NemesisCoreMainHeader.hpp>
#include <BatchJob.hpp>

//------------------------------------------------------------------------------

/// list of batch jobs

class NEMESIS_CORE_PACKAGE CBatchJobList : public CExtComObject {
    Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    CBatchJobList(CExtComObject* p_parent);
    ~CBatchJobList(void);

// executive methods -----------------------------------------------------------
    /// register new job
    void RegisterBatchJob(CBatchJob* p_job);

    /// load jobs at startup
    void LoadJobs(void);

// signals ---------------------------------------------------------------------
signals:
    /// job was added to the list
    void OnBatchJobAdded(CBatchJob* p_job);

    /// job was added to the list
    void OnBatchJobStatusChanged(CBatchJob* p_job);

    /// job was deleted
    void OnBatchJobReleased(CBatchJob* p_job);

// section of private data -----------------------------------------------------
private:
    void EmitBatchJobStatusChanged(CBatchJob* p_job);
    void EmitBatchJobReleased(CBatchJob* p_job);

    friend class CBatchJob;
};

//------------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE CBatchJobList*  BatchJobs;

//------------------------------------------------------------------------------

#endif
