#ifndef TrajectoryListH
#define TrajectoryListH
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
#include <Trajectory.hpp>

// -----------------------------------------------------------------------------

class CTrajectory;
class CProject;
class CXMLElement;

// -----------------------------------------------------------------------------

extern CExtUUID NEMESIS_CORE_PACKAGE TrajectoryListID;

// -----------------------------------------------------------------------------

///  declare trajectory list
class NEMESIS_CORE_PACKAGE CTrajectoryList: public CProObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CTrajectoryList(CProject* p_project);
    CTrajectoryList(CProject* p_project,bool no_index);
    ~CTrajectoryList(void);

//------------------------------------------------------------------------------
    /// create new trajectory
    CTrajectory* CreateTrajectoryWH(bool activate,const QString& name = QString());

    /// activate trajectory - trajectory must be owned by the list
    bool SetActiveTrajectoryWH(CTrajectory* p_str);

    /// delete all trajectories
    bool DeleteAllTrajectoriesWH(void);

// executive methods -----------------------------------------------------------
    /// create empty trajectory
    CTrajectory* CreateTrajectory(const QString& name = QString(), CHistoryNode* p_history=NULL);

    /// create trajectory from XML stream
    CTrajectory* CreateTrajectory(CXMLElement* p_ele,CHistoryNode* p_history=NULL);

    /// activate trajectory - trajectory must be owned by the list
    void SetActiveTrajectory(CTrajectory* p_str,CHistoryNode* p_history=NULL);

    /// delete all trajectorys
    void DeleteAllTrajectories(CHistoryNode* p_history=NULL);

    /// clear trajectories
    void ClearTrajectories(void);

// informational methods -------------------------------------------------------
    /// get active structire
    CTrajectory*  GetActiveTrajectory(void) const;

    /// return number of trajectorys
    int GetNumberOfTrajectories(void) const;

// input/output methods --------------------------------------------------------
    /// load all trajectorys
    virtual void LoadData(CXMLElement* p_ele);

    /// save all trajectorys
    virtual void SaveData(CXMLElement* p_ele);

// external notification -------------------------------------------------------
    /// block signals for massive update, only trajectories
    void BeginUpdate(void);

    /// end of massive update
    void EndUpdate(void);

    /// emit OnTrajectoryListChanged signal
    void EmitOnTrajectoryListChanged(void);

    /// emit OnSnapshotChanged signal
    void EmitOnSnapshotChanged(void);

    /// list size changed
    void ListSizeChanged(void);

signals:
    /// emmited when trajectory list is changed
    void OnTrajectoryListChanged(void);

    /// emmitted when snapshot of active trajectory is change
    void OnSnapshotChanged(void);

// section of private data -----------------------------------------------------
private:
    CTrajectory*    ActiveTrajectory;
    bool            Changed;
    int             UpdateLevel;

    friend class CTrajectory;
};

// -----------------------------------------------------------------------------

#endif

