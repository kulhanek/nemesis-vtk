#ifndef LightSetupH
#define LightSetupH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <NemesisCoreMainHeader.hpp>
#include <GraphicsSetup.hpp>
#include <SmallColor.hpp>

//------------------------------------------------------------------------------

class CLightSetup : public CGraphicsSetup {
public:
// constructors and destructors ------------------------------------------------
    CLightSetup(CProObject* p_owner);
    ~CLightSetup(void){}
// input/output methods --------------------------------------------------------
    virtual void LoadData(CXMLElement* p_ele);
    virtual void SaveData(CXMLElement* p_ele);

// helper methods --------------------------------------------------------------
    void SetColor(CColor color,float ambient=0.2, float diffuse=1.0,
                  float specular=1.0);

// section of public data ------------------------------------------------------
public:
    // light
    CColor  Diffuse;
    CColor  Specular;
    CColor  Ambient;
    float   SpotExponent;
    float   SpotCutoff;
    float   Attenuation[3];

    // cover
    CColor  CoverColor;
    float   CoverRadius;
    float   CoverHeight;
    int     CoverTesselation;
};

//------------------------------------------------------------------------------

#endif
