// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <ProObjectHistory.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,ProObjectNameHI,
                        "{NAME:a3bf8c35-7470-4875-a406-12d5b93e8450}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,ProObjectDescrHI,
                        "{DESCR:0fffadef-1503-4330-ab35-95e1927272af}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,ProObjectFlagsHI,
                        "{FLAGS:1dcbc2f4-a57b-4f01-ae39-b23ff802d46c}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CProObjectNameHI::CProObjectNameHI(CProObject* p_object,
                                   const QString& newname)
    : CHistoryItem(&ProObjectNameHIObject,p_object->GetProject(),EHID_FORWARD)
{
    // object ID
    ObjectIndex    = p_object->GetIndex();

    // recorded change
    NewName = newname;
    OldName = p_object->GetName();
}

//------------------------------------------------------------------------------

void CProObjectNameHI::Forward(void)
{
    CProObject* p_object = GetProject()->FindObject(ObjectIndex);
    if(p_object != NULL) p_object->SetName(NewName);
}

// ----------------------------------------------------------------------------

void CProObjectNameHI::Backward(void)
{
    CProObject* p_object = GetProject()->FindObject(ObjectIndex);
    if(p_object != NULL) p_object->SetName(OldName);
}

//------------------------------------------------------------------------------

void CProObjectNameHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("oi",ObjectIndex);
    p_ele->GetAttribute("nn",NewName);
    p_ele->GetAttribute("on",OldName);
}

//------------------------------------------------------------------------------

void CProObjectNameHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("oi",ObjectIndex);
    p_ele->SetAttribute("nn",NewName);
    p_ele->SetAttribute("on",OldName);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CProObjectDescrHI::CProObjectDescrHI(CProObject* p_object,
                                     const QString& newdescr)
    : CHistoryItem(&ProObjectDescrHIObject,p_object->GetProject(),EHID_FORWARD)
{
    // object ID
    ObjectIndex    = p_object->GetIndex();

    // recorded change
    NewDescr = newdescr;
    OldDescr = p_object->GetDescription();
}

//------------------------------------------------------------------------------

void CProObjectDescrHI::Forward(void)
{
    CProObject* p_object = GetProject()->FindObject(ObjectIndex);
    if(p_object != NULL) p_object->SetDescription(NewDescr);
}

// ----------------------------------------------------------------------------

void CProObjectDescrHI::Backward(void)
{
    CProObject* p_object = GetProject()->FindObject(ObjectIndex);
    if(p_object != NULL) p_object->SetDescription(OldDescr);
}

//------------------------------------------------------------------------------

void CProObjectDescrHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("oi",ObjectIndex);
    p_ele->GetAttribute("nd",NewDescr);
    p_ele->GetAttribute("od",OldDescr);
}

//------------------------------------------------------------------------------

void CProObjectDescrHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("oi",ObjectIndex);
    p_ele->SetAttribute("nd",NewDescr);
    p_ele->SetAttribute("od",OldDescr);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CProObjectFlagsHI::CProObjectFlagsHI(CProObject* p_object,
                                     const CProObjectFlags& flags)
    : CHistoryItem(&ProObjectFlagsHIObject,p_object->GetProject(),EHID_FORWARD)
{
    // object ID
    ObjectIndex    = p_object->GetIndex();

    // recorded change
    OldFlags = p_object->GetFlags();
    NewFlags = flags;
}

//------------------------------------------------------------------------------

void CProObjectFlagsHI::Forward(void)
{
    CProObject* p_object = GetProject()->FindObject(ObjectIndex);
    if(p_object != NULL) p_object->SetFlags(NewFlags);
}

// ----------------------------------------------------------------------------

void CProObjectFlagsHI::Backward(void)
{
    CProObject* p_object = GetProject()->FindObject(ObjectIndex);
    if(p_object != NULL) p_object->SetFlags(OldFlags);
}

//------------------------------------------------------------------------------

void CProObjectFlagsHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("oi",ObjectIndex);
    p_ele->GetAttribute("nf",NewFlags);
    p_ele->GetAttribute("of",OldFlags);
}

//------------------------------------------------------------------------------

void CProObjectFlagsHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("oi",ObjectIndex);
    p_ele->SetAttribute("nf",NewFlags);
    p_ele->SetAttribute("of",OldFlags);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
