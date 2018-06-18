// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <StructureList.hpp>
#include <QMessageBox>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <Graphics.hpp>
#include <GraphicsProfileList.hpp>
#include <GraphicsObjectList.hpp>
#include <GraphicsObject.hpp>
#include <GraphicsView.hpp>

#include "ExtraImpexModule.hpp"

#include "Mol2ImportJob.hpp"
#include "Mol2ImportJob.moc"
#include "Mol2ImportTypes.hpp"

#include <fstream>

using namespace std;
using namespace boost;

//------------------------------------------------------------------------------

#if defined _WIN32 || defined __CYGWIN__
#undef AddAtom
#endif

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* Mol2ImportJobCB(void* p_data);

CExtUUID        Mol2ImportJobID(
                    "{MOL2_IMPORT_JOB:29d3aa01-9cff-4e0a-bd4c-5ba5936e5c59}",
                    "Mol2 Import");

CPluginObject   Mol2ImportJobObject(&ExtraImpexPlugin,
                    Mol2ImportJobID,JOB_CAT,
                    Mol2ImportJobCB);

// -----------------------------------------------------------------------------

QObject* Mol2ImportJobCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CMol2ImportJob requires active project");
        return(NULL);
    }

    QObject* p_importjob = new CMol2ImportJob(p_project);
    return(p_importjob);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMol2ImportJob::CMol2ImportJob(CProject* p_project)
    : CImportJob(&Mol2ImportJobObject,p_project)
{
    Structure = NULL;
    FileName = "";

    p_project->GetJobs()->RegisterJob(this);

    MaxTicks = 0;
    Quantum = 0;
    Tick = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CMol2ImportJob::JobAboutToBeSubmitted(void)
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
    CHistoryNode* p_history = Structure->BeginChangeWH(EHCL_COMPOSITE,"import Mol2 structure");
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

bool CMol2ImportJob::InitializeJob(void)
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

bool CMol2ImportJob::ExecuteJob(void)
{
    Structure->BeginUpdate(History);

    bool result = true;
    try {
        getline( sin, Line );
        while( sin ){
            LineNumber++;
            if( Line.find("@<TRIPOS>MOLECULE") != string::npos ){
                ReadHead();
            } else
            if( Line.find("@<TRIPOS>ATOM") != string::npos ){
                ReadAtoms();
            } else
            if( Line.find("@<TRIPOS>BOND") != string::npos ){
                ReadBonds();
            }
            if( Line.find("@<TRIPOS>CRYSIN") != string::npos ){
                ReadCrystal();
            }

            getline( sin, Line );
        }
    } catch(...) {
        ES_ERROR("exception in data processing");
        result = false;
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

bool CMol2ImportJob::FinalizeJob(void)
{
    emit OnProgressNotification(Tick,"Total progress %p% - Finalization ...");

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

void CMol2ImportJob::ReadHead(void)
{
    string name, moltype, chgtype, status, comment;

    // line 0
    getline( sin, Line );
    LineNumber++;
    name = Line;

    // line 1
    getline( sin, Line );
    LineNumber++;

    stringstream    str(Line);
    int             m_residues;

    str >> NumOfAtoms >> NumOfBonds >> m_residues;

    // update progress
    MaxTicks = NumOfAtoms + NumOfBonds;

    emit OnStartProgressNotification(MaxTicks);

    Tick = 1;
    Quantum = MaxTicks / 100;
    if( Quantum == 0 ){
        Quantum = 1;
    }

    // ignore rest of header

    // line 2
    getline( sin, moltype );
    LineNumber++;

    // line 3
    getline( sin, chgtype );
    LineNumber++;

    // line 4 : optional
    if( sin.peek() != '@' ) {
        getline( sin, status );
        LineNumber++;
    }

    // line 5: optional
    if( sin.peek() != '@' ) {
        getline( sin, comment );
        LineNumber++;
    }
}

// -------------------------------------------------------------------------

void CMol2ImportJob::ReadAtoms(void)
{
    int         ratid = 1;
    int         prev_resid = 0;
    CResidue*   p_res = NULL;
    bool        first = true;

    int top_seridx = Structure->GetAtoms()->GetTopSerIndex();
    int top_seqidx = Structure->GetResidues()->GetTopSeqIndex();

    getline( sin, Line );
    while( sin ){
        LineNumber++;

        int         atid = 0;
        string      atname;
        double      x = 0;
        double      y = 0;
        double      z = 0;
        string      type;
        int         resid = 0;
        string      resname;
        double      charge = 0;

        stringstream str( Line );
        str >> atid >> atname >> x >> y >> z >> type >> resid >> resname >> charge;

        if( (prev_resid < resid) || first ){
            first = false;
            prev_resid = resid;
            // create new residue
            p_res = Structure->GetResidues()->CreateResidue(QString(resname.c_str()),"",resid+top_seqidx,History);
        }

        CAtomData atomdata;
        atomdata.Index = -1;
        atomdata.Name = atname.c_str();
        atomdata.SerIndex = ratid+top_seridx;
        atomdata.Z = GuessZ(atname.c_str(),type.c_str());
        atomdata.Pos = CPoint(x,y,z);
        atomdata.Type = type.c_str();
        atomdata.Charge = charge;

        CAtom* p_atom = Structure->GetAtoms()->CreateAtom(atomdata,History);
        if( p_res != NULL ) p_res->AddAtom(p_atom,History);
        IndexMap[ratid] = p_atom;

        if( Tick % Quantum == 0 ){
            emit OnProgressNotification(Tick,"Total progress %p% - Reading atoms (1/2) ...");
        }

        if( ratid == NumOfAtoms ){
            // no more atoms
            break;
        }

        Tick++;
        ratid++;
        getline( sin, Line );
    }
}

// -------------------------------------------------------------------------

int CMol2ImportJob::GuessZ(const QString& atname,const QString& attype)
{
    // search in registered types
    int z = Mol2Types[attype.toStdString()];
    if( z > 0 ) return(z);

    // search in PeriodicTable
    const CElement* p_ele = PeriodicTable.SearchBySymbol(attype);
    if( p_ele != NULL ) return(p_ele->GetZ());

    // try to guess from atname
    CSmallString symbol;
    if( atname.size() >= 1 ){
        symbol += atname.toStdString()[0];
    }
    if( atname.size() >= 2 ){
        if( isalpha(atname.toStdString()[1]) ){
            symbol += atname.toStdString()[0];
        }
    }

    const CElement* p_ele1 = PeriodicTable.SearchBySymbol(symbol);
    if( p_ele1 != NULL ) return(p_ele1->GetZ());

    // use carbon if nothing was recognize
    return(6);
}

// -------------------------------------------------------------------------

void CMol2ImportJob::ReadBonds(void)
{
    int rbnid = 1;

    getline( sin, Line );
    while( sin ){
        LineNumber++;

        int         boid  = 0;
        int         atid1 = 0;
        int         atid2 = 0;
        string      sorder;

        stringstream str( Line );
        str >> boid >> atid1 >> atid2 >> sorder;

        EBondOrder order = BO_SINGLE;
        CAtom* p_at1 = IndexMap[atid1];
        CAtom* p_at2 = IndexMap[atid2];

        if ( sorder == "1" ) {
            order = BO_SINGLE;
        } else if ( sorder == "2" ) {
            order = BO_DOUBLE;
        } else if ( sorder == "3" ) {
            order = BO_TRIPLE;
        } else if ( sorder == "ar" ) {
            order = BO_DOUBLE_H;
        } else if ( sorder == "am" ) {
            order = BO_DOUBLE_H;
        } else if ( sorder == "du" ) {
            order = BO_SINGLE;
        } else if ( sorder == "nc" ) {
            order = BO_NONE;
        }

        Structure->GetBonds()->CreateBond(p_at1,p_at2,order,History);

        if( Tick % Quantum == 0 ){
            emit OnProgressNotification(Tick,"Total progress %p% - Reading nonds (2/2) ...");
        }
        Tick++;

        if( rbnid == NumOfBonds ){
            // no more bonds
            break;
        }
        rbnid++;

        getline( sin, Line );
    }
}

//------------------------------------------------------------------------------

void CMol2ImportJob::ReadCrystal(void)
{
    getline( sin, Line );
    LineNumber++;

    double a = 0;
    double b = 0;
    double c = 0;
    double alpha = 0;
    double beta = 0;
    double gamma = 0;
    int    h1,h2;

    stringstream str( Line );
    str >> a >> b >> c >> alpha >> beta >> gamma  >> h1 >> h2;
    alpha = alpha * M_PI / 180.0;
    beta = beta * M_PI / 180.0;
    gamma = gamma * M_PI / 180.0;

    if( ! Structure->PBCInfo.IsValid() ){
        Structure->SetBox(true,true,true,CPoint(a,b,c),CPoint(alpha,beta,gamma),History);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
