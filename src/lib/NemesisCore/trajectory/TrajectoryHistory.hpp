#ifndef TrajectoryHistoryH
#define TrajectoryHistoryH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <HistoryItem.hpp>
#include <XMLDocument.hpp>
#include <Trajectory.hpp>

//------------------------------------------------------------------------------

class CTrajectory;
class CStructure;

//------------------------------------------------------------------------------

class CTrajectoryHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CTrajectoryHI(CProject* p_object);
    CTrajectoryHI(CTrajectory* p_traj,EHistoryItemDirection change);

// section of private data -----------------------------------------------------
private:
    int             TrajectoryIndex;
    CXMLDocument    TrajectoryData;

// executive methods -----------------------------------------------------------
    /// perform the change in the forward direction
    virtual void Forward(void);

    /// perform the change in the backward direction
    virtual void Backward(void);

// input/output methods --------------------------------------------------------
    /// load data
    virtual void LoadData(CXMLElement* p_ele);

    /// save data
    virtual void SaveData(CXMLElement* p_ele);
};

//------------------------------------------------------------------------------

class CTrajectorySetStructureHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CTrajectorySetStructureHI(CProject* p_object);
    CTrajectorySetStructureHI(CTrajectory* p_traj,CStructure* p_str);

// section of private data -----------------------------------------------------
private:
    int TrajectoryIndex;
    int OldStructureIndex;
    int NewStructureIndex;

// executive methods -----------------------------------------------------------
    /// perform the change in the forward direction
    virtual void Forward(void);

    /// perform the change in the backward direction
    virtual void Backward(void);

// input/output methods --------------------------------------------------------
    /// load data
    virtual void LoadData(CXMLElement* p_ele);

    /// save data
    virtual void SaveData(CXMLElement* p_ele);
};

//------------------------------------------------------------------------------

class CTrajectorySetPlayModeHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CTrajectorySetPlayModeHI(CProject* p_object);
    CTrajectorySetPlayModeHI(CTrajectory* p_traj,ETrajectoryPlayMode mode);

// section of private data -----------------------------------------------------
private:
    int                 TrajectoryIndex;
    ETrajectoryPlayMode OldMode;
    ETrajectoryPlayMode NewMode;

// executive methods -----------------------------------------------------------
    /// perform the change in the forward direction
    virtual void Forward(void);

    /// perform the change in the backward direction
    virtual void Backward(void);

// input/output methods --------------------------------------------------------
    /// load data
    virtual void LoadData(CXMLElement* p_ele);

    /// save data
    virtual void SaveData(CXMLElement* p_ele);
};

//------------------------------------------------------------------------------

class CTrajectorySetPlayTickTimeHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CTrajectorySetPlayTickTimeHI(CProject* p_object);
    CTrajectorySetPlayTickTimeHI(CTrajectory* p_traj,int time);

// section of private data -----------------------------------------------------
private:
    int TrajectoryIndex;
    int OldTime;
    int NewTime;

// executive methods -----------------------------------------------------------
    /// perform the change in the forward direction
    virtual void Forward(void);

    /// perform the change in the backward direction
    virtual void Backward(void);

// input/output methods --------------------------------------------------------
    /// load data
    virtual void LoadData(CXMLElement* p_ele);

    /// save data
    virtual void SaveData(CXMLElement* p_ele);
};

//------------------------------------------------------------------------------

#endif
