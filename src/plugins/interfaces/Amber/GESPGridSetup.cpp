// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2016 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <PluginObject.hpp>
#include <XMLElement.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <ErrorSystem.hpp>

#include "AmberModule.hpp"
#include "GESPGridSetup.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GESPGridSetupCB(void* p_data);

CExtUUID        CGESPGridSetupID(
                    "{GESPGRID_SETUP:0c8facc0-f6a0-49fe-969a-7c4fdd451f45}",
                    "GESPGrid");

CPluginObject   CGESPGridSetupObject(&AmberPlugin,
                    CGESPGridSetupID,GRAPHICS_SETUP_CAT,
                    ":/images/Amber/GESPGrid.svg",
                    GESPGridSetupCB);

// -----------------------------------------------------------------------------

QObject* GESPGridSetupCB(void* p_data)
{
    return(new CGESPGridSetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGESPGridSetup::CGESPGridSetup(CProObject* p_owner)
    : CGraphicsSetup(&CGESPGridSetupObject,p_owner)
{
   PESPColor.SetRGB(0.0,0.0,1.0,1.0);
   NESPColor.SetRGB(1.0,0.0,0.0,1.0);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGESPGridSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    // if setup does not contain data, use default setup
    p_sel = p_ele->GetFirstChildElement("pcolor");
    if( p_sel != NULL ) {
        PESPColor.Load(p_sel);
    }
    p_sel = p_ele->GetFirstChildElement("ncolor");
    if( p_sel != NULL ) {
        NESPColor.Load(p_sel);
    }
}

//---------------------------------------------------------------------------

void CGESPGridSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;
    p_sel = p_ele->CreateChildElement("pcolor");
    PESPColor.Save(p_sel);
    p_sel = p_ele->CreateChildElement("ncolor");
    NESPColor.Save(p_sel);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



