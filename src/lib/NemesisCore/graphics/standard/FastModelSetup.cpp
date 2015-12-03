// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <XMLElement.hpp>
#include <PluginObject.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>

#include <NemesisCoreModule.hpp>
#include <FastModelSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* FastModelSetupCB(void* p_data);

CExtUUID        FastModelSetupID(
                    "{FAST_MODEL_SETUP:0c8436f2-3e81-47f2-ac7b-c63507d4097e}",
                    "Fast Model");

CPluginObject   FastModelSetupObject(&NemesisCorePlugin,
                    FastModelSetupID,
                    GRAPHICS_SETUP_CAT,
                    ":/images/NemesisCore/graphics/objects/FastModel.svg",
                    FastModelSetupCB);

// -----------------------------------------------------------------------------

QObject* FastModelSetupCB(void* p_data)
{
    return(new CFastModelSetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFastModelSetup::CFastModelSetup(CProObject* p_owner)
    : CGraphicsSetup(&FastModelSetupObject,p_owner)
{
    LineWidth = 1;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFastModelSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }
}

//---------------------------------------------------------------------------

void CFastModelSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }
}

//---------------------------------------------------------------------------

