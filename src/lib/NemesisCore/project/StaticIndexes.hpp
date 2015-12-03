#ifndef StaticIndexesH
#define StaticIndexesH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012        Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <map>
#include <string>
#include <ExtUUID.hpp>

//------------------------------------------------------------------------------

// do not change these IDs as it might invalidate saved structures
#define PROJECT_INDEX                   1
#define STRUCTURES_INDEX                2
#define PROPERTIES_INDEX                3
#define GRAPHICS_INDEX                  4
#define GRAPHICS_OBJECTS_INDEX          5
#define GRAPHICS_PROFILES_INDEX         6
#define GRAPHICS_VIEWS_INDEX            7
#define GRAPHICS_PRIMARY_VIEW_INDEX     8
#define GRAPHICS_SETUPS_INDEX           9
#define SELECTION_LIST_INDEX            10
#define HISTORY_LIST_INDEX              11
#define MOUSE_HANDLER_INDEX             12
#define JOB_LIST_INDEX                  13
#define TRAJECTORY_LIST_INDEX           14
#define CHEMINFO_INFO_LIST_INDEX        15
#define CHEMINFO_STRUCTURE_LIST_PROJECT_INDEX 16

#define LAST_CORE_STATIC_INDEX      50
#define LAST_USER_STATIC_INDEX     100

//------------------------------------------------------------------------------

/// it contains the list of all static indexes that might be used in all projects
class NEMESIS_CORE_PACKAGE CStaticIndexList {
public:
    /// init list of static indexes
    CStaticIndexList(void);

    /// check if the index is static one, if it so then check if it is registered
    void CheckIndex(int index,const CExtUUID& type);

// section of private data -----------------------------------------------------
private:
    std::map<int,CExtUUID>   RegIndexes;
};

//------------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE CStaticIndexList StaticIndexes;

//------------------------------------------------------------------------------

#endif
