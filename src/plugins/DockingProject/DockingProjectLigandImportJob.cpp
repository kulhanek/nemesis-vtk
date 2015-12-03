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
#include <QFileInfo>

#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>

#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <AtomList.hpp>
#include <BondList.hpp>
#include <ResidueList.hpp>
#include <Atom.hpp>
#include <Bond.hpp>
#include <Residue.hpp>
#include <PeriodicTable.hpp>
#include <MainWindow.hpp>
#include <JobList.hpp>

#include <Graphics.hpp>
#include <GraphicsView.hpp>
#include <GraphicsProfile.hpp>
#include <GraphicsProfileList.hpp>
#include <StructureList.hpp>

#include "DockingProject.hpp"
#include "DockingProjectModule.hpp"
#include "DockingProjectLigandImportJob.hpp"
#include "DockingProjectLigandImportJob.moc"
#include "OpenBabelUtils.hpp"

#include "openbabel/mol.h"
#include "openbabel/obconversion.h"

using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DockingProjectLigandImportJobCB(void* p_data);

CExtUUID        DockingProjectLigandImportJobID(
                    "{DOCKING_PROJECT_LIGAND_IMPORT_JOB:070bf859-4c52-4461-ab35-3c21b69461d8}",
                    "DockingProject Ligand Import");

CPluginObject   DockingProjectLigandImportJobObject(&DockingProjectPlugin,
                    DockingProjectLigandImportJobID,JOB_CAT,
                    ":/images/OpenBabel/Openbabel.png",
                    DockingProjectLigandImportJobCB);

// -----------------------------------------------------------------------------

QObject* DockingProjectLigandImportJobCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CDockingProjectLigandImportJob requires active project");
        return(NULL);
    }

    QObject* p_importjob = new CDockingProjectLigandImportJob(p_project);
    return(p_importjob);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDockingProjectLigandImportJob::CDockingProjectLigandImportJob(CProject* p_project)
    : CImportJob(&DockingProjectLigandImportJobObject,p_project)
{
    p_project->GetJobs()->RegisterJob(this);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CDockingProjectLigandImportJob::JobAboutToBeSubmitted(void)
{
    // check if it is allowed to import to empty structure
    Project = Structure->GetProject();
    if( Project->IsFlagSet<EProjecFlag>(EPF_ALLOW_IMP_TO_EMPTY_STR) ){
        if( Structure->IsEmpty() ){
            History = NULL;
            return(true);
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CDockingProjectLigandImportJob::InitializeJob(void)
{
    if( Project->GetHistory()->IsLocked(EHCL_TOPOLOGY) || Project->GetHistory()->IsLocked(EHCL_GEOMETRY)
        || Project->GetHistory()->IsLocked(EHCL_STRUCTURES))
    {
        return (false);
    }

    // Return only a file name
    QFileInfo file(FileName);
    QString name = file.fileName();

    // initialize topology change
    CDockingProject* p_docproj = dynamic_cast<CDockingProject*>(Project);
    CHistoryNode* p_history = NULL;

    if( p_docproj != NULL ){
        // We must open locks for right function
//        GetProject()->GetHistory()->SetLockModeLevels(CLockLevels(EHCL_NONE));
//        GetProject()->GetHistory()->SetMinLockModeLevels(CLockLevels(EHCL_NONE));
//        p_project->GetHistory()->SetLockModeLevels(CLockLevels(EHCL_NONE));
//        p_project->GetHistory()->SetMinLockModeLevels(CLockLevels(EHCL_NONE));

        // save ligand file name info
        p_docproj->SetLigandFileName(file);

        // following change is composite
        // it is composed from EHCL_TOPOLOGY and EHCL_GRAPHICS
        p_history = Structure->BeginChangeWH(EHCL_COMPOSITE,"import Ligand " + name + " structure");
        if( p_history == NULL ) return(false);

        History = Structure->BeginChangeWH(EHCL_TOPOLOGY,"import");
        if( History == NULL ){
            // end composite change
          Structure->EndChangeWH();
          return(false); // is change permitted?
        }

        // We must lock project again
//        p_project->GetHistory()->SetMinLockModeLevels(CLockLevels(EHCL_STRUCTURES|EHCL_TOPOLOGY|EHCL_GEOMETRY));
//        p_project->GetHistory()->SetMinLockModeLevels(CLockLevels(EHCL_STRUCTURES|EHCL_TOPOLOGY|EHCL_GEOMETRY));

//        GetProject()->GetHistory()->SetMinLockModeLevels(CLockLevels(EHCL_STRUCTURES|EHCL_TOPOLOGY|EHCL_GEOMETRY));
//        GetProject()->GetHistory()->SetLockModeLevels(CLockLevels(EHCL_STRUCTURES|EHCL_TOPOLOGY|EHCL_GEOMETRY));
    }
    else {
        // following change is composite
        // it is composed from EHCL_TOPOLOGY and EHCL_GRAPHICS
        p_history = Structure->BeginChangeWH(EHCL_COMPOSITE,"import Ligand structure");
        if( p_history == NULL ) return(false);

        History = Structure->BeginChangeWH(EHCL_TOPOLOGY,"import");
        if( History == NULL ){
            // end composite change
          Structure->EndChangeWH();
          return(false); // is change permitted?
        }


    }

    BackupLockLevels = Project->GetHistory()->GetLockModeLevels();
    CLockLevels super_lock = ~CLockLevels();
    Project->GetHistory()->SetLockModeLevels(super_lock);

    CGraphics* p_grp = Project->GetGraphics();
    p_grp->GetProfiles()->SetDataManipulationMode(true);

    connect(this,SIGNAL(OnStartProgressNotification(int)),
            Project,SLOT(StartProgressNotification(int)));

    connect(this,SIGNAL(OnProgressNotification(int,const QString&)),
            Project,SLOT(ProgressNotification(int,const QString&)));

    Structure->BeginUpdate(History);

    // THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // do some magic with parent and thread ownership
    OldMoleculeParent = Structure->parent();
    if( History ) OldHistoryParent = History->parent();

    // temporarily remove parent
    Structure->setParent(NULL);
    if( History ) History->setParent(NULL);

    // move objects to processing thread
    Structure->moveToThread(GetJobThread());
    if( History ) History->moveToThread(GetJobThread());
    // END OF THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    return(true);
}

//------------------------------------------------------------------------------

bool CDockingProjectLigandImportJob::ExecuteJob(void)
{
    emit OnStartProgressNotification(2);

    emit OnProgressNotification(0,"Total progress %p% - Reading data (1/2) ...");

    sin.open(FileName.toLatin1());
    if( !sin ) {
        QString message(tr("Unable to open file for reading!"));
        ES_ERROR("Cannot open file to write");
        Project->TextNotification(ETNT_ERROR,message,ETNT_ERROR_DELAY);
        return(false);
    }

    // read molecule from file to babel internal
    OBConversion    conv(&sin, &cout);
    OBFormat*       obFormat = conv.FormatFromExt(FileName.toStdString());
    OBMol           mol;

    if( obFormat == NULL ) {
        ES_ERROR("unsupported format");
        QString message(tr("Failed to set format for %1"));
        message = message.arg(QString(FileName));
        Project->TextNotification(ETNT_ERROR,message,ETNT_ERROR_DELAY);
        // close file
        sin.close();
        return(false);
    }

    if( ! conv.SetInFormat(obFormat) ) {
        ES_ERROR("Cannot set format");
        QString message(tr("Failed to set format for %1"));
        message = message.arg(QString(FileName));
        Project->TextNotification(ETNT_ERROR,message,ETNT_ERROR_DELAY);
        // close file
        sin.close();
        return(false);
    }

    if( ! conv.Read(&mol) ) {
        ES_ERROR("Cannot read molecule from file");
        QString message(tr("Failed to read molecule from file %1"));
        message = message.arg(QString(FileName));
        Project->TextNotification(ETNT_ERROR,message,ETNT_ERROR_DELAY);
        // close file
        sin.close();
        return(false);
    }

    mol.ConnectTheDots();
    mol.PerceiveBondOrders();

    emit OnProgressNotification(1,"Total progress %p% - Converting data (2/2) ...");

    // convert data
    COpenBabelUtils::OpenBabel2Nemesis(mol,Structure,History);

    // close file
    sin.close();

    // THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // do some magic with parent and thread ownership

    // return objects back to main thread
    Structure->moveToThread(QCoreApplication::instance()->thread());
    if( History ) History->moveToThread(QCoreApplication::instance()->thread());
    // END OF THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    return(true);
}

//------------------------------------------------------------------------------

bool CDockingProjectLigandImportJob::FinalizeJob(void)
{
    // THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // do some magic with parent and thread ownership

    // restore parents
    Structure->setParent(OldMoleculeParent);
    if( History ) History->setParent(OldHistoryParent);

    // notify master list - that the structure is back in the list
    if( Structure->GetStructures() ){
        Structure->GetStructures()->EmitOnStructureListChanged();
    }
    // END OF THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    emit OnProgressNotification(2,"Total progress %p% - Finalization ...");

    // we do not need to sort the lists
    Structure->EndUpdate(true,History);

    // unlock history list
    CProject* p_project = Structure->GetProject();
    p_project->EndProgressNotification();
    p_project->GetHistory()->SetLockModeLevels(BackupLockLevels);

    CGraphics* p_grp = p_project->GetGraphics();
    p_grp->GetProfiles()->SetDataManipulationMode(false);

    if( History != NULL ){
        // end topology change
        Structure->EndChangeWH();
    }

    // adjust graphics, but not right on ligand
    if (GetProject()->GetGraphics()->GetPrimaryView()->IsOpenGLCanvasAttached()) {
        AddGraphicModel(false);
    }

    if( History != NULL ){
        // end composite change
        Structure->EndChangeWH();
    }
    // after load data to structure set it to active state
    GetProject()->GetStructures()->SetActiveStructure(Structure);
    // sort structure list, because in other case will be ligand structure at the end of list
    GetProject()->GetStructures()->SortStructures();
    PushToRecentFiles();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
