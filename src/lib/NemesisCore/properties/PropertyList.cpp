// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <PropertyList.hpp>
#include <CategoryUUID.hpp>
#include <NemesisCoreModule.hpp>
#include <Project.hpp>
#include <Property.hpp>
#include <ErrorSystem.hpp>
#include <HistoryList.hpp>
#include <PluginDatabase.hpp>
#include <PropertyHistory.hpp>
#include <XMLElement.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        PropertyListID(
                    "{PROPERTY_LIST:c760d6ae-74af-4abd-a192-c4ba8ac21255}",
                    "Properties");

CPluginObject   PropertyListObject(&NemesisCorePlugin,
                    PropertyListID,GENERIC_CAT,
                    ":/images/NemesisCore/properties/PropertyList.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPropertyList::CPropertyList(CProject* p_project)
    : CProObject(&PropertyListObject,p_project,p_project)
{
}

//------------------------------------------------------------------------------

CPropertyList::CPropertyList(CProject* p_project,bool no_index)
    : CProObject(&PropertyListObject,p_project,p_project,no_index)
{
}

//------------------------------------------------------------------------------

CPropertyList::~CPropertyList(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CProperty* CPropertyList::CreatePropertyWH(const CUUID& objectuuid, const QString& name,
                            const QString& descr)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,tr("create a new property"));
    if( p_history == NULL ) return (NULL);

    CProperty* p_prop = NULL;
    p_prop = CreateProperty(objectuuid,name,descr,p_history);

    EndChangeWH();
    return(p_prop);
}

//------------------------------------------------------------------------------

bool CPropertyList::DeleteAllPropertiesWH(void)
{
    if( children().count() == 0 ){
        GetProject()->TextNotification(ETNT_WARNING,"no property is available to be deleted",ETNT_WARNING_DELAY);
        return(false);
    }

    CHistoryNode*    p_history;
    p_history = BeginChangeWH(EHCL_STRUCTURES,tr("delete all properties"));
    if( p_history == NULL ) return (false);

        DeleteAllProperties(p_history);

    EndChangeWH();

    return(true);
}

//------------------------------------------------------------------------------

bool CPropertyList::DeleteInvalidPropertiesWH(void)
{
    if( GetNumberOfInvalidProperties() == 0 ){
        GetProject()->TextNotification(ETNT_WARNING,"no invalid property is available to be deleted",ETNT_WARNING_DELAY);
        return(false);
    }

    CHistoryNode*    p_history;
    p_history = BeginChangeWH(EHCL_STRUCTURES,tr("delete invalid properties"));
    if( p_history == NULL ) return (false);

        DeleteInvalidProperties(p_history);

    EndChangeWH();

    return(true);
}

//------------------------------------------------------------------------------

bool CPropertyList::DeleteSelectedPropertiesWH(void)
{
    if( GetNumberOfSelectedProperties()  == 0 ){
        GetProject()->TextNotification(ETNT_WARNING,"no selected property is available to be deleted",ETNT_WARNING_DELAY);
        return(false);
    }

    CHistoryNode*    p_history;
    p_history = BeginChangeWH(EHCL_STRUCTURES,tr("delete selected properties"));
    if( p_history == NULL ) return (false);

        DeleteSelectedProperties(p_history);

    EndChangeWH();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CProperty* CPropertyList::CreateProperty(const CUUID& objectuuid,
                const QString& name, const QString& descr, CHistoryNode* p_history)
{
    // create object
    CProperty* p_prop = static_cast<CProperty*>(PluginDatabase.CreateObject(objectuuid,this));

    if( p_prop == NULL ) {
        CSmallString error;
        error << "unable to create graphics object " << objectuuid.GetStringForm();
        RUNTIME_ERROR(error);
    }

    QString obj_name = name;
    if( obj_name.isEmpty() ){
        CPluginObject* p_object = PluginDatabase.FindPluginObject(objectuuid);
        if( p_object != NULL ){
            obj_name = GenerateName(p_object->GetName() + " %1");
        }
    }

    // setup general properties
    p_prop->SetName(obj_name);
    p_prop->SetDescription(descr);

    // register change to history
    if( p_history != NULL ){
        CPropertyHI* p_propdata = new CPropertyHI(p_prop,EHID_FORWARD);
        p_history->Register(p_propdata);
    }

    // notify list models
    emit EmitOnPropertyListChanged();

    return(p_prop);
}

//------------------------------------------------------------------------------

CProperty* CPropertyList::CreateProperty(CXMLElement* p_data, CHistoryNode* p_history)
{
    if( p_data == NULL ) return(NULL);

    QString suuid;
    if( p_data->GetAttribute("uuid",suuid) == false ) {
        INVALID_ARGUMENT("unable to get uuid attribute");
    }
    CExtUUID  ext_uuid;
    if( ext_uuid.LoadFromString(suuid) == false ) {
        CSmallString error;
        error << "unable to decode uuid attribute " << suuid;
        INVALID_ARGUMENT(error);
    }

    // create object
    CProperty* p_prop = static_cast<CProperty*>(PluginDatabase.CreateObject(ext_uuid,this));

    if( p_prop == NULL ) {
        CSmallString error;
        error << "unable to create graphics object " << ext_uuid.GetStringForm();
        RUNTIME_ERROR(error);
    }

    // load data
    GetProject()->BeginLinkProcedure(0);
    p_prop->LoadData(p_data);
    GetProject()->EndLinkProcedure();

    // register change to history
    if( p_history != NULL ){
        CPropertyHI* p_propdata = new CPropertyHI(p_prop,EHID_FORWARD);
        p_history->Register(p_propdata);
    }

    // notify list models
    emit EmitOnPropertyListChanged();

    return(p_prop);
}

//------------------------------------------------------------------------------

void CPropertyList::DeleteAllProperties(CHistoryNode* p_history)
{
    foreach(QObject* p_qobj,children()) {
        CProperty* p_prop = static_cast<CProperty*>(p_qobj);
        p_prop->RemoveFromBaseList(p_history);
    }
}

//------------------------------------------------------------------------------

void CPropertyList::DeleteSelectedProperties(CHistoryNode* p_history)
{
    foreach(QObject* p_qobj,children()) {
        CProperty* p_prop = static_cast<CProperty*>(p_qobj);
        if( p_prop->IsFlagSet(EPOF_SELECTED) == true ){
            p_prop->RemoveFromBaseList(p_history);
        }
    }
}

//------------------------------------------------------------------------------

void CPropertyList::DeleteInvalidProperties(CHistoryNode* p_history)
{
    foreach(QObject* p_qobj,children()) {
        CProperty* p_prop = static_cast<CProperty*>(p_qobj);
        if( p_prop->IsReady() == false ){
            p_prop->RemoveFromBaseList(p_history);
        }
    }
}

//------------------------------------------------------------------------------

void CPropertyList::ClearProperties(void)
{
    foreach(QObject* p_object,children()){
        delete p_object;
    }

    // notify list models
    emit EmitOnPropertyListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CPropertyList::GetNumberOfProperties(void)
{
    return(children().size());
}

//------------------------------------------------------------------------------

int CPropertyList::GetNumberOfSelectedProperties(void)
{
    int count = 0;
    foreach(QObject* p_qobj,children()) {
        CProperty* p_prop = static_cast<CProperty*>(p_qobj);
        if( p_prop->IsFlagSet(EPOF_SELECTED) == true ) count++;
    }
    return(count);
}

//------------------------------------------------------------------------------

int CPropertyList::GetNumberOfInvalidProperties(void)
{
    int count = 0;
    foreach(QObject* p_qobj,children()) {
        CProperty* p_prop = static_cast<CProperty*>(p_qobj);
        if( p_prop->IsReady() == false ) count++;
    }
    return(count);
}

//------------------------------------------------------------------------------

CProperty* CPropertyList::GetProperty(int index)
{
    return(static_cast<CProperty*>(children()[index]));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPropertyList::LoadData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "properties" ) {
        LOGIC_ERROR("p_ele is not 'properties'");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    CXMLElement* p_propele;
    p_propele = p_ele->GetFirstChildElement("property");

    while( p_propele != NULL ) {
        CreateProperty(p_propele);
        p_propele = p_propele->GetNextSiblingElement("property");
    }
}

//------------------------------------------------------------------------------

void CPropertyList::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "properties" ) {
        LOGIC_ERROR("p_ele is not 'properties'");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    foreach(QObject* p_qobj,children()) {
        CProperty* p_prop = static_cast<CProperty*>(p_qobj);
        CXMLElement* p_propele = p_ele->CreateChildElement("property");
        p_prop->SaveData(p_propele);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPropertyList::EmitOnPropertyListChanged(void)
{
    emit OnPropertyListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

