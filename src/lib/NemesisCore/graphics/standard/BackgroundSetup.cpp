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

#include <PluginObject.hpp>
#include <XMLElement.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <ErrorSystem.hpp>

#include <NemesisCoreModule.hpp>
#include <BackgroundSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* BackgroundSetupCB(void* p_data);

CExtUUID        CBackgroundSetupID(
                    "{BACKGROUND_SETUP:c4983c35-3d1e-4a26-ad16-bb131cd51fa9}",
                    "Background");

CPluginObject   CBackgroundSetupObject(&NemesisCorePlugin,
                    CBackgroundSetupID,GRAPHICS_SETUP_CAT,
                    ":/images/NemesisCore/graphics/objects/Background.svg",
                    BackgroundSetupCB);

// -----------------------------------------------------------------------------

QObject* BackgroundSetupCB(void* p_data)
{
    return(new CBackgroundSetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBackgroundSetup::CBackgroundSetup(CProObject* p_owner)
    : CGraphicsSetup(&CBackgroundSetupObject,p_owner)
{
   // Color.SetRGB(0.0,0.0,0.4,1.0); old color
   Color.SetRGB(1.0,1.0,1.0,1.0);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBackgroundSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    // if setup does not contain data, use default setup
    p_sel = p_ele->GetFirstChildElement("color");
    if( p_sel != NULL ) {
        Color.Load(p_sel);
    }
}

//---------------------------------------------------------------------------

void CBackgroundSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;
    p_sel = p_ele->CreateChildElement("color");
    Color.Save(p_sel);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



