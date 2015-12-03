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
#include <GlobalSetup.hpp>
#include <ErrorSystem.hpp>

#include <NemesisCoreModule.hpp>
#include <AtomLabelSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* AtomLabelSetupCB(void* p_data);

CExtUUID        CAtomLabelSetupID(
                    "{ATOM_LABEL_SETUP:e943f64f-7e45-4e33-b4fa-155d8b426468}",
                    "Atom label");

CPluginObject   CAtomLabelSetupObject(&NemesisCorePlugin,
                    CAtomLabelSetupID,GRAPHICS_SETUP_CAT,
                    ":/images/NemesisCore/graphics/objects/AtomLabel.svg",
                    AtomLabelSetupCB);

// -----------------------------------------------------------------------------

QObject* AtomLabelSetupCB(void* p_data)
{
    return(new CAtomLabelSetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomLabelSetup::CAtomLabelSetup(CProObject* p_owner)
    : CGraphicsSetup(&CAtomLabelSetupObject,p_owner)
{
    LabelFontName = "LiberationSans-Regular";
    LabelFontSize = 12;
    LabelColor.SetRGB(1.0,1.0,1.0);

    LineStippleFactor = 2;
    LineStipplePattern = 0x5555;
    LineWidth = 1;
    LineColor.SetRGB(0.5,0.5,0.0);
    DepthTest = false;

    UpdateFTGLFont();
}

//------------------------------------------------------------------------------

CAtomLabelSetup::~CAtomLabelSetup(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CAtomLabelSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    p_ele->GetAttribute("lfn",LabelFontName);
    p_ele->GetAttribute("lfs",LabelFontSize);

    p_sel = p_ele->GetFirstChildElement("lac");
    if( p_sel != NULL ) LabelColor.Load(p_sel);

    p_sel = p_ele->GetFirstChildElement("lic");
    if( p_sel != NULL ) LineColor.Load(p_sel);

    p_ele->GetAttribute("liw",LineWidth);
    p_ele->GetAttribute("lsf",LineStippleFactor);
    p_ele->GetAttribute("lsp",LineStipplePattern);

    p_ele->GetAttribute("dt",DepthTest);

    UpdateFTGLFont();
}

//---------------------------------------------------------------------------

void CAtomLabelSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CXMLElement* p_sel;

    p_ele->SetAttribute("lfn",LabelFontName);
    p_ele->SetAttribute("lfs",LabelFontSize);

    p_sel = p_ele->CreateChildElement("lac");
    LabelColor.Save(p_sel);

    p_sel = p_ele->CreateChildElement("lic");
    LineColor.Save(p_sel);

    p_ele->SetAttribute("liw",LineWidth);
    p_ele->SetAttribute("lsf",LineStippleFactor);
    p_ele->SetAttribute("lsp",LineStipplePattern);

    p_ele->SetAttribute("dt",DepthTest);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CAtomLabelSetup::UpdateFTGLFont()
{
    FTGLFont.Setup(LabelFontName,LabelFontSize);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



