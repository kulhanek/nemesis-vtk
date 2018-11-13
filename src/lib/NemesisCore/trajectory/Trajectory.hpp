#ifndef TrajectoryH
#define TrajectoryH
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
#include <IndexCounter.hpp>
#include <QTimer>

// -----------------------------------------------------------------------------

class CTrajectoryList;
class CTrajectorySegment;
class CSnapshot;
class CSnapshotFilter;
class CStructure;

// -----------------------------------------------------------------------------

/// trajectory play mode

enum ETrajectoryPlayMode {
    ETPM_ONCE,
    ETPM_LOOP,
    ETPM_ROLL
};

/// trajectory play status

enum ETrajectoryPlayStatus {
    ETPS_STOP,
    ETPS_PAUSE,
    ETPS_PLAY
};

    // user flags               = 0x00010000    // first user flag
#define EPOF_TRAJ_REBUILD_BONDS   0x00010000

// -----------------------------------------------------------------------------

///  base class for trajectory based data

class NEMESIS_CORE_PACKAGE CTrajectory : public CProObject {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    CTrajectory(CTrajectoryList* p_list);
    CTrajectory(CTrajectoryList* p_list,bool no_index);
    virtual ~CTrajectory(void);

    //! remove structure from base list
    void RemoveFromBaseList(CHistoryNode* p_history=NULL);

// methods with changes registered into history list ---------------------------
    /// set structure
    bool SetStructureWH(CStructure* p_str);

    /// delete trajectory from the trajectory list
    bool DeleteWH(void);

    /// delete all trajectory segments
    bool DeleteAllSegmentsWH(void);

    /// delete all snapshot filters
    bool DeleteAllFiltersWH(void);

    /// create new filter
    CSnapshotFilter* CreateFilterWH(const CUUID& objectuuid,const QString& name = QString(),
                                    const QString& descr = QString());

    /// set play mode
    bool SetPlayModeWH(ETrajectoryPlayMode mode);

    /// get play tick time in ms
    bool SetPlayTickTimeWH(int time);

// executive methods without change registered to history list -----------------
    /// first snapshot
    bool FirstSnapshot(void);

    /// next snapshot
    bool NextSnapshot(void);

    /// previous snapshots
    bool PrevSnapshot(void);

    /// last snapshot
    bool LastSnapshot(void);

    /// next segment
    bool NextSegment(void);

    /// previous segment
    bool PrevSegment(void);

    /// move to last segment
    bool LastSegment(void);

    /// move to given segment
    bool MoveToSegment(long int index);

    /// move to given snapshot
    bool MoveToSnapshot(long int index);

//------------------------------------------------------------------------------
    /// play trajectory, tick is in ms
    void Play(void);

    /// pause trajectory
    void Pause(void);

    /// stop trajectory
    void Stop(void);

    /// is playing?
    bool IsPlaying(void);

    /// is puased?
    bool IsPaused(void);

    /// get play mode
    ETrajectoryPlayMode GetPlayMode(void);

    /// get play tick time in ms
    int GetPlayTickTime(void);

//------------------------------------------------------------------------------
    /// is first snapshot
    bool IsFirstSnapshot(void);

    /// is last snapshot
    bool IsLastSnapshot(void);

    /// is previous snapshot
    bool IsPrevSnapshot(void);

    /// is next snapshot
    bool IsNextSnapshot(void);

    /// is next segment
    bool IsPrevSegment(void);

    /// is next segment
    bool IsNextSegment(void);

// information methods ---------------------------------------------------------
    /// get base list
    CTrajectoryList* GetTrajectories(void) const;

    /// get associated structure
    CStructure* GetStructure(void);

    /// get number of all snaphots
    long int GetNumberOfSnapshots(void);

    /// get number of snaphots before segment with index
    long int GetNumberOfSnapshotsBefore(long int index);

    /// get number of segments
    long int GetNumberOfSegments(void);

    /// get current snapshot index
    long int GetCurrentSnapshotIndex(void);

    /// get current segment index
    long int GetCurrentSegmentIndex(void);

    /// get number of filters
    long int GetNumberOfFilters(void);

    /// get current snapshot
    CSnapshot* GetCurrentSnapshot(void);

    /// is segment active
    bool IsSegmentActive(CTrajectorySegment* p_segment);

// executive methods -----------------------------------------------------------
    /// set structure
    void SetStructure(CStructure* p_str,CHistoryNode* p_history=NULL);

    /// delete all segments
    void DeleteAllSegments(CHistoryNode* p_history=NULL);

    /// delete all snapshot filters
    void DeleteAllFilters(CHistoryNode* p_history=NULL);

    /// create segment by CXMLElement
    CTrajectorySegment* CreateSegment(CXMLElement* p_ele,CHistoryNode* p_history=NULL);

    /// register segment
    void RegisterSegment(CTrajectorySegment* p_seg,CHistoryNode* p_history=NULL);

    /// create new filter
    CSnapshotFilter* CreateFilter(const CUUID& objectuuid,const QString& name = QString(),
                             const QString& descr = QString(),CHistoryNode* p_history=NULL);

    /// create filter by CXMLElement
    CSnapshotFilter* CreateFilter(CXMLElement* p_ele,CHistoryNode* p_history=NULL);

    /// set play mode
    void SetPlayMode(ETrajectoryPlayMode mode,CHistoryNode* p_history=NULL);

    /// get play tick time in ms
    void SetPlayTickTime(int time,CHistoryNode* p_history=NULL);

// input/output methods --------------------------------------------------------
    /// load all atoms
    virtual void LoadData(CXMLElement* p_ele);

    /// save all atoms
    virtual void SaveData(CXMLElement* p_ele);

// external notification -------------------------------------------------------
    /// set name
    virtual void SetName(const QString& name,CHistoryNode* p_history=NULL);

    /// set description
    virtual void SetDescription(const QString& descrip,CHistoryNode* p_history=NULL);

    /// block signals for massive update, only trajectory
    void BeginUpdate(void);

    /// end of massive update
    void EndUpdate(void);

    /// emit OnSnapshotChanged signal
    void EmitOnSnapshotChanged(void);

    /// emit OnTrajectorySegmentsChanged signal
    void EmitOnTrajectorySegmentsChanged(void);

    /// emit OnSnaphsotFiltersChanged signal
    void EmitOnSnapshotFiltersChanged(void);

signals:
    /// on snapshot updated
    void OnSnapshotChanged(void);

    /// emmited when trajectory segment list is changed
    void OnTrajectorySegmentsChanged(void);

    /// emmited when snapshot filter list is changed
    void OnSnapshotFiltersChanged(void);

// section of private data -----------------------------------------------------
private:   
    bool                SegmentsChanged;
    bool                FiltersChanged;
    int                 UpdateLevel;

    CStructure*         Structure;
    long int            CurrentSegmentIndex;
    long int            CurrentSnapshotIndex;

    // playback
    ETrajectoryPlayMode     PlayMode;
    ETrajectoryPlayStatus   PlayStatus;
    int                     PlayTickTime;
    bool                    PlayForward;
    QTimer*                 PlayTimer;

    /// lists - ordered by seqindex
    QList<CTrajectorySegment*>  Segments;
    CIndexCounter               SegmentCounter;

    friend class CTrajectorySegment;
    friend class CTrajectoryModelSegments;

    /// sort segments
    void SortSegments(void);
    static bool LessThanBySeqIndexSegment(CTrajectorySegment* p_left,CTrajectorySegment* p_right);
    void SortSegmentsByID(void);
    static bool LessThanByIdIndexSegment(CTrajectorySegment* p_left,CTrajectorySegment* p_right);

    QList<CSnapshotFilter*>     Filters;
    CIndexCounter               FilterCounter;

    friend class CSnapshotFilter;
    friend class CTrajectoryModelFilters;

    /// sort filters
    void SortFilters(void);
    static bool LessThanBySeqIndexFilter(CSnapshotFilter* p_left,CSnapshotFilter* p_right);

    /// return segment pointer by index - index is counted from 1
    CTrajectorySegment* GetSegment(long int segid);

private slots:
    void PlayTick(void);
};

// -----------------------------------------------------------------------------

#endif

