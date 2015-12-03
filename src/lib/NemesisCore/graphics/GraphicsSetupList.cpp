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

#include <GraphicsSetupList.hpp>
#include <TemplIterator.hpp>
#include <PluginDatabase.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <Graphics.hpp>
#include <GraphicsSetupProfile.hpp>
#include <StaticIndexes.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        GraphicSetupListID(
                    "{GRAPHICS_SETUP_LIST:553dd8be-3ee0-4db0-97c7-7b28af1a1968}",
                    "Setup");

CPluginObject   GraphicSetupListObject(&NemesisCorePlugin,
                    GraphicSetupListID,GRAPHICS_CAT,
                    ":/images/NemesisCore/graphics/SetupList.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsSetupList::CGraphicsSetupList(CGraphics* p_owner,bool no_index)
    : CProObject(&GraphicSetupListObject,p_owner,p_owner->GetProject(),no_index)
{
    ConFlags |= EECOF_SUB_CONTAINERS;
}

//------------------------------------------------------------------------------

CGraphicsSetupList::CGraphicsSetupList(CPluginObject* p_object,CExtComObject* p_parent)
    : CProObject(p_object,p_parent,NULL)
{
}

//------------------------------------------------------------------------------

CGraphicsSetupList::~CGraphicsSetupList(void)
{
    foreach(QObject* p_qobj,children()){
        if( p_qobj ){
            p_qobj->setParent(NULL);
            delete p_qobj;
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsSetup* CGraphicsSetupList::FindSetup(const CUUID& object_type,int object_id)
{
    CUUID setup_type;
    FindSetupType(object_type,setup_type);

    foreach(QObject* p_qobj,children()) {
        if( p_qobj == NULL ) continue; // due to deleteChildren
        CGraphicsSetup* p_setup = static_cast<CGraphicsSetup*>(p_qobj);
        if( object_id > 0 ){
            if( p_setup->GetObjectID() == object_id ){
                if( p_setup->GetType() != setup_type ){
                    LOGIC_ERROR("inconsistent graphics object setup");
                }
                return(p_setup);
            }
        } else {
            if( (p_setup->GetType() == setup_type) && (p_setup->GetObjectID() == -1) ) return(p_setup);
        }
    }

    return(NULL);
}

//------------------------------------------------------------------------------

void CGraphicsSetupList::FindSetupType(const CUUID& objecttype,CUUID& setuptype)
{
    setuptype.Clear();
    CPluginObject* p_pobj = PluginDatabase.FindPluginObject(objecttype);
    if( p_pobj == NULL ) return;
    p_pobj->FindObjectConfigValue("_setup",setuptype);
}

//------------------------------------------------------------------------------

void CGraphicsSetupList::RemoveSetup(CGraphicsSetup* p_setup)
{
    delete p_setup;
}

//------------------------------------------------------------------------------

CGraphicsSetup* CGraphicsSetupList::CreateSetup(const CUUID& object_type, int object_id)
{
    // find global setup
    CGraphicsSetup* p_setup = GraphicsSetupProfile->FindSetup(object_type);
    if( p_setup == NULL ){
        LOGIC_ERROR("project/object setup for type that does not have global setup");
    }

    CGraphicsSetup* p_new_setup = static_cast<CGraphicsSetup*>(PluginDatabase.CreateObject(p_setup->GetType(),this));
    if( p_new_setup == NULL ){
        LOGIC_ERROR("unable to create setup for the same type as global setup has");
    }

    if( object_id > 0 ){
        p_new_setup->ObjectID = object_id;
    }

    // copy setup
    // TODO
    return(p_new_setup);
}

//------------------------------------------------------------------------------

void CGraphicsSetupList::ClearSetups(void)
{
    foreach(QObject* p_object,children()){
        if( p_object == NULL ) continue;
        delete p_object;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsSetupList::Load(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "setups" ){
        LOGIC_ERROR("p_ele is not setups");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    // load data ------------------------------------
    CXMLElement* p_ael;
    p_ael = p_ele->GetFirstChildElement("setup");

    while( p_ael != NULL ){
        // create setup
        CExtUUID  ext_uuid;
        ext_uuid.GetValue(p_ael,"uuid");

        CExtComObject* p_obj;
        if( (p_obj = static_cast<CExtComObject*>(PluginDatabase.CreateObject(ext_uuid,this)) ) == NULL ){
            CSmallString error;
            error << "unable to create object with uuid=" << ext_uuid.GetFullStringForm();
            RUNTIME_ERROR(error);
        }

        CGraphicsSetup* p_setup;
        p_setup = dynamic_cast<CGraphicsSetup*>(p_obj);

        if( p_setup == NULL ){
            delete p_obj;
            CSmallString error;
            error << "object is not graphics setup (" << ext_uuid.GetFullStringForm() << ")";
            RUNTIME_ERROR(error);
        }
        p_setup->Load(p_ael);

        p_ael = p_ael->GetNextSiblingElement("setup");
    }
}

// -----------------------------------------------------------------------------

void CGraphicsSetupList::Save(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "setups" ){
        RUNTIME_ERROR("p_ele is not setups");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    // save data -----------------------------------
    foreach(QObject* p_qobj,children()) {
        if( p_qobj == NULL ) continue;
        CGraphicsSetup* p_setup = static_cast<CGraphicsSetup*>(p_qobj);
        CXMLElement* p_ael = p_ele->CreateChildElement("setup");
        p_setup->Save(p_ael);
    }
}

// -----------------------------------------------------------------------------

CGraphicsSetup* CGraphicsSetupList::FindSetupByType(const CUUID& setup_type)
{
    foreach(QObject* p_qobj,children()) {
        CGraphicsSetup* p_setup = dynamic_cast<CGraphicsSetup*>(p_qobj);
        if( p_setup == NULL ) continue;
        if( p_setup->GetType() == setup_type ) return(p_setup);
    }
    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




