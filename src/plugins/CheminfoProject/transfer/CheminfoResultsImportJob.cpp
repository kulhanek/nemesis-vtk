// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
//
//     This program sin free software; you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation; either version 2 of the License, or
//     (at your option) any later version.
//
//     This program sin distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License along
//     with this program; if not, write to the Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// =============================================================================

#include <QtGui>
#include <QMessageBox>

#include <PluginObject.hpp>
#include <Project.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <MainWindow.hpp>
#include <JobList.hpp>

#include <Graphics.hpp>
#include <GraphicsView.hpp>
#include <GraphicsProfileList.hpp>
#include <Structure.hpp>
#include <StructureList.hpp>
#include <Trajectory.hpp>
#include <TrajectoryList.hpp>
#include <TrajectorySegment.hpp>
#include <PDBQTTrajSegment.hpp>

#include "OpenBabelUtils.hpp"

#include "openbabel/mol.h"
#include "openbabel/obconversion.h"

#include "CheminfoProject.hpp"
#include "CheminfoProjectModule.hpp"

#include "CheminfoResultsImportJob.hpp"
#include "CheminfoResultsImportJob.moc"

using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CheminfoResultsImportJobCB(void* p_data);

CExtUUID        CheminfoResultsImportJobID(
                    "{CHEMINFO_RESULTS_IMPORT_JOB:513c3413-f3a1-4616-93c9-0cb1c8358584}",
                    "Cheminfo Results Import");

CPluginObject   CheminfoResultsImportJobObject(&CheminfoProjectPlugin,
                    CheminfoResultsImportJobID,JOB_CAT,
                    ":/images/OpenBabel/Openbabel.png",
                    CheminfoResultsImportJobCB);

// -----------------------------------------------------------------------------

QObject* CheminfoResultsImportJobCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CCheminfoResultsImportJob requires active project");
        return(NULL);
    }

    QObject* p_importjob = new CCheminfoResultsImportJob(p_project);
    return(p_importjob);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoResultsImportJob::CCheminfoResultsImportJob(CProject* p_project)
    : CImportJob(&CheminfoResultsImportJobObject,p_project)
{
    p_project->GetJobs()->RegisterJob(this);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCheminfoResultsImportJob::JobAboutToBeSubmitted(void)
{
    // check if it is allowed to import to empty structure
    NewProject = Structure->GetProject();
    Project = GetProject();
    if( NewProject->IsFlagSet<EProjecFlag>(EPF_ALLOW_IMP_TO_EMPTY_STR) ){
        if( Structure->IsEmpty() ){
            return(true);
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CCheminfoResultsImportJob::InitializeJob(void)
{
    if( Project->GetHistory()->IsLocked(EHCL_TOPOLOGY) || Project->GetHistory()->IsLocked(EHCL_GEOMETRY)
        || Project->GetHistory()->IsLocked(EHCL_STRUCTURES))
    {
        return (false);
    }

    // lock history
    BackupLockLevels = Project->GetHistory()->GetLockModeLevels();
    CLockLevels super_lock = ~CLockLevels();
    Project->GetHistory()->SetLockModeLevels(super_lock);

    connect(this,SIGNAL(OnStartProgressNotification(int)),
            Project,SLOT(StartProgressNotification(int)));

    connect(this,SIGNAL(OnProgressNotification(int,const QString&)),
            Project,SLOT(ProgressNotification(int,const QString&)));

    // THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // do some magic with parent and thread ownership
    OldMoleculeParent = Structure->parent();
    OldTrajectoryParent = Structure->GetTrajectory()->GetTrajectories();

    // temporarily remove parent
    Structure->setParent(NULL);
    Structure->GetTrajectory()->setParent(NULL);

    // move objects to processing thread
    Structure->moveToThread(GetJobThread());
    // END OF THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    CGraphics* p_grp = NewProject->GetGraphics();
    p_grp->GetProfiles()->SetDataManipulationMode(true);

    return(true);
}

//------------------------------------------------------------------------------

bool CCheminfoResultsImportJob::ExecuteJob(void)
{
    emit OnStartProgressNotification(2);
    Structure->BeginUpdate();
    emit OnProgressNotification(0,"Total progress %p% - Reading data (1/2) ...");
    CTrajectory* p_traj = Structure->GetTrajectory();
    if( p_traj == NULL ) {
        QString message(tr("The structure does not have any trajectory!"));
        Project->TextNotification(ETNT_ERROR,message,ETNT_ERROR_DELAY);
        ES_ERROR(message);
        return (false);
    }

    if( Structure->GetAtoms()->GetNumberOfAtoms() == 0 ){
        // structure does not contain any atoms - import initial structure
        //if( ImportPDBQTStructure(Structure,FileName) == false ){
        //    return  (false);
        // }

        std::ifstream  sin;

        sin.open(FileName.toLatin1());
        if( !sin ) {
           QString message(tr("Unable to open file for reading! Cannot open file to read."));
           ES_ERROR(message);
           Project->TextNotification(ETNT_ERROR,message,ETNT_ERROR_DELAY);
           return(false);
       }

        // read molecule from file to babel internal
        OBConversion    conv(&sin, &cout);
        OBFormat*       obFormat = conv.FormatFromExt(FileName.toStdString());
        OBMol           mol;

        if( obFormat == NULL ) {
            QString message(tr("Unsupported format. Failed to set format for %1"));
            message = message.arg(QString(FileName));
            ES_ERROR(message);
            Project->TextNotification(ETNT_ERROR,message,ETNT_ERROR_DELAY);
            // close file
            sin.close();
            return(false);
        }

        if( ! conv.SetInFormat(obFormat) ) {
            QString message(tr("Cannot set format. Failed to set format for %1"));
            message = message.arg(QString(FileName));
            Project->TextNotification(ETNT_ERROR,message,ETNT_ERROR_DELAY);
            ES_ERROR(message);
            // close file
            sin.close();
            return(false);
        }

        if( ! conv.Read(&mol) ) {
            QString message(tr("Cannot read molecule from file. Failed to read molecule from file %1"));
            message = message.arg(QString(FileName));
            Project->TextNotification(ETNT_ERROR,message,ETNT_ERROR_DELAY);
            ES_ERROR(message);
            // close file
            sin.close();
            return(false);
        }

        mol.ConnectTheDots();
        mol.PerceiveBondOrders();

        // convert data and save structure
        COpenBabelUtils::OpenBabel2Nemesis(mol, Structure);

        // dont end composite change here!
        // close file
        sin.close();
    }
    emit OnProgressNotification(1,"Total progress %p% - Converting data (2/2) ...");

    Structure->EndUpdate(true);

    // THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // do some magic with parent and thread ownership

    // return objects back to main thread
    Structure->moveToThread(QCoreApplication::instance()->thread());
    // END OF THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    return(true);
}

//------------------------------------------------------------------------------

bool CCheminfoResultsImportJob::FinalizeJob(void)
{
    emit OnProgressNotification(2,"Total progress %p% - Finalization ...");

    // THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // do some magic with parent and thread ownership

    // restore parents
    Structure->setParent(OldMoleculeParent);
    Structure->GetTrajectory()->setParent(OldTrajectoryParent);

    // notify master list - that the structure is back in the list
    if( Structure->GetStructures() ){
        Structure->GetStructures()->EmitOnStructureListChanged();
    }
    // END OF THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // after load data to structure set it to active state
    NewProject->GetStructures()->SetActiveStructure(Structure);

    // update trajectory indexes after load for right function
    Structure->UpdateAtomTrajIndexes();

    // create segment
    CPDBQTTrajSegment* p_seg = new CPDBQTTrajSegment(Structure->GetTrajectory());

    // setup segment
    p_seg->SetFileName(FileName);

    // register segment
    Structure->GetTrajectory()->RegisterSegment(p_seg);

    // start data import
    p_seg->LoadTrajectoryData();

    // set first snapshot if necessary
    if( (Structure->GetTrajectory()->GetNumberOfSnapshots() > 0) && (Structure->GetTrajectory()->GetCurrentSnapshotIndex() == 0) ){
        Structure->GetTrajectory()->FirstSnapshot();
    }

    Project->EndProgressNotification();
    Project->GetHistory()->SetLockModeLevels(BackupLockLevels);

    CGraphics* p_grp = NewProject->GetGraphics();
    p_grp->GetProfiles()->SetDataManipulationMode(false);

    // adjust graphics
    if (NewProject->GetGraphics()->GetPrimaryView()->IsOpenGLCanvasAttached()) {
        AddGraphicModel(false);
    }

    // after load data to structure set it to active state
    NewProject->GetStructures()->SetActiveStructure(Structure);
    // sort structure list, because in other case will be receptor structure at the end of list
    NewProject->GetStructures()->SortStructures();
    PushToRecentFiles();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCheminfoResultsImportJob::ImportPDBQTStructure(CStructure* p_str,const QString& file)
{
    try {
        std::ifstream  sin;

        sin.open(file.toLatin1());
        if( !sin ) {
            ES_ERROR(tr("Unable to open file for reading! Cannot open file to read."));
            return(false);
        }

        // read molecule from file to babel internal
        OBConversion    conv(&sin, &cout);
        OBFormat*       obFormat = conv.FormatFromExt(file.toStdString());
        OBMol           mol;

        if( obFormat == NULL ) {
            QString message(tr("Unsupported format. Failed to set format for %1"));
            message = message.arg(QString(file));
            ES_ERROR(message);
            Project->TextNotification(ETNT_ERROR,message,ETNT_ERROR_DELAY);
            // close file
            sin.close();
            return(false);
        }

        if( ! conv.SetInFormat(obFormat) ) {
            QString message(tr("Cannot set format. Failed to set format for %1"));
            message = message.arg(QString(file));
            ES_ERROR(message);
            Project->TextNotification(ETNT_ERROR,message,ETNT_ERROR_DELAY);
            // close file
            sin.close();
            return(false);
        }

        if( ! conv.Read(&mol) ) {
            QString message(tr("Cannot read molecule from file. Failed to read molecule from file %1"));
            message = message.arg(QString(file));
            ES_ERROR(message);
            Project->TextNotification(ETNT_ERROR,message,ETNT_ERROR_DELAY);
            // close file
            sin.close();
            return(false);
        }

        mol.ConnectTheDots();
        mol.PerceiveBondOrders();

        // convert data and save structure
        COpenBabelUtils::OpenBabel2Nemesis(mol, p_str);

    } catch(...) {

        QString message(tr("An exception occured. Failed to read molecule from file %1"));
        message = message.arg(QString(file));
        Project->TextNotification(ETNT_ERROR,message,ETNT_ERROR_DELAY);
        ES_ERROR(message);

        // close file
        sin.close();
        return(false);
    }
    CProject* p_project = Structure->GetProject();

    // after load data to structure set it to active state
    p_project->GetStructures()->SetActiveStructure(p_str);

    // update trajectory indexes after load for right function
    p_str->UpdateAtomTrajIndexes();

    // dont end composite change here!
    // close file
    sin.close();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

