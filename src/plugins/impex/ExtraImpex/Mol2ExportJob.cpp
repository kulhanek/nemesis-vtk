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

#include "Mol2ExportJob.hpp"

#include <fstream>

using namespace std;
using namespace boost;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* Mol2ExportJobCB(void* p_data);

CExtUUID        Mol2ExportJobID(
                    "{MOL2_EXPORT_JOB:0e45101d-9c25-450e-8a80-e7602a633d26}",
                    "Tripos Mol2 File (*.mol2)",
                    "Export Tripos Mol2 file");

CPluginObject   Mol2ExportJobObject(&ExtraImpexPlugin,
                    Mol2ExportJobID,JOB_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMol2ExportJob::CMol2ExportJob(CStructure* p_mol,const QString& name)
    : CJob(&Mol2ExportJobObject,p_mol->GetProject())
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

bool CMol2ExportJob::JobAboutToBeSubmitted(void)
{
    if( Structure->GetAtoms()->HasInvalidAtoms() ){
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Structure contains invalid atoms!"),QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    if( Structure->GetBonds()->HasInvalidBonds() ){
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Structure contains invalid bonds!"),QMessageBox::Ok,QMessageBox::Ok);
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

bool CMol2ExportJob::InitializeJob(void)
{
    MaxTicks = Structure->GetAtoms()->GetNumberOfAtoms()
             + Structure->GetBonds()->GetNumberOfBonds()
             + Structure->GetResidues()->GetNumberOfResidues();

    CProject* p_project = Structure->GetProject();
    p_project->StartProgressNotification(MaxTicks);

    connect(this,SIGNAL(OnProgressNotification(int,const QString&)),
            p_project,SLOT(ProgressNotification(int,const QString&)));

    return(true);
}

//------------------------------------------------------------------------------

bool CMol2ExportJob::ExecuteJob(void)
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
    WriteBonds(Structure->GetBonds());
    WriteResidues(Structure->GetResidues());
    WriteCrystal(Structure);

    emit OnProgressNotification(Tick,"Total progress %p% - Finalization ...");

    return(true);
}

//------------------------------------------------------------------------------

bool CMol2ExportJob::FinalizeJob(void)
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

void CMol2ExportJob::WriteHead(CStructure* p_mol)
{
    sout << "@<TRIPOS>MOLECULE" << endl;

    string molname = p_mol->GetName().toStdString();
    sout << (  molname.empty() ? "untitled" : molname ) << endl;

    // ----------------
    sout << format( "%8d " ) % p_mol->GetAtoms()->GetNumberOfAtoms();
    sout << format( "%8d " ) % p_mol->GetBonds()->GetNumberOfBonds();
    sout << format( "%8d " ) % p_mol->GetResidues()->GetNumberOfResidues() << endl;

    // ----------------
    sout << ( p_mol->GetResidues()->GetNumberOfResidues() < 2 ? "SMALL" : "POLYMER" ) << endl;

    // ----------------
    sout << "USER" << endl;
    sout << endl;
    sout << endl;
}

// -------------------------------------------------------------------------

void CMol2ExportJob::WriteAtoms(CAtomList* p_atoms)
{
    if( p_atoms->GetNumberOfAtoms() == 0 ) {
        return;
    }

    sout << "@<TRIPOS>ATOM" << endl;

    int indx = 1;
    foreach(QObject* p_qobj, p_atoms->children()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);

        IndexMap[p_atom->GetIndex()] = indx;
        sout << format( "%8d " ) % indx;
        sout << format( "%-8s " ) % p_atom->GetName().toStdString();
        CPoint pos = p_atom->GetPos();
        sout << format( "%12.6f " ) % pos.x;
        sout << format( "%12.6f " ) % pos.y;
        sout << format( "%12.6f " ) % pos.z;
        if( p_atom->GetType().isEmpty() ) {
            sout << format( "%-8s " ) % PeriodicTable.GetSymbol(p_atom->GetZ());
        } else {
            sout << format( "%-8s " ) % p_atom->GetType().toStdString();
        }
        if( p_atom->GetResidue() ){
            sout << format( "%8d " ) % p_atom->GetResidue()->GetSeqIndex();
            sout << format( "%-8s " ) % p_atom->GetResidue()->GetName().toStdString();
        } else {
            sout << format( "%8d " ) % 1;
            sout << format( "%-8s " ) % "UNK";
        }
        sout << format( "%10.5f" ) % p_atom->GetCharge();
        sout << endl;

        if( Tick % Quantum == 0 ){
            emit OnProgressNotification(Tick,"Total progress %p% - Saving atoms (1/3) ...");
        }
        Tick++;

        indx++;
    }
}

// -------------------------------------------------------------------------

void CMol2ExportJob::WriteBonds(CBondList* p_bonds)
{
    if( p_bonds->GetNumberOfBonds() == 0 ) {
        return;
    }
    sout << "@<TRIPOS>BOND" << std::endl;

    int id = 1;
    foreach(QObject* p_qobj, p_bonds->children()){
        CBond* p_bond = static_cast<CBond*>(p_qobj);

        sout << format("%8d ") % id;
        int first = IndexMap[p_bond->GetFirstAtom()->GetIndex()];
        int second = IndexMap[p_bond->GetSecondAtom()->GetIndex()];
        sout << format( "%8d " ) % std::min( first, second );
        sout << format( "%8d " ) % std::max( first, second );

        switch( p_bond->GetBondOrder() ){
            case BO_NONE:
                // nothing
                break;
            case BO_WEAK:
                // nothing
                break;
            case BO_SINGLE:
                sout << format( "%8d " ) % 1;
                break;
            case BO_SINGLE_H:
                sout << format( "%8s " ) % "ar";
                break;
            case BO_DOUBLE:
                sout << format( "%8d " ) % 2;
                break;
            case BO_DOUBLE_H:
                sout << format( "%8d " ) % 3;
                break;
            case BO_TRIPLE:
                sout << format( "%8d " ) % 3;
                break;
        }

        sout << std::endl;

        if( Tick % Quantum == 0 ){
            emit OnProgressNotification(Tick,"Total progress %p% - Saving bonds (2/3) ...");
        }
        Tick++;

        id++;
    }
}

// -------------------------------------------------------------------------

void CMol2ExportJob::WriteResidues(CResidueList* p_reslist)
{
    if( p_reslist->GetNumberOfResidues() == 0 ) {
        return;
    }
    sout << "@<TRIPOS>SUBSTRUCTURE" << std::endl;

    int id = 1;
    foreach(QObject* p_qobj, p_reslist->children()){
        CResidue* p_res = static_cast<CResidue*>(p_qobj);

        sout << format("%8d %10s") % id % p_res->GetName().toStdString();

        if( p_res->GetNumberOfAtoms() > 0 ){
            sout << format("%5d RESIDUE          0 ****  ****    0 ROOT") % p_res->GetAtoms()[0]->GetSerIndex();
        } else {
            sout << format("%5d RESIDUE          0 ****  ****    0 ROOT") % 0;
        }
        sout << endl;

        if( Tick % Quantum == 0 ){
            emit OnProgressNotification(Tick,"Total progress %p% - Saving residues (3/3) ...");
        }
        Tick++;

        id++;
    }

}

//------------------------------------------------------------------------------

void CMol2ExportJob::WriteCrystal(CStructure* p_mol)
{
    if( p_mol->PBCInfo.IsValid() == false ) return;

    sout << "@<TRIPOS>CRYSIN" << std::endl;
    sout << format(" %12.6f") % p_mol->PBCInfo.GetAVectorSize();
    sout << format(" %12.6f") % p_mol->PBCInfo.GetBVectorSize();
    sout << format(" %12.6f") % p_mol->PBCInfo.GetCVectorSize();
    sout << format(" %12.6f") % (p_mol->PBCInfo.GetAlpha()*180.0/M_PI);
    sout << format(" %12.6f") % (p_mol->PBCInfo.GetBeta()*180.0/M_PI);
    sout << format(" %12.6f") % (p_mol->PBCInfo.GetGamma()*180.0/M_PI);
    sout << format(" %d") % 0;
    sout << format(" %d") % 0;
    sout << std::endl;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
