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
#include <QMessageBox>
#include <fstream>

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

#include <OpenBabelUtils.hpp>

#include "DockingProject.hpp"
#include "DockingProjectModule.hpp"
#include "DockingProjectReceptorExportJob.hpp"
#include "DockingProjectReceptorExportJob.moc"

#include "openbabel/mol.h"
#include "openbabel/obconversion.h"

using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DockingProjectReceptorExportJobCB(void* p_data);

CExtUUID        DockingProjectReceptorExportJobID(
                    "{DOCKING_PROJECT_RECEPTOR_EXPORT_JOB:2561b35c-708c-4554-90f5-1042102511b7}",
                    "PDBQT Receptor Export");

CPluginObject   DockingProjectReceptorExportJobObject(&DockingProjectPlugin,
                    DockingProjectReceptorExportJobID,JOB_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDockingProjectReceptorExportJob::CDockingProjectReceptorExportJob(CStructure* p_mol, const QString& name, bool flexible, bool combine, bool renumber)
    : CJob(&DockingProjectReceptorExportJobObject,p_mol->GetProject())
{
    Structure = p_mol;
    FileName = name;
    Flexible = flexible;
    Combine = combine;
    Renumber = renumber;

    Structure->GetProject()->GetJobs()->RegisterJob(this);

    MaxTicks = 0;
    Quantum = 0;
    Tick = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CDockingProjectReceptorExportJob::JobAboutToBeSubmitted(void)
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

bool CDockingProjectReceptorExportJob::InitializeJob(void)
{
    MaxTicks = Structure->GetAtoms()->GetNumberOfAtoms();

    CProject* p_project = Structure->GetProject();
    p_project->StartProgressNotification(MaxTicks);

    connect(this,SIGNAL(OnProgressNotification(int,const QString&)),
            p_project,SLOT(ProgressNotification(int,const QString&)));

    return(true);
}

//------------------------------------------------------------------------------

bool CDockingProjectReceptorExportJob::ExecuteJob(void)
{
    Tick = 1;
    Quantum = MaxTicks / 100;
    if( Quantum == 0 ){
        Quantum = 1;
    }

    emit OnProgressNotification(Tick,"Total progress %p% - Initialization ...");

    Tick++;

    emit OnProgressNotification(Tick,"Total progress %p% - Read data (1/2) ...");

    OpenBabel::OBConversion conv(&cin, &sout);
    OpenBabel::OBFormat*    obFormat = conv.FormatFromExt(FileName.toLatin1().constData());
    if(obFormat == NULL) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("Unable to find export format!"),QMessageBox::Ok,QMessageBox::Ok);
        return (false);

    }

    if (!conv.SetOutFormat(obFormat)) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("Unable to set export format!"),QMessageBox::Ok,QMessageBox::Ok);
        return (false);
    }

    // output write settings from user interface and offer by openbabel PDBQT convert
    if (!Flexible) {
        // r - Output as a rigid molecule (i.e. no branches or torsion tree)
        conv.AddOption("r",OBConversion::OUTOPTIONS);
    }

    if (Combine && (!Flexible) ) {
        // c  Combine separate molecular pieces of input into a single rigid molecule (requires "r" option or will have no effect)
        conv.AddOption("c",OBConversion::OUTOPTIONS);
    }

    if (!Renumber) {
        // p - Preserve atom indices from input file (default is to renumber atoms sequentially)
        conv.AddOption("p",OBConversion::OUTOPTIONS);
    }

    OBMol mol;

    Tick++;
    emit OnProgressNotification(Tick,"Total progress %p% - Converting data (2/2) ...");
    // convert data
    COpenBabelUtils::Nemesis2OpenBabel(Structure,mol);

    // write data
    if( ! conv.Write(&mol) ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("Unable to write file!"),QMessageBox::Ok,QMessageBox::Ok);
        return (false);
    }
    Tick++;
    emit OnProgressNotification(Tick,"Total progress %p% - Finalization ...");

    return(true);
}

//------------------------------------------------------------------------------

bool CDockingProjectReceptorExportJob::FinalizeJob(void)
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
