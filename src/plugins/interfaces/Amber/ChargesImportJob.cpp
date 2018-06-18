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

#include <QtGui>

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>

#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <ProjectList.hpp>

#include <Project.hpp>
#include <Structure.hpp>
#include <AtomList.hpp>
#include <BondList.hpp>
#include <ResidueList.hpp>
#include <Atom.hpp>
#include <Bond.hpp>
#include <Residue.hpp>
#include <HistoryList.hpp>
#include <JobList.hpp>
#include <QMessageBox>
#include <fstream>

#include <Graphics.hpp>
#include <GraphicsProfileList.hpp>
#include <GraphicsObjectList.hpp>
#include <GraphicsObject.hpp>
#include <GraphicsView.hpp>
#include <StructureList.hpp>

#include <AmberTopology.hpp>

#include "AmberModule.hpp"
#include "ChargesImportJob.hpp"

//------------------------------------------------------------------------------

#if defined _WIN32 || defined __CYGWIN__
#undef AddAtom
#endif

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* ChargesImportJobCB(void* p_data);

CExtUUID        ChargesImportJobID(
                    "{CHARGES_IMPORT_JOB:0c643559-bf1c-4ce6-b937-ed44d904b7de}",
                    "Point Charges Import");

CPluginObject   ChargesImportJobObject(&AmberPlugin,
                    ChargesImportJobID,JOB_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CChargesImportJob::CChargesImportJob(CProject* p_project)
    : CImportJob(&ChargesImportJobObject,p_project)
{
    History = NULL;
    MaxTicks = 0;
    p_project->GetJobs()->RegisterJob(this);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CChargesImportJob::JobAboutToBeSubmitted(void)
{
    History = NULL;
    CProject* p_project = Structure->GetProject();

    QFileInfo finfo(FileName);
    if( ! finfo.exists() ){
        ES_ERROR("unable to open file with charges");
        QMessageBox::critical(NULL, tr("Load charges"),
                              tr("Specified file with charges does not exist!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        return(false);
    }

    // initialize topology change
    History = Structure->BeginChangeWH(EHCL_TOPOLOGY,"import charges");
    if( History == NULL ){
        return(false); // is change permitted?
    }

    // lock history
    BackupLockLevels = p_project->GetHistory()->GetLockModeLevels();
    CLockLevels super_lock = ~CLockLevels();
    p_project->GetHistory()->SetLockModeLevels(super_lock);

    return(true);
}

//------------------------------------------------------------------------------

bool CChargesImportJob::InitializeJob(void)
{
    CProject* p_project = Structure->GetProject();

    MaxTicks = Structure->GetAtoms()->GetNumberOfAtoms();

    p_project->StartProgressNotification(MaxTicks);

    connect(this,SIGNAL(OnProgressNotification(int,const QString&)),
            p_project,SLOT(ProgressNotification(int,const QString&)));

    // THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // do some magic with parent and thread ownership
    if( History) OldHistoryParent = History->parent();

    // temporarily remove parent
    if( History) History->setParent(NULL);

    // move objects to processing thread
    if( History) History->moveToThread(GetJobThread());
    // END OF THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // disable scene repainting
    CGraphics* p_grp = p_project->GetGraphics();
    p_grp->GetProfiles()->SetDataManipulationMode(true);

    return(true);
}

//------------------------------------------------------------------------------

bool CChargesImportJob::ExecuteJob(void)
{
    Structure->BeginUpdate(History);

    ifstream fin(FileName.toLatin1());

    int tick = 1;
    int quantum = MaxTicks / 100;
    if( quantum == 0 ){
        quantum = 1;
    }

    // go throught all atoms in structure
    foreach(QObject* p_qobj,Structure->GetAtoms()->children()) {
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        double charge = 0.0;
        fin >> charge;
        p_atom->SetCharge(charge,History);
        if( tick % quantum == 0 ){
            emit OnProgressNotification(tick,"Total progress %p% - Loading charges ...");
        }
        tick++;
    }

    if( ! fin ){
        QMessageBox::warning(NULL, tr("Load charges"),
                              tr("Not enough data to read charges for all atoms!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
    }

    // we do not need to sort the lists
    Structure->EndUpdate(true,History);

    // THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // do some magic with parent and thread ownership

    // return objects back to main thread
    if( History) History->moveToThread(QCoreApplication::instance()->thread());
    // END OF THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    return(true);
}

//------------------------------------------------------------------------------

bool CChargesImportJob::FinalizeJob(void)
{
    emit OnProgressNotification(MaxTicks,"Total progress %p% - Finalization ...");

    // THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // do some magic with parent and thread ownership

    // restore parents
    if( History) History->setParent(OldHistoryParent);

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

    // end topology change
    if( History) Structure->EndChangeWH();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


