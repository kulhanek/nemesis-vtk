// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ErrorSystem.hpp>
#include <Project.hpp>

#include <NemesisCoreModule.hpp>
#include <PBCBoxSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PBCBoxSetupCB(void* p_data);

CExtUUID        PBCBoxSetupID(
                    "{PBC_BOX_SETUP:733a1345-128d-414e-ba09-c0a0c00074e1}",
                    "PBCBox");

CPluginObject   PBCBoxSetupObject(&NemesisCorePlugin,
                    PBCBoxSetupID,GRAPHICS_SETUP_CAT,
                    ":/images/NemesisCore/graphics/objects/PBCBox.svg",
                    PBCBoxSetupCB);

// -----------------------------------------------------------------------------

QObject* PBCBoxSetupCB(void* p_data)
{
    return(new CPBCBoxSetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPBCBoxSetup::CPBCBoxSetup(CProObject* p_owner)
    : CGraphicsSetup(&PBCBoxSetupObject,p_owner)
{
    UseTubes = false;
    LineWidth = 0.01;
    LineColor.SetRGB(0.0,0.5,0.0);
    LineStippleFactor = 0;
    LineStipplePattern = 0x5555;

    TubeTesselationQuality = 6;
    SphereTesselationQuality = 2;

    PointWidth = 1;
    PointSize = 0.15;
    PointColor.SetRGB(0.5,0.5,0.0);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPBCBoxSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;
    p_sel = p_ele->GetFirstChildElement("lc");
    if( p_sel != NULL ) {
        LineColor.Load(p_sel);
    }
    p_ele->GetAttribute("lw",LineWidth);
    p_ele->GetAttribute("ut",UseTubes);

    p_sel = p_ele->GetFirstChildElement("pc");
    if( p_sel != NULL ) {
        PointColor.Load(p_sel);
    }
    p_ele->GetAttribute("pw",PointWidth);
    p_ele->GetAttribute("ps",PointSize);
}

//---------------------------------------------------------------------------

void CPBCBoxSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;
    p_sel = p_ele->CreateChildElement("lc");
    LineColor.Save(p_sel);
    p_ele->SetAttribute("lw",LineWidth);
    p_ele->SetAttribute("ut",UseTubes);

    p_sel = p_ele->CreateChildElement("pc");
    PointColor.Save(p_sel);
    p_ele->SetAttribute("pw",PointWidth);
    p_ele->SetAttribute("ps",PointSize);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


