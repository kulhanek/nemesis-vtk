#ifndef VibTrajSegmentH
#define VibTrajSegmentH
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

#include <NemesisCoreMainHeader.hpp>
#include <TrajectorySegment.hpp>
#include <QVector>
#include <SimpleVector.hpp>

// -----------------------------------------------------------------------------

class CTrajectory;
class CSnapshot;
class CNormalVibMode;

// -----------------------------------------------------------------------------

///  common segment definition for visualization of structure normal vibrations

class NEMESIS_CORE_PACKAGE CVibTrajSegment : public CTrajectorySegment {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    CVibTrajSegment(CPluginObject* p_po,CTrajectory* p_traj,bool no_index);
    ~CVibTrajSegment(void);

// information methods ---------------------------------------------------------
    /// get number of snaphots in the file
    virtual long int GetNumberOfSnapshots(void);

    /// get current segment
    virtual CSnapshot* GetCurrentSnapshot(void);

    /// set amplitude
    void SetAmplitude(double amplitude);

    /// get amplitude
    double GetAmplitude(void);

    /// get number of modes
    int GetNumberOfModes(void);

// section of private data -----------------------------------------------------
protected:
    CSnapshot*                  Snapshot;
    QVector<CNormalVibMode*>    NormalModes;
    CSimpleVector<CPoint>       Coordinates;
    int                         NumberOfModes;
    int                         NumberOfSnapshots;
    double                      ModeAmplitude;

    /// actualize snapshot
    void SnapshotChanged(void);

    /// allocate modes
    void AllocateModes(int natoms);

    /// update number of modes
    void UpdateNumberOfModes(int nmodes);

    friend class CVibTrajSegmentDesigner;
};

// -----------------------------------------------------------------------------

#endif

