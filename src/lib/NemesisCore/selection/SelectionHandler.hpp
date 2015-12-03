#ifndef SelectionHandlerH
#define SelectionHandlerH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QString>
#include <GLSelection.hpp>

// -----------------------------------------------------------------------------

enum ESelResult {
    ESR_NONE_SELECTED,              // none object selected
    ESR_SELECTED_OBJECTS_CHANGED,   // list of selected objects changed
    ESR_SELECTED_OBJECTS_END        // selection completed
};

// -----------------------------------------------------------------------------

class CSelectionList;
class CStructure;
class CRestraint;

// -----------------------------------------------------------------------------

/// base class for all selection handlers

class NEMESIS_CORE_PACKAGE CSelectionHandler {
public:
// constructor and destructor -------------------------------------------------
    CSelectionHandler(void);
    virtual ~CSelectionHandler(void);

// handler main method -----------------------------------------------------
    virtual ESelResult RegisterObject(CSelectionList* p_sel,const CSelObject& obj);

// handler description
    virtual const QString GetHandlerDescription(void) const;

// helper methods
    /// return associated structure if it exists
    CStructure* GetStructure(CSelectionList* p_sel,CProObject* p_obj);

    /// return associated restraint if it exists
    CRestraint* GetRestraint(CSelectionList* p_sel,CProObject* p_obj);

    /// test if objects obeys structure object selection mode
    bool TestStrObjectSelMode(CSelectionList* p_sel,CProObject* p_obj);
};

// -----------------------------------------------------------------------------

#endif

