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

#include <ErrorSystem.hpp>
#include <CategoryUUID.hpp>
#include <GlobalSetup.hpp>
#include <FileSystem.hpp>

#include <ProjectList.hpp>

#include <JobList.hpp>
#include <StaticIndexes.hpp>

#include <Structure.hpp>
#include <StructureList.hpp>
#include <Trajectory.hpp>
#include <TrajectoryList.hpp>

#include "transfer/CheminfoStructureImportJob.hpp"
#include "transfer/CheminfoResultsImportJob.hpp"

#include "CheminfoProjectModule.hpp"
#include "CheminfoProjectStructure.hpp"
#include "CheminfoProjectTransfer.hpp"
#include "CheminfoProjectTransfer.moc"


//------------------------------------------------------------------------------

QObject* CheminfoProjectTransferCB(void* p_data);

CExtUUID        CheminfoProjectTransferID(
                    "{CHEMINFO_TRANSFER:cf0ddd72-9891-46b1-bba9-78479f865633}",
                    "Cheminfo Transfer to the New project");

CPluginObject   CheminfoProjectTransferObject(&CheminfoProjectPlugin,
                    CheminfoProjectTransferID,CHEMINFO_CAT,
                    ":/images/CheminfoProject/12.cheminfo/Database.svg",
                    CheminfoProjectTransferCB);

// -----------------------------------------------------------------------------

QObject* CheminfoProjectTransferCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CCheminfoProjectTransfer requires active project");
        return(NULL);
    }

    QObject* p_obj = new CCheminfoProjectTransfer(p_project);
    return(p_obj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectTransfer::CCheminfoProjectTransfer(CProject *p_project)
    : CProObject(&CheminfoProjectTransferObject,p_project,p_project)
{
    CheminfoProject = dynamic_cast<CCheminfoProject*>(p_project);
}

//------------------------------------------------------------------------------

CCheminfoProjectTransfer::~CCheminfoProjectTransfer(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectTransfer::LoadStructureToNewProject(CCheminfoProjectStructure* p_str)
{
    CExtUUID mp_uuid(NULL);
    //mp_uuid.LoadFromString("{BUILD_PROJECT:b64d16f0-b73f-4747-9a13-212ab9a15d38}");
    mp_uuid.LoadFromString("{DOCKING_PROJECT:48bddbcd-7158-47be-8ff1-09a79989e52e}");
    NewProject = Projects->NewProject(mp_uuid);
    if( NewProject == NULL ){
        ES_ERROR("unable to create new docking project");
        QMessageBox::critical(NULL, tr("New Docking Project"),
                              tr("An error occurred during project opening!\nIs connected Docking Project?"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        return;
    }
    // get entry to database of structures
    CCheminfoProjectDatabaseStructure* p_strbase = CheminfoProject->GetDatabaseStructure();

    CFileName molname(p_strbase->GetPath());
    CSmallString str_prefix = p_strbase->GetPrefix();
    CSmallString str_type = p_strbase->GetLigandType();
    CSmallString res_type = p_strbase->GetResultsType();
    CSmallString molid = p_str->GetID();

    // for hierarchy structure database we get subpath from molecule id
    if(p_strbase->IsHierarchy()) {
        molname = molname / CFileName(molid.GetSubString(0,2)) / CFileName(molid.GetSubString(2,2))
                  / CFileName(molid.GetSubString(4,2));
    }

    unsigned int molname_l = molname.GetLength();

    // path must be with '/' on the end
    if (molname.GetSubStringFromTo(molname_l-1,molname_l) != "/") {
        molname += "/";
    }

    CSmallString ligand_name = molname + str_prefix + molid + "." + str_type;
    CSmallString results_name  = molname + str_prefix + molid + "." + res_type;

    LigandFile = ligand_name.GetBuffer();
    ResultsFile = results_name.GetBuffer();

    StructureList = NewProject->GetStructures();
    StructureList->DeleteAllStructures();
    if(NewProject->GetTrajectories()) {
        NewProject->GetTrajectories()->DeleteAllTrajectories();
    }

    if (ExistsThisFile(LigandFile) ) {
        CStructure* p_str_lig = StructureList->CreateStructure("Ligand");
        CCheminfoStructureImportJob* p_job_lig = new CCheminfoStructureImportJob(CheminfoProject);
        if ((p_job_lig) == NULL ) {
            ES_ERROR("unable to open new docking project ligand file");
            QMessageBox::critical(NULL, tr("New Docking Project open ligand molecule"),
                                  tr("An error occurred during ligand molecules open!"),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
        } else {
            // create job and setup job
            p_job_lig->SetImportedStructure(p_str_lig,LigandFile);
            // submit job
            if( p_job_lig->SubmitJob() == false ){
                delete p_job_lig;
            }
        }
    }

    if (ExistsThisFile(CheminfoProject->GetReceptorFile() ) )
    {
        CStructure* p_str_rec = StructureList->CreateStructure("Receptor");

        CCheminfoStructureImportJob* p_job_rec = new CCheminfoStructureImportJob(CheminfoProject);
        if ((p_job_rec) == NULL ) {
            ES_ERROR("unable to open new docking project ligand file");
            QMessageBox::critical(NULL, tr("New Docking Project open ligand molecule"),
                                  tr("An error occurred during ligand molecules open!"),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
        } else {
            // create job and setup job
            p_job_rec->SetImportedStructure(p_str_rec,CheminfoProject->GetReceptorFile());
            // submit job
            if( p_job_rec->SubmitJob() == false ){
                delete p_job_rec;
            }
        }

    }

    if (ExistsThisFile(ResultsFile) ) {
        CStructure* p_str_res = CreateResults(NewProject);
        CCheminfoResultsImportJob* p_job_res = new CCheminfoResultsImportJob(CheminfoProject);
        if ((p_job_res) == NULL ) {
            ES_ERROR("unable to open new docking project ligand file");
            QMessageBox::critical(NULL, tr("New Docking Project open ligand molecule"),
                                  tr("An error occurred during ligand molecules open!"),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
        } else {
            // create job and setup job
            p_job_res->SetImportedStructure(p_str_res,ResultsFile);
            // submit job
            if( p_job_res->SubmitJob() == false ){
                delete p_job_res;
            }
        }
    }

    if (CheminfoProject->GetJobs()->HasChildren()) {
        //------------
        connect(CheminfoProject->GetJobs(), SIGNAL(OnJobRemoved(CJob*)),
                this,SLOT(FinishLoadToNewProject(void)));
    } else {
        ES_ERROR("unable to open any of files belongs to choosen structe");
        QMessageBox::critical(NULL, tr("New Docking Project open files from Structure database"),
                              tr("Structure database dont contain any files belongs to choosen record!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
    }

}

//------------------------------------------------------------------------------

void CCheminfoProjectTransfer::FinishLoadToNewProject(void)
{
    if (CheminfoProject->GetJobs()->HasChildren()) {
        return;
    }

    NewProject->ShowProject();

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CStructure* CCheminfoProjectTransfer::CreateResults(CProject* p_project)
{
    CTrajectoryList* p_trajlist = p_project->GetTrajectories();
    // metod to add new structure with trajectory for results
    QString res = tr("Results %1");
    QString trj = tr("Poses %1");

    int nmbr_traj = 1;
    if (p_trajlist != NULL) {
        nmbr_traj =  p_trajlist->GetNumberOfTrajectories() + 1;
    } else {
        if (p_trajlist == NULL) {
            p_trajlist = new CTrajectoryList(NewProject,true);
            p_trajlist->SetIndex(TRAJECTORY_LIST_INDEX);
        }
    }

    if (nmbr_traj == 1) {
        res = res.arg("");
        trj = trj.arg("");

    } else {
        res = res.arg(nmbr_traj);
        trj = trj.arg(nmbr_traj);
    }

    CStructure* p_results_str = p_project->GetStructures()->CreateStructure(res);
    // create and assign trajectory for view results
    CTrajectory* p_results_traj = p_trajlist->CreateTrajectory(trj);
    // set structure and save it to the structure history
    p_results_traj->SetStructure(p_results_str);

    p_trajlist->SetActiveTrajectory(p_results_traj);
    p_project->GetStructures()->SetActiveStructure(p_results_str);
    return (p_results_str);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCheminfoProjectTransfer::ExistsThisFile(QString file_name)
{
    if (file_name.isEmpty()) return(false);

    std::ifstream  sin;

    sin.open(file_name.toLatin1());
    if( !sin ) {
        return(false);
    }

    sin.close();
    return(true);

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
