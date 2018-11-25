// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2018 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <PluginDatabase.hpp>
#include "SymmetryModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool SymmetryPluginCB(EModuleRequest request,void* p_data);

// -----------------------------------------------------------------------------

CExtUUID        SymmetryPluginID(
                    "{SYMMETRY_WP:ac42023e-6568-4bde-9abd-5b04b50a80d4}");

CPluginModule   SymmetryPlugin(
                    SymmetryPluginID,
                    SymmetryPluginCB);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool SymmetryPluginCB(EModuleRequest request,void* p_data)
{
    switch(request) {
    case EMR_INITIALIZE:
        break;
    case EMR_FINALIZE:
        break;
    };

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
