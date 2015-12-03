// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010      Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008-2009 Petr Kulhanek, kulhanek@enzim.hu,
//                            Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <ExtUUID.hpp>
#include <PluginModule.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID::CExtUUID(void)
{
}

// -----------------------------------------------------------------------------

CExtUUID::CExtUUID(const QString& extuuid)
{
    LoadFromString(extuuid);
}

// -----------------------------------------------------------------------------

CExtUUID::CExtUUID(const QString& extuuid,const QString& name)
{
    LoadFromString(extuuid);
    Name = name;
}

// -----------------------------------------------------------------------------

CExtUUID::CExtUUID(const QString& extuuid,const QString& name,const QString& description)
{
    LoadFromString(extuuid);
    Name = name;
    Description = description;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

// {class_name:xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}

bool CExtUUID::LoadFromString(const QString& string)
{
    int namelength=0;

    CSmallString tmp(string);

    for(unsigned int i=1; i<tmp.GetLength(); i++) {
        if( tmp.GetBuffer()[i] == ':'  ) {
            namelength = i-1;
            break;
        }
    }

    if(namelength == 0)return(false);

    UUIDName = tmp.GetSubString(1,namelength);
    return(SetFromStringForm(tmp.GetSubString(namelength+2,tmp.GetLength()-3-namelength)));
}

// -----------------------------------------------------------------------------

const QString CExtUUID::GetFullStringForm(void) const
{
    QString string;
    string = "{" + UUIDName + ":" + GetStringForm() + "}";
    return(string);
}

// -----------------------------------------------------------------------------

const QString CExtUUID::GetMaskedStringForm(void) const
{
    QString string;
    string = "{" + UUIDName + "#" + GetStringForm() + "}";
    return(string);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString& CExtUUID::GetUUIDName(void) const
{
    return(UUIDName);
}

// -----------------------------------------------------------------------------

const QString& CExtUUID::GetName(void) const
{
    return(Name);
}

// -----------------------------------------------------------------------------

const QString& CExtUUID::GetDescription(void) const
{
    return(Description);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CExtUUID::SetValue(CXMLElement* p_ele,const CSmallString& attrname) const
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }
    p_ele->SetAttribute(attrname,GetFullStringForm());
}

//------------------------------------------------------------------------------

bool CExtUUID::GetValue(CXMLElement* p_ele,const CSmallString& attrname)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }
    QString suuid;
    if( p_ele->GetAttribute(attrname,suuid) == false ){
        return(false);
    }
    if( LoadFromString(suuid) == false ){
        return(false);
    }
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
