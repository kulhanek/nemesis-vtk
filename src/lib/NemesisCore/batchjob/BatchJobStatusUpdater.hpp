#ifndef JobSchedulerH
#define JobSchedulerH
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

#include <NemesisCoreMainHeader.hpp>

// this is here because Windows has defined a macro for GetJob->GetJobA
// substitution, we need to aply it on all GetJob occurences
#if defined _WIN32 || defined __CYGWIN__
#include <windows.h>
#endif

#include <FileName.hpp>
#include <ExtComObject.hpp>
#include <SimpleList.hpp>
#include <QTimer>

//------------------------------------------------------------------------------

class CJob;
class CProject;

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CJobScheduler : public CExtComObject {
    Q_OBJECT
public:
// constructor and destructor -------------------
    CJobScheduler(CExtComObject* p_parent);
    ~CJobScheduler(void);

// setup ----------------------------------------
    /// load user or optionaly system scheduler setup
    bool LoadConfig(void);

    /// save scheduler setup
    bool SaveUserConfig(void);

// informative methods --------------------------
    /// return number of jobs
    int GetNumberOfJobs(void);

    /// get job
    CJob* GetJob(unsigned int index);

    /// return max number of running jobs
    int  GetMaxNumberOfRunningJobs(void);

// executive methods -----------------------------------------------------------
    /// start job scheduler
    void StartScheduler(void);

    /// stop job scheduler - all running jobs are terminated
    void StopScheduler(void);

    /// add job to a queue
    bool AddJob(CJob* p_job);

    /// set max number of running jobs
    void SetMaxNumberOfRunningJobs(int setup);

    /// send soft terminate status to running project jobs
    void TerminateProjectRunningJobs(CProject* p_project);

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
    QTimer*             Timer;
    int                 MaxRunningJobs;
    int                 ScheduleTime;       // in miliseconds
    CSimpleList<CJob>   Jobs;               // scheduled jobs

    // job object deleted - received from job
    void JobDeleted(CJob* p_job);

private slots:
    // scheduling loop - received from timer
    void MainLoopTick(void);

private:
    // emit signals
    void EmitJobStatusChanged(CJob* p_job);

    friend class CJobSchedulerModel;
    friend class CJob;
};

//------------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE CJobScheduler* JobScheduler;

//------------------------------------------------------------------------------

#endif
