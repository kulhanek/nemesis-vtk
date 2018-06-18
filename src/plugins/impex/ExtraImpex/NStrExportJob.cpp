// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <XMLPrinter.hpp>
#include <QMessageBox>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "ExtraImpexModule.hpp"

#include "NStrExportJob.hpp"
#include "NStrExportJob.moc"

#include <fstream>

using namespace std;
using namespace boost;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* NStrExportJobCB(void* p_data);

CExtUUID        NStrExportJobID(
                    "{NSTR_EXPORT_JOB:d0c3fded-2d48-45e5-b437-8dcadb30b226}",
                    "Nemesis Structure Export");

CPluginObject   NStrExportJobObject(&ExtraImpexPlugin,
                    NStrExportJobID,JOB_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNStrExportJob::CNStrExportJob(CStructure* p_mol,const QString& name)
    : CJob(&NStrExportJobObject,p_mol->GetProject())
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

bool CNStrExportJob::JobAboutToBeSubmitted(void)
{
    if( Structure->GetAtoms()->HasInvalidAtoms() ){
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Structure contains invalid atoms!"),QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    if( Structure->GetBonds()->HasInvalidBonds() ){
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Structure contains invalid bonds!"),QMessageBox::Ok,QMessageBox::Ok);
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

bool CNStrExportJob::InitializeJob(void)
{
    MaxTicks = Structure->GetAtoms()->GetNumberOfAtoms()
             + Structure->GetBonds()->GetNumberOfBonds();

    CProject* p_project = Structure->GetProject();
    p_project->StartProgressNotification(MaxTicks);

    connect(this,SIGNAL(OnProgressNotification(int,const QString&)),
            p_project,SLOT(ProgressNotification(int,const QString&)));

    return(true);
}

//------------------------------------------------------------------------------

bool CNStrExportJob::ExecuteJob(void)
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

    emit OnProgressNotification(Tick,"Total progress %p% - Finalization ...");

    return(true);
}

//------------------------------------------------------------------------------

bool CNStrExportJob::FinalizeJob(void)
{
    // save the structure
    CXMLPrinter xml_printer;
    xml_printer.SetPrintedXMLNode(&OutputDoc);

    bool result = xml_printer.Print(FileName);
    if( result == false ){
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Unable to save Nemesis structure!"),QMessageBox::Ok,QMessageBox::Ok);
    }

    // unlock history list
    CProject* p_project = Structure->GetProject();
    p_project->EndProgressNotification();
    p_project->GetHistory()->SetLockModeLevels(BackupLockLevels);

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CNStrExportJob::WriteHead(CStructure* p_mol)
{
    OutputDoc.CreateChildDeclaration();
    OutputDoc.CreateChildComment("Nemesis structure");
    OutputDoc.CreateChildElement("structure");
}

// -------------------------------------------------------------------------

void CNStrExportJob::WriteAtoms(CAtomList* p_atoms)
{
    if( p_atoms->GetNumberOfAtoms() == 0 ) {
        return;
    }

    CXMLElement* p_sele = OutputDoc.GetFirstChildElement("structure");
    if( p_sele == NULL ){
        RUNTIME_ERROR("no structure element");
    }
    CXMLElement* p_ele = p_sele->CreateChildElement("atoms");

    // we must start from 1
    Indexes.SetIndex(1);

    foreach(QObject* p_qobj, p_atoms->children()){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);

        CXMLElement* p_ale = p_ele->CreateChildElement("atom");
        int index = Indexes.GetIndex();
        IndexMap[p_atom->GetIndex()] = index;
        p_ale->SetAttribute("id",index);
        p_ale->SetAttribute("Z",p_atom->GetZ());
        p_ale->SetAttribute("px",p_atom->GetPos().x);
        p_ale->SetAttribute("py",p_atom->GetPos().y);
        p_ale->SetAttribute("pz",p_atom->GetPos().z);

        if( Tick % Quantum == 0 ){
            emit OnProgressNotification(Tick,"Total progress %p% - Saving atoms (1/2) ...");
        }
        Tick++;
    }
}

// -------------------------------------------------------------------------

void CNStrExportJob::WriteBonds(CBondList* p_bonds)
{
    if( p_bonds->GetNumberOfBonds() == 0 ) {
        return;
    }

    CXMLElement* p_sele = OutputDoc.GetFirstChildElement("structure");
    if( p_sele == NULL ){
        RUNTIME_ERROR("no structure element");
    }
    CXMLElement* p_ele = p_sele->CreateChildElement("bonds");

    foreach(QObject* p_qobj, p_bonds->children()){
        CBond* p_bond = static_cast<CBond*>(p_qobj);

        CXMLElement* p_ble = p_ele->CreateChildElement("bond");
        int index = Indexes.GetIndex();
        p_ble->SetAttribute("id",index);
        p_ble->SetAttribute("ord",p_bond->GetBondOrder());

        int first = IndexMap[p_bond->GetFirstAtom()->GetIndex()];
        int second = IndexMap[p_bond->GetSecondAtom()->GetIndex()];
        p_ble->SetAttribute("A1",first);
        p_ble->SetAttribute("A2",second);

        if( Tick % Quantum == 0 ){
            emit OnProgressNotification(Tick,"Total progress %p% - Saving bonds (2/2) ...");
        }
        Tick++;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
