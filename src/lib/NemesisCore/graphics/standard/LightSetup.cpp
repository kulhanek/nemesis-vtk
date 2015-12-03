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
#include <LightSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* LightSetupCB(void* p_data);

CExtUUID        LightSetupID(
                    "{LIGHT_SETUP:08f36a6d-85b2-42a7-bfb8-69813b544823}",
                    "Light");

CPluginObject   LightSetupObject(&NemesisCorePlugin,
                    LightSetupID,GRAPHICS_SETUP_CAT,
                    ":/images/NemesisCore/graphics/objects/Light.svg",
                    LightSetupCB);

// -----------------------------------------------------------------------------

QObject* LightSetupCB(void* p_data)
{
    return(new CLightSetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CLightSetup::CLightSetup(CProObject* p_owner)
    : CGraphicsSetup(&LightSetupObject,p_owner)
{
    CColor LightColor(1,1,1);
    SetColor(LightColor);

    SpotExponent = 4;
    SpotCutoff = 90.0;

    Attenuation[0]=1;
    Attenuation[1]=0;
    Attenuation[2]=0;

    CoverColor.SetRGB(0,0,1);
    CoverRadius = 0.6;
    CoverHeight = 0.9;
    CoverTesselation = 10;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CLightSetup::SetColor(CColor color,float ambient,float diffuse,
                          float specular)
{
    Diffuse = color*diffuse;
    Ambient = color*ambient;
    Specular = color*specular;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CLightSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    // if setup does not contain data, use default setup

    p_sel = p_ele->GetFirstChildElement("ambient");
    if( p_sel != NULL ) {
        Ambient.Load(p_sel);
    }
    p_sel = p_ele->GetFirstChildElement("diffuse");
    if( p_sel != NULL ) {
        Diffuse.Load(p_sel);
    }
    p_sel = p_ele->GetFirstChildElement("specular");
    if( p_sel != NULL ) {
        Specular.Load(p_sel);
    }

    p_ele->GetAttribute("se",SpotExponent);
    p_ele->GetAttribute("sc",SpotCutoff);
    p_ele->GetAttribute("a1",Attenuation[0]);
    p_ele->GetAttribute("a2",Attenuation[1]);
    p_ele->GetAttribute("a3",Attenuation[2]);

    p_sel = p_ele->GetFirstChildElement("cover_color");
    if( p_sel != NULL ) {
        CoverColor.Load(p_sel);
    }

    p_ele->GetAttribute("cr",CoverRadius);
    p_ele->GetAttribute("ch",CoverHeight);
    p_ele->GetAttribute("ct",CoverTesselation);
}

//---------------------------------------------------------------------------

void CLightSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    p_sel = p_ele->CreateChildElement("ambient");
    Ambient.Save(p_sel);

    p_sel = p_ele->CreateChildElement("diffuse");
    Diffuse.Save(p_sel);

    p_sel = p_ele->CreateChildElement("specular");
    Specular.Save(p_sel);

    p_ele->SetAttribute("se",SpotExponent);
    p_ele->SetAttribute("sc",SpotCutoff);
    p_ele->SetAttribute("a1",Attenuation[0]);
    p_ele->SetAttribute("a2",Attenuation[1]);
    p_ele->SetAttribute("a3",Attenuation[2]);

    p_sel = p_ele->CreateChildElement("cover_color");
    CoverColor.Save(p_sel);

    p_ele->SetAttribute("cr",CoverRadius);
    p_ele->SetAttribute("ch",CoverHeight);
    p_ele->SetAttribute("ct",CoverTesselation);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================











