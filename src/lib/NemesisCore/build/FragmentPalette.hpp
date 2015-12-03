#ifndef FragmentPaletteH
#define FragmentPaletteH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <SimpleList.hpp>
#include <Fragment.hpp>

//------------------------------------------------------------------------------

class CXMLElement;

//------------------------------------------------------------------------------

/// list of fragments

class NEMESIS_CORE_PACKAGE CFragmentPalette {
public:
// constructor and destructor -------------------------------------------------
    CFragmentPalette(void);
    ~CFragmentPalette(void);

// input/output methods -------------------------------------------------------
    /// load the structure from a file
    bool Load(const CSmallString& name);

    /// save the structure to a file
    bool Save(const CSmallString& name);

    /// load the structure from a XML element
    bool Load(CXMLElement* p_element);

    /// save the structure to a XML element
    bool Save(CXMLElement* p_element);

// information methods --------------------------------------------------------
    /// get number of fragments
    int GetNumberOfFragments(void);

    /// get fragment
    CFragment* GetFragment(int index);

// executive methods ----------------------------------------------------------
    /// delete all fragments
    void Clear(void);

// section of private data ----------------------------------------------------
private:
    CSimpleList<CFragment>     Fragments;
};

//------------------------------------------------------------------------------

#endif
