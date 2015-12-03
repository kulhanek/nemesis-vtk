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

#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <XMLDocument.hpp>
#include <XMLPrinter.hpp>
#include <XMLParser.hpp>

#include <Structure.hpp>
#include <Graphics.hpp>
#include <GraphicsProfile.hpp>
#include <GraphicsObject.hpp>
#include <HistoryList.hpp>
#include <StructureList.hpp>
#include <TrajectoryList.hpp>
#include <StaticIndexes.hpp>
#include <QFileInfo>

#include "DockingProjectWindow.hpp"
#include "DockingProject.hpp"
#include "DockingProjectModule.hpp"

#include "DockingProjectLigandImportJob.hpp"
#include "DockingProjectReceptorImportJob.hpp"
#include "DockingProjectResultsImportTool.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DockingProjectCB(void* p_data);

CExtUUID        DockingProjectID(
                    "{DOCKING_PROJECT:48bddbcd-7158-47be-8ff1-09a79989e52e}",
                    "Docking",
                    "Basic UI for molecular docking.");

CPluginObject   DockingProjectObject(&DockingProjectPlugin,
                    DockingProjectID,PROJECT_CAT,
                    ":/images/DockingProject/DockingProject.svg",
                    DockingProjectCB);

// -----------------------------------------------------------------------------

QObject* DockingProjectCB(void* p_data)
{
    return(new CDockingProject(static_cast<CExtComObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDockingProject::CDockingProject(CExtComObject* p_owner)
    : CProject(&DockingProjectObject,p_owner)
{
    // main window is created upon the external request
    MainWindow = NULL;

    // setup object
    Receptor = NULL;
    Ligand = NULL;
    // setup trajectory object
    Results = NULL;
    Trajectories = new CTrajectoryList(this,true);
    Trajectories->SetIndex(TRAJECTORY_LIST_INDEX);
    ResultTraj = NULL;

    // set locks for docking project - STRUCTURE, TOPOLOGY and GEOMETRY subsystems
//    GetHistory()->SetMinLockModeLevels(CLockLevels(EHCL_STRUCTURES|EHCL_TOPOLOGY|EHCL_GEOMETRY));

    // set top index for dynamical objects
    SetTopObjectIndex(LAST_USER_STATIC_INDEX);
}

//---------------------------------------------------------------------------

CDockingProject::~CDockingProject(void)
{
    Trajectories->DeleteAllTrajectories();
    delete Trajectories;
    GetProject()->GetStructures()->DeleteAllStructures();
    if( MainWindow ){
        // delete visual part of project
        delete MainWindow;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMainWindow* CDockingProject::GetMainWindow(void)
{
    // get visual part of project
    return(MainWindow);
}

//------------------------------------------------------------------------------

void CDockingProject::CreateMainWindow(void)
{
    if( MainWindow ) {
        ES_ERROR("main window has been already created");
        return;
    }
    // create visual part of project
    MainWindow = new CDockingProjectWindow(this);
    MainWindow->RestoreDesktop();
    MainWindow->ConnectAllMenu();
    MainWindow->show();
}

//------------------------------------------------------------------------------

void CDockingProject::CreateResults(void)
{
    // metod to add new structure with trajectory for results

    QString res = tr("Results %1");
    QString trj = tr("Poses %1");
    int nmbr_traj = GetTrajectories()->GetNumberOfTrajectories() + 1;

    if (nmbr_traj == 1) {
        res = res.arg("");
        trj = trj.arg("");

    } else {
        res = res.arg(nmbr_traj);
        trj = trj.arg(nmbr_traj);
    }

    // history change start
    CHistoryNode* p_history_str = GetStructures()->BeginChangeWH(EHCL_COMPOSITE,"add " + res);
    if( p_history_str == NULL ) return;

    CHistoryNode* p_history_trj = GetTrajectories()->BeginChangeWH(EHCL_TRAJECTORIES,"add " + trj);
    if( p_history_trj == NULL ){
        // end composite change
      GetStructures()->EndChangeWH();
      return; // is change permitted?
    }

    CStructure* p_results_str = GetStructures()->CreateStructure(res,"",p_history_str);

    // create and assign trajectory for view results
    CTrajectory* p_results_traj = GetTrajectories()->CreateTrajectory(trj,p_history_trj);
    p_results_traj->BeginUpdate();
    // set structure and save it to the structure history
    p_results_traj->SetStructure(p_results_str, p_history_str);
    p_results_traj->EndUpdate();

    GetTrajectories()->SetActiveTrajectory(p_results_traj,p_history_trj);
    // trajectory change end
    GetTrajectories()->EndChangeWH();
    GetStructures()->SetActiveStructure(p_results_str);
    // composite change end
    GetStructures()->EndChangeWH();
}

//------------------------------------------------------------------------------

void CDockingProject::NewData(void)
{
    // create structures
    Receptor = GetStructures()->CreateStructure("Receptor");
    Ligand = GetStructures()->CreateStructure("Ligand");
    Results = GetStructures()->CreateStructure("Results");

    // create and assign trajectory for view results
    ResultTraj = GetTrajectories()->CreateTrajectory("Poses 1");
    ResultTraj->SetStructure(Results);

    // init remaining part
    CProject::NewData();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectoryList* CDockingProject::GetTrajectories(void)
{
    // get trajectory in which are stored results
    return(Trajectories);
}

//------------------------------------------------------------------------------

CStructure* CDockingProject::GetReceptorStructure(void)
{
    // get receptor structure
    return(Receptor);
}

//------------------------------------------------------------------------------

CStructure* CDockingProject::GetLigandStructure(void)
{
    // get ligand structure
    return(Ligand);
}

//------------------------------------------------------------------------------

QFileInfo CDockingProject::GetLigandFileName(void)
{
    // get filename of opened ligand sctructure - path/filename.extension
    return(LigandFile);
}

//------------------------------------------------------------------------------

QFileInfo CDockingProject::GetReceptorFileName(void)
{
    // get filename of opened receptor sctructure - path/filename.extension
    return(ReceptorFile);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProject::SetLigandStructure(CStructure* str_ligand)
{
    Ligand = str_ligand;
}

//------------------------------------------------------------------------------

void CDockingProject::SetReceptorStructure(CStructure* str_receptor)
{
    Receptor = str_receptor;
}

//------------------------------------------------------------------------------

CDockingProjectLigandImportJob* CDockingProject::LoadLigandStructure(const QString& file_ligand)
{   
    std::ifstream   sin;
    sin.open(file_ligand.toLatin1());
    if( !sin ) {
        return (NULL);
    }
    sin.close();

    // Return only a file name
    QFileInfo file(file_ligand);
    // save ligand file name info
    SetLigandFileName(file);

    // create job and setup job
    CDockingProjectLigandImportJob* p_job = new CDockingProjectLigandImportJob(GetProject());
    p_job->SetImportedStructure(Ligand,file_ligand);

    // submit job
    if( p_job->SubmitJob() == false ){
        delete p_job;
        return (NULL);
    }
    return (p_job);
}

//------------------------------------------------------------------------------

CDockingProjectReceptorImportJob* CDockingProject::LoadReceptorStructure(const QString& file_receptor)
{

    std::ifstream   sin;
    sin.open(file_receptor.toLatin1());
    if( !sin ) {
       return (NULL);
    }
    sin.close();

     // Return only a file name
     QFileInfo file(file_receptor);
     // save receptor file name info
     SetReceptorFileName(file);

     // create job and setup job
     CDockingProjectReceptorImportJob* p_job = new CDockingProjectReceptorImportJob(GetProject());
     p_job->SetImportedStructure(Receptor,file_receptor);

    // submit job
    if( p_job->SubmitJob() == false ){
        delete p_job;
        return (NULL);

    }
    return (p_job);
}

//------------------------------------------------------------------------------

bool CDockingProject::LoadResultsStructure(const QString& file_results)
{    
    std::ifstream   sin;
    sin.open(file_results.toLatin1());
    if( !sin ) {
       return (false);
    }
    sin.close();

    CDockingProjectResultsImportTool* p_tool = new CDockingProjectResultsImportTool(GetProject());
    CTrajectoryList* p_trajlist = GetTrajectories();
    bool found = false;
    // look for first empty structure in trajectory and set this trajectory to active
    foreach(QObject* p_qobj,p_trajlist->children()) {
        CTrajectory* p_traj = static_cast<CTrajectory*>(p_qobj);
        if (p_traj->GetStructure()->IsEmpty()) {
            p_trajlist->SetActiveTrajectory(p_traj,NULL);
            found = true;
            break;
        }
    }
    if (!found) {
        CreateResults();
    }

    bool result = p_tool->LunchJob(file_results);
    delete p_tool;
    return (result);
}

//------------------------------------------------------------------------------
void CDockingProject::SetLigandFileName(QFileInfo ligand_name)
{
    // store filename of opened ligand file
    LigandFile = ligand_name;
}

//------------------------------------------------------------------------------

void CDockingProject::SetReceptorFileName(QFileInfo receptor_name)
{
    // store filename of opened receptor file
    ReceptorFile = receptor_name;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




