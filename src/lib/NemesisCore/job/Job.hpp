#ifndef JobH
#define JobH
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
#include <ProObject.hpp>

//------------------------------------------------------------------------------

class CProject;
class CJobList;
class CJobThread;

//------------------------------------------------------------------------------

enum EJobStatus {
    EJS_NEW,
    EJS_QUEUED,
    EJS_RUNNING,
    EJS_FINISHED,
    EJS_ABORTED,
    EJS_ENDED
};

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CJob : public CProObject {
    Q_OBJECT
public:
// constructor and destructor -------------------
    CJob(CPluginObject* p_objectinfo,CProject* p_project);
    ~CJob(void);

// information methods ---------------------------------------------------------
    /// get current job status
    EJobStatus GetJobStatus(void);

    /// get current job status string
    QString GetJobStatusString(void);

    /// return owner list
    CJobList* GetJobList(void);

    /// get job thread
    QThread* GetJobThread(void);

// executive methods -----------------------------------------------------------
    /// submit job to scheduler
    bool SubmitJob(void);

    /// set Terminated flag to true
    void TerminateJob(void);

// signals ---------------------------------------------------------------------
signals:
    /// when status is changed
    void OnJobStatusChanged(CJob* p_job);

// section of protected data ---------------------------------------------------
protected:
    bool        Terminated;

    /// can we submit the job?
    virtual bool JobAboutToBeSubmitted(void);

    /// initialize job - executed from main thread
    virtual bool InitializeJob(void);

    /// job main execution point - executed from job thread
    virtual bool ExecuteJob(void);

    /// finalize job - executed from main thread
    virtual bool FinalizeJob(void);

// section of private data -----------------------------------------------------
private:
    EJobStatus      JobStatus;
    CJobThread*     Thread;

    /// start job
    void StartJob(void);

    /// set job status
    void SetJobStatus(EJobStatus status);

    /// job thread main execution point
    virtual void run(void);

    friend class CJobScheduler;
    friend class CJobThread;

private slots:
    void JobStatusSlot(void);
    void FinalizeJobSlot(void);

signals:
    void JobStatusSignal(void);
    void FinalizeJobSignal(void);
};

//------------------------------------------------------------------------------

#endif
