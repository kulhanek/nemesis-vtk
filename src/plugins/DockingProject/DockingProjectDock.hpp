#ifndef DockingProjectDockH
#define DockingProjectDockH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <Project.hpp>
#include <JobList.hpp>

#include "DockingProject.hpp"

//------------------------------------------------------------------------------

class CDockingProject;

//------------------------------------------------------------------------------

class CDockingProjectDock : public CJob {
    Q_OBJECT
public:
    // constructors and destructors -----------------------------------------------
    /// constructor
    CDockingProjectDock(CPluginObject* p_objectinfo,CProject* p_project);
    /// destructor
    ~CDockingProjectDock(void);

    // section of public data ----------------------------------------------------

    virtual void SetComputePath(CFileName c_path);
    virtual void SetConfigFile(CFileName config_file);
    virtual void SetResultsFile(CFileName config_file);
    virtual void SetSaveLogOutput(bool log);
    virtual void SetSaveConfigFile(bool config);

    // section of protected data ----------------------------------------------------
protected:
    CProject*               DockProject;
    CFileName               ComputePath;
    CFileName               ConfigFile;
    CFileName               ResultsFile;
    bool                    SaveLogOutput;
    bool                    SaveCofigFile;

    // section of private data ----------------------------------------------------
private:
    CLockLevels             BackupLockLevels;

    /// Can we submit the job? - executed from main thread during JobSubmit
    virtual bool JobAboutToBeSubmitted(void);

    /// Initialize job - executed from main thread
    virtual bool InitializeJob(void);

    /// Job main execution point - executed from job thread
    virtual bool ExecuteJob(void);

    /// Finalize job - executed from main thread
    virtual bool FinalizeJob(void);

    // this should be redefined according to docking algorithm
    /// Initialize docker for given set of molecules
    virtual bool Initialization(void);

    /// Make dock
    virtual bool Docking(void);

    /// Finalize docking open the results
    virtual bool Finalization(void);

private slots:
    /// Start docking progress
    void StartDockingProgress(int num_of_steps);
    /// Init docking progress
    void SetInitProgress(int n_step);
    /// Inform about progress
    void SetDockingProgress(int n_step, bool final=false);

private:
signals:
    /// This signal is used due to thread safety - all GUI must be done by master thread
    void OnStartProgressNotification(int length);
    void OnProgressNotification(int progress,const QString& text);


};

//------------------------------------------------------------------------------
#endif
