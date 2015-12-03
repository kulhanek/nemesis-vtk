#ifndef TrajectorySegmentH
#define TrajectorySegmentH
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
#include <ProObject.hpp>

// -----------------------------------------------------------------------------

class CTrajectory;
class CSnapshot;
class CStructure;

// -----------------------------------------------------------------------------

///  base class for trajectory file segment

class NEMESIS_CORE_PACKAGE CTrajectorySegment : public CProObject {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    CTrajectorySegment(CPluginObject* p_po,CTrajectory* p_traj,bool no_index);
    virtual ~CTrajectorySegment(void);

    //! remove structure from base list
    void RemoveFromBaseList(CHistoryNode* p_history=NULL);

// methods with changes registered into history list ---------------------------
    /// delete segment from the trajectory
    bool DeleteWH(void);

    /// move up
    bool MoveUpWH(void);

    /// move up
    bool MoveDownWH(void);

    /// set seq in the first place
    bool MoveInTheFirstWH(void);

    /// set seq in the first place but dont set history checkpoint
    bool MoveInTheFirst(void);

    /// set seq index
    bool SetSeqIndexWH(int index);

// executive methods without change registered to history list -----------------
    /// set sequential index
    void SetSeqIndex(int index,bool signal,CHistoryNode* p_history=NULL);

    /// set file name
    virtual void SetFileName(const QString& name);

    /// first snapshot
    virtual bool FirstSnapshot(void);

    /// next snapshot
    virtual bool NextSnapshot(void);

    /// previous snapshots
    virtual bool PrevSnapshot(void);

    /// last snapshot
    virtual bool LastSnapshot(void);

    /// last snapshot
    virtual bool MoveToSnapshot(long int index);

// information methods ---------------------------------------------------------
    /// get base list
    CTrajectory* GetTrajectory(void) const;

    /// get associtaed structure
    CStructure* GetStructure(void) const;

    /// return number of atoms associated with the trajectory
    int GetNumberOfAtoms(void) const;

    /// get number of snaphots in the file
    virtual long int GetNumberOfSnapshots(void);

    /// get file name including path
    const QString& GetFileName(void) const;

    /// get seq index
    int GetSeqIndex(void);

    /// is first segment
    bool IsFirstSegment(void);

    /// is last segment
    bool IsLastSegment(void);

    /// get previous segment
    CTrajectorySegment* GetPrevSegment(void);

    /// get next segment
    CTrajectorySegment* GetNextSegment(void);

    /// get current snapshot
    virtual CSnapshot* GetCurrentSnapshot(void);

    /// get current snapshot index
    virtual long int GetCurrentSnapshotIndex(void);

    /// snapshot index was changed
    virtual void SnapshotChanged(void);

    /// emit snapshot changed signal on Trajectory
    void EmitOnSnapshotChanged(void);

// external notification -------------------------------------------------------
    /// set name
    virtual void SetName(const QString& name,CHistoryNode* p_history=NULL);

    /// set description
    virtual void SetDescription(const QString& descrip,CHistoryNode* p_history=NULL);

// input/output methods --------------------------------------------------------
    /// load all atoms
    virtual void LoadData(CXMLElement* p_el);

    /// save all atoms
    virtual void SaveData(CXMLElement* p_ele);

    /// load trajectory data
    virtual void LoadTrajectoryData(void);

// section of private data -----------------------------------------------------
protected:
    int         SeqIndex;       // sequential index
    QString     FileName;       // file name
    long int    SnapshotIndex;  // snapshot index within the segment

    friend class CTrajectory;
};

// -----------------------------------------------------------------------------

#endif

