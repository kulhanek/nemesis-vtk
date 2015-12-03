#ifndef HistoryListH
#define HistoryListH
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
#include <HistoryNode.hpp>
#include <ProObject.hpp>
#include <IndexCounter.hpp>

//------------------------------------------------------------------------------

enum EHistoryChangeMessage {
    EHCM_UNDO,          // undo executed
    EHCM_REDO,          // redo executed
    EHCM_BUFFER,        // content of history buffer changed
    EHCM_LOCK_LEVEL     // lock level was changed
};

//------------------------------------------------------------------------------

/// implementation of history

class NEMESIS_CORE_PACKAGE CHistoryList :  public CProObject {
    Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CHistoryList(CProObject* p_parent,bool no_index);
    ~CHistoryList(void);

// setup methods --------------------------------------------------------------
    /// setup the max number of registered changes
    void SetDepthOfBuffer(int depth);

    /// set lock levelw
    void SetLockModeLevels(const CLockLevels& levels);

    /// set minimum lock levelw
    void SetMinLockModeLevels(const CLockLevels& levels);

// informational methods ------------------------------------------------------
    /// does undo buffer contain any data?
    bool IsUndoActive(void);

    /// does redo buffer contain any data?
    bool IsRedoActive(void);

    /// how many undo is available?
    int  GetNumberOfUndoAvailable(void);

    /// how many redo is available?
    int  GetNumberOfRedoAvailable(void);

    /// how many undo is really available?
    int  GetMaxNumberOfUndoAvailable(void);

    /// how many redo is really available?
    int  GetMaxNumberOfRedoAvailable(void);

    /// is this subsystem locked
    bool IsLocked(EHistoryChangeLevel level);

    /// return lock levels
    const CLockLevels&  GetLockModeLevels(void) const;

    /// return minimum lock levels
    const CLockLevels&  GetMinLockModeLevels(void) const;

    /// return undo description
    QString GetUndoDescr(int i,bool short_ver=true);

    /// return redo description
    QString GetRedoDescr(int i,bool short_ver=true);

// executive methods ----------------------------------------------------------
    /// begin change recording
    bool BeginChange(EHistoryChangeLevel lockmodelevel);

    /// record change
    void RegisterChange(CHistoryNode* p_history);

    /// close recording
    void EndChange(void);

    /// execute undo
    bool Undo(int i=0);

    /// execute redo
    bool Redo(int i=0);

    /// remove all history nodes
    void ClearHistory(void);

// input/output methods --------------------------------------------------------
    /// load action and execute it
    void LoadActionAndExecute(CXMLElement* p_ele);

    /// save last action into XML stream
    void SaveLastAction(CXMLElement* p_ele);

// signals ---------------------------------------------------------------------
signals:
    void OnHistoryChanged(EHistoryChangeMessage message);

// section of private data -----------------------------------------------------
private:
    CLockLevels                 MinLockModeLevels;  // minimum allowed level of lock
    CLockLevels                 LockModeLevels;     // level of lock
    EHistoryChangeLevel         CurrentChangeLevel;
    bool                        RedoStatus;         // is redo allowed
    int                         NumOfRedo;          // number of redo changes
    int                         NumOfMaxChanges;    // max number of allowed changes
    CSimpleList<CHistoryNode>   RegHistories;       // list of changes in BeginChange/EndChange

// debug subsystem -------------------------------
    bool                    EnableDebug;
    int                     ProjectID;
    int                     ActionID;
    static CIndexCounter    ProjectCounter;
    CIndexCounter           ActionCounter;

    /// write last action during debug mode
    void WriteDebugData(void);
};

//------------------------------------------------------------------------------

#endif
