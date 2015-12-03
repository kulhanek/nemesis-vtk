// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <FreezedAtomsSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* FreezedAtomsSetupCB(void* p_data);

CExtUUID        CFreezedAtomsSetupID(
                    "{FREEZED_ATOMS_SETUP:976f7af5-d108-4edf-af7a-f2adbaae1c86}",
                    "Freezed Atoms");

CPluginObject   CFreezedAtomsSetupObject(&NemesisCorePlugin,
                    CFreezedAtomsSetupID,GRAPHICS_SETUP_CAT,
                    ":/images/NemesisCore/graphics/objects/FreezedAtoms.svg",
                    FreezedAtomsSetupCB);

// -----------------------------------------------------------------------------

QObject* FreezedAtomsSetupCB(void* p_data)
{
    return(new CFreezedAtomsSetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFreezedAtomsSetup::CFreezedAtomsSetup(CProObject* p_owner)
    : CGraphicsSetup(&CFreezedAtomsSetupObject,p_owner)
{
    Color.SetRGB(1.0,0,0.5);
    Width = 1;
    Ratio = 0.3;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFreezedAtomsSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    p_sel = p_ele->GetFirstChildElement("color");
    if( p_sel != NULL ) Color.Load(p_sel);

    p_ele->GetAttribute("w",Width);
    p_ele->GetAttribute("r",Ratio);
}

//---------------------------------------------------------------------------

void CFreezedAtomsSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    p_sel = p_ele->CreateChildElement("color");
    Color.Save(p_sel);

    p_ele->SetAttribute("w",Width);
    p_ele->SetAttribute("r",Ratio);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



