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

#include "TVibTrajSegment.hpp"
#include "TurbomoleModule.hpp"

//------------------------------------------------------------------------------

using namespace std;
using namespace boost;

//------------------------------------------------------------------------------

QObject* TVibTrajSegmentCB(void* p_data);

CExtUUID        TVibTrajSegmentID(
                    "{TVIB_TRAJ_SEGMENT:c102477a-a45c-4270-9768-557b009d64e0}",
                    "Turbomole Vibrations");

CPluginObject   TVibTrajSegmentObject(&TurbomolePlugin,
                    TVibTrajSegmentID,TRAJECTORY_SEGMENT_CAT,
                    ":/images/NemesisCore/trajectory/segments/VibTrajSegment.svg",
                    TVibTrajSegmentCB);

// -----------------------------------------------------------------------------

QObject* TVibTrajSegmentCB(void* p_data)
{
    CTrajectory* p_traj = static_cast<CTrajectory*>(p_data);
    if( p_traj == NULL ){
        ES_ERROR("CTVibTrajSegment requires active trajectory");
        return(NULL);
    }

    QObject* p_build_wp = new CTVibTrajSegment(p_traj);
    return(p_build_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTVibTrajSegment::CTVibTrajSegment(CTrajectory* p_traj)
    : CVibTrajSegment(&TVibTrajSegmentObject,p_traj,true)
{
}

//------------------------------------------------------------------------------

CTVibTrajSegment::~CTVibTrajSegment(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTVibTrajSegment::LoadTrajectoryData(void)
{
    std::ifstream   sin;

    sin.open(FileName.toLatin1());
    if( !sin ) {
        CSmallString error;
        error << tr("unable open trajectory segment") << " '" << FileName << "'";
        GetProject()->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
        return;
    }

    try {
        // read data
        int lineno = 0;
        std::string line;

        while( sin.eof() == false ){
            getline(sin,line);
            lineno++;
            if( line.find("$rundimensions") == 0 ){
                ReadDimmensions(sin,lineno);
            }
            if( NormalModes.count() > 0 ) {
                if( line.find("$vibrational normal modes") == 0 ){
                    ReadModes(sin,lineno);
                }
                if( line.find("$vibrational reduced masses") == 0 ){
                    ReadMasses(sin,lineno);
                }
                if( line.find("$vibrational spectrum") == 0 ){
                    ReadSpectrum(sin,lineno);
                }
            }
        }
    } catch(std::exception& e) {
        GetProject()->TextNotification(ETNT_ERROR,e.what(),ETNT_ERROR_DELAY);
    }

    // notify about changes
    GetTrajectory()->EmitOnTrajectorySegmentsChanged();
}

//------------------------------------------------------------------------------

void CTVibTrajSegment::ReadDimmensions(std::istream& sin,int lineno)
{
    while( sin.eof() == false ){
        string line;
        getline(sin,line);
        lineno++;
        if( line.find("natoms=") != string::npos ){
            // read number of atoms in the system
            vector<string> items;
            split( items, line, is_any_of("=") );
            if( items.size() == 2 ){
                int natoms = 0;
                istringstream str;
                str.str(items[1]);
                str >> natoms;
                // allocate modes
                AllocateModes(natoms);
                return;
            }
        }
    }

    RUNTIME_ERROR("unable to read number of atoms from $rundimensions section");
}

//------------------------------------------------------------------------------

void CTVibTrajSegment::ReadModes(std::istream& sin,int lineno)
{
    int nmodes = GetNumberOfModes();
    int nlines = nmodes / 5;
    if( nmodes % 5 != 0 ) nlines++;

    // read modes
    for(int i = 0; i < nmodes; i++){
        // read lines
        int p = 0;
        for(int j = 0; j < nlines; j++){

            string line;
            getline(sin,line);
            lineno++;

            if( sin.good() == false ){
                CSmallString error;
                error << "unable to read normal mode vector (I) on line " << lineno;
                RUNTIME_ERROR(error);
            }

            istringstream str;
            str.str(line.substr(5,string::npos));

            // parse line
            int max = 5;
            if( j == nmodes / 5 ){
                max = nmodes % 5;
            }
            for(int k=0; k < max; k++){
                if( i % 3 == 0 ){
                    str >> NormalModes[p]->Mode[i/3].x;
                }
                if( i % 3 == 1 ){
                    str >> NormalModes[p]->Mode[i/3].y;
                }
                if( i % 3 == 2 ){
                    str >> NormalModes[p]->Mode[i/3].z;
                }

                if( str.fail() == true ){
                    CSmallString error;
                    error << "unable to read normal mode vector (II) on line " << lineno;
                    RUNTIME_ERROR(error);
                }
                p++;
            }
        }
    }
}

//------------------------------------------------------------------------------

void CTVibTrajSegment::ReadMasses(std::istream& sin,int lineno)
{

}

//------------------------------------------------------------------------------

void CTVibTrajSegment::ReadSpectrum(std::istream& sin,int lineno)
{
    int nmodes = GetNumberOfModes();
    int nlines = nmodes / 5;
    if( nmodes % 5 != 0 ) nlines++;

    // two comment lines
    string line;

    getline(sin,line);
    lineno++;

    getline(sin,line);
    lineno++;

    // read modes
    for(int i = 0; i < nmodes; i++){
        CNormalVibMode* p_data = NormalModes[i];

        string line;
        getline(sin,line);
        lineno++;

        if( sin.good() == false ){
            CSmallString error;
            error << "unable to read vibrational spectrum (I) on line " << lineno;
            RUNTIME_ERROR(error);
        }

        vector<string> items;
        split( items, line, is_any_of(" \t"), token_compress_on );

        istringstream str;
        str.str(line);

        if( items.size() == 7 ){
            int     id = 0;
            string  ir,ram;
            str >> id >> p_data->Symmetry >> p_data->Frequency >> p_data->Intensity >> ir >> ram;
            if( ir == "YES" ) p_data->IsIR = true;
            if( ram == "YES" ) p_data->IsRaman = true;
        } else {
            int     id = 0;
            string  ir,ram;
            str >> id >> p_data->Frequency >> p_data->Intensity >> ir >> ram;
            if( ir == "YES" ) p_data->IsIR = true;
            if( ram == "YES" ) p_data->IsRaman = true;
        }

        if( str.fail() == true ){
            CSmallString error;
            error << "unable to read vibrational spectrum (II) on line " << lineno;
            RUNTIME_ERROR(error);
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

