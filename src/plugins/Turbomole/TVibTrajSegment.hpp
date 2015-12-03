#ifndef TVibTrajSegmentH
#define TVibTrajSegmentH
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

#include <VibTrajSegment.hpp>

// -----------------------------------------------------------------------------

class CTrajectory;

// -----------------------------------------------------------------------------

///  turbomole vibration trajectory segment

class CTVibTrajSegment : public CVibTrajSegment {
public:
// constructor -----------------------------------------------------------------
    CTVibTrajSegment(CTrajectory* p_traj);
    ~CTVibTrajSegment(void);

    /// load trajectory data
    virtual void LoadTrajectoryData(void);

// section of private data -----------------------------------------------------
private:
    void ReadDimmensions(std::istream& sin,int lineno);
    void ReadModes(std::istream& sin,int lineno);
    void ReadMasses(std::istream& sin,int lineno);
    void ReadSpectrum(std::istream& sin,int lineno);
};

// -----------------------------------------------------------------------------

#endif

