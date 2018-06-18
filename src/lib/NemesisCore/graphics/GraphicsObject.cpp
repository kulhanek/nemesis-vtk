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

#include <GraphicsObject.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <Graphics.hpp>
#include <GraphicsObjectList.hpp>
#include <GraphicsObjectHistory.hpp>
#include <Project.hpp>
#include <SelectionList.hpp>
#include <ProObjectHistory.hpp>
#include <GraphicsSetupProfile.hpp>
#include <CategoryUUID.hpp>
#include <PluginObject.hpp>
#include <GraphicsViewList.hpp>

// -----------------------------------------------------------------------------

CExtUUID PrivateObjectID(
    "{PRIVATE_OBJECT:9b66ba34-3b16-4506-93b2-0d7a6742a3b4}",
    "Private object");

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsObject::CGraphicsObject(CPluginObject* p_objectinfo,
                                 CGraphicsObjectList* p_glist)
    : CProObject(p_objectinfo,p_glist,p_glist->GetProject())
{
    SetupMode = EGSM_GLOBAL_SETUP;
    Setup = NULL;
    UpdateSetupLink();
}

//------------------------------------------------------------------------------

CGraphicsObject::~CGraphicsObject(void)
{
    if( Setup ){
        // unregister GO for given setup
        Setup->UnregisterObject(this);
        Setup = NULL;
    }

    CGraphicsObjectList* p_list = GetObjects();
    if( p_list ) {
        setParent(NULL);    // remove object from the list
        p_list->EmitOnGraphicsObjectListChanged();
    }
}

//------------------------------------------------------------------------------

void CGraphicsObject::RemoveFromBaseList(CHistoryNode* p_history)
{
    CProObject* p_obj;

    // unregister other objects
    while( (p_obj = RemoveFirstFromROList()) !=NULL ) {
        p_obj->RemoveFromRegistered(this,p_history);
    }

    // register elementary change to history list
    if( p_history != NULL ) {
        CGraphicsObjectHI* p_atomdata = new CGraphicsObjectHI(this,EHID_BACKWARD);
        p_history->Register(p_atomdata);
    }

    // --------------------------------
    delete this;  // destroy object
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CGraphicsObject::DeleteWH(void)
{
    // object will be destroyed - use graphics list instead
    CGraphicsObjectList* p_list = GetObjects();

    CHistoryNode* p_history = p_list->BeginChangeWH(EHCL_GRAPHICS,"delete graphics object");
    if( p_history == NULL ) return (false);

    RemoveFromBaseList(p_history);

    p_list->EndChangeWH();
    return(true);
}

//---------------------------------------------------------------------------

bool CGraphicsObject::ToggleShowHideFlagWH(void)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"show/hide graphics object");
    if( p_history == NULL ) return (false);

    CProObjectFlags flags = GetFlags();
    if( flags.testFlag(EPOF_VISIBLE) ){
        flags = flags & (~ EPOF_VISIBLE);
    } else {
        flags |= EPOF_VISIBLE;
    }

    SetFlagsWH(flags);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CGraphicsObject::AddObjectWH(CProObject* p_object)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("add object to graphics object"));
    if( p_history == NULL ) return (false);
        AddObject(p_object,p_history);
    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CGraphicsObject::AddSelectedObjectsWH(void)
{
    int nsel = GetProject()->GetSelection()->NumOfSelectedObjects();
    if( nsel == 0 ) return(false);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("add selected objects to graphics object"));
    if( p_history == NULL ) return (false);
        AddSelectedObjects(p_history);
    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CGraphicsObject::RemoveObjectWH(CProObject* p_object)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("remove object from graphics object"));
    if( p_history == NULL ) return (false);
        RemoveObject(p_object,p_history);
    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CGraphicsObject::RemoveSelectedObjectsWH(void)
{
    int nsel = GetProject()->GetSelection()->NumOfSelectedObjects();
    if( nsel == 0 ) return(false);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("remove selected objects from graphics object"));
    if( p_history == NULL ) return (false);
        RemoveSelectedObjects(p_history);
    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CGraphicsObject::RemoveAllObjectsWH(void)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("remove all objects from graphics object"));
    if( p_history == NULL ) return (false);

        while( ! Objects.isEmpty() ){
            CProObject* p_object = static_cast<CProObject*>(Objects.first());
            RemoveObject(p_object,p_history);
        }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CGraphicsObject::SetFlagsWH(const CProObjectFlags& flags)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("change flags"));
    if( p_history == NULL ) return (false);
        SetFlags(flags,p_history);
    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

void CGraphicsObject::SetFlags(const CProObjectFlags& flags,CHistoryNode* p_history)
{
    CProObject::SetFlags(flags,p_history);
    CGraphicsObjectList* p_list = GetObjects();
    if( p_list ){
        p_list->EmitOnGraphicsObjectListChanged();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsObject::AddObject(CProObject* p_object,CHistoryNode* p_history)
{
    if( p_object == NULL ) {
        INVALID_ARGUMENT("p_object is NULL");
    }

    // can the object be added?
    if( CanBeObjectAdded(p_object) == false ) return;

    // is the object already in the list?
    if( ContainsObject(p_object) ){
        // do not do nothing
        return;
    }

    // add objects
    Objects.append(p_object);

    // register object for original object
    p_object->RegisterObject(this);

    if( p_history != NULL ) {
        CHistoryItem* p_item = new CGraphicsObjectAddProObjectHI(this,p_object,EHID_FORWARD);
        p_history->Register(p_item);
    }

    // notify about added object
    emit OnGraphicsObjectContentsChanged();
    ObjectListObjectAdded(p_object);
    ObjectListChanged();
}

//------------------------------------------------------------------------------

void CGraphicsObject::AddSelectedObjects(CHistoryNode* p_history)
{
    int nsel = GetProject()->GetSelection()->NumOfSelectedObjects();

    for(int i=0; i < nsel; i++){
        CProObject* p_sel = GetProject()->GetSelection()->GetSelectedObject(i);
        if( p_sel != NULL ){
            if( CanBeObjectAdded(p_sel) == false ) continue;
            AddObject(p_sel,p_history);
        }
    }
}

//------------------------------------------------------------------------------

void CGraphicsObject::RemoveObject(CProObject* p_object,CHistoryNode* p_history)
{
    if( p_object == NULL ) {
        INVALID_ARGUMENT("p_object is NULL");
    }

    if( ! Objects.contains(p_object) ){
        LOGIC_ERROR("list does not contain the object");
    }

    // remove object from the list
    Objects.removeOne(p_object);

    // unregister object
    p_object->UnregisterObject(this);

    if( p_history != NULL ) {
        CHistoryItem* p_item = new CGraphicsObjectAddProObjectHI(this,p_object,EHID_BACKWARD);
        p_history->Register(p_item);
    }

    // notify about removal
    emit OnGraphicsObjectContentsChanged();
    ObjectListObjectRemoved(p_object);
    ObjectListChanged();
}

//------------------------------------------------------------------------------

void CGraphicsObject::RemoveSelectedObjects(CHistoryNode* p_history)
{
    int nsel = GetProject()->GetSelection()->NumOfSelectedObjects();
    for(int i=0; i < nsel; i++){
        CProObject* p_sel = GetProject()->GetSelection()->GetSelectedObject(i);
        if( p_sel != NULL ){
            if( ContainsObject(p_sel) == false ) continue;
            RemoveObject(p_sel,p_history);
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsObject::SetGlobalSetup(void)
{
    SetupMode = EGSM_GLOBAL_SETUP;
    SetupChanged();
}

//------------------------------------------------------------------------------

void CGraphicsObject::SetProjectSetup(void)
{
    if( HasProjectSetup() == false ){
        // create project setup
        GetGraphics()->GetSetups()->CreateSetup(GetGraphicsType());
    }
    SetupMode = EGSM_PROJECT_SETUP;
    SetupChanged();
}

//------------------------------------------------------------------------------

void CGraphicsObject::RemoveProjectSetup(void)
{
    CGraphicsSetup* p_setup = GetGraphics()->GetSetups()->FindSetup(GetGraphicsType());
    if( p_setup != NULL ){
        GetGraphics()->GetSetups()->RemoveSetup(p_setup);
    }
}

//------------------------------------------------------------------------------

bool CGraphicsObject::HasProjectSetup(void)
{
    CGraphicsSetup* p_setup = GetGraphics()->GetSetups()->FindSetup(GetGraphicsType());
    return( p_setup != NULL );
}

//------------------------------------------------------------------------------

void CGraphicsObject::SetObjectSetup(void)
{
    if( HasObjectSetup() == false ){
        // create project setup
        GetGraphics()->GetSetups()->CreateSetup(GetGraphicsType(),GetIndex());
    }
    SetupMode = EGSM_OBJECT_SETUP;
    SetupChanged();
}

//------------------------------------------------------------------------------

void CGraphicsObject::RemoveObjectSetup(void)
{
    CGraphicsSetup* p_setup = GetGraphics()->GetSetups()->FindSetup(GetGraphicsType(),GetIndex());
    if( p_setup != NULL ){
        GetGraphics()->GetSetups()->RemoveSetup(p_setup);
    }
}

//------------------------------------------------------------------------------

bool CGraphicsObject::HasObjectSetup(void)
{
    CGraphicsSetup* p_setup = GetGraphics()->GetSetups()->FindSetup(GetGraphicsType(),GetIndex());
    return( p_setup != NULL );
}

//------------------------------------------------------------------------------

void CGraphicsObject::UpdateSetupLink(void)
{
    CGraphicsSetup* p_new_setup = NULL;

    // get new setup link
    switch(SetupMode){
        case EGSM_GLOBAL_SETUP:
            p_new_setup = GraphicsSetupProfile->FindSetup(GetGraphicsType());
            break;
        case EGSM_PROJECT_SETUP:
            p_new_setup = GetGraphics()->GetSetups()->FindSetup(GetGraphicsType());
            if( p_new_setup == NULL ){
                // not found - degrade setup mode
                p_new_setup = GraphicsSetupProfile->FindSetup(GetGraphicsType());
                SetupMode = EGSM_GLOBAL_SETUP;
            }
            break;
        case EGSM_OBJECT_SETUP:
            p_new_setup = GetGraphics()->GetSetups()->FindSetup(GetGraphicsType(),GetIndex());
            if( p_new_setup == NULL ){
                // not found - degrade setup mode
                p_new_setup = GetGraphics()->GetSetups()->FindSetup(GetGraphicsType());
                if( p_new_setup == NULL ){
                    // not found - degrade setup mode
                    p_new_setup = GraphicsSetupProfile->FindSetup(GetGraphicsType());
                    SetupMode = EGSM_GLOBAL_SETUP;
                } else {
                    SetupMode = EGSM_PROJECT_SETUP;
                }
            }
            break;
    }

    if( p_new_setup == Setup ) return;

    if( Setup ){
        // unregister GO for given setup
        Setup->UnregisterObject(this);
    }

    Setup = p_new_setup;

    if( Setup ){
        // register GO for given setup
        Setup->RegisterObject(this);
    }
}

//------------------------------------------------------------------------------

void CGraphicsObject::SetupChanged(void)
{
    // update setup link
    UpdateSetupLink();

    // and emit the signal
    emit OnSetupChanged();
}

//------------------------------------------------------------------------------

CGraphicsSetup* CGraphicsObject::GetSetup(void) const
{
    if( Setup == NULL ){
        LOGIC_ERROR("Setup is NULL");
    }
    return(Setup);
}

//------------------------------------------------------------------------------

const CExtUUID& CGraphicsObject::GetGraphicsType(void)
{
    return(GetType());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsObject::RemoveFromRegistered(CProObject* p_object,CHistoryNode* p_history)
{
    // remove it from the object
    if( Objects.contains(p_object) ){
        RemoveObject(p_object,p_history);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void  CGraphicsObject::Draw(void)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

CGraphicsObjectList* CGraphicsObject::GetObjects(void)
{
    return(dynamic_cast<CGraphicsObjectList*>(parent()));
}

//------------------------------------------------------------------------------

CGraphicsProfileList* CGraphicsObject::GetProfiles(void)
{
    return(GetGraphics()->GetProfiles());
}

//------------------------------------------------------------------------------

CGraphicsViewList* CGraphicsObject::GetViews(void)
{
    return(GetGraphics()->GetViews());
}

//------------------------------------------------------------------------------

CGraphics* CGraphicsObject::GetGraphics(void)
{
    return(GetObjects()->GetGraphics());
}

//------------------------------------------------------------------------------

bool CGraphicsObject::ContainsObject(CProObject* p_object)
{
    return( Objects.contains(p_object) );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CGraphicsObject::CanBeObjectAdded(CProObject* p_object)
{
    if( p_object == NULL ){
        INVALID_ARGUMENT("p_object == NULL");
    }

    // all private objects are allowed
    if( IsAllowedObjectType(PrivateObjectID) ) return(true);

    // invalid object means that none object can be added
    if( IsAllowedObjectType(InvalidObjectID) ) return(false);

    if (AllowedObjectTypes.size() == 0 ) return(true);

    bool allowed = false;

    allowed |= IsAllowedObjectType(p_object->GetType());
    allowed |= IsAllowedObjectType(p_object->GetPluginObject()->GetCategoryUUID());

    return( allowed );
}

//------------------------------------------------------------------------------

void CGraphicsObject::RegisterAllowedObjectType(const CExtUUID& type)
{
    AllowedObjectTypes.push_back(type);
}

//------------------------------------------------------------------------------

bool CGraphicsObject::IsAllowedObjectType(const CExtUUID& type)
{
    std::vector<CExtUUID>::iterator it = AllowedObjectTypes.begin();
    std::vector<CExtUUID>::iterator ie = AllowedObjectTypes.end();

    while( it != ie ){
        if( *it == type ){
            return(true);
        }
        it++;
    }

    return(false);
}

//------------------------------------------------------------------------------

std::vector<CExtUUID>::iterator CGraphicsObject::BeginAllowedObjectTypes(void)
{
    return(AllowedObjectTypes.begin());
}

//------------------------------------------------------------------------------

std::vector<CExtUUID>::iterator CGraphicsObject::EndAllowedObjectTypes(void)
{
    return(AllowedObjectTypes.end());
}

//------------------------------------------------------------------------------

void CGraphicsObject::ObjectListChanged(void)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CGraphicsObject::ObjectListObjectAdded(CProObject* p_obj)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CGraphicsObject::ObjectListObjectRemoved(CProObject* p_obj)
{
    // nothing to be here
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsObject::LoadData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "object" ) {
        LOGIC_ERROR("p_ele is not object");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    // load objects ---------------------------------
    CXMLElement* p_osele = p_ele->GetFirstChildElement("objects");
    CXMLElement* p_oele = NULL;
    if( p_osele ) {
        p_oele = p_osele->GetFirstChildElement("object");
    }
    while( p_oele ){
        GetProject()->RequestToLinkObject(this,p_oele,"id","object");
        p_oele = p_oele->GetNextSiblingElement("object");
    }

    // update setup link
    UpdateSetupLink();
}

//------------------------------------------------------------------------------

void CGraphicsObject::LinkObject(const QString& role,CProObject* p_object)
{
    if( role == "object" ){
        AddObject(p_object);
    }
}

//------------------------------------------------------------------------------

void CGraphicsObject::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "object" ) {
        LOGIC_ERROR("p_ele is not object");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    // save data ------------------------------------
    p_ele->SetAttribute("uuid", GetType().GetFullStringForm());

    // save objects ---------------------------------
    CXMLElement* p_osele = p_ele->CreateChildElement("objects");

    foreach(CProObject* p_obj, Objects){
        CXMLElement* p_oele = p_osele->CreateChildElement("object");
        p_oele->SetAttribute("id",p_obj->GetIndex());
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsObject::ApplyGlobalViewTransformation(void)
{
   GetViews()->ApplyGlobalViewTransformation();
}

//------------------------------------------------------------------------------

void CGraphicsObject::ApplyFaceTransformation(void)
{
   GetViews()->ApplyFaceTransformation();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
