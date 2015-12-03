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

#include <GraphicsProfileObject.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <GraphicsProfile.hpp>
#include <GraphicsObject.hpp>
#include <GraphicsProfileObject.hpp>
#include <GraphicsProfileList.hpp>
#include <Graphics.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

// NOTE: CGraphicsProfileObject must have valid indexes for correct treatement of history changes

CGraphicsProfileObject::CGraphicsProfileObject(CGraphicsProfile* p_profile,CGraphicsObject* p_object)
    : CProObject(p_object->GetPluginObject(),p_profile,p_profile->GetProject(),false)
{
    Object = p_object;
    NextObject = NULL;
    CGraphicsProfile* p_list = GetGraphicsProfile();
    if( p_list ){
        connect(Object,SIGNAL(OnStatusChanged(EStatusChanged)),
                p_list,SLOT(EmitOnGraphicsProfileChanged()));
    }
}

//------------------------------------------------------------------------------

CGraphicsProfileObject::~CGraphicsProfileObject(void)
{
    CGraphicsProfile* p_list = GetGraphicsProfile();
    if( p_list ){
        setParent(NULL);    // remove object from the list
        Object->disconnect(p_list);
        p_list->EmitOnGraphicsProfileChanged();
    }
}

//------------------------------------------------------------------------------

bool CGraphicsProfileObject::ToggleShowHideFlagWH(void)
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

void CGraphicsProfileObject::SetFlags(const CProObjectFlags& flags,CHistoryNode* p_history)
{
    CProObject::SetFlags(flags,p_history);
    CGraphicsProfile* p_list = GetGraphicsProfile();
    if( p_list ){
        p_list->EmitOnGraphicsProfileChanged();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsProfile* CGraphicsProfileObject::GetGraphicsProfile(void) const
{
    return(dynamic_cast<CGraphicsProfile*>(parent()));
}

//------------------------------------------------------------------------------

CGraphicsObject* CGraphicsProfileObject::GetObject(void) const
{
    return(Object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsProfileObject::LoadData(CXMLElement* p_ele)
{
    // check input data --------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "object" ) {
        LOGIC_ERROR("p_ele is not object");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    // get object
    // Object is already set in constructor
}

//------------------------------------------------------------------------------

void CGraphicsProfileObject::SaveData(CXMLElement* p_ele)
{
    // check input data --------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "object" ) {
        LOGIC_ERROR("p_ele is not object");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    // save data ---------------------------------
    if( Object ) {
        p_ele->SetAttribute("oid",Object->GetIndex());
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
