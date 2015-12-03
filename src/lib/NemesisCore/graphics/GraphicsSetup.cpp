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

#include <GraphicsSetup.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <GraphicsSetupProfile.hpp>
#include <GraphicsObject.hpp>
#include <ProObject.hpp>

#include "GraphicsSetup.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

 CGraphicsSetup::CGraphicsSetup(CPluginObject* p_objectinfo, CProObject* p_owner)
    : CProObject(p_objectinfo,p_owner,p_owner->GetProject())
{
    ObjectID = -1;
    if( GetProject() == NULL ) {
        SetName(QString(GetType().GetName()));
        SetFlag(EPOF_RO_NAME,true);
    }
}

 //------------------------------------------------------------------------------

CGraphicsSetup::~CGraphicsSetup(void)
{
    // now inform all registered objects about the change
    foreach(CProObject* p_obj, GetROList() ){
        CGraphicsObject* p_gobj = dynamic_cast<CGraphicsObject*>(p_obj);
        if( p_gobj != NULL ){
            p_gobj->SetupChanged();
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void  CGraphicsSetup::Load(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "setup" ) {
        LOGIC_ERROR("p_ele is not setup");
    }

    // load base data ----------------------------
    CProObject::LoadData(p_ele);

    // uuid is restored by PluSuLa
    // p_ele->GetAttribute("uuid",suuid)
    // object_uuid is restored by given object setup creator
    // p_ele->GetAttribute("object_uuid",suuid)

    ObjectID = -1;
    p_ele->GetAttribute("object_id",ObjectID);

    // load setup data ---------------------------
    CXMLElement* p_oele = p_ele->GetFirstChildElement("data");
    if( p_oele == NULL ) {
        CSmallString error;
        error << "unable to open data element (" << GetType().GetFullStringForm() << ")";
        LOGIC_ERROR(error);
    }
    LoadData(p_oele);

    // notify about update all partners
    // emit local signal
    emit OnSetupChanged();

    // and also for all registered graphics objects
    foreach( CProObject* p_obj, GetROList() ){
        CGraphicsObject* p_gobj = dynamic_cast<CGraphicsObject*>(p_obj);
        if( p_gobj ){
            p_gobj->SetupChanged();
        }
    }
}

//------------------------------------------------------------------------------

void CGraphicsSetup::Save(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "setup" ) {
        LOGIC_ERROR("p_ele is not setup");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    // save data ------------------------------------

    // uuid is required for PluSuLa object creation
    GetType().SetValue(p_ele,"uuid");

    // object_uuid is restored by given object setup creator thus
    // we do not need to save it
    // p_ele->SetAttribute("object_uuid",suuid)

    p_ele->SetAttribute("object_id",ObjectID);

    CXMLElement* p_oele = p_ele->CreateChildElement("data");
    SaveData(p_oele);
}
// -----------------------------------------------------------------------------

void CGraphicsSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }
}

// -----------------------------------------------------------------------------

void CGraphicsSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsSetup::LoadUserDefault(void)
{
    GraphicsSetupProfile->LoadUserDefault(this);
}

// -----------------------------------------------------------------------------

void CGraphicsSetup::LoadSystemDefault(void)
{
    GraphicsSetupProfile->LoadSystemDefault(this);
}

// -----------------------------------------------------------------------------

void CGraphicsSetup::EmitOnSetupChanged(void)
{
    emit OnSetupChanged();

    // now inform all registered objects about the change
    foreach(CProObject* p_obj, GetROList() ){
        CGraphicsObject* p_gobj = dynamic_cast<CGraphicsObject*>(p_obj);
        if( p_gobj != NULL ){
            p_gobj->SetupChanged();
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

EGraphicsSetupMode  CGraphicsSetup::GetSetupMode(void) const
{
    if( GetObjectID() > 0 ) {
        return(EGSM_OBJECT_SETUP);
    } else {
        if( GetProject() != NULL ) {
            return(EGSM_PROJECT_SETUP);
        } else {
            return(EGSM_GLOBAL_SETUP);
        }
    }
}

//------------------------------------------------------------------------------

int  CGraphicsSetup::GetObjectID(void) const
{
    return(ObjectID);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================





