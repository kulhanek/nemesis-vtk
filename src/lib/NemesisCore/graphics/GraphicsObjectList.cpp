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

#include <GraphicsObjectList.hpp>
#include <GraphicsProfileList.hpp>
#include <ErrorSystem.hpp>
#include <PluginDatabase.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <XMLElement.hpp>
#include <Graphics.hpp>
#include <GraphicsObjectHistory.hpp>
#include <GraphicsObjectLink.hpp>
#include <GraphicsObjectEnd.hpp>
#include <StructureList.hpp>
#include <GraphicsProfileObject.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        GraphicsObjectListID(
                    "{GRAPHICS_OBJECT_LIST:07c60ac0-2919-45f6-ad87-5b7e43b1aa38}",
                    "Objects");

CPluginObject   GraphicsObjectListObject(&NemesisCorePlugin,
                    GraphicsObjectListID,GRAPHICS_CAT,
                    ":/images/NemesisCore/graphics/ObjectList.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsObjectList::CGraphicsObjectList(CGraphics* p_owner,bool no_index)
    : CProObject(&GraphicsObjectListObject,p_owner,p_owner->GetProject(),no_index)
{  
    SetFlag(EPOF_RO_NAME,true);
}

//------------------------------------------------------------------------------

CGraphicsObjectList::~CGraphicsObjectList(void)
{
    foreach(QObject* p_qobj,children()){
        if( p_qobj ){
            p_qobj->setParent(NULL);
            delete p_qobj;
        }
    }
    EmitOnGraphicsObjectListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsObject* CGraphicsObjectList::CreateObjectWH(
                                            const CUUID& objectuuid,
                                            const QString& name,
                                            const QString& descr)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("create a new graphics object"));
    if( p_history == NULL ) return (NULL);

    CGraphicsObject* p_go = NULL;
    p_go = CreateObject(objectuuid,name,descr,p_history);

    EndChangeWH();
    return(p_go);
}

//------------------------------------------------------------------------------

bool CGraphicsObjectList::HideAllObjectsWH(const CUUID& objectuuid)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("hide all objects of given type"));
    if( p_history == NULL ) return (false);

    foreach(QObject* p_qobj,children()) {
        CGraphicsObject* p_go = static_cast<CGraphicsObject*>(p_qobj);
        if( p_go->GetType() != objectuuid ) continue;
        if( p_go->IsFlagSet(EPOF_VISIBLE) == false ) continue;
        CProObjectFlags flags = p_go->GetFlags();
        SET_FLAG(flags,EPOF_VISIBLE,false);
        p_go->SetFlagsWH(flags);
    }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CGraphicsObjectList::ShowAllObjectsWH(const CUUID& objectuuid)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("show all objects of given type"));
    if( p_history == NULL ) return (false);

    foreach(QObject* p_qobj,children()) {
        CGraphicsObject* p_go = static_cast<CGraphicsObject*>(p_qobj);
        if( p_go->GetType() != objectuuid ) continue;
        if( p_go->IsFlagSet(EPOF_VISIBLE) == true ) continue;
        CProObjectFlags flags = p_go->GetFlags();
        SET_FLAG(flags,EPOF_VISIBLE,true);
        p_go->SetFlagsWH(flags);
    }

    EndChangeWH();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsObject* CGraphicsObjectList::CreateObject(
                                            const CUUID& objectuuid,
                                            const QString& name,
                                            const QString& descr,
                                            CHistoryNode* p_history)
{
    // create object
    CGraphicsObject* p_go = static_cast<CGraphicsObject*>(PluginDatabase.CreateObject(objectuuid,this));

    if( p_go == NULL ) {
        CSmallString error;
        error << "unable to create graphics object " << objectuuid.GetStringForm();
        ES_ERROR(error);
        return(NULL);
    }

    QString obj_name = name;
    if( obj_name.isEmpty() ){
        CPluginObject* p_object = PluginDatabase.FindPluginObject(objectuuid);
        if( p_object != NULL ){
            obj_name = GenerateName(p_object->GetName() + " %1");
        }
    } else {
        // dont allow add two same names
        obj_name = GenerateName(name + " %1");
    }

    // setup general properties
    p_go->SetName(obj_name);
    p_go->SetDescription(descr);

    // register change to history
    if( p_history != NULL ){
        CGraphicsObjectHI* p_godata = new CGraphicsObjectHI(p_go,EHID_FORWARD);
        p_history->Register(p_godata);
    }

    // notify list models
    emit OnGraphicsObjectListChanged();

    return(p_go);
}

//------------------------------------------------------------------------------

CGraphicsObject* CGraphicsObjectList::CreateObject(CXMLElement* p_data,
                                                    CHistoryNode* p_history)
{
    if( p_data == NULL ) return(NULL);

    QString suuid;
    if( p_data->GetAttribute("uuid",suuid) == false ) {
        ES_ERROR("unable to get uuid attribute");
        return(NULL);
    }
    CExtUUID  ext_uuid;
    if( ext_uuid.LoadFromString(suuid) == false ) {
        CSmallString error;
        error << "unable to decode uuid attribute " << suuid;
        ES_ERROR(error);
        return(NULL);
    }

    // create object
    CGraphicsObject* p_go = static_cast<CGraphicsObject*>(PluginDatabase.CreateObject(ext_uuid,this));

    if( p_go == NULL ) {
        CSmallString error;
        error << "unable to create graphics object " << ext_uuid.GetStringForm();
        ES_ERROR(error);
        return(NULL);
    }

    // load data
    GetProject()->BeginLinkProcedure(0);
    p_go->LoadData(p_data);
    GetProject()->EndLinkProcedure();

    // register change to history
    if( p_history != NULL ){
        CGraphicsObjectHI* p_godata = new CGraphicsObjectHI(p_go,EHID_FORWARD);
        p_history->Register(p_godata);
    }

    // notify list models
    emit OnGraphicsObjectListChanged();

    return(p_go);
}

//------------------------------------------------------------------------------

CGraphicsObject* CGraphicsObjectList::CreateObject(CGraphicsObjectEnd* p_end,
                                                   CHistoryNode* p_history)
{
    // create object
    CGraphicsObject* p_go = new CGraphicsObjectLink(this,p_end);

    if( p_end->GetEndObject() ){
        p_go->SetName(p_end->GetEndObject()->GetName());
    }

    // register change to history
    if( p_history != NULL ){
        CGraphicsObjectHI* p_godata = new CGraphicsObjectHI(p_go,EHID_FORWARD);
        p_history->Register(p_godata);
    }

    // notify list models
    emit OnGraphicsObjectListChanged();

    return(p_go);
}

//------------------------------------------------------------------------------

void CGraphicsObjectList::ClearObjects(void)
{
    foreach(QObject* p_object,children()){
        delete p_object;
    }
    emit OnGraphicsObjectListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphics*  CGraphicsObjectList::GetGraphics(void)
{
    return(dynamic_cast<CGraphics*>(parent()));
}

//------------------------------------------------------------------------------

CGraphicsProfileList*  CGraphicsObjectList::GetProfiles(void)
{
    return( GetGraphics()->GetProfiles() );
}

//------------------------------------------------------------------------------

CGraphicsObject* CGraphicsObjectList::GetPrimaryObject(const CUUID& objectuuid)
{
    foreach(QObject* p_qobj,children()) {
        CGraphicsObject* p_pgo;
        p_pgo = static_cast<CGraphicsObject*>(p_qobj);

        // filter out non-compatible objects
        if( p_pgo->GetType() != objectuuid ) continue;

        // do we have active profile?
        CGraphicsProfile* p_profile = GetGraphics()->GetProfiles()->GetActiveProfile();
        if( p_profile == NULL ) continue;

        // is object in the active profile?
        if( p_profile->IsInProfile(p_pgo) == false ) continue;

        // does it contains all structures
        if( p_pgo->ContainsObject(GetProject()->GetStructures()) == false ) continue;

        // is visible?
        if( p_pgo->IsFlagSet(EPOF_VISIBLE) == false ) continue;

        // is visible in profile?
        CGraphicsProfileObject* p_gpobj = p_profile->GetGraphicsProfileObject(p_pgo);
        if( p_gpobj == NULL ) continue;
        if( p_gpobj->IsFlagSet(EPOF_VISIBLE) == false ) continue;

        // return the object
        return(p_pgo);
    }

    return(NULL);
}

//------------------------------------------------------------------------------

void CGraphicsObjectList::EmitOnGraphicsObjectListChanged(void)
{
    emit OnGraphicsObjectListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsObjectList::LoadData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "objects" ) {
        LOGIC_ERROR("p_ele is not 'objects'");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    CXMLElement* p_goele;
    p_goele = p_ele->GetFirstChildElement("object");

    while( p_goele != NULL ) {
        CreateObject(p_goele);
        p_goele = p_goele->GetNextSiblingElement("object");
    }
}

//------------------------------------------------------------------------------

void CGraphicsObjectList::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "objects" ) {
        LOGIC_ERROR("p_ele is not 'objects'");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    foreach(QObject* p_qobj,children()) {
        CGraphicsObject* p_go = static_cast<CGraphicsObject*>(p_qobj);
        CXMLElement* p_goele = p_ele->CreateChildElement("object");
        p_go->SaveData(p_goele);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
