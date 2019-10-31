#ifndef ElementColorsH
#define ElementColorsH
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

#include <NemesisCoreMainHeader.hpp>
#include <XMLElement.hpp>
#include <SmallColor.hpp>

//------------------------------------------------------------------------------

// FIXME
enum EECModes {
    EECM_FRONT_AND_BACK = 1,
};

//------------------------------------------------------------------------------

/// extended color definition for OpenGL

class NEMESIS_CORE_PACKAGE CElementColors {
// constructors and destructors ------------------------------------------------
public:
    CElementColors(void);

// setup methods ---------------------------------------------------------------
    void ApplyMaterialColor(EECModes face=EECM_FRONT_AND_BACK) const;
    void ApplyUniformColor(EECModes face=EECM_FRONT_AND_BACK) const;
    void ApplyTransparency(float transparency);

// input/output methods --------------------------------------------------------
    bool Load(CXMLElement* p_ele);
    bool Save(CXMLElement* p_ele);

// operators -------------------------------------------------------------------
    bool operator == (const CElementColors& left) const;

// section of public data ------------------------------------------------------
public:
    CColor      Color;

    CColor      Ambient;
    CColor      Diffuse;
    CColor      Specular;
    CColor      Emission;
    int         Shininess;
};

//------------------------------------------------------------------------------

#endif

