// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QMessageBox>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "ExtraImpexModule.hpp"

#include "XYZExportJob.hpp"

#include <fstream>

using namespace std;
using namespace boost;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* XYZExportJobCB(void* p_data);

CExtUUID        XYZExportJobID(
                    "{XYZ_EXPORT_JOB:56684e57-8f94-4a4d-ac04-ffcea1f7e004}",
                    "XYZ File (*.xyz)",
                    "Export XYZ file");

CPluginObject   XYZExportJobObject(&ExtraImpexPlugin,
                    XYZExportJobID,JOB_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CXYZExportJob::CXYZExportJob(CStructure* p_mol,const QString& name)
    : CJob(&XYZExportJobObject,p_mol->GetProject())
{
    Structure = p_mol;
    FileName = name;

    Structure->GetProject()->GetJobs()->RegisterJob(this);

    MaxTicks = 0;
    Quantum = 0;
    Tick = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CXYZExportJob::JobAboutToBeSubmitted(void)
{
    if( Structure->GetAtoms()->HasInvalidAtoms() ){
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Structure contains invalid atoms!"),QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    sout.open(FileName.toLatin1());
    if( !sout ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Unable to open file for writing!"),QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    // lock history
    CProject* p_project = Structure->GetProject();

    BackupLockLevels = p_project->GetHistory()->GetLockModeLevels();
    CLockLevels super_lock = ~CLockLevels();
    p_project->GetHistory()->SetLockModeLevels(super_lock);

    return(true);
}

//------------------------------------------------------------------------------

bool CXYZExportJob::InitializeJob(void)
{
    MaxTicks = Structure->GetAtoms()->GetNumberOfAtoms();

    CProject* p_project = Structure->GetProject();
    p_project->StartProgressNotification(MaxTicks);

    connect(this,SIGNAL(OnProgressNotification(int,const QString&)),
            p_project,SLOT(ProgressNotification(int,const QString&)));

    return(true);
}

//------------------------------------------------------------------------------

bool CXYZExportJob::ExecuteJob(void)
{
    Tick = 1;
    Quantum = MaxTicks / 100;
    if( Quantum == 0 ){
        Quantum = 1;
    }

    emit OnProgressNotification(Tick,"Total progress %p% - Initialization ...");

    // write to stream
    WriteHead(Structure);
    WriteAtoms(Structure->GetAtoms());

    emit OnProgressNotification(Tick,"Total progress %p% - Finalization ...");

    return(true);
}

//------------------------------------------------------------------------------

bool CXYZExportJob::FinalizeJob(void)
{
    // close file
    sout.close();

    // unlock history list
    CProject* p_project = Structure->GetProject();
    p_project->EndProgressNotification();
    p_project->GetHistory()->SetLockModeLevels(BackupLockLevels);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CXYZExportJob::WriteHead(CStructure* p_mol)
{
    sout << format( "%d" ) % p_mol->GetAtoms()->GetNumberOfAtoms() << endl;
    sout << "XYZ file exported from Nemesis" <<  endl;
}

// -------------------------------------------------------------------------

void CXYZExportJob::WriteAtoms(CAtomList* p_atoms)
{
    if( p_atoms->GetNumberOfAtoms() == 0 ) {
        return;
    }
    foreach(QObject* p_qobj, p_atoms->children()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        sout << format( "%2s " ) % PeriodicTable.GetSymbol(p_atom->GetZ());
        CPoint pos = p_atom->GetPos();
        sout << format( "%20.12f " ) % pos.x;
        sout << format( "%20.12f " ) % pos.y;
        sout << format( "%20.12f " ) % pos.z << endl;

        if( Tick % Quantum == 0 ){
            emit OnProgressNotification(Tick,"Total progress %p% - Saving atoms (1/1) ...");
        }
        Tick++;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
