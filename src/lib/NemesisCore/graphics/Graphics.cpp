// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

#include <Graphics.hpp>
#include <ErrorSystem.hpp>
#include <PluginDatabase.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <XMLElement.hpp>
#include <Project.hpp>
#include <StandardModelObject.hpp>
#include <GraphicsObjectList.hpp>
#include <GraphicsProfileList.hpp>
#include <GraphicsViewList.hpp>
#include <GraphicsSetupList.hpp>
#include <StaticIndexes.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        GraphicsID(
                    "{GRAPHICS:32bbf75b-edc0-4321-a188-a7dd37cd034f}",
                    "Graphics");

CPluginObject   GraphicsObject(&NemesisCorePlugin,
                    GraphicsID,GRAPHICS_CAT,
                    ":/images/NemesisCore/graphics/Graphics.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphics::CGraphics(CProject* p_owner,bool no_index)
    : CProObject(&GraphicsObject,p_owner,p_owner,no_index)
{
    ConFlags |= EECOF_SUB_CONTAINERS;

    Objects = new CGraphicsObjectList(this,no_index);
    Profiles = new CGraphicsProfileList(this,no_index);
    Views = new CGraphicsViewList(this,no_index);
    Setups = new CGraphicsSetupList(this,no_index);
}

//------------------------------------------------------------------------------

CGraphics::~CGraphics(void)
{
    foreach(QObject* p_qobj,children()){
        if( p_qobj ){
            p_qobj->setParent(NULL);
            delete p_qobj;
        }
    }
}

//------------------------------------------------------------------------------

void CGraphics::SetStaticIndexes(void)
{
    SetIndex(GRAPHICS_INDEX);

    Objects->SetIndex(GRAPHICS_OBJECTS_INDEX);
    Profiles->SetIndex(GRAPHICS_PROFILES_INDEX);
    Views->SetIndex(GRAPHICS_VIEWS_INDEX);
    Views->GetPrimaryView()->SetIndex(GRAPHICS_PRIMARY_VIEW_INDEX);
    Setups->SetIndex(GRAPHICS_SETUPS_INDEX);
}

//------------------------------------------------------------------------------

void CGraphics::ClearGraphics(void)
{
    Profiles->ClearProfiles();
    Objects->ClearObjects();
    Setups->ClearSetups();
    Views->ClearViews();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsObjectList* CGraphics::GetObjects(void)
{
    return(Objects);
}

//------------------------------------------------------------------------------

CGraphicsProfileList* CGraphics::GetProfiles(void)
{
    return(Profiles);
}

//------------------------------------------------------------------------------

CGraphicsSetupList* CGraphics::GetSetups(void)
{
    return(Setups);
}

//------------------------------------------------------------------------------

CGraphicsViewList* CGraphics::GetViews(void)
{
    return(Views);
}

//------------------------------------------------------------------------------

CGraphicsView* CGraphics::GetPrimaryView(void)
{
    return(Views->GetPrimaryView());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphics::LoadData(CXMLElement* p_ele)
{
    // check input data --------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "graphics" ) {
        LOGIC_ERROR("p_ele is not 'graphics'");
    }

    // load object info --------------------------
    CProObject::LoadData(p_ele);

    // all graphics objects ----------------------
    CXMLElement* p_oele = p_ele->GetFirstChildElement("objects");
    if( p_oele == NULL ) {
        LOGIC_ERROR("unable to get 'objects' element");
    }
    Objects->LoadData(p_oele);

    // profiles ----------------------------------
    CXMLElement* p_pele = p_ele->GetFirstChildElement("profiles");
    if( p_pele == NULL ) {
        LOGIC_ERROR("unable to get 'profiles' element");
    }
    Profiles->LoadData(p_pele);

    // views -------------------------------------
    CXMLElement* p_vele = p_ele->GetFirstChildElement("views");
    if( p_vele != NULL ) {
        Views->LoadData(p_vele); // this is optional, compatibility with old versions
    }

    // setups ------------------------------------
    CXMLElement* p_sele = p_ele->GetFirstChildElement("setups");
    if( p_sele == NULL ) {
        LOGIC_ERROR("unable to get 'setups' element");
    }
    Setups->Load(p_sele);
}

//------------------------------------------------------------------------------

void CGraphics::SaveData(CXMLElement* p_ele)
{
    // check input data --------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "graphics" ) {
        LOGIC_ERROR("p_ele is not 'graphics'");
    }

    // save object info --------------------------
    CProObject::SaveData(p_ele);

    // all graphics objects ----------------------
    CXMLElement* p_oele = p_ele->CreateChildElement("objects");
    Objects->SaveData(p_oele);

    // profiles ----------------------------------
    CXMLElement* p_pele = p_ele->CreateChildElement("profiles");
    Profiles->SaveData(p_pele);

    // views -------------------------------------
    CXMLElement* p_vele = p_ele->CreateChildElement("views");
    Views->SaveData(p_vele);

    // profiles ----------------------------------
    CXMLElement* p_sele = p_ele->CreateChildElement("setups");
    Setups->Save(p_sele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
