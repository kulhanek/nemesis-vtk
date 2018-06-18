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

#include <fstream>

#include <NemesisCoreModule.hpp>

#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>

#include <AtomList.hpp>
#include <Structure.hpp>
#include <Trajectory.hpp>

#include <PDBQTTrajSegment.hpp>

//------------------------------------------------------------------------------

QObject* PDBQTTrajSegmentCB(void* p_data);

CExtUUID        PDBQTTrajSegmentID(
                    "{PDBQT_TRAJ_SEGMENT:7710e8c7-39c8-436c-a822-4e4871d3edbc}",
                    "PDBQT Trajectory");

CPluginObject   PDBQTTrajSegmentObject(&NemesisCorePlugin,
                    PDBQTTrajSegmentID,TRAJECTORY_SEGMENT_CAT,
                    ":/images/NemesisCore/trajectory/segments/PDBQTTrajSegment.svg",
                    PDBQTTrajSegmentCB);

// -----------------------------------------------------------------------------

QObject* PDBQTTrajSegmentCB(void* p_data)
{
    CTrajectory* p_traj = static_cast<CTrajectory*>(p_data);
    if( p_traj == NULL ){
        ES_ERROR("CPDBQTTrajSegment requires active trajectory");
        return(NULL);
    }

    QObject* p_build_wp = new CPDBQTTrajSegment(p_traj);
    return(p_build_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPDBQTTrajSegment::CPDBQTTrajSegment(CTrajectory* p_traj)
    : CTrajectorySegment(&PDBQTTrajSegmentObject,p_traj,true)
{

}

//------------------------------------------------------------------------------

CPDBQTTrajSegment::~CPDBQTTrajSegment(void)
{
    // tidy up
    foreach(CSnapshot* p_snap, Snapshots){
        delete p_snap;
    }

    Snapshots.clear();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

long int CPDBQTTrajSegment::GetNumberOfSnapshots(void)
{
    // get snapshots count in pdbqt segment
    return(Snapshots.count());
}

//------------------------------------------------------------------------------

bool CPDBQTTrajSegment::IsSnapshotActive (CSnapshot* p_snap)
{
    // determine if snapshot is current and active
    CSnapshot* p_curr_snap = GetCurrentSnapshot();
    if (p_curr_snap == NULL) {
        return (false);
    }
    int curr_indx = int(p_curr_snap->GetProperty(0) );
    int snap_indx = int(p_snap->GetProperty(0) );
    if (curr_indx == snap_indx) {
        return (true);
    } else {
        return (false);
    }
}

//------------------------------------------------------------------------------

CSnapshot* CPDBQTTrajSegment::GetCurrentSnapshot(void)
{
    // get current snapshot in pdbqt segment
    if( (SnapshotIndex < 1) || (SnapshotIndex > Snapshots.count()) ) return(NULL);
    // Snapshots are in QList from 0
    return(Snapshots.at(SnapshotIndex-1));
}

//------------------------------------------------------------------------------

long int CPDBQTTrajSegment::GetSnapshotIndex(CSnapshot* p_snap)
{
    // determine snapshot index in pdbqt segment
    return(Snapshots.indexOf(p_snap));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPDBQTTrajSegment::LoadTrajectoryData(void)
{
    // load pdbqt results file from Autodock vina
    std::ifstream   sin;

    sin.open(FileName.toLatin1());
    if( !sin ) {
        CSmallString error;
        error << tr("unable open trajectory segment") << " '" << FileName << "'";
        GetProject()->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
        return;
    }

    // read snapshots
    int i = 1;
    while( sin.eof() == false ){
        CSnapshot* p_snap = new CSnapshot(this);
        p_snap->InitSnapshot();
        if( ReadSnapshot(sin,p_snap,i) == true ) {
            Snapshots.append(p_snap);
        } else {
            delete p_snap;
            break;
        }
        i++;
    }

    // notify about changes
    MoveToSnapshot(1);
    GetTrajectory()->EmitOnTrajectorySegmentsChanged();
    sin.close();
    return;
}

//------------------------------------------------------------------------------

bool CPDBQTTrajSegment::ReadSnapshot(std::istream& sin,CSnapshot* p_snap,int snap)
{
    int natoms = 0;
    int nmodel = 0;
    // Score in kcal/mol
    double vina_score = 0.0;
    //rmsd/lb (RMSD lower bound) and rmsd/ub (RMSD upper bound)
    double rmsd_lb, rmsd_ub;

    std::string model, get_remark, get_vina, get_result;
    // read header
    sin >> model >> nmodel;
    if( sin.eof() == true ) return(false); // no data

    // read info about pose
    sin >> get_remark >> get_vina >> get_result >> vina_score >> rmsd_lb >> rmsd_ub;
    p_snap->SetProperty(0,nmodel);
    p_snap->SetProperty(1,vina_score);
    p_snap->SetProperty(2,rmsd_lb);
    p_snap->SetProperty(3,rmsd_ub);
    // read end of line from parsed line
    std::string end_of_line;
    getline(sin,end_of_line);

    // read model data
    std::string line;
    while ((getline(sin,line).good()))
    {
        if (line.length() == 0)
        {
            // skip the empty line
            continue;
        }
        if (line.length() < 3)
        {
            CSmallString error;
            error << tr("ERROR: not a valid PDBQT file") << " '" << FileName << "'";
            GetProject()->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
            return (false);
        }

        if (EQn(line.c_str(),"REMARK",6)) {continue;}

        if (EQn(line.c_str(),"ROOT",4)) {continue;}
        if (EQn(line.c_str(),"ENDROOT",7)) {continue;}
        if (EQn(line.c_str(),"BRANCH",6)) {continue;}
        if (EQn(line.c_str(),"ENDBRANCH",9)) {continue;}
        if (EQn(line.c_str(),"TORSDOF",7)) {continue;}

        if (EQn(line.c_str(),"ENDMDL",6)) {break;}

        if (EQn(line.c_str(),"ATOM",4) || EQn(line.c_str(),"HETATM",6))
        {
            std::stringstream sstr(line);
            int order, nres;
            std::string label,symbol, res;
            // read n atom properties
            CPoint pos;
            sstr >> label >> order >> symbol >> res >> nres >> pos.x >> pos.y >> pos.z;
            p_snap->SetPos(natoms,pos);
        }
        natoms++;
    }

    if (natoms != GetTrajectory()->GetStructure()->GetAtoms()->GetNumberOfAtoms())
    {
        CSmallString error;
        error << tr("unable read snapshot") << " '" << snap << "' (inconsistent number of atoms)";
        GetProject()->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
        return(false);
    }
    return( sin.fail() == false );

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

