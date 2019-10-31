// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2016 Matej Stevuliak, 423943@mail.muni.cz
//    Copyright (C) 2016 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <QMessageBox>
#include <QCoreApplication>

#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>

#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <JobList.hpp>
#include <MainWindow.hpp>
#include <StructureList.hpp>
#include <QMessageBox>
#include <AtomListHistory.hpp>
#include <Graphics.hpp>
#include <GraphicsProfileList.hpp>
#include <GraphicsObjectList.hpp>
#include <XYZStructure.hpp>
#include <Atom.hpp>
#include <PeriodicTable.hpp>
#include <BondList.hpp>
#include <AtomList.hpp>
#include <Atom.hpp>
#include <OpenBabelUtils.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <SmallString.hpp>
#include <openbabel/mol.h>
#include <map>
#include <PeriodicTable.hpp>
#include <openbabel/mol.h>
#include <openbabel/atom.h>

#include "AmberModule.hpp"
#include "GESPGridObject.hpp"
#include "GESPImportJob.hpp"

//------------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------------

#if defined _WIN32 || defined __CYGWIN__
#undef AddAtom
#endif

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GESPImportJobCB(void* p_data);

CExtUUID        GESPImportJobID(
                    "{GESP_IMPORT_JOB:bd188b3a-0591-4a7f-979d-00e14bacdcf0}",
                    "GESP Import");

CPluginObject   GESPImportJobObject(&AmberPlugin,
                    GESPImportJobID,JOB_CAT,
                    GESPImportJobCB);

// -----------------------------------------------------------------------------

QObject* GESPImportJobCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CGESPImportJob requires active project");
        return(NULL);
    }

    QObject* p_importjob = new CGESPImportJob(p_project);
    return(p_importjob);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGESPImportJob::CGESPImportJob(CProject* p_project)
    : CImportJob(&GESPImportJobObject,p_project)
{
    p_project->GetJobs()->RegisterJob(this);
    GESPGridObject = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CGESPImportJob::JobAboutToBeSubmitted(void)
{
    sin.open(FileName.toLatin1());
    if( !sin ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Unable to open file for reading!"),QMessageBox::Ok,QMessageBox::Ok);
        ES_ERROR("Cannot open file to read");
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

    // following change is composite
    // it is composed from EHCL_TOPOLOGY and EHCL_GRAPHICS

    CHistoryNode* p_history;

    p_history = Structure->BeginChangeWH(EHCL_COMPOSITE,"import GESP data");
    if( p_history == NULL ) return(false);

    // initialize graphics change - this is optional
    CHistoryNode* p_ghistory = Structure->BeginChangeWH(EHCL_GRAPHICS,"import GESP grid data");
    if( p_ghistory != NULL ){
        CGraphics* p_grp = Structure->GetProject()->GetGraphics();
        GESPGridObject = dynamic_cast<CGESPGridObject*>(p_grp->GetObjects()->CreateObject(GESPGridObjectID,"GESP data","",p_ghistory));
        if( GESPGridObject != NULL ){
            if( p_grp->GetProfiles()->GetActiveProfile() != NULL ){
                p_grp->GetProfiles()->GetActiveProfile()->AddObject(GESPGridObject,-1,p_ghistory);
            }
        }
    }

    // end graphics change
    Structure->EndChangeWH();

    // initialize topology change
    History = Structure->BeginChangeWH(EHCL_TOPOLOGY,"import GESP structure");
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

bool CGESPImportJob::InitializeJob(void)
{
    CProject* p_project = Structure->GetProject();

    connect(this,SIGNAL(OnStartProgressNotification(int)),
            p_project,SLOT(StartProgressNotification(int)));

    connect(this,SIGNAL(OnProgressNotification(int,const QString&)),
            p_project,SLOT(ProgressNotification(int,const QString&)));

    connect(this,SIGNAL(OnTextNotification(ETextNotificationType,const QString&,int)),
            p_project,SLOT(TextNotification(ETextNotificationType,const QString&,int)));

    // create graphics



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

    emit OnStartProgressNotification(2);

    return(true);
}

//------------------------------------------------------------------------------

bool CGESPImportJob::ExecuteJob(void)
{
    return(ImportStructure());
}

//------------------------------------------------------------------------------

bool CGESPImportJob::ImportStructure(void)
{

// read structure

    Structure->BeginUpdate(History);

    // read molecule from file to babel internal
    emit OnProgressNotification(0,"Total progress %p% - Loading structure ...");

    bool result = true;   

    int num;
    double bohr = 0.529177249;
    double  x, y, z;
    double  qesp;
    string s;
    string temp;
    string  type;
    stringstream stream;

 // read header --------------------------------------
    for( int i = 0; i < 3; i++ ) {
        getline(sin,s);
    }

    stream.str(s);
    stream >> temp >> temp >> temp >> temp >> temp >> temp >> temp >> num;


// read structure ------------------------------------
    OpenBabel::OBMol mol;

    for( int i = 0; i < num; i++ ) {
        getline(sin,s);
        replace(s.begin(),s.end(),'D','E');

        stream.str(s);
        stream.clear();
        stream >> type >> x >> y >> z >> qesp;

        // convert coordenates to angstroms
        x=(x*bohr);
        y=(y*bohr);
        z=(z*bohr);

        // create OB molecule
        OpenBabel::OBAtom* p_atom = mol.NewAtom(i);
        p_atom->SetAtomicNum(PeriodicTable.SearchZBySymbol(type));
        p_atom->SetVector(x,y,z);
        p_atom->SetPartialCharge(qesp);

    }

    // make bonds and convert to Nemesis data
    mol.ConnectTheDots();
    mol.PerceiveBondOrders();

    COpenBabelUtils::OpenBabel2Nemesis(mol,Structure,History);


    // we do not need to sort the lists
    Structure->EndUpdate(true,History);

// read ESP grid points --------------------------------

    emit OnProgressNotification(1,"Total progress %p% - Loading ESP grid potential ...");
    if( GESPGridObject ) {
        int count;
        double  esp;

        for( int i = 0; i < 6; i++ ) {
            getline(sin,s);
        }

        stream.str(s);
        stream.clear();
        stream >> temp >> temp >> temp >> temp >> temp >> temp >> temp >> temp >> count;

        // go trough all grid points
        for(int i = 0; i < count; i++) {
            getline(sin,s);
            replace(s.begin(),s.end(),'D','E');

            stream.str(s);
            stream.clear();
            stream >> esp >> x >> y >> z;

            // convert coordenates to angstroms
            x=(x*bohr);
            y=(y*bohr);
            z=(z*bohr);

            // adds point to the grid
            GESPGridObject->AddPoint(x,y,z,esp);

        }
    }

// final

    // THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // do some magic with parent and thread ownership
    // return objects back to main thread
    Structure->moveToThread(QCoreApplication::instance()->thread());
    if( History ) History->moveToThread(QCoreApplication::instance()->thread());
    // END OF THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    return(result);
}


//------------------------------------------------------------------------------

bool CGESPImportJob::FinalizeJob(void)
{   
    // close the stream
    sin.close();

    emit OnProgressNotification(2,"Total progress %p% - Finalization ...");

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
