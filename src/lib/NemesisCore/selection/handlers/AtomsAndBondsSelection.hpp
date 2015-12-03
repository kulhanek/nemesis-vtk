#ifndef AtomsAndBondsSelectionH
#define AtomsAndBondsSelectionH
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

#include <SelectionHandler.hpp>

// -----------------------------------------------------------------------------

class CStructure;

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CAtomsAndBondsSelection : public CSelectionHandler {
public:
// handler main method -----------------------------------------------------
    virtual ESelResult RegisterObject(CSelectionList* p_sel,const CSelObject& obj);

// handler description
    virtual const QString GetHandlerDescription(void) const;

// section of private data ----------------------------------------------------
private:
    ESelResult InvertSelection(CSelectionList* p_sel);
    void InvertSelection(CSelectionList* p_sel,CStructure* p_str);

    ESelResult CompleteAllStructures(CSelectionList* p_sel);
    ESelResult CompleteActiveStructure(CSelectionList* p_sel);
    void CompleteStructure(CSelectionList* p_sel,CStructure* p_str);

    ESelResult CompleteMolecules(CSelectionList* p_sel);

    ESelResult CompleteBonds(CSelectionList* p_sel);
    void CompleteBonds(CSelectionList* p_sel,CStructure* p_str);

    ESelResult CompleteValences(CSelectionList* p_sel);
    void CompleteValences(CSelectionList* p_sel,CStructure* p_str);
};

// -----------------------------------------------------------------------------

extern CAtomsAndBondsSelection NEMESIS_CORE_PACKAGE SH_AtomsAndBonds;

// -----------------------------------------------------------------------------

#endif

