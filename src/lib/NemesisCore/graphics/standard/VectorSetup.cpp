
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Jiri Klusak, anim4tor@mail.muni.cz
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
#include <VectorSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* VectorSetupCB(void* p_data);

CExtUUID        VectorSetupID(
                    "{VECTOR_SETUP:bfbc1b4e-f685-43b4-8284-074688e61760}",
                    "Vector");

CPluginObject   VectorSetupObject(&NemesisCorePlugin,
                    VectorSetupID,GRAPHICS_SETUP_CAT,
                    ":/images/NemesisCore/graphics/objects/Vector.svg",
                    VectorSetupCB);

// -----------------------------------------------------------------------------

QObject* VectorSetupCB(void* p_data)
{
    return(new CVectorSetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CVectorSetup::CVectorSetup(CProObject* p_owner)
    : CGraphicsSetup(&VectorSetupObject,p_owner)
{
    AxisColor.SetRGB(0.8,0.8,0.1,1.0);
    AxisTesselation = 8;
    AxisRadius = 0.075;
    AxisDifuseColor = true;

    ConeColor.SetRGB(0.8,0.8,0.1,1.0);
    ConeTesselation = 3;
    ConeRadius = 0.15;
    ConeHeight = 0.50;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CVectorSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    p_sel = p_ele->GetFirstChildElement("axis");
    if( p_sel != NULL ) AxisColor.Load(p_sel);

    p_ele->GetAttribute("at",AxisTesselation);
    p_ele->GetAttribute("ar",AxisRadius);
    p_ele->GetAttribute("ad",AxisDifuseColor);

    p_sel = p_ele->GetFirstChildElement("cone");
    if( p_sel != NULL ) ConeColor.Load(p_sel);

    p_ele->GetAttribute("ct",ConeTesselation);
    p_ele->GetAttribute("cr",ConeRadius);
    p_ele->GetAttribute("ch",ConeHeight);
}

//---------------------------------------------------------------------------

void CVectorSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }
    CXMLElement* p_sel;

    p_sel = p_ele->CreateChildElement("axis");
    AxisColor.Save(p_sel);

    p_ele->SetAttribute("at",AxisTesselation);
    p_ele->SetAttribute("ar",AxisRadius);
    p_ele->SetAttribute("ad",AxisDifuseColor);

    p_sel = p_ele->CreateChildElement("cone");
    ConeColor.Save(p_sel);

    p_ele->SetAttribute("ct",ConeTesselation);
    p_ele->SetAttribute("cr",ConeRadius);
    p_ele->SetAttribute("ch",ConeHeight);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



