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
#include <GraphicsProfile.hpp>
#include <GraphicsProfileList.hpp>
#include <StructureList.hpp>
#include <QMessageBox>

#include "OpenBabelModule.hpp"
#include "OpenBabelImportJob.hpp"
#include "OpenBabelImportJob.moc"
#include "OpenBabelUtils.hpp"

#include "openbabel/mol.h"
#include "openbabel/obconversion.h"

using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* OpenBabelImportJobCB(void* p_data);

CExtUUID        OpenBabelImportJobID(
                    "{OPENBABEL_IMPORT_JOB:35df9a19-7e5a-40d9-97c4-cc7441530b6b}",
                    "OpenBabel Import");

CPluginObject   OpenBabelImportJobObject(&OpenBabelPlugin,
                    OpenBabelImportJobID,JOB_CAT,
                    ":/images/OpenBabel/Openbabel.png",
                    OpenBabelImportJobCB);

// -----------------------------------------------------------------------------

QObject* OpenBabelImportJobCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("COpenBabelImportJob requires active project");
        return(NULL);
    }

    QObject* p_importjob = new COpenBabelImportJob(p_project);
    return(p_importjob);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

COpenBabelImportJob::COpenBabelImportJob(CProject* p_project)
    : CImportJob(&OpenBabelImportJobObject,p_project)
{
    p_project->GetJobs()->RegisterJob(this);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool COpenBabelImportJob::JobAboutToBeSubmitted(void)
{
    sin.open(FileName.toLatin1());
    if( !sin ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Unable to open file for reading!"),QMessageBox::Ok,QMessageBox::Ok);
        ES_ERROR("Cannot open file to write");
        return(false);
    }

    // check if it is allowed to import to empty structure
    CProject* p_project = Structure->GetProject();
    if( p_project->IsFlagSet<EProjecFlag>(EPF_ALLOW_IMP_TO_EMPTY_STR) ){
        if( Structure->IsEmpty() ){
            History = NULL;
            BackupLockLevels = p_project->GetHistory()->GetLockModeLevels();
            CLockLevels super_lock = ~CLockLevels();
            p_project->GetHistory()->SetLockModeLevels(super_lock);
            return(true);
        }
    }

    // Return only a file name
    QFileInfo file(FileName);
    QString name = file.fileName();

    // following change is composite
    // it is composed from EHCL_TOPOLOGY and EHCL_GRAPHICS
    CHistoryNode* p_history = Structure->BeginChangeWH(EHCL_COMPOSITE,"import " + name + " structure");
    if( p_history == NULL ) return(false);

    // initialize topology change
    History = Structure->BeginChangeWH(EHCL_TOPOLOGY,"import");
    if( History == NULL ){
        // end composite change
        Structure->EndChangeWH();
        return(false); // is change permitted?
    }

    // lock history
    BackupLockLevels = p_project->GetHistory()->GetLockModeLevels();
    CLockLevels super_lock = ~CLockLevels();
    p_project->GetHistory()->SetLockModeLevels(super_lock);

    return(true);
}

//------------------------------------------------------------------------------

bool COpenBabelImportJob::InitializeJob(void)
{
    CProject* p_project = Structure->GetProject();

    connect(this,SIGNAL(OnStartProgressNotification(int)),
            p_project,SLOT(StartProgressNotification(int)));

    connect(this,SIGNAL(OnProgressNotification(int,const QString&)),
            p_project,SLOT(ProgressNotification(int,const QString&)));

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

    CGraphics* p_grp = p_project->GetGraphics();
    p_grp->GetProfiles()->SetDataManipulationMode(true);

    return(true);
}

//------------------------------------------------------------------------------

bool COpenBabelImportJob::ExecuteJob(void)
{
    emit OnStartProgressNotification(2);

    Structure->BeginUpdate(History);

    emit OnProgressNotification(0,"Total progress %p% - Reading data (1/2) ...");

    // read molecule from file to babel internal
    OBConversion    conv(&sin, &cout);
    OBFormat*       obFormat = conv.FormatFromExt(FileName.toStdString());
    OBMol           mol;

    if( obFormat == NULL ) {
        ES_ERROR("unsupported format");
        QString message(tr("Failed to set format for %1"));
        message = message.arg(QString(FileName));
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),message,QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    if( ! conv.SetInFormat(obFormat) ) {
        ES_ERROR("Cannot set format");
        QString message(tr("Failed to set format for %1"));
        message = message.arg(QString(FileName));
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),message,QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    if( ! conv.Read(&mol) ) {
        ES_ERROR("Cannot read molecule from file");
        QString message(tr("Failed to read molecule from file %1"));
        message = message.arg(QString(FileName));
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),message,QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    mol.ConnectTheDots();
    mol.PerceiveBondOrders();

    emit OnProgressNotification(1,"Total progress %p% - Converting data (2/2) ...");

    // convert data
    COpenBabelUtils::OpenBabel2Nemesis(mol,Structure,History);

    // we do not need to sort the lists
    Structure->EndUpdate(true,History);

    // THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // do some magic with parent and thread ownership

    // return objects back to main thread
    Structure->moveToThread(QCoreApplication::instance()->thread());
    if( History ) History->moveToThread(QCoreApplication::instance()->thread());
    // END OF THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    return(true);
}

//------------------------------------------------------------------------------

bool COpenBabelImportJob::FinalizeJob(void)
{
    emit OnProgressNotification(2,"Total progress %p% - Finalization ...");

    // close file
    sin.close();

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

    // adjust graphics
    AdjustGraphics();

    if( History != NULL ){
        // end composite change
        Structure->EndChangeWH();
    }

    PushToRecentFiles();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
