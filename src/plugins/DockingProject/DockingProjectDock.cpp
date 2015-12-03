// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <QMessageBox>
#include <QFileInfo>


#include <Project.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <MainWindow.hpp>
#include <FileSystem.hpp>

#include <Structure.hpp>
#include <StructureList.hpp>
#include <Trajectory.hpp>
#include <TrajectoryList.hpp>

#include "DockingProjectModule.hpp"
#include "DockingProjectDock.hpp"
#include "DockingProjectDock.moc"


////------------------------------------------------------------------------------

//QObject* DockingProjectDockCB(void* p_data);

//CExtUUID        DockingProjectDockID(
//                    "{DOCKING_PROJECT_DOCK:3dc6820e-4551-4744-98dc-769d055b31d7}",
//                    "Docking Project module for Dock");

//CPluginObject   DockingProjectDockObject(&DockingProjectPlugin,
//                    DockingProjectDockID,JOB_CAT,
//                    ":/images/DockingProject/13.docking/Dock.svg",
//                    DockingProjectDockCB);

//// -----------------------------------------------------------------------------

//QObject* DockingProjectDockCB(void* p_data)
//{
//    CProject* p_project = static_cast<CProject*>(p_data);
//    if( p_project == NULL ){
//        ES_ERROR("CDockingProjectDock requires active project");
//        return(NULL);
//    }

//    QObject* p_obj = new CDockingProjectDock(p_project);
//    return(p_obj);
//}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDockingProjectDock::CDockingProjectDock(CPluginObject *p_objectinfo, CProject *p_project)
    : CJob(p_objectinfo,p_project)
{
    DockProject = p_project;
    DockProject->GetJobs()->RegisterJob(this);

}

//------------------------------------------------------------------------------

CDockingProjectDock::~CDockingProjectDock(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectDock::SetComputePath(CFileName c_path)
{
    ComputePath = c_path;
}

//------------------------------------------------------------------------------

void CDockingProjectDock::SetConfigFile(CFileName config_file)
{
     ConfigFile = config_file;
}

//------------------------------------------------------------------------------

void CDockingProjectDock::SetResultsFile(CFileName results_file)
{
    ResultsFile = results_file;
}

//------------------------------------------------------------------------------

void CDockingProjectDock::SetSaveLogOutput(bool log)
{
     SaveLogOutput = log;
}

//------------------------------------------------------------------------------

void CDockingProjectDock::SetSaveConfigFile(bool config)
{
     SaveCofigFile = config;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CDockingProjectDock::JobAboutToBeSubmitted(void)
{
    return(true);
}

//------------------------------------------------------------------------------

bool CDockingProjectDock::InitializeJob(void)
{
    if( DockProject->GetHistory()->IsLocked(EHCL_TOPOLOGY) || DockProject->GetHistory()->IsLocked(EHCL_GEOMETRY)
        || DockProject->GetHistory()->IsLocked(EHCL_STRUCTURES))
    {
        return (false);
    }
    // lock history
    BackupLockLevels = DockProject->GetHistory()->GetLockModeLevels();
    CLockLevels super_lock = ~CLockLevels();
    DockProject->GetHistory()->SetLockModeLevels(super_lock);

    //--------
    connect(this,SIGNAL(OnStartProgressNotification(int)),
            DockProject,SLOT(StartProgressNotification(int)));

    connect(this,SIGNAL(OnProgressNotification(int,const QString&)),
            DockProject,SLOT(ProgressNotification(int,const QString&)));


    return(true);
}

//------------------------------------------------------------------------------

bool CDockingProjectDock::ExecuteJob(void)
{
    bool state;
    if( Initialization() == false ){
        return(false);
    }

    // dock

    if (Terminated == false) {
        state = Docking();
    }

    return(state);
}

//------------------------------------------------------------------------------

bool CDockingProjectDock::FinalizeJob(void)
{
    DockProject->EndProgressNotification();
    // unlock history list
    DockProject->GetHistory()->SetLockModeLevels(BackupLockLevels);
    // load results of dock & finish notification
    Finalization();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CDockingProjectDock::Initialization(void)
{
    return(true);
}

//------------------------------------------------------------------------------

bool CDockingProjectDock::Docking(void)
{
    return(true);
}

//------------------------------------------------------------------------------

bool CDockingProjectDock::Finalization(void)
{
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectDock::StartDockingProgress(int num_of_steps)
{
    emit OnStartProgressNotification(num_of_steps);
}

//------------------------------------------------------------------------------

void CDockingProjectDock::SetInitProgress(int n_step)
{
    emit OnProgressNotification(n_step,"Total Docking progress %p% - Reading data (1/2) ...");
}


//------------------------------------------------------------------------------

void CDockingProjectDock::SetDockingProgress(int n_step, bool final)
{
    if (final) {
        emit OnProgressNotification(n_step,"Total Docking progress %p% - Finalization ...");
    } else {
        emit OnProgressNotification(n_step,"Total Docking progress %p% - Docking (2/2) ...");
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
