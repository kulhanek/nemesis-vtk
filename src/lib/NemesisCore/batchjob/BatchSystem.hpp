#ifndef BatchJobH
#define BatchJobH
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

#include <NemesisCoreMainHeader.hpp>
#include <ProObject.hpp>

//------------------------------------------------------------------------------

class CProject;
class CBatchJobList;
class CBatchJobThread;

//------------------------------------------------------------------------------

enum EBatchJobStatus {
    EJS_NEW,
    EJS_QUEUED,
    EJS_RUNNING,
    EJS_FINISHED,
    EJS_ABORTED
};

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CBatchJob : public CProObject {
    Q_OBJECT
public:
// constructor and destructor -------------------
    CBatchJob(CPluginObject* p_objectinfo,CProject* p_project);
    ~CBatchJob(void);

// information methods ---------------------------------------------------------
    /// get current job status
    EBatchJobStatus GetBatchJobStatus(void);

    /// get current job status string
    QString GetBatchJobStatusString(void);

    /// return owner list
    CBatchJobList* GetBatchJobList(void);

    /// get job thread
    QThread* GetBatchJobThread(void);

// executive methods -----------------------------------------------------------
    /// submit job to scheduler
    bool SubmitBatchJob(void);

    /// set Terminated flag to true
    void TerminateBatchJob(void);

// signals ---------------------------------------------------------------------
signals:
    /// when status is changed
    void OnBatchJobStatusChanged(CBatchJob* p_job);

// section of protected data ---------------------------------------------------
protected:
    bool        Terminated;

    /// can we submit the job?
    virtual bool BatchJobAboutToBeSubmitted(void);

    /// initialize job - executed from main thread
    virtual bool InitializeBatchJob(void);

    /// job main execution point - executed from job thread
    virtual bool ExecuteBatchJob(void);

    /// finalize job - executed from main thread
    virtual bool FinalizeBatchJob(void);

// section of private data -----------------------------------------------------
private:
    EBatchJobStatus      BatchJobStatus;
    CBatchJobThread*     Thread;

    /// start job
    void StartBatchJob(void);

    /// set job status
    void SetBatchJobStatus(EBatchJobStatus status);

    /// job thread main execution point
    virtual void run(void);

    friend class CBatchJobScheduler;
    friend class CBatchJobThread;

private slots:
    void BatchJobStatusSlot(void);
    void FinalizeBatchJobSlot(void);

signals:
    void BatchJobStatusSignal(void);
    void FinalizeBatchJobSignal(void);
};

//------------------------------------------------------------------------------

#endif
