#ifndef InserterListH
#define InserterListH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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
#include <ExtComObject.hpp>
#include <Point.hpp>

//------------------------------------------------------------------------------

class CStructure;
class CFragment;
class CAtom;
class CInserter;

//------------------------------------------------------------------------------

/// keep information about inserters

class NEMESIS_CORE_PACKAGE CInserterList : public CExtComObject {
Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CInserterList(CExtComObject* p_parent);
    ~CInserterList(void);

// setup methods --------------------------------------------------------------
    /// init list of all inserters
    bool InsertAllInserters(void);

    /// load active profile setup
    bool LoadConfig(void);

    /// load active profile setup
    bool SaveConfig(void);

// executive methods ----------------------------------------------------------
    /// insert new structure
    bool InsertStructureWH(CFragment* p_str,CAtom* p_atom);

    /// insert new structure
    bool InsertStructureWH(CFragment* p_str,CStructure* p_mol,const CPoint& pos = CPoint());  

// informational methods ------------------------------------------------------
    /// get active inserter
    CInserter* GetActiveInserter(void);

    /// is auto centering on?
    bool IsAutoCenterEnabled(void);

    /// is new structire allowed
    bool IsNewStructureAllowed(void);

// setup method ---------------------------------------------------------------
public slots:
    /// set auto center option
    void SetAutoCenter(bool set);

    /// set auto center option
    void SetNewStructureAllowed(bool set);

// list of signals ------------------------------------------------------------
signals:
    /// emmited when general setup is changed
    void SetupChanged(void);

// section of private data ----------------------------------------------------
private:
    CInserter*              ActiveInserter;
    bool                    AutoCenter;
    bool                    AllowNewStructure;
};

//------------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE CInserterList*  Inserters;

//------------------------------------------------------------------------------

#endif
