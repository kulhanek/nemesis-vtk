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

#include <GraphicsProfile.hpp>
#include <NemesisCoreModule.hpp>
#include <GraphicsProfileList.hpp>
#include <GraphicsObjectList.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <XMLElement.hpp>
#include <Graphics.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GraphicsProfileObject.hpp>
#include <HistoryNode.hpp>
#include <GraphicsProfileHistory.hpp>
#include <QtOpenGL>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        GraphicsProfileID(
                    "{GRAPHICS_PROFILE:0ab66d74-4cd6-4788-b513-251c45700e78}",
                    "Graphics profile");

CPluginObject   GraphicsProfileObject(&NemesisCorePlugin,
                    GraphicsProfileID,GRAPHICS_CAT,NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsProfile::CGraphicsProfile(CGraphicsProfileList* p_bl)
    : CProObject(&GraphicsProfileObject,p_bl,p_bl->GetProject())
{    
    FirstObject = NULL;
}

//------------------------------------------------------------------------------

CGraphicsProfile::~CGraphicsProfile(void)
{
    foreach(QObject* p_qobj,children()){
        if( p_qobj ){
            p_qobj->setParent(NULL);
            delete p_qobj;
        }
    }
    EmitOnGraphicsProfileChanged();

    CGraphicsProfileList* p_list = GetProfiles();
    if( p_list ){
        setParent(NULL);    // remove object from the list
        p_list->EmitChildContainerRemoved(this);
        p_list->EmmitGraphicsProfileRemovedSignal(this);
    }
}

//------------------------------------------------------------------------------

bool CGraphicsProfile::RemoveFromBaseList(CHistoryNode* p_history)
{
    bool result = true;

    // unregister registered objects
    CProObject* p_obj;
    while( (p_obj = RemoveFirstFromROList()) !=NULL ) {
        p_obj->RemoveFromRegistered(this,p_history);
    }

    // clean registration for all objects
    CGraphicsProfileObject* p_gpo = FirstObject;
    while(p_gpo != NULL){
        CProObject* p_obj = p_gpo->GetObject();
        // remove registration
        p_obj->UnregisterObject(this);
        p_gpo = p_gpo->NextObject;
    }

    // register elementary change to history list
    if( p_history != NULL ) {
        CGraphicsProfileHI* p_item = new CGraphicsProfileHI(this,EHID_BACKWARD);
        p_history->Register(p_item);
    }

    // --------------------------------
    delete this;  // destroy object

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CGraphicsProfile::ActivateProfileWH(void)
{
    CHistoryNode*  p_history = BeginChangeWH(EHCL_GRAPHICS,tr("activate the profile"));
    if( p_history == NULL ) return (false);

    GetProfiles()->SetActiveProfile(this,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CGraphicsProfile::DeleteProfileWH(void)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("delete the profile"));
    if( p_history == NULL ) return (false);

    // set other active profile if the removed profile is the active one
    if( IsProfileActive() ){
        CGraphicsProfile* p_profile = static_cast<CGraphicsProfile*>(GetProfiles()->children().first());
        if(  p_profile !=  this ){
            GetProfiles()->SetActiveProfile(p_profile,p_history);
        } else {
            if( GetProfiles()->children().count() > 1 ){
                p_profile = static_cast<CGraphicsProfile*>(GetProfiles()->children()[1]);
                GetProfiles()->SetActiveProfile(p_profile,p_history);
            } else {
                GetProfiles()->SetActiveProfile(NULL,p_history);
            }
        }
    }

    // remove profile
    bool result = RemoveFromBaseList(p_history);

    EndChangeWH();
    return(result);
}

//------------------------------------------------------------------------------

CGraphicsObject* CGraphicsProfile::CreateObjectWH(
                    const CUUID& objectuuid,
                    const QString& name,
                    const QString& descr)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("create graphics object"));
    if( p_history == NULL ) return (NULL);

    CGraphicsObject* p_object = GetObjects()->CreateObject(objectuuid,name,descr,p_history);
    if( p_object != NULL ){
        AddObject(p_object,-1,p_history);
    }

    EndChangeWH();
    return(p_object);
}

//------------------------------------------------------------------------------

bool CGraphicsProfile::AddObjectWH(CGraphicsObject* p_object)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("add the object to the profile"));
    if( p_history == NULL ) return (false);

    bool result = AddObject(p_object,-1,p_history);

    EndChangeWH();
    return(result);
}

//------------------------------------------------------------------------------

bool CGraphicsProfile::RemoveObjectWH(CGraphicsObject* p_object)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("remove the object from the profile"));
    if( p_history == NULL ) return (false);

    bool result = RemoveObject(p_object,p_history);

    EndChangeWH();
    return(result);
}

//------------------------------------------------------------------------------

bool CGraphicsProfile::MoveObjectWH(CGraphicsObject* p_object,
                                    EMoveDirection direction)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("move the object in the profile"));
    if( p_history == NULL ) return (false);

    bool result = MoveObject(p_object,direction,p_history);

    EndChangeWH();
    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsProfileObject* CGraphicsProfile::AddObject(CGraphicsObject* p_object,
                                                    int position,
                                                    CHistoryNode* p_history)
{
    if( p_object == NULL ) {
        ES_ERROR("p_object is NULL");
        return(NULL);
    }

    // is the object already in the profile?
    if( IsInProfile(p_object) ){
        // ES_ERROR("p_object is already in the profile");
        // do not throw error into the error stack
        return(NULL);
    }

    // create CGraphicsProfileObject
    CGraphicsProfileObject* p_gpo = new CGraphicsProfileObject(this,p_object);

    if( (position == 0) || (FirstObject == NULL) ){
        p_gpo->NextObject = FirstObject;
        FirstObject = p_gpo;
    } else {
        // find previous object
        CGraphicsProfileObject* p_prev = FirstObject;
        int curr_pos = 1;
        while(p_prev != NULL){
            if( curr_pos == position ) break;
            curr_pos++;
            p_prev = p_prev->NextObject;
        }
        if( p_prev ){
            // add to given position
            p_gpo->NextObject = p_prev->NextObject;
            p_prev->NextObject = p_gpo;
        } else {
            // find last object
            CGraphicsProfileObject* p_last = FirstObject;
            while(p_last != NULL){
                if( p_last->NextObject ==  NULL ) break;
                p_last = p_last->NextObject;
            }
            // at at the end
            p_last->NextObject = p_gpo;
        }
    }

    // register profile for original object
    p_object->RegisterObject(this);

    if( p_history != NULL ) {
        CHistoryItem* p_item = new CGraphicsProfileAddObjectHI(this,p_object,position,EHID_FORWARD);
        p_history->Register(p_item);
    }

    emit OnGraphicsProfileChanged();

    return(p_gpo);
}

//------------------------------------------------------------------------------

CGraphicsProfileObject* CGraphicsProfile::AddObject(CXMLElement* p_ele,CHistoryNode* p_history)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        return(NULL);
    }

    int index = -1;
    p_ele->GetAttribute("oid",index);

    CGraphicsObject* p_object = static_cast<CGraphicsObject*>(GetProject()->FindObject(index));
    if( p_object == NULL ){
        ES_ERROR("unable to find object");
        return(NULL);
    }

    CGraphicsProfileObject* p_gobj =  AddObject(p_object,-1,p_history);
    if( p_gobj ){
        p_gobj->LoadData(p_ele);
    }
    return(p_gobj);
}

//------------------------------------------------------------------------------

bool CGraphicsProfile::RemoveObject(CGraphicsObject* p_object,CHistoryNode* p_history)
{
    if( p_object == NULL ) {
        ES_ERROR("p_object is NULL");
        return(false);
    }

    CGraphicsProfileObject* p_gpo = GetGraphicsProfileObject(p_object);
    if( p_gpo == NULL ){
        ES_ERROR("p_object is not in the profile");
        return(false);
    }

    // remove object from the list and determine its position
    int position = 0;
    if( FirstObject == p_gpo ){
        FirstObject = p_gpo->NextObject;
        position = 0;
    } else {
        CGraphicsProfileObject* p_prev = FirstObject;
        while( p_prev != NULL ){
            position++;
            if( p_prev->NextObject == p_gpo ) break;
            p_prev = p_prev->NextObject;
        }
        p_prev->NextObject = p_gpo->NextObject;
    }

    // unregister object
    p_object->UnregisterObject(this);

    // delete object - it emmits the OnGraphicsProfileObjectRemoved signal
    delete p_gpo;

    if( p_history != NULL ) {
        CHistoryItem* p_item = new CGraphicsProfileAddObjectHI(this,p_object,position,EHID_BACKWARD);
        p_history->Register(p_item);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CGraphicsProfile::MoveObject(CGraphicsObject* p_object,EMoveDirection direction,
                                  CHistoryNode* p_history)
{
    if( p_object == NULL ) {
        ES_ERROR("p_object is NULL");
        return(false);
    }

    CGraphicsProfileObject* p_gpo = GetGraphicsProfileObject(p_object);
    if( p_gpo == NULL ){
        ES_ERROR("p_object is not in the profile");
        return(false);
    }

    // find previous object
    CGraphicsProfileObject* p_prev = FirstObject;
    CGraphicsProfileObject* p_prevprev = NULL;
    if( FirstObject != p_gpo ){
        while(p_prev != NULL){
            if( p_prev->NextObject == p_gpo ) break;
            p_prevprev = p_prev;
            p_prev = p_prev->NextObject;
        }
    } else {
        p_prev = NULL;
    }

    if( direction == EMD_UP ){
        if( p_gpo == FirstObject ){
            ES_ERROR("object is already on top");
            return(false);
        }
        if( p_prevprev == NULL ){
            CGraphicsProfileObject* p_backup = FirstObject;
            FirstObject = p_gpo;
            p_backup->NextObject = p_gpo->NextObject;
            p_gpo->NextObject = p_backup;
        } else {
            CGraphicsProfileObject* p_backup = p_prevprev->NextObject;
            p_prevprev->NextObject = p_gpo;
            p_backup->NextObject = p_gpo->NextObject;
            p_gpo->NextObject = p_backup;
        }
    } else {
        if( p_gpo->NextObject == NULL ){
            ES_ERROR("object is already on bottom");
            return(false);
        }
        if( p_prev == NULL ){
            FirstObject = p_gpo->NextObject;
            p_gpo->NextObject = FirstObject->NextObject;
            FirstObject->NextObject = p_gpo;
        } else {
            p_prev->NextObject = p_gpo->NextObject;
            CGraphicsProfileObject* p_backup = p_gpo->NextObject->NextObject;
            p_gpo->NextObject->NextObject = p_gpo;
            p_gpo->NextObject = p_backup;
        }

    }

    // record to history
    if( p_history != NULL ) {
        CHistoryItem* p_item;
        if( direction == EMD_UP ){
            p_item = new CGraphicsProfileMoveObjectHI(this,p_object,EHID_FORWARD);
        } else {
            p_item = new CGraphicsProfileMoveObjectHI(this,p_object,EHID_BACKWARD);
        }
        p_history->Register(p_item);
    }

    emit OnGraphicsProfileChanged();

    return(true);
}

//------------------------------------------------------------------------------

void CGraphicsProfile::SetName(const QString& name,CHistoryNode* p_history)
{
    CProObject::SetName(name,p_history);
    if( GetProfiles() ) GetProfiles()->EmitOnGraphicsProfileListChanged();
}

//------------------------------------------------------------------------------

void CGraphicsProfile::SetDescription(const QString& descrip,CHistoryNode* p_history)
{
    CProObject::SetDescription(descrip,p_history);
    if( GetProfiles() ) GetProfiles()->EmitOnGraphicsProfileListChanged();
}

//------------------------------------------------------------------------------

void CGraphicsProfile::RemoveFromRegistered(CProObject* p_object,
                                            CHistoryNode* p_history)
{
    // is it graphical object?
    CGraphicsObject* p_gpo = dynamic_cast<CGraphicsObject*>(p_object);
    if( p_gpo ) {
        RemoveObject(p_gpo,p_history);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsProfileList* CGraphicsProfile::GetProfiles(void) const
{
    return(dynamic_cast<CGraphicsProfileList*>(parent()));
}

//------------------------------------------------------------------------------

CGraphicsObjectList* CGraphicsProfile::GetObjects(void) const
{
    if( GetProfiles() == NULL ) return(NULL);
    return(GetProfiles()->GetGraphics()->GetObjects());
}

//------------------------------------------------------------------------------

int CGraphicsProfile::GetNumberOfGraphicsObject(void) const
{
    return(children().count());
}

//------------------------------------------------------------------------------

CGraphicsObject* CGraphicsProfile::GetGraphicsObject(int index) const
{
    CGraphicsProfileObject* p_gpo = GetGraphicsProfileObject(index);
    if( p_gpo == NULL ) return(NULL);
    return( p_gpo->GetObject() );
}

//------------------------------------------------------------------------------

CGraphicsProfileObject* CGraphicsProfile::GetGraphicsProfileObject(CGraphicsObject* p_obj) const
{
    CGraphicsProfileObject* p_gpo = FirstObject;
    while(p_gpo != NULL){
        if( p_gpo->GetObject() == p_obj ) return(p_gpo);
        p_gpo = p_gpo->NextObject;
    }
    return(NULL);
}

//------------------------------------------------------------------------------

CGraphicsProfileObject* CGraphicsProfile::GetGraphicsProfileObject(int index) const
{
    CGraphicsProfileObject* p_gpo = FirstObject;
    int i = 0;
    while(p_gpo != NULL){
        if( i == index ) return(p_gpo);
        i++;
        p_gpo = p_gpo->NextObject;
    }
    return(NULL);
}

//------------------------------------------------------------------------------

void CGraphicsProfile::GetSceneMetrics(CObjMetrics& metrics)
{   
    foreach(QObject* p_qobj,children()) {
        CGraphicsProfileObject* p_gpo = dynamic_cast<CGraphicsProfileObject*>(p_qobj);
        CGraphicsObject* p_go = p_gpo->GetObject();
        if( p_gpo->IsFlagSet(EPOF_VISIBLE) && p_go->IsFlagSet(EPOF_VISIBLE) ){
            p_go->GetObjectMetrics(metrics);
        }
    }
}

//------------------------------------------------------------------------------

bool CGraphicsProfile::IsProfileActive(void) const
{
    return( GetProfiles()->GetActiveProfile() == this );
}

//------------------------------------------------------------------------------

bool CGraphicsProfile::IsFirstObject(CGraphicsObject* p_object) const
{
    if( FirstObject == NULL ) return(false);
    return( FirstObject->GetObject() == p_object );
}

//------------------------------------------------------------------------------

bool CGraphicsProfile::IsLastObject(CGraphicsObject* p_object) const
{
    if( FirstObject == NULL ) return(false);
    // find last object
    CGraphicsProfileObject* p_last = FirstObject;
    while(p_last != NULL){
        if( p_last->NextObject ==  NULL ) break;
        p_last = p_last->NextObject;
    }
    return( p_last->GetObject() == p_object );
}

//------------------------------------------------------------------------------

bool CGraphicsProfile::IsInProfile(CGraphicsObject* p_obj)
{
    // is the object already in the profile?
    CGraphicsProfileObject* p_gpo = FirstObject;
    while(p_gpo != NULL){
        if( p_gpo->GetObject() == p_obj ){
            return(true);
        }
        p_gpo = p_gpo->NextObject;
    }

    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsProfile::LoadData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "profile" ) {
        LOGIC_ERROR("p_ele is not profile");
    }

    if( GetObjects() == NULL ) {
        LOGIC_ERROR("GetObjects is NULL");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    // load data ------------------------------------
    CXMLElement* p_oele = p_ele->GetFirstChildElement("objects");
    if( p_oele == NULL ) {
        LOGIC_ERROR("unable to open objects element");
    }

    CXMLElement* p_goele;
    p_goele = p_oele->GetFirstChildElement("object");

    while( p_goele != NULL ) {
        AddObject(p_goele);
        p_goele = p_goele->GetNextSiblingElement("object");
    }
}

//------------------------------------------------------------------------------

void CGraphicsProfile::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "profile" ) {
        LOGIC_ERROR("p_ele is not profile");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele) ;

    // save data ------------------------------------
    CXMLElement* p_oele = p_ele->CreateChildElement("objects");

    CGraphicsProfileObject* p_gpo = FirstObject;
    while(p_gpo != NULL){
        CXMLElement* p_goele = p_oele->CreateChildElement("object");
        p_gpo->SaveData(p_goele);
        p_gpo = p_gpo->NextObject;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsProfile::Draw(void)
{
    if( FirstObject == NULL ) {
        glClearColor(0.0,0.0,0.0,0.0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }

    // disable all clipping planes
    for(int i=0; i < GL_MAX_CLIP_PLANES; i++){
        glDisable(GL_CLIP_PLANE0+i);
    }

    // normal scene draw
    CGraphicsProfileObject* p_gpo = FirstObject;
    while(p_gpo != NULL){
        if( p_gpo->IsFlagSet(EPOF_VISIBLE) && p_gpo->GetObject()->IsFlagSet(EPOF_VISIBLE) ){
            p_gpo->GetObject()->Draw();
        }
        p_gpo = p_gpo->NextObject;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsProfile::EmitOnGraphicsProfileChanged(void)
{
    emit OnGraphicsProfileChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
