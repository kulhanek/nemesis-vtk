// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008-2009 Petr Kulhanek, kulhanek@enzim.hu,
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
#include <SpecAxesSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* SpecAxesSetupCB(void* p_data);

CExtUUID        SpecAxesSetupID(
                    "{SPEC_AXES_SETUP:cbdc9945-c110-4130-8885-e813849f0c4b}",
                    "Special Axes");

CPluginObject   SpecAxesSetupObject(&NemesisCorePlugin,
                    SpecAxesSetupID,GRAPHICS_SETUP_CAT,
                    ":/images/NemesisCore/graphics/objects/SpecAxes.svg",
                    SpecAxesSetupCB);

// -----------------------------------------------------------------------------

QObject* SpecAxesSetupCB(void* p_data)
{
    return(new CSpecAxesSetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSpecAxesSetup::CSpecAxesSetup(CProObject* p_owner)
    : CGraphicsSetup(&SpecAxesSetupObject,p_owner)
{
    PXColor.SetRGB(0.8,0,0,1);
    ConeXColor.SetRGB(0.8,0,0,1);

    PYColor.SetRGB(0,0.8,0,1);
    ConeYColor.SetRGB(0,0.8,0,1);

    PZColor.SetRGB(0,0,0.8,1);
    ConeZColor.SetRGB(0,0,0.8,1);

    AxisRadius = 0.05;
    ConeRadius = 0.10;
    ConeHeight = 0.40;
    CylinderTesselation = 3;
    ConeTesselation = 3;

    MagnificationX = 1.0;
    MagnificationY = 1.0;
    MagnificationZ = 1.0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSpecAxesSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;
    p_sel = p_ele->GetFirstChildElement("X");
    if( p_sel != NULL ) {
        PXColor.Load("pc",p_sel);
        ConeXColor.Load("cc",p_sel);
    }
    p_sel = p_ele->GetFirstChildElement("Y");
    if( p_sel != NULL ) {
        PYColor.Load("pc",p_sel);
        ConeYColor.Load("cc",p_sel);
    }
    p_sel = p_ele->GetFirstChildElement("Z");
    if( p_sel != NULL ) {
        PZColor.Load("pc",p_sel);
        ConeZColor.Load("cc",p_sel);
    }

    p_ele->GetAttribute("ar",AxisRadius);
    p_ele->GetAttribute("cr",ConeRadius);
    p_ele->GetAttribute("ch",ConeHeight);

    p_ele->GetAttribute("cyt",CylinderTesselation);
    p_ele->GetAttribute("cot",ConeTesselation);

    p_ele->GetAttribute("mx",MagnificationX);
    p_ele->GetAttribute("my",MagnificationY);
    p_ele->GetAttribute("mz",MagnificationZ);
}

//---------------------------------------------------------------------------

void CSpecAxesSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;
    p_sel = p_ele->CreateChildElement("X");
    PXColor.Save("pc",p_sel);
    ConeXColor.Save("cc",p_sel);

    p_sel = p_ele->CreateChildElement("Y");
    PYColor.Save("pc",p_sel);
    ConeYColor.Save("cc",p_sel);

    p_sel = p_ele->CreateChildElement("Z");
    PZColor.Save("pc",p_sel);
    ConeZColor.Save("cc",p_sel);

    p_ele->SetAttribute("ar",AxisRadius);
    p_ele->SetAttribute("cr",ConeRadius);
    p_ele->SetAttribute("ch",ConeHeight);

    p_ele->SetAttribute("cyt",CylinderTesselation);
    p_ele->SetAttribute("cot",ConeTesselation);

    p_ele->SetAttribute("mx",MagnificationX);
    p_ele->SetAttribute("my",MagnificationY);
    p_ele->SetAttribute("mz",MagnificationZ);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



