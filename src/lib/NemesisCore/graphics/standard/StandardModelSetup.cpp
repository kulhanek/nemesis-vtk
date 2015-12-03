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

#include <XMLElement.hpp>
#include <PluginObject.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>

#include <NemesisCoreModule.hpp>
#include <StandardModelSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* StandardModelSetupCB(void* p_data);

CExtUUID        StandardModelSetupID(
                    "{STANDARD_MODEL_SETUP:ca769af6-6332-4fc7-a378-47423b837fa9}",
                    "Standard Model");

CPluginObject   StandardModelSetupObject(&NemesisCorePlugin,
                    StandardModelSetupID,GRAPHICS_SETUP_CAT,
                    ":/images/NemesisCore/graphics/objects/StandardModel.svg",
                    StandardModelSetupCB);

// -----------------------------------------------------------------------------

QObject* StandardModelSetupCB(void* p_data)
{
    return(new CStandardModelSetup(static_cast<CProObject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CStandardModelSetup::CStandardModelSetup(CProObject* p_owner)
    : CGraphicsSetup(&StandardModelSetupObject,p_owner)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStandardModelSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_el is NULL");
    }

    CXMLElement* p_mele = p_ele->GetFirstChildElement("models");
    if( p_mele == NULL ) {
        LOGIC_ERROR("unable to open MODELS element");
    }

    CXMLElement* p_aele = p_mele->GetFirstChildElement("model");

    while( p_aele != NULL ) {
        int     model;
        bool    result = true;

        result &= p_aele->GetAttribute("model",model);

        if( result == false ) {
            LOGIC_ERROR("unable to get level and model attribute");
        }

        if( (model < 0) || (model >= MODEL_NUM_OF_MODELS) ){
            CSmallString error;
            error << "incorrect model " << model;
            LOGIC_ERROR(error);
        }

        Models[model].LoadData(p_aele);

        p_aele = p_aele->GetNextSiblingElement("model");
    }
}

//---------------------------------------------------------------------------

void CStandardModelSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_el is NULL");
    }

    CXMLElement* p_mele = p_ele->CreateChildElement("models");
    for(int model=0; model < MODEL_NUM_OF_MODELS; model++) {
        CXMLElement* p_aele = p_mele->CreateChildElement("model");
        p_aele->SetAttribute("model",model);
        Models[model].SaveData(p_aele);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomModelSettings::CAtomModelSettings(void)
{
    Type = 1;                       // use gluSphere or icosahedron
    Radius = 0.0;                   // radius of isolated atoms
    Ratio = 0.2;                    // ratio between vdw radius and graphic radius
    TessellationQuality = 3;        // quality of gluSphere or icosahedron
}

//------------------------------------------------------------------------------

void CAtomModelSettings::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "atom" ) {
        LOGIC_ERROR("element is not atom");
    }

    bool result = true;

    result &= p_ele->GetAttribute("Type",Type);
    result &= p_ele->GetAttribute("Radius",Radius);
    result &= p_ele->GetAttribute("Ratio",Ratio);
    result &= p_ele->GetAttribute("TessellationQuality",TessellationQuality);

    if( result == false ) {
        LOGIC_ERROR("unable to load atom model setup");
    }
}

//------------------------------------------------------------------------------

void CAtomModelSettings::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "atom" ) {
        LOGIC_ERROR("element is not atom");
    }

    p_ele->SetAttribute("Type",Type);
    p_ele->SetAttribute("Radius",Radius);
    p_ele->SetAttribute("Ratio",Ratio);
    p_ele->SetAttribute("TessellationQuality",TessellationQuality);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBondModelSettings::CBondModelSettings(void)
{
    Type = 1;                   // use stick or gluCone
    Radius = 0.06;              // tube radius
    Pitch = 0.25;               // pitch for bonds with order higher than 1
    ShowOrder = true;          // show order for tubes
    Diffuse = false;           // continuous color transfer or van der Waals division
    TessellationQuality = 3;   // quality of gluCone or icosahedron
}

//------------------------------------------------------------------------------

void CBondModelSettings::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "bond" ) {
        LOGIC_ERROR("element is not bond");
    }

    bool result = true;

    result &= p_ele->GetAttribute("Type",Type);
    result &= p_ele->GetAttribute("Radius",Radius);
    result &= p_ele->GetAttribute("Pitch",Pitch);
    result &= p_ele->GetAttribute("ShowOrder",ShowOrder);
    result &= p_ele->GetAttribute("Diffuse",Diffuse);
    result &= p_ele->GetAttribute("TessellationQuality",TessellationQuality);

    if( result == false ) {
        LOGIC_ERROR("unable to load bond model setup");
    }
}

//------------------------------------------------------------------------------

void CBondModelSettings::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "bond" ) {
        LOGIC_ERROR("element is not bond");
    }

    p_ele->SetAttribute("Type",Type);
    p_ele->SetAttribute("Radius",Radius);
    p_ele->SetAttribute("Pitch",Pitch);
    p_ele->SetAttribute("ShowOrder",ShowOrder);
    p_ele->SetAttribute("Diffuse",Diffuse);
    p_ele->SetAttribute("TessellationQuality",TessellationQuality);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CModelSettings::CModelSettings(void)
{
}

//------------------------------------------------------------------------------

void CModelSettings::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "model" ) {
        LOGIC_ERROR("element is not model");
    }

    CXMLElement* p_aele = p_ele->GetFirstChildElement("atom");
    if( p_aele != NULL ) {
        Atoms.LoadData(p_aele);
    }

    CXMLElement* p_bele = p_ele->GetFirstChildElement("bond");
    if( p_bele != NULL ) {
        Bonds.LoadData(p_bele);
    }

}

//------------------------------------------------------------------------------

void CModelSettings::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "model" ) {
        LOGIC_ERROR("element is not model");
    }

    CXMLElement* p_aele = p_ele->CreateChildElement("atom");
    Atoms.SaveData(p_aele);

    CXMLElement* p_bele = p_ele->CreateChildElement("bond");
    Bonds.SaveData(p_bele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

