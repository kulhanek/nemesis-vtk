#ifndef XYZAxesSetupH
#define XYZAxesSetupH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008-2009 Petr Kulhanek, kulhanek@enzim.hu,
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

//---------------------------------------------------------------------------

/// XYZ axes setup

class CXYZAxesSetup : public CGraphicsSetup {
public:
// constructors and destructors -----------------------------------------------
    CXYZAxesSetup(CProObject* p_owner);

// input/output methods -------------------------------------------------------
    virtual void LoadData(CXMLElement* p_ele);
    virtual void SaveData(CXMLElement* p_ele);

// section of public data -----------------------------------------------------
public:
    bool        ShowX;
    bool        ShowY;
    bool        ShowZ;

    bool        ShowMX;
    bool        ShowMY;
    bool        ShowMZ;

    float       MagnificationX;
    float       MagnificationY;
    float       MagnificationZ;

    float       MagnificationMX;
    float       MagnificationMY;
    float       MagnificationMZ;

    float       AxisRadius;
    float       ConeRadius;
    float       ConeHeight;

    CColor      PXColor;
    CColor      NXColor;
    CColor      ConeXColor;

    CColor      PYColor;
    CColor      NYColor;
    CColor      ConeYColor;

    CColor      PZColor;
    CColor      NZColor;
    CColor      ConeZColor;

    int         CylinderTesselation;
    int         ConeTesselation;
};

//---------------------------------------------------------------------------

#endif
