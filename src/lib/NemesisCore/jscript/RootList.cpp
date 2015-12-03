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

#include <RootList.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        RootListID(
                    "{ROOT_LIST:e8a210c9-c2f5-4233-a2a5-b21eca1f6fcd}",
                    "Root list");

CPluginObject   RootListObject(&NemesisCorePlugin,
                    RootListID,GENERIC_CAT,NULL);

//------------------------------------------------------------------------------

CRootList*   RootList = NULL;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRootList::CRootList(CExtComObject* p_parent)
    : CExtComObject(&RootListObject,p_parent)
{
    ConFlags |= EECOF_SUB_CONTAINERS;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



