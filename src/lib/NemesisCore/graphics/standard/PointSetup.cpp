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
#include <Project.hpp>
#include <ErrorSystem.hpp>

#include <NemesisCoreModule.hpp>
#include <PointSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PointSetupCB(void* p_data);

CExtUUID        CPointSetupID(
                    "{POINT_SETUP:0071bbd8-c368-4571-8164-30e3c7467084}",
                    "Point");

CPluginObject   CPointSetupObject(&NemesisCorePlugin,
                    CPointSetupID,GRAPHICS_SETUP_CAT,
                    ":/images/NemesisCore/graphics/objects/Point.svg",
                    PointSetupCB);

// -----------------------------------------------------------------------------

QObject* PointSetupCB(void* p_data)
{
    return(new CPointSetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPointSetup::CPointSetup(CProObject* p_owner)
    : CGraphicsSetup(&CPointSetupObject,p_owner)
{
    PointColor.SetRGB(0.15,0.8,0.15,1.0);
    PointWidth = 2;
    PointSize = 0.2;

    QuotationColor.SetRGB(0.9,0.15,0.15,1.0);
    QuotationWidth = 1;
    QuotationStippleFactor = 2;
    QuotationStipplePattern = 0x33333333;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPointSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    p_sel = p_ele->GetFirstChildElement("lc");
    if( p_sel != NULL ) PointColor.Load(p_sel);
    p_ele->GetAttribute("lw",PointWidth);
    p_ele->GetAttribute("ls",PointSize);

    p_sel = p_ele->GetFirstChildElement("qc");
    if( p_sel != NULL ) QuotationColor.Load(p_sel);
    p_ele->GetAttribute("qw",QuotationWidth);
    p_ele->GetAttribute("qsf",QuotationStippleFactor);
    p_ele->GetAttribute("qsp",QuotationStipplePattern);
}

//---------------------------------------------------------------------------

void CPointSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    p_sel = p_ele->CreateChildElement("lc");
    PointColor.Save(p_sel);
    p_ele->SetAttribute("lw",PointWidth);
    p_ele->SetAttribute("ls",PointSize);

    p_sel = p_ele->CreateChildElement("qc");
    QuotationColor.Save(p_sel);
    p_ele->SetAttribute("qw",QuotationWidth);
    p_ele->SetAttribute("qsf",QuotationStippleFactor);
    p_ele->SetAttribute("qsp",QuotationStipplePattern);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



