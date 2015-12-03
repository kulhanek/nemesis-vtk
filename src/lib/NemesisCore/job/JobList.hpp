#ifndef JobListH
#define JobListH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <Job.hpp>

//------------------------------------------------------------------------------

class CProject;

//------------------------------------------------------------------------------

/*
  Project cannot contain more than one job due to possible problems with history list.
  Therefore the job list can contain only one item.
*/

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CJobList : public CProObject {
    Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    CJobList(CProject* p_owner,bool no_index);
    ~CJobList(void);

// executive methods -----------------------------------------------------------
    /// register new job
    bool RegisterJob(CJob* p_job);

    /// terminate all jobs owned by the project
    void TerminateAllJobs(void);

// signals ---------------------------------------------------------------------
signals:
    /// job was added to the list
    void OnJobAdded(CJob* p_job);

    /// job was added to the list
    void OnJobStatusChanged(CJob* p_job);

    /// job was deleted
    void OnJobRemoved(CJob* p_job);

// section of private data -----------------------------------------------------
private:
    void EmitJobStatusChanged(CJob* p_job);
    void EmitJobRemoved(CJob* p_job);

    friend class CJob;
};

//------------------------------------------------------------------------------

#endif
