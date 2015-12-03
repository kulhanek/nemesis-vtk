#ifndef HistoryNodeH
#define HistoryNodeH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QFlags>

//------------------------------------------------------------------------------

// for history level

enum EHistoryChangeLevel {
    EHCL_NONE           = 0x00000000,
    EHCL_CORE_NODE      = 0x10000000,
    // user available levels
    EHCL_STRUCTURES     = 0x00000001,
    EHCL_TOPOLOGY       = 0x00000002,
    EHCL_GEOMETRY       = 0x00000004,
    EHCL_GRAPHICS       = 0x00000008,
    EHCL_DESCRIPTION    = 0x00000010,
    EHCL_PROPERTY       = 0x00000020,
    EHCL_TRAJECTORIES   = 0x00000040,
    // subsystem levels
    EHCL_COMPOSITE      = 0x00010000,
    EHCL_HISTORY        = 0x00020000,
    // all levels
    EHCL_ALL            = 0x000FFFFF
};

//------------------------------------------------------------------------------

typedef QFlags<EHistoryChangeLevel> CLockLevels;

//------------------------------------------------------------------------------

/// node for elementary changes (it can contain several items)

class NEMESIS_CORE_PACKAGE CHistoryNode : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CHistoryNode(CProject* p_project);
    CHistoryNode(CProject* p_project,const QString& short_descr,const QString& long_descr);
    virtual ~CHistoryNode(void);

// executive methods -----------------------------------------------------------
    /// set short description of change
    void SetShortDescription(const QString& desc);

    /// set long description of change
    void SetLongDescription(const QString& desc);

    /// register item
    void Register(CHistoryItem* p_data);

    /// is change allowed - deep tree check
    bool IsChangeAllowed(const CLockLevels& locks);

    /// reverse the change direction
    virtual void ReverseDirection(void);

// information methods ---------------------------------------------------------
    /// get current change level
    EHistoryChangeLevel GetChangeLevel(void);

    /// set change level
    void SetChangeLevel(EHistoryChangeLevel level);

    /// get short description of change
    const QString& GetShortDescription(void) const;

    /// get long description of change
    const QString& GetLongDescription(void) const;

// section of private data -----------------------------------------------------
private:
    EHistoryChangeLevel ChangeLevel;
    QString             ShortDescription;
    QString             LongDescription;

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

    friend class CHistoryList;
};

//------------------------------------------------------------------------------

#endif
