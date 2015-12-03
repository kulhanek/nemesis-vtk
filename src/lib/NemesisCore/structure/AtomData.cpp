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

#include <AtomData.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomData::CAtomData(void)
{
    Index = -1;
    SerIndex = -1;
    LocIndex = -1;
    Z = 0;
    Charge = 0;
}

//------------------------------------------------------------------------------

CAtomData::~CAtomData(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CAtomData::Load(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "atom" ) {
        LOGIC_ERROR("element is not atom");
    }

    // read atom specific data (all of them are optional)
    p_ele->GetAttribute("id",Index);
    p_ele->GetAttribute("name",Name);
    p_ele->GetAttribute("descr",Description);

    p_ele->GetAttribute("ai",SerIndex);
    p_ele->GetAttribute("ai",LocIndex);
    p_ele->GetAttribute("at",Type);
    p_ele->GetAttribute("Z",Z);
    p_ele->GetAttribute("ch",Charge);

    p_ele->GetAttribute("px",Pos.x);
    p_ele->GetAttribute("py",Pos.y);
    p_ele->GetAttribute("pz",Pos.z);

    p_ele->GetAttribute("vx",Vel.x);
    p_ele->GetAttribute("vy",Vel.y);
    p_ele->GetAttribute("vz",Vel.z);
}

//------------------------------------------------------------------------------

void CAtomData::Save(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    p_ele->SetAttribute("id",Index);
    if( Name != NULL ) p_ele->SetAttribute("name",Name);
    if( Description != NULL ) p_ele->SetAttribute("descr",Description);

    if( SerIndex > 0 ) p_ele->SetAttribute("ai",SerIndex);
    if( LocIndex > 0 ) p_ele->SetAttribute("al",LocIndex);
    if( Type != NULL ) p_ele->SetAttribute("at",Type);
    p_ele->SetAttribute("Z",Z);
    p_ele->SetAttribute("ch",Charge);

    p_ele->SetAttribute("px",Pos.x);
    p_ele->SetAttribute("py",Pos.y);
    p_ele->SetAttribute("pz",Pos.z);

    if( Size(Vel) > 0.0 ) {
        p_ele->SetAttribute("vx",Vel.x);
        p_ele->SetAttribute("vy",Vel.y);
        p_ele->SetAttribute("vz",Vel.z);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

