// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <PluginObject.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <GraphicsObjectList.hpp>
#include <GraphicsObjectLink.hpp>
#include <GraphicsObjectEnd.hpp>
#include <Project.hpp>
#include <XMLElement.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GraphicsObjectLinkCB(void* p_data);

CExtUUID        GraphicsObjectLinkID(
                    "{GRAPHICS_OBJECT_LINK:eda5a997-2e6c-4dfd-bc59-ad00a90a391b}",
                    "Object Link");

CPluginObject   GraphicsObjectLink(&NemesisCorePlugin,
                     GraphicsObjectLinkID,GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/Link.svg",
                     GraphicsObjectLinkCB);

//------------------------------------------------------------------------------

QObject* GraphicsObjectLinkCB(void* p_data)
{
    return(new CGraphicsObjectLink(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsObjectLink::CGraphicsObjectLink(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&GraphicsObjectLink,p_gl)
{
    ObjectEnd = NULL;
}

//------------------------------------------------------------------------------

CGraphicsObjectLink::CGraphicsObjectLink(CGraphicsObjectList* p_gl,
                                         CGraphicsObjectEnd* p_end)
    : CGraphicsObject(&GraphicsObjectLink,p_gl)
{
    ObjectEnd = p_end;
    if( ObjectEnd != NULL ){
        if( ObjectEnd->GetEndObject() != NULL ){
            ObjectEnd->GetEndObject()->RegisterObject(this);
        }
        ObjectEnd->Link = this;
    }

    // it must be here to correctly call virtual method GetGraphicsType
    UpdateSetupLink();
}

//------------------------------------------------------------------------------

CGraphicsObjectLink::~CGraphicsObjectLink(void)
{
    if( ObjectEnd ){
        ObjectEnd->GetEndObject()->UnregisterObject(this);
        ObjectEnd->Link = NULL;
        ObjectEnd->LinkObjectUpdated();
    }
    ObjectEnd = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const CExtUUID& CGraphicsObjectLink::GetGraphicsType(void)
{
    static CExtUUID null;
    if( ObjectEnd == NULL ) return(null);
    if( ObjectEnd->GetEndObject() == NULL ) return(null);
    return(ObjectEnd->GetEndObject()->GetType());
}

//------------------------------------------------------------------------------

CProObject* CGraphicsObjectLink::GetEndObject(void)
{
    return(ObjectEnd->GetEndObject());
}

//------------------------------------------------------------------------------

void CGraphicsObjectLink::Draw(void)
{
    if( ObjectEnd == NULL ) return;
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;
    ObjectEnd->Draw();
}

//------------------------------------------------------------------------------

void CGraphicsObjectLink::GetObjectMetrics(CObjMetrics& metrics)
{
    if( ObjectEnd == NULL ) return;
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;
    ObjectEnd->GetObjectMetrics(metrics);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsObjectLink::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load object info --------------------------
    CGraphicsObject::LoadData(p_ele);

    // specific data -----------------------------
    GetProject()->RequestToLinkObject(this,p_ele,"oend");
}

//------------------------------------------------------------------------------

void CGraphicsObjectLink::LinkObject(const QString& role,CProObject* p_object)
{
    if( role != "oend" ) return;

    CGraphicsObjectEnd* p_end = dynamic_cast<CGraphicsObjectEnd*>(p_object);
    if( p_end != NULL ){
        ObjectEnd = p_end;
        if( ObjectEnd->GetEndObject() != NULL ){
            ObjectEnd->GetEndObject()->RegisterObject(this);
        }
        ObjectEnd->Link = this;
    }

    // it must be here to correctly call virtual method GetGraphicsType
    UpdateSetupLink();

    if( ObjectEnd ){
        ObjectEnd->LinkObjectUpdated();
    }
}

//------------------------------------------------------------------------------

void CGraphicsObjectLink::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // core data ---------------------------------
    CGraphicsObject::SaveData(p_ele);

    // specific data -----------------------------
    if( GetEndObject() ){
        p_ele->SetAttribute("oend",GetEndObject()->GetIndex());
    }
}

//------------------------------------------------------------------------------

void CGraphicsObjectLink::RemoveFromRegistered(CProObject* p_object,
                                               CHistoryNode* p_history)
{
    if( ObjectEnd == NULL ) return;
    if( p_object == ObjectEnd->GetEndObject() ){
        // destroy object
        RemoveFromBaseList(p_history);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

