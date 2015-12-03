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

#include <GraphicsProperty.hpp>
#include <XMLElement.hpp>

#include "GraphicsProperty.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsProperty::CGraphicsProperty(CPluginObject* p_po,CPropertyList *p_bl)
    : CProperty(p_po,p_bl), CGraphicsObjectEnd(this)
{

}

//------------------------------------------------------------------------------

void CGraphicsProperty::LinkObjectUpdated(void)
{
    emit OnGraphicsLinkUpdated();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsProperty::LoadData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "property" ) {
        LOGIC_ERROR("p_ele is not property");
    }

    // load object info -----------------------------
    CProperty::LoadData(p_ele);
}

//------------------------------------------------------------------------------

void CGraphicsProperty::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "property" ) {
        LOGIC_ERROR("p_ele is not property");
    }

    // save object info -----------------------------
    CProperty::SaveData(p_ele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


