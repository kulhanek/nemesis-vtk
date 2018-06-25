// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2011 Jiri Klusak, anim4tor@mail.muni.cz
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

#include <NemesisCoreModule.hpp>
#include <PluginObject.hpp>
#include <XMLElement.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <GlobalSetup.hpp>
#include <ErrorSystem.hpp>

#include <GeometryLabelSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GeometryLabelSetupCB(void* p_data);

CExtUUID        CGeometryLabelSetupID(
                    "{GEOMETRY_LABEL_SETUP:209c0d4c-3919-46bd-bf1f-810439e7f5c1}",
                    "Geometry Label");

CPluginObject   CGeometryLabelSetupObject(&NemesisCorePlugin,
                    CGeometryLabelSetupID,GRAPHICS_SETUP_CAT,
                    ":/images/NemesisCore/graphics/objects/GeometryLabel.svg",
                    GeometryLabelSetupCB);

// -----------------------------------------------------------------------------

QObject* GeometryLabelSetupCB(void* p_data)
{
    return(new CGeometryLabelSetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGeometryLabelSetup::CGeometryLabelSetup(CProObject* p_owner)
    : CGraphicsSetup(&CGeometryLabelSetupObject,p_owner)
{
    LineColor.SetRGB(0.8,0.8,0.15,1.0);
    LineWidth = 2;
    LineStippleFactor = 2;
    LineStipplePattern = 0x5555;
    AngleArcScope = 0.5;

    QuotationColor.SetRGB(0.9,0.15,0.15,1.0);
    QuotationLineWidth = 2;
    QuotationStippleFactor = 2;
    QuotationStipplePattern = 0x5555;

    LabelColor.SetRGB(0.8,0.8,0.15,1.0);
    LabelFontName = "NotoSans-Regular";
    LabelFontSize = 30;
    ShowUnit = true;
    DepthTest = false;

    UpdateFTGLFont();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeometryLabelSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    p_sel = p_ele->GetFirstChildElement("lc");
    if( p_sel != NULL ) LineColor.Load(p_sel);
    p_ele->GetAttribute("lw",LineWidth);
    p_ele->GetAttribute("lsf",LineStippleFactor);
    p_ele->GetAttribute("lsp",LineStipplePattern);
    p_ele->GetAttribute("lars",AngleArcScope);

    p_sel = p_ele->GetFirstChildElement("qc");
    if( p_sel != NULL ) QuotationColor.Load(p_sel);
    p_ele->GetAttribute("qw",QuotationLineWidth);
    p_ele->GetAttribute("qsf",QuotationStippleFactor);
    p_ele->GetAttribute("qsp",QuotationStipplePattern);

    p_sel = p_ele->GetFirstChildElement("lac");
    if( p_sel != NULL ) LabelColor.Load(p_sel);
    p_ele->GetAttribute("lan",LabelFontName);
    p_ele->GetAttribute("las",LabelFontSize);
    p_ele->GetAttribute("lasu",ShowUnit);
    p_ele->GetAttribute("ladt",DepthTest);

    UpdateFTGLFont();
}

//---------------------------------------------------------------------------

void CGeometryLabelSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    p_sel = p_ele->CreateChildElement("lc");
    LineColor.Save(p_sel);
    p_ele->SetAttribute("lw",LineWidth);
    p_ele->SetAttribute("lsf",LineStippleFactor);
    p_ele->SetAttribute("lsp",LineStipplePattern);
    p_ele->SetAttribute("lars",AngleArcScope);

    p_sel = p_ele->CreateChildElement("qc");
    QuotationColor.Save(p_sel);
    p_ele->SetAttribute("qw",QuotationLineWidth);
    p_ele->SetAttribute("qsf",QuotationStippleFactor);
    p_ele->SetAttribute("qsp",QuotationStipplePattern);

    p_sel = p_ele->CreateChildElement("lac");
    LabelColor.Save(p_sel);
    p_ele->SetAttribute("lan",LabelFontName);
    p_ele->SetAttribute("las",LabelFontSize);
    p_ele->SetAttribute("lasu",ShowUnit);
    p_ele->SetAttribute("ladt",DepthTest);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeometryLabelSetup::UpdateFTGLFont()
{
    FTGLFont.Setup(LabelFontName,LabelFontSize);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



