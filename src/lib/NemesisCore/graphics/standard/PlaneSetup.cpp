// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Jiri Klusak, anim4tor@mail.muni.cz
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
#include <PlaneSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PlaneSetupCB(void* p_data);

CExtUUID        PlaneSetupID(
                    "{PLANE_SETUP:ab0df312-7e6d-4b1d-af90-5722d7cadcf2}",
                    "Plane");

CPluginObject   PlaneSetupObject(&NemesisCorePlugin,
                    PlaneSetupID,GRAPHICS_SETUP_CAT,
                    ":/images/NemesisCore/graphics/objects/Plane.svg",
                    PlaneSetupCB);

// -----------------------------------------------------------------------------

QObject* PlaneSetupCB(void* p_data)
{
    return(new CPlaneSetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPlaneSetup::CPlaneSetup(CProObject* p_owner)
    : CGraphicsSetup(&PlaneSetupObject,p_owner)
{
    FrontColor.SetRGB(0.8,0.8,0.15,0.3);
    BackColor.SetRGB(0.9,0.15,0.15,0.3);
    Transparent = false;
    NumOfVertices = 4;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPlaneSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    // if setup does not contain data, use default setup
    p_sel = p_ele->GetFirstChildElement("active");
    if( p_sel != NULL ) {
        FrontColor.Load(p_sel);
    }

    p_sel = p_ele->GetFirstChildElement("deactive");
    if( p_sel != NULL ) {
        BackColor.Load(p_sel);
    }

    p_ele->GetAttribute("tr",Transparent);
    p_ele->GetAttribute("nv",NumOfVertices);
}

//---------------------------------------------------------------------------

void CPlaneSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;
    p_sel = p_ele->CreateChildElement("active");
    FrontColor.Save(p_sel);

    p_sel = p_ele->CreateChildElement("deactive");
    BackColor.Save(p_sel);

    p_ele->SetAttribute("tr",Transparent);
    p_ele->SetAttribute("nv",NumOfVertices);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


