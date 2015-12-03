#ifndef SnapshotH
#define SnapshotH
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

#include <NemesisCoreMainHeader.hpp>
#include <Point.hpp>
#include <SimpleVector.hpp>
#include <QMap>

// -----------------------------------------------------------------------------

class CTrajectory;
class CTrajectorySegment;

// -----------------------------------------------------------------------------

///  trajectory snapshot

class NEMESIS_CORE_PACKAGE CSnapshot {
public:
// constructor -----------------------------------------------------------------
    CSnapshot(CTrajectorySegment* p_seg);

    /// init snapshot
    void InitSnapshot(bool include_vel=false);

// input/output methods --------------------------------------------------------
    /// get atom position
    const CPoint& GetPos(int seqindex);

    /// get atom velocity
    const CPoint& GetVel(int seqindex);

    /// get property
    double GetProperty(int key);

    /// set atom position
    void SetPos(int seqindex,const CPoint& pos);

    /// get atom velocity
    void SetVel(int seqindex,const CPoint& vel);

    /// get property
    void SetProperty(int key,double value);

public:
    CSimpleVector<CPoint>   Coordinates;
    CSimpleVector<CPoint>   Velocities;         // optional velocities
    QMap<int,double>        ScalarProperties;

// section of private data -----------------------------------------------------
private:
    CTrajectory*            Trajectory;
};

// -----------------------------------------------------------------------------

#endif

