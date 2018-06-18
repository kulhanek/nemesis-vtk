// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include "AmberModule.hpp"

#include "Rst7ExportJob.hpp"
#include "Rst7ExportJob.moc"

#include <fstream>

using namespace std;
using namespace boost;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* Rst7ExportJobCB(void* p_data);

CExtUUID        Rst7ExportJobID(
                    "{RST7_EXPORT_JOB:49560bca-a0da-4303-8759-ba7d294d97f2}",
                    "Amber Restart (rst7) Export");

CPluginObject   Rst7ExportJobObject(&AmberPlugin,
                    Rst7ExportJobID,JOB_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRst7ExportJob::CRst7ExportJob(CStructure* p_mol,const QString& name)
    : CJob(&Rst7ExportJobObject,p_mol->GetProject())
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

bool CRst7ExportJob::JobAboutToBeSubmitted(void)
{
    sout.open(FileName.toLatin1());
    if( !sout ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Unable to open file for writing!"),QMessageBox::Ok,QMessageBox::Ok);
        ES_ERROR("Cannot open file to write");
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

bool CRst7ExportJob::InitializeJob(void)
{
    MaxTicks = 2*Structure->GetAtoms()->GetNumberOfAtoms();

    CProject* p_project = Structure->GetProject();
    p_project->StartProgressNotification(MaxTicks);

    connect(this,SIGNAL(OnProgressNotification(int,const QString&)),
            p_project,SLOT(ProgressNotification(int,const QString&)));

    return(true);
}

//------------------------------------------------------------------------------

bool CRst7ExportJob::ExecuteJob(void)
{
    Tick = 1;
    Quantum = MaxTicks / 100;
    if( Quantum == 0 ){
        Quantum = 1;
    }

    emit OnProgressNotification(Tick,"Total progress %p% - Initialization ...");

    // write to stream
    WriteHead();
    WriteAtoms();
    WriteVelocities();
    WriteBox();

    emit OnProgressNotification(Tick,"Total progress %p% - Finalization ...");

    return(true);
}

//------------------------------------------------------------------------------

bool CRst7ExportJob::FinalizeJob(void)
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

void CRst7ExportJob::WriteHead(void)
{
    sout << "AMBER_7_RESTART_FILE" << endl;
    sout << format( "%6d" ) % Structure->GetAtoms()->GetNumberOfAtoms() << endl;
}

// -------------------------------------------------------------------------

void CRst7ExportJob::WriteAtoms(void)
{
    if( Structure->GetAtoms()->GetNumberOfAtoms() == 0 ) {
        return;
    }

    int idx = 0;
    foreach(QObject* p_qobj, Structure->GetAtoms()->children()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        CPoint pos = p_atom->GetPos();
        sout << format( "%12.7f" ) % pos.x;
        sout << format( "%12.7f" ) % pos.y;
        sout << format( "%12.7f" ) % pos.z;
        idx += 3;
        if( idx % 6 == 0 ){
            sout << endl;
        }

        if( Tick % Quantum == 0 ){
            emit OnProgressNotification(Tick,"Total progress %p% - Saving atom positions (1/2) ...");
        }
        Tick++;
    }
    if( idx % 6 != 0 ){
        sout << endl;
    }
}

// -------------------------------------------------------------------------

void CRst7ExportJob::WriteVelocities(void)
{
    if( Structure->GetAtoms()->GetNumberOfAtoms() == 0 ) {
        return;
    }

    int idx = 0;
    foreach(QObject* p_qobj, Structure->GetAtoms()->children()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        CPoint vel = p_atom->GetVel();
        sout << format( "%12.7f" ) % vel.x;
        sout << format( "%12.7f" ) % vel.y;
        sout << format( "%12.7f" ) % vel.z;
        idx += 3;
        if( idx % 6 == 0 ){
            sout << endl;
        }

        if( Tick % Quantum == 0 ){
            emit OnProgressNotification(Tick,"Total progress %p% - Saving atom velocities (2/2) ...");
        }
        Tick++;
    }
    if( idx % 6 != 0 ){
        sout << endl;
    }
}

// -------------------------------------------------------------------------

void CRst7ExportJob::WriteBox(void)
{
    if( ! Structure->PBCInfo.IsValid() ) return;

    sout << format( "%12.7f" ) % Structure->PBCInfo.GetAVectorSize();
    sout << format( "%12.7f" ) % Structure->PBCInfo.GetBVectorSize();
    sout << format( "%12.7f" ) % Structure->PBCInfo.GetCVectorSize();

    sout << format("%12.7f") % (Structure->PBCInfo.GetAlpha()*180.0/M_PI);
    sout << format("%12.7f") % (Structure->PBCInfo.GetBeta()*180.0/M_PI);
    sout << format("%12.7f") % (Structure->PBCInfo.GetGamma()*180.0/M_PI);

    sout << endl;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
