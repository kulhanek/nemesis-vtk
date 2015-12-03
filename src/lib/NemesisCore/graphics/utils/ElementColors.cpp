// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <ElementColors.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CElementColors::CElementColors(void)
{
    Shininess = 0;
}

//------------------------------------------------------------------------------

void CElementColors::ApplyMaterialColor(GLenum face) const
{
    glDisable(GL_COLOR_MATERIAL);
    glColor4fv(Diffuse);
    glMaterialfv(face, GL_AMBIENT,Ambient);
    glMaterialfv(face, GL_DIFFUSE,Diffuse);
    glMaterialfv(face, GL_EMISSION,Emission);
    glMaterialfv(face, GL_SPECULAR,Specular);
    glMaterialf(face, GL_SHININESS,Shininess);
}

//------------------------------------------------------------------------------

void CElementColors::ApplyUniformColor(GLenum face) const
{
    glMaterialf(face, GL_SHININESS,Shininess);
    glEnable(GL_COLOR_MATERIAL);
    glColor4fv(Color);
}

//------------------------------------------------------------------------------

void CElementColors::ApplyTransparency(float transparency)
{
    Color.SetAlpha(transparency);
    Diffuse.SetAlpha(transparency);
}

//------------------------------------------------------------------------------

bool CElementColors::operator == (const CElementColors& left) const
{
    if( Shininess != left.Shininess ) return(false);
    if( Color != left.Color ) return(false);
    if( Ambient != left.Ambient ) return(false);
    if( Diffuse != left.Diffuse ) return(false);
    if( Specular != left.Specular ) return(false);
    if( Emission != left.Emission ) return(false);
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CElementColors::Load(CXMLElement* p_ele)
{
    if(p_ele == NULL) {
        return(false);
    }

    bool result = true;
    result &= p_ele->GetAttribute("shininess", Shininess);

    result &= Color.Load("color", p_ele);
    result &= Ambient.Load("ambient", p_ele);
    result &= Diffuse.Load("difusse", p_ele);
    result &= Specular.Load("specular", p_ele);
    result &= Emission.Load("emission", p_ele);

    return(result);
}

//------------------------------------------------------------------------------

bool CElementColors::Save(CXMLElement* p_ele)
{
    if(p_ele == NULL) {
        return(false);
    }

    bool result = true;
    p_ele->SetAttribute("shininess", Shininess);
    Color.Save("color", p_ele);
    Ambient.Save("ambient", p_ele);
    Diffuse.Save("difusse", p_ele);
    Specular.Save("specular", p_ele);
    Emission.Save("emission", p_ele);

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

