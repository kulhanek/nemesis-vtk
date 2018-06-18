// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <OpenBabelUtils.hpp>
#include <MainWindow.hpp>

#include <Graphics.hpp>
#include <GraphicsProfileList.hpp>
#include <GraphicsObjectList.hpp>
#include <PeriodicTable.hpp>
#include <StructureList.hpp>

#include "TurbomoleModule.hpp"

#include "CoordImportJob.hpp"

//------------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------------

#if defined _WIN32 || defined __CYGWIN__
#undef AddAtom
#endif

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CoordImportJobCB(void* p_data);

CExtUUID        CoordImportJobID(
                    "{COORD_IMPORT_JOB:ee7e1879-6ea5-47ee-a46b-50ad14dfa30c}",
                    "Turbomole Coord Import");

CPluginObject   CoordImportJobObject(&TurbomolePlugin,
                    CoordImportJobID,JOB_CAT,
                    CoordImportJobCB);

// -----------------------------------------------------------------------------

QObject* CoordImportJobCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CCoordImportJob requires active project");
        return(NULL);
    }

    QObject* p_importjob = new CCoordImportJob(p_project);
    return(p_importjob);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCoordImportJob::CCoordImportJob(CProject* p_project)
    : CImportJob(&CoordImportJobObject,p_project)
{
    Structure = NULL;
    FileName = "";
    LineNumber = 0;

    p_project->GetJobs()->RegisterJob(this);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCoordImportJob::JobAboutToBeSubmitted(void)
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

    // following change is composite
    // it is composed from EHCL_TOPOLOGY and EHCL_GRAPHICS
    CHistoryNode* p_history = Structure->BeginChangeWH(EHCL_COMPOSITE,"import XYZ structure");
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

bool CCoordImportJob::InitializeJob(void)
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

    emit OnStartProgressNotification(4);

    return(true);
}

//------------------------------------------------------------------------------

bool CCoordImportJob::ExecuteJob(void)
{
    Structure->BeginUpdate(History);

    // read molecule from file to babel internal
    emit OnProgressNotification(0,"Total progress %p% - Loading structure ...");

    bool result = true;

    mol.BeginModify();

    try {
        getline( sin, Line );
        while( sin ){
            LineNumber++;
            if( Line.find("$coord") == 0 ){
                ReadCoords();
                break;
            }
            getline( sin, Line );
        }
    } catch(...) {
        ES_ERROR("exception in data processing");
        result = false;
    }

    if( result ){
        emit OnProgressNotification(1,"Total progress %p% - Generating bonds ...");
        mol.ConnectTheDots();

        emit OnProgressNotification(2,"Total progress %p% - Generating bond types ...");
        mol.PerceiveBondOrders();

        mol.EndModify();

        // convert data
        emit OnProgressNotification(3,"Total progress %p% - Converting to internal representation ...");
        COpenBabelUtils::OpenBabel2Nemesis(mol,Structure,History);
    }

    // we do not need to sort the lists
    Structure->EndUpdate(true,History);

    // THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // do some magic with parent and thread ownership
    // return objects back to main thread
    Structure->moveToThread(QCoreApplication::instance()->thread());
    if( History ) History->moveToThread(QCoreApplication::instance()->thread());
    // END OF THREAD SAFETY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    return(result);
}

//------------------------------------------------------------------------------

void CCoordImportJob::ReadCoords(void)
{
    // bohr to A
    const double b2a = 0.52917721092;

    getline( sin, Line );
    while( sin ){
        LineNumber++;
        if( (Line.size() > 0) && Line[0] == '$' ) break; // another keyword - stop processing
        // split line to tokens
        istringstream   str;
        double          x,y,z;
        string          sym;
        str.str(Line);
        str >> x >> y >> z >> sym;
        if( ! str ){
            CSmallString error;
            error << "unable to parse line " << LineNumber << " containing '" << Line << "'";
            RUNTIME_ERROR(error);
        }

        // convert to A
        x *= b2a;
        y *= b2a;
        z *= b2a;

        // get symbol
        if( sym.size() > 0 ){
            sym[0] = toupper(sym[0]);
        }
        int az = PeriodicTable.SearchZBySymbol(sym.c_str());
        if( az == 0 ){
            CSmallString error;
            error << "unable to find element for symbol '" << sym << "' on line " << LineNumber << " containing '" << Line << "'";
            RUNTIME_ERROR(error);
        }

        // create new atom
        OBAtom* p_obatom = mol.NewAtom();
        p_obatom->SetAtomicNum(az);
        p_obatom->SetVector(x,y,z);

        getline( sin, Line );
    }
}

//------------------------------------------------------------------------------

bool CCoordImportJob::FinalizeJob(void)
{
    emit OnProgressNotification(4,"Total progress %p% - Finalization ...");

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
