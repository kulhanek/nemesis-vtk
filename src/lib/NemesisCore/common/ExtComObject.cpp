// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010        Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <PluginDatabase.hpp>
#include <ContainerModel.hpp>
#include <QApplication>
#include <XMLElement.hpp>

#include <ExtComObject.hpp>
#include <ExtComObjectDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtComObject::CExtComObject(CPluginObject* p_objectinfo,QObject* p_parent)
    : QObject(p_parent), CComObject(p_objectinfo)
{
    // do not call virtual method here SetName(), since inherited objects might not be
    // fully initialized yet
    CExtComObject::SetName(GetType().GetName());
    ObjectDesigner = NULL;
}

//------------------------------------------------------------------------------

CExtComObject::~CExtComObject(void)
{
    if( ObjectDesigner != NULL ){
        ObjectDesigner->Object = NULL;
        ObjectDesigner->deleteLater();
        ObjectDesigner = NULL;
    }
}

//------------------------------------------------------------------------------

bool CExtComObject::HasChildren(void)
{
    return( children().count() > 0 );
}

//------------------------------------------------------------------------------

CContainerModel* CExtComObject::GetContainerModel(QObject* p_parent,const QString& type)
{
    QString name = "_" + type;

    CUUID uuid;
    if( PluginDatabase.FindObjectConfigValue(GetType(),name,uuid) == false) {
        ES_ERROR("unable to find enum id");
        return(NULL);
    }

    // data sent to CreateObject points to enumerated object
    // reason is that only object itself known how to use this information
    // model ownership is then solved via setParent() method
    CContainerModel* p_obj = static_cast<CContainerModel*>(PluginDatabase.CreateObject(uuid,this));
    if( p_obj == NULL ) {
        ES_ERROR("unable to create model object");
        return(NULL);
    }
    p_obj->setParent(p_parent);
    return(p_obj);
}

//------------------------------------------------------------------------------

void CExtComObject::SetName(const QString& name)
{
    setObjectName(name);

    CExtComObject*  p_parent = dynamic_cast<CExtComObject*>(parent());
    if( p_parent != NULL ) {
        if( p_parent->ConFlags.testFlag(EECOF_DYNAMICAL_CONTENTS) ) {
            p_parent->EmitChildContainerChanged(this);
        }
    }
}

//------------------------------------------------------------------------------

const QString CExtComObject::GetName(void) const
{
    return(objectName());
}


//------------------------------------------------------------------------------

const QString CExtComObject::GenerateName(const QString& tmp)
{
    QString gen_name;

    int     i = 1;
    bool    name_ok;
    do {
        name_ok = true;
        gen_name = tmp.arg(i);
        i++;
        foreach(QObject* p_qobj,children()) {
            if( p_qobj->objectName() == gen_name ) {
                name_ok = false;
                break;
            }
        }

    } while( ! name_ok );

    return(gen_name);
}

//------------------------------------------------------------------------------

bool CExtComObject::HasSubContainers(void) const
{
    return(ConFlags.testFlag(EECOF_SUB_CONTAINERS));
}

//------------------------------------------------------------------------------

bool CExtComObject::HasDynamicalContents(void) const
{
    return(ConFlags.testFlag(EECOF_DYNAMICAL_CONTENTS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtComObjectDesigner* CExtComObject::OpenObjectDesigner(void)
{
    if( ObjectDesigner != NULL ) {
        ObjectDesigner->raise();
        ObjectDesigner->activateWindow();
        return(ObjectDesigner);
    }

    // create new designer
    CUUID uuid;
    if( PluginDatabase.FindObjectConfigValue(GetType(),"_designer",uuid) == false) {
        ES_ERROR("unable to find _designer id");
        return(NULL);
    }

    ObjectDesigner = static_cast<CExtComObjectDesigner*>(PluginDatabase.CreateObject(uuid,this));
    if( ObjectDesigner == NULL ) {
        ES_ERROR("unable to create _designer object");
        return(NULL);
    }

    ObjectDesigner->LoadObjectDesignerSetup();
    ObjectDesigner->show();
    ObjectDesigner->raise();

    UpdateDesignerRegistration();

    return(ObjectDesigner);
}

//------------------------------------------------------------------------------

bool CExtComObject::IsObjectDesignerOpened(void)
{
    return(ObjectDesigner != NULL);
}

//------------------------------------------------------------------------------

void CExtComObject::UpdateDesignerRegistration(void)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CExtComObject::CloseObjectDesigner(bool erase_open_flag)
{
    if( ObjectDesigner != NULL ){
        ObjectDesigner->Object = NULL;
        delete ObjectDesigner;
        ObjectDesigner = NULL;
    }
    if( erase_open_flag ){
        CXMLElement* p_ele = GetDesignerData(true);
        p_ele->SetAttribute("open",false);
    }
}

//------------------------------------------------------------------------------

void CExtComObject::SaveObjectDesignerSetup(void)
{
    if( ObjectDesigner != NULL ) {
        ObjectDesigner->SaveObjectDesignerSetup();
    }
}

//------------------------------------------------------------------------------

CXMLElement* CExtComObject::GetDesignerData(bool create)
{
    return(DesignerData.GetChildElementByPath("designer",create));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CExtComObject::EmitChildContainerRemoved(QObject* p_obj)
{
    emit OnChildContainerRemoved(p_obj);
}

//------------------------------------------------------------------------------

void CExtComObject::EmitChildContainerChanged(QObject* p_obj)
{
    emit OnChildContainerChanged(p_obj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

