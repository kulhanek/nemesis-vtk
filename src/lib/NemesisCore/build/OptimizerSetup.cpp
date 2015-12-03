// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <OptimizerSetup.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>

#include "OptimizerSetup.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

COptimizerSetup::COptimizerSetup(CPluginObject* p_objectinfo,CExtComObject* p_parent)
    : CExtComObject(p_objectinfo,p_parent)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const CUUID& COptimizerSetup::GetOptimizerUUID(void)
{
    static CUUID invalid;
    return(invalid);
}

//------------------------------------------------------------------------------

int COptimizerSetup::GetNotifyTickInterval(void)
{
    return(10);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void COptimizerSetup::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    QString buf;

    if( p_ele->GetAttribute("uuid",buf) == true ) {
        if( buf != GetType().GetFullStringForm() ){
            LOGIC_ERROR("illegal uuid for optimizer setup");
        }
    }

    // load the rest
    if( p_ele->GetAttribute("name",buf) == true ) {
        SetName(buf);
    }
}

//------------------------------------------------------------------------------

void COptimizerSetup::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    p_ele->SetAttribute("uuid",GetType().GetFullStringForm());
    p_ele->SetAttribute("name",GetName());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


