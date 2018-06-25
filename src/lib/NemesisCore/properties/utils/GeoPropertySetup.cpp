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

#include <NemesisCoreModule.hpp>
#include <GeoPropertySetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GeoPropertySetupCB(void* p_data);

CExtUUID        CGeoPropertySetupID(
                    "{GEO_PROPERTY_SETUP:243ce6af-61ab-429d-a53d-af9cc4d1ef3e}",
                    "Geometry");

CPluginObject   GeoPropertySetupObject(&NemesisCorePlugin,
                    CGeoPropertySetupID,GRAPHICS_SETUP_CAT,
                    ":/images/NemesisCore/properties/Geo.svg",
                    GeoPropertySetupCB);

// -----------------------------------------------------------------------------

QObject* GeoPropertySetupCB(void* p_data)
{
    return(new CGeoPropertySetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGeoPropertySetup::CGeoPropertySetup(CProObject* p_owner)
    : CGraphicsSetup(&GeoPropertySetupObject,p_owner)
{
    LineStippleFactor = 2;
    LineStipplePattern = 0x5555;
    LineWidth = 1;
    LineColor.SetRGB(0.8,0.1,0.0);

    LabelFontName = "NotoSans-Regular";
    LabelFontSize = 30;
    LabelColor.SetRGB(0.8,0.1,0.0);

    LabelQuotationColor.SetRGB(0.9,0.15,0.15,1.0);
    LabelQuotationLineWidth = 2;
    LabelQuotationStippleFactor = 2;
    LabelQuotationStipplePattern = 0x5555;

    PointColor.SetRGB(0.15,0.8,0.15,1.0);
    PointWidth = 2;
    PointSize = 0.2;

    PointQuotationColor.SetRGB(0.9,0.15,0.15,1.0);
    PointQuotationWidth = 1;
    PointQuotationStippleFactor = 2;
    PointQuotationStipplePattern = 0x33333333;

    AngleArcScope = 0.9;

    DepthTest = true;
    ShowUnit = true;

    UpdateFTGLFont();
}

//------------------------------------------------------------------------------

CGeoPropertySetup::~CGeoPropertySetup(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeoPropertySetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    p_ele->GetAttribute("liw",LineWidth);
    p_ele->GetAttribute("lsf",LineStippleFactor);
    p_ele->GetAttribute("lsp",LineStipplePattern);
    p_sel = p_ele->GetFirstChildElement("lic");
    if( p_sel != NULL ) LineColor.Load(p_sel);

    p_ele->GetAttribute("lfn",LabelFontName);
    p_ele->GetAttribute("lfs",LabelFontSize);
    p_sel = p_ele->GetFirstChildElement("lac");
    if( p_sel != NULL ) LabelColor.Load(p_sel);

    p_ele->GetAttribute("lqiw",LabelQuotationLineWidth);
    p_ele->GetAttribute("lqsf",LabelQuotationStippleFactor);
    p_ele->GetAttribute("lqsp",LabelQuotationStipplePattern);
    p_sel = p_ele->GetFirstChildElement("lqc");
    if( p_sel != NULL ) LabelQuotationColor.Load(p_sel);

    p_ele->GetAttribute("pw",PointWidth);
    p_ele->GetAttribute("ps",PointSize);
    p_sel = p_ele->GetFirstChildElement("lpc");
    if( p_sel != NULL ) PointColor.Load(p_sel);

    p_ele->GetAttribute("lpqiw",PointQuotationWidth);
    p_ele->GetAttribute("lpqsf",PointQuotationStippleFactor);
    p_ele->GetAttribute("lpqsp",PointQuotationStipplePattern);
    p_sel = p_ele->GetFirstChildElement("lpqc");
    if( p_sel != NULL ) PointQuotationColor.Load(p_sel);

    p_ele->GetAttribute("dt",DepthTest);
    p_ele->GetAttribute("su",ShowUnit);

    UpdateFTGLFont();
}

//---------------------------------------------------------------------------

void CGeoPropertySetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    p_ele->SetAttribute("liw",LineWidth);
    p_ele->SetAttribute("lsf",LineStippleFactor);
    p_ele->SetAttribute("lsp",LineStipplePattern);
    p_sel = p_ele->CreateChildElement("lic");
    LineColor.Save(p_sel);

    p_ele->SetAttribute("lfn",LabelFontName);
    p_ele->SetAttribute("lfs",LabelFontSize);
    p_sel = p_ele->CreateChildElement("lac");
    LabelColor.Save(p_sel);

    p_ele->SetAttribute("lqiw",LabelQuotationLineWidth);
    p_ele->SetAttribute("lqsf",LabelQuotationStippleFactor);
    p_ele->SetAttribute("lqsp",LabelQuotationStipplePattern);
    p_sel = p_ele->CreateChildElement("lqc");
    LabelQuotationColor.Save(p_sel);

    p_ele->SetAttribute("pw",PointWidth);
    p_ele->SetAttribute("ps",PointSize);
    p_sel = p_ele->CreateChildElement("lpc");
    PointColor.Save(p_sel);

    p_ele->SetAttribute("lpqiw",PointQuotationWidth);
    p_ele->SetAttribute("lpqsf",PointQuotationStippleFactor);
    p_ele->SetAttribute("lpqsp",PointQuotationStipplePattern);
    p_sel = p_ele->CreateChildElement("lpqc");
    PointQuotationColor.Save(p_sel);

    p_ele->SetAttribute("dt",DepthTest);
    p_ele->SetAttribute("su",ShowUnit);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeoPropertySetup::UpdateFTGLFont()
{
    FTGLFont.Setup(LabelFontName,LabelFontSize);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



