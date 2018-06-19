// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <GlobalSetup.hpp>
#include <ErrorSystem.hpp>

#include "RMSDToolsModule.hpp"
#include "RMSDPropertySetup.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* RMSDPropertySetupCB(void* p_data);

CExtUUID        RMSDPropertySetupID(
                    "{RMSD_PROPERTY_SETUP:6695a72e-6d2c-445a-aad2-f222785e1959}",
                    "RMSD");

CPluginObject   RMSDPropertySetupObject(&RMSDToolsPlugin,
                    RMSDPropertySetupID,GRAPHICS_SETUP_CAT,
                    ":/images/RMSDTools/RMSD.svg",
                    RMSDPropertySetupCB);

// -----------------------------------------------------------------------------

QObject* RMSDPropertySetupCB(void* p_data)
{
    return(new CRMSDPropertySetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRMSDPropertySetup::CRMSDPropertySetup(CProObject* p_owner)
    : CGraphicsSetup(&RMSDPropertySetupObject,p_owner)
{
    LineStippleFactor = 2;
    LineStipplePattern = 0x5555;
    LineWidth = 1;
    LineColor.SetRGB(0.5,0.5,0.0);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRMSDPropertySetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    p_sel = p_ele->GetFirstChildElement("lic");
    if( p_sel != NULL ) LineColor.Load(p_sel);

    p_ele->GetAttribute("liw",LineWidth);
    p_ele->GetAttribute("lsf",LineStippleFactor);
    p_ele->GetAttribute("lsp",LineStipplePattern);
}

//---------------------------------------------------------------------------

void CRMSDPropertySetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    p_sel = p_ele->CreateChildElement("lic");
    LineColor.Save(p_sel);

    p_ele->SetAttribute("liw",LineWidth);
    p_ele->SetAttribute("lsf",LineStippleFactor);
    p_ele->SetAttribute("lsp",LineStipplePattern);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



