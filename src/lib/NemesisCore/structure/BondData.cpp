// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <BondData.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBondData::CBondData(void)
{
    Index = -1;          // bond index
    A1 = -1;             // first atom
    A2 = -1;             // second atom
    Order = BO_NONE;          // bond order
}

//------------------------------------------------------------------------------

CBondData::~CBondData(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CBondData::Load(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        return(false);
    }

    p_ele->GetAttribute("id",Index);
    p_ele->GetAttribute("name",Name);
    p_ele->GetAttribute("descr",Description);

    int    int_buf;
    if( p_ele->GetAttribute("ord",int_buf) == true ) {
        Order = (EBondOrder)int_buf;
    }

    p_ele->GetAttribute("A1",A1);
    p_ele->GetAttribute("A2",A2);

    return(true);
}

//------------------------------------------------------------------------------

bool CBondData::Save(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_ele is NULL");
        return(false);
    }

    bool result = true;

    p_ele->SetAttribute("id",Index);
    if( Name != NULL ) p_ele->SetAttribute("name",Name);
    if( Description != NULL ) p_ele->SetAttribute("descr",Description);

    p_ele->SetAttribute("ord",Order);
    p_ele->SetAttribute("A1",A1);
    p_ele->SetAttribute("A2",A2);

    if( result == false ) {
        ES_ERROR("unable to save data");
    }

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

