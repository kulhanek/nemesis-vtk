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
#include "boost/assign.hpp"

#include "ExtraImpexModule.hpp"

#include "PdbqtExportJob.hpp"
#include "PdbqtExportJob.moc"

#include <fstream>

using namespace std;
using namespace boost;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PdbqtExportJobCB(void* p_data);

CExtUUID        PdbqtExportJobID(
                    "{PDBQT_EXPORT_JOB:6ed616e8-c969-4ed5-9b36-39aa82e5098a}",
                    "PDBQT Export");

CPluginObject   PdbqtExportJobObject(&ExtraImpexPlugin,
                    PdbqtExportJobID,JOB_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPdbqtExportJob::CPdbqtExportJob(CStructure* p_mol,const QString& name)
    : CJob(&PdbqtExportJobObject,p_mol->GetProject())
{
    Structure = p_mol;
    FileName = name;

    Structure->GetProject()->GetJobs()->RegisterJob(this);
    // if item has a bond to a hydrogen:
    // change acceptor type 'XA' to 'XB' for 'B'oth 'A'cceptor and 'D'onor
    PDBQTTypes = boost::assign::map_list_of ("H", "HD") ("N", "NA") ("C", "CA") ("S", "SA");

    MaxTicks = 0;
    Quantum = 0;
    Tick = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CPdbqtExportJob::JobAboutToBeSubmitted(void)
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

bool CPdbqtExportJob::InitializeJob(void)
{
    MaxTicks = Structure->GetAtoms()->GetNumberOfAtoms();

    CProject* p_project = Structure->GetProject();
    p_project->StartProgressNotification(MaxTicks);

    connect(this,SIGNAL(OnProgressNotification(int,const QString&)),
            p_project,SLOT(ProgressNotification(int,const QString&)));

    return(true);
}

//------------------------------------------------------------------------------

bool CPdbqtExportJob::ExecuteJob(void)
{
    Tick = 1;
    Quantum = MaxTicks / 100;
    if( Quantum == 0 ){
        Quantum = 1;
    }

    emit OnProgressNotification(Tick,"Total progress %p% - Initialization ...");

    // write to stream
    InitMoleculeIndexes(Structure);
    WriteHead(Structure);
    WriteAtoms(Structure->GetAtoms());

    emit OnProgressNotification(Tick,"Total progress %p% - Finalization ...");

    return(true);
}

//------------------------------------------------------------------------------

bool CPdbqtExportJob::FinalizeJob(void)
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

void CPdbqtExportJob::WriteHead(CStructure* p_mol)
{
    // nothing to do here
    // reserved for future
}

// -------------------------------------------------------------------------

void CPdbqtExportJob::WriteAtoms(CAtomList* p_atoms)
{
    if( p_atoms->GetNumberOfAtoms() == 0 ) {
        return;
    }

// PDBQT file:
//    1 -  6        Record name   "ATOM  "
//    7 - 11        Integer       serial       Atom  serial number.
//   13 - 16        Atom          name         Atom name.
//   17             Character     altLoc       Alternate location indicator.
//   18 - 20        Residue name  resName      Residue name.
//   22             Character     chainID      Chain identifier.
//   23 - 26        Integer       resSeq       Residue sequence number.
//   27             AChar         iCode        Code for insertion of residues.
//   31 - 38        Real(8.3)     x            Orthogonal coordinates for X in Angstroms.
//   39 - 46        Real(8.3)     y            Orthogonal coordinates for Y in Angstroms.
//   47 - 54        Real(8.3)     z            Orthogonal coordinates for Z in Angstroms.
//   55 - 60        Real(6.2)     occupancy    Occupancy.
//   61 - 66        Real(6.2)     tempFactor   Temperature  factor.
//   71 - 76        LString(2)    charge       Charge  on the atom.
//   78 - 79        LString(2)    element      PDBQT type element symbol, left-justified.

    int indx = 1;
    int last_molindx = -1;
    foreach(QObject* p_qobj, p_atoms->children()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        CResidue* p_res = p_atom->GetResidue();

        // should we separate new molecule?
        int molindx = MoleculeIndexes[p_atom];
        if( last_molindx == -1  ){
            last_molindx = molindx;
        }
        if( last_molindx != molindx ){
            last_molindx = molindx;
            sout << "TER" << endl;
        }

        // write atom data
        sout << format( "%-6s" ) % "ATOM";
        sout << format( "%5d " ) % (p_atom->GetSerIndex() % 100000);
        sout << format( "%-4s" ) % p_atom->GetName().toStdString();

        if( p_res ){
            sout << format( "%1s" ) % " ";
            sout << format( "%-3s " ) % p_res->GetName().toStdString();
            sout << format( "%1s" ) % p_res->GetChain().toStdString();
            sout << format( "%4d" ) % (p_res->GetSeqIndex() % 10000);
            sout << format( "%1s   " ) % " ";
        } else {
            sout << format( "%1s" ) % " ";
            sout << format( "%-3s " ) % "UNK";
            sout << format( "%1s" ) % "";
            sout << format( "%4d" ) % 1;
            sout << format( "%1s   " ) % " ";
        }
        CPoint pos = p_atom->GetPos();

        sout << format( "%8.3f" ) % pos.x;
        sout << format( "%8.3f" ) % pos.y;
        sout << format( "%8.3f" ) % pos.z;

        sout << format( "%6.2f" ) % 1.0;
        sout << format( "%6.2f" ) % 0.0;

        sout << format( "%1s   " ) % " ";

        sout << format( "%6.3f" ) % p_atom->GetCharge();

        sout << format( "%1s" ) % " ";

        sout << format( "%-2.2s" ) % GetPDBQTSymbol(p_atom).toStdString();

        sout << endl;

        if( Tick % Quantum == 0 ){
            emit OnProgressNotification(Tick,"Total progress %p% - Saving atoms (2/2) ...");
        }
        Tick++;

        indx++;
    }
}

//------------------------------------------------------------------------------

void CPdbqtExportJob::InitMoleculeIndexes(CStructure* p_mol)
{
    emit OnProgressNotification(Tick,"Total progress %p% - Initializing molecule indexes (1/2) ...");

    int         mol_index = 0;
    CResidue*   p_last_res = NULL;

    // assign initial indexes, we assume that residues are not built from several molecules

    foreach(QObject* p_qobj, p_mol->GetAtoms()->children()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        if( p_last_res == NULL ) {
            p_last_res = p_atom->GetResidue();
        } else {
            if( p_last_res != p_atom->GetResidue() ) {
                p_last_res = p_atom->GetResidue();
                mol_index++;
            }
        }
        MoleculeIndexes[p_atom] = mol_index;
    }

    CAtom*  p_at1;
    CAtom*  p_at2;
    bool    passed = false;
    int     pass_index = 0;

    // this is iterative aproach, but it is quit fast

    while( passed == false ) {
        // printf("Pass index: %d\n",pass_index);
        pass_index++;
        passed = true;

        foreach(QObject* p_qobj, p_mol->GetBonds()->children()){
            CBond* p_bond = static_cast<CBond*>(p_qobj);

            p_at1 = p_bond->GetFirstAtom();
            p_at2 = p_bond->GetSecondAtom();

            if( MoleculeIndexes[p_at1] != MoleculeIndexes[p_at2] ) {
                passed = false;
                // use lower index
                if( MoleculeIndexes[p_at1] < MoleculeIndexes[p_at2] ) {
                    MoleculeIndexes[p_at2] = MoleculeIndexes[p_at1];
                } else {
                    MoleculeIndexes[p_at1] = MoleculeIndexes[p_at2];
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

QString CPdbqtExportJob::GetPDBQTSymbol(const CAtom* p_atom)
{
    ///TODO Add map with right values and write the functionality
    return (p_atom->GetName());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
