#ifndef SnapshotFilterH
#define SnapshotFilterH
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

// -----------------------------------------------------------------------------

///  base class for snapshot filters

class NEMESIS_CORE_PACKAGE CSnapshotFilter : public CProObject {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    CSnapshotFilter(CPluginObject* p_po,CTrajectory* p_traj,bool no_index);
    virtual ~CSnapshotFilter(void);

    //! remove structure from base list
    void RemoveFromBaseList(CHistoryNode* p_history=NULL);

// methods with changes registered into history list ---------------------------
    /// delete filter from the trajectory
    bool DeleteWH(void);

    /// move up
    bool MoveUpWH(void);

    /// move up
    bool MoveDownWH(void);

    /// set seq index
    bool SetSeqIndexWH(int index);

// executive methods without change registered to history list -----------------
    /// set sequential index
    void SetSeqIndex(int index,CHistoryNode* p_history=NULL);

// information methods ---------------------------------------------------------
    /// get base list
    CTrajectory* GetTrajectory(void) const;

    /// is first filter
    bool IsFirstFilter(void);

    /// is last filter
    bool IsLastFilter(void);

    /// get seq index
    int GetSeqIndex(void);

    /// get previous filter
    CSnapshotFilter* GetPrevFilter(void);

    /// get next filter
    CSnapshotFilter* GetNextFilter(void);

// input/output methods --------------------------------------------------------
    /// load all atoms
    virtual void LoadData(CXMLElement* p_el);

    /// save all atoms
    virtual void SaveData(CXMLElement* p_ele);

// section of private data -----------------------------------------------------
protected:
    int     SeqIndex;       // sequential index

    friend class CTrajectory;
};

// -----------------------------------------------------------------------------

#endif

