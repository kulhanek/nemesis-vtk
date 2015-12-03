#ifndef RootListH
#define RootListH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010        Petr Kulhanek, kulhanek@chemi.muni.cz
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

// -----------------------------------------------------------------------------

/// root of all dynamic objects

class NEMESIS_CORE_PACKAGE CRootList : public CExtComObject {

public:
// constructors and destructors -----------------------------------------------
    CRootList(CExtComObject* p_parent);
};

// -----------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE CRootList*   RootList;

// -----------------------------------------------------------------------------

#endif
