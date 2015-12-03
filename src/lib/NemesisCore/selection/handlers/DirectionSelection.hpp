#ifndef DirectionSelectionH
#define DirectionSelectionH
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

#include <SelectionHandler.hpp>

// -----------------------------------------------------------------------------

// Object ID
// atom   1     - primary object - second is moving
// atom   2     - moving atom

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CDirectionSelection : public CSelectionHandler {
public:
// handler main method -----------------------------------------------------
    virtual ESelResult RegisterObject(CSelectionList* p_sel,const CSelObject& obj);

// handler description
    virtual const QString GetHandlerDescription(void) const;

private:
    void UnselectAll(CSelectionList* p_sel);
    void InvertDirection(CSelectionList* p_sel);
    void SelectDirection(CSelectionList* p_sel);
    void SelectMolecule(CSelectionList* p_sel);
};

// -----------------------------------------------------------------------------

extern CDirectionSelection NEMESIS_CORE_PACKAGE SH_Direction;

// -----------------------------------------------------------------------------

#endif

