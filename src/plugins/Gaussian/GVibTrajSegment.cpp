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

#include <Trajectory.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <fstream>
#include <Structure.hpp>
#include <AtomList.hpp>
#include <string>
#include <sstream>
#include <NormalVibMode.hpp>
#include <boost/algorithm/string.hpp>

#include "GVibTrajSegment.hpp"
#include "GaussianModule.hpp"

//------------------------------------------------------------------------------

using namespace std;
using namespace boost;

//------------------------------------------------------------------------------

QObject* GVibTrajSegmentCB(void* p_data);

CExtUUID        GVibTrajSegmentID(
                    "{GVIB_TRAJ_SEGMENT:4e92c43e-b74c-4bda-a80d-804a561b6f03}",
                    "Gaussian Vibrations");

CPluginObject   GVibTrajSegmentObject(&GaussianPlugin,
                    GVibTrajSegmentID,TRAJECTORY_SEGMENT_CAT,
                    ":/images/NemesisCore/trajectory/segments/VibTrajSegment.svg",
                    GVibTrajSegmentCB);

// -----------------------------------------------------------------------------

QObject* GVibTrajSegmentCB(void* p_data)
{
    CTrajectory* p_traj = static_cast<CTrajectory*>(p_data);
    if( p_traj == NULL ){
        ES_ERROR("CGVibTrajSegment requires active trajectory");
        return(NULL);
    }

    QObject* p_build_wp = new CGVibTrajSegment(p_traj);
    return(p_build_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGVibTrajSegment::CGVibTrajSegment(CTrajectory* p_traj)
    : CVibTrajSegment(&GVibTrajSegmentObject,p_traj,true)
{
}

//------------------------------------------------------------------------------

CGVibTrajSegment::~CGVibTrajSegment(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGVibTrajSegment::LoadTrajectoryData(void)
{
    std::ifstream   sin;

    sin.open(FileName.toLatin1());
    if( !sin ) {
        CSmallString error;
        error << tr("unable open trajectory segment") << " '" << FileName << "'";
        GetProject()->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
        ES_ERROR(error);
        return;
    }

    // allocate all vectors
    AllocateModes(GetNumberOfAtoms());

    try {
        // read data
        int         lineno = 0;
        std::string line;

        // find beginning
        for(;;){
            getline(sin,line);
            if( ! sin ){
                CSmallString error;
                error << tr("premature end of file");
                GetProject()->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
                ES_ERROR(error);
                return;
            }
            lineno++;
            if( line.find(" Harmonic frequencies (cm**-1)") == 0 ) break;
        }

        // skip header
        getline(sin,line);
        lineno++;
        getline(sin,line);
        lineno++;
        getline(sin,line);
        lineno++;

        int curr_block = 0;
        for(;;) {
            // how many vectors?
            getline(sin,line);
            lineno++;
            std::stringstream str(line);
            int nvecs = 0;
            for(int i=0; i < 3; i++){
                int buffer = 0;
                str >> buffer;
                if( str && (buffer > 0) ) nvecs++;
            }
            if( nvecs == 0 ) break; // no vectors - end

            for(;;){
                getline(sin,line);
                if( ! sin ){
                    CSmallString error;
                    error << tr("premature end of file");
                    GetProject()->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
                    ES_ERROR(error);
                    return;
                }
                lineno++;
                if( line.find(" Frequencies --") == 0 ){
                    LoadFrequencies(curr_block,nvecs,line);
                }
                if( line.find("  Atom  AN      X") == 0 ){
                    LoadModes(curr_block,nvecs,sin,lineno);
                    curr_block += nvecs;
                    break;
                }
            }

        }
        // update number of real normal modes
        UpdateNumberOfModes(curr_block);

    } catch(std::exception& e) {
        GetProject()->TextNotification(ETNT_ERROR,e.what(),ETNT_ERROR_DELAY);
    }

    // notify about changes
    GetTrajectory()->EmitOnTrajectorySegmentsChanged();
}

//------------------------------------------------------------------------------

void CGVibTrajSegment::LoadFrequencies(int curr_block,int nvecs,const std::string& line)
{
    std::stringstream str(line);
    std::string buffer;
    str >> buffer >> buffer;
    for(int i=0; i < nvecs; i++){
        int vec = curr_block + i;
        if( (vec >= 0) && (vec < GetNumberOfModes()) ) {
            CNormalVibMode* p_mode = NormalModes[vec];
            str >> p_mode->Frequency;
        }
    }
}

//------------------------------------------------------------------------------

void CGVibTrajSegment::LoadModes(int curr_block,int nvecs,std::istream& sin,int& lineno)
{
    for(int atom = 0; atom < GetNumberOfAtoms(); atom++){
        std::string line;
        getline(sin,line);
        lineno++;
        std::stringstream str(line);
        int buffer;
        str >> buffer >> buffer;
        for(int i=0; i < nvecs; i++){
            int vec = curr_block + i;
            if( (vec >= 0) && (vec < GetNumberOfModes()) ) {
                CNormalVibMode* p_mode = NormalModes[vec];
                CPoint mode;
                str >> mode.x >> mode.y >> mode.z;
                p_mode->Mode[atom] = mode;
            }
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

