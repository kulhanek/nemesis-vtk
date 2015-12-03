#ifndef ElementColorsListH
#define ElementColorsListH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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
#include <ElementColors.hpp>
#include <PeriodicTable.hpp>
#include <XMLDocument.hpp>

//------------------------------------------------------------------------------

/// list of elements' colors

class NEMESIS_CORE_PACKAGE CElementColorsList {
public:
// constructors and destructors -----------------------------------------------
    CElementColorsList(void);

// configuation methods -------------------------------------------------------
    /// load user or optionaly system color definition
    bool LoadColors(void);

    /// load system color definition
    bool LoadSystemColors(void);

    /// save user color definition
    bool SaveUserColors(void);

// access methods -------------------------------------------------------------
    /// get element
    const CElementColors& GetElementColors(int z);

    /// set element
    void SetElementColors(int z,const CElementColors&);

    /// get element
    CElementColors* GetElementColorPointer(int z);

// section of public data ----------------------------------------------------
public:
    CElementColors  GlobalMaterial;
    GLenum          MaterialMode;

    CElementColors  SelectionMaterial;

// section of private data ----------------------------------------------------
private:
    CElementColors  Elements[CPT_NUM_OF_ELEMENTS];

    bool LoadColorsFile(const CFileName& config_name);
};

//------------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE CElementColorsList ColorsList;

//-----------------------------------------------------------------------------

#endif

