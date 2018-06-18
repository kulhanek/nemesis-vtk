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

#include <Trajectory.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <fstream>
#include <Structure.hpp>
#include <AtomList.hpp>

#include <XYZTrajSegment.hpp>
#include <NemesisCoreModule.hpp>

//------------------------------------------------------------------------------

QObject* XYZTrajSegmentCB(void* p_data);

CExtUUID        XYZTrajSegmentID(
                    "{XYZ_TRAJ_SEGMENT:da7a0e6c-5089-4255-b8d5-e05be341122b}",
                    "XYZ Trajectory");

CPluginObject   XYZTrajSegmentObject(&NemesisCorePlugin,
                    XYZTrajSegmentID,TRAJECTORY_SEGMENT_CAT,
                    ":/images/NemesisCore/trajectory/segments/XYZTrajSegment.svg",
                    XYZTrajSegmentCB);

// -----------------------------------------------------------------------------

QObject* XYZTrajSegmentCB(void* p_data)
{
    CTrajectory* p_traj = static_cast<CTrajectory*>(p_data);
    if( p_traj == NULL ){
        ES_ERROR("CXYZTrajSegment requires active trajectory");
        return(NULL);
    }

    QObject* p_build_wp = new CXYZTrajSegment(p_traj);
    return(p_build_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CXYZTrajSegment::CXYZTrajSegment(CTrajectory* p_traj)
    : CTrajectorySegment(&XYZTrajSegmentObject,p_traj,true)
{
}

//------------------------------------------------------------------------------

CXYZTrajSegment::~CXYZTrajSegment(void)
{
    foreach(CSnapshot* p_snap, Snapshots){
        delete p_snap;
    }
    Snapshots.clear();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

long int CXYZTrajSegment::GetNumberOfSnapshots(void)
{
    return(Snapshots.count());
}

//------------------------------------------------------------------------------

CSnapshot* CXYZTrajSegment::GetCurrentSnapshot(void)
{
    if( (SnapshotIndex < 1) || (SnapshotIndex > Snapshots.count()) ) return(NULL);
    return(Snapshots.at(SnapshotIndex-1));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CXYZTrajSegment::LoadTrajectoryData(void)
{
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
    GetTrajectory()->EmitOnTrajectorySegmentsChanged();
}

//------------------------------------------------------------------------------

bool CXYZTrajSegment::ReadSnapshot(std::istream& sin,CSnapshot* p_snap,int snap)
{
    int natoms = 0;

    // read header
    sin >> natoms;
    if( sin.eof() == true ) return(false); // no data

    // check header
    if( natoms != GetTrajectory()->GetStructure()->GetAtoms()->GetNumberOfAtoms() ){
        CSmallString error;
        error << tr("unable read snapshot") << " '" << snap << "' (inconsistent number of atoms)";
        GetProject()->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
        return(false);
    }

    // read comment
    std::string comment;
    getline(sin,comment); // end of first line winth number of atoms
    getline(sin,comment); // comment

    for(int i=0; i < natoms; i++){
        if( sin.eof() == true ) {
            CSmallString error;
            error << tr("unable read snapshot") << " '" << snap << "' (premature end of snapshot)";
            GetProject()->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
            return(false);
        }
        std::string symbol;
        CPoint pos;
        sin >> symbol >> pos.x >> pos.y >> pos.z;
        p_snap->SetPos(i,pos);
    }

    return( sin.fail() == false );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

