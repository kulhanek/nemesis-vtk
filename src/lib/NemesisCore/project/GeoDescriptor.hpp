#ifndef GeoDescriptorH
#define GeoDescriptorH
/*! \file GeoDescriptor.hpp
*/
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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
#include <Point.hpp>

//------------------------------------------------------------------------------

enum EPlaneType {
    EPT_PLANE_XY,
    EPT_PLANE_XZ,
    EPT_PLANE_YZ
};

//------------------------------------------------------------------------------

enum EAxisType {
    EAT_AXIS_X,
    EAT_AXIS_Y,
    EAT_AXIS_Z
};

//------------------------------------------------------------------------------

enum EGetDescriptorType {
    EGDT_NILL,
    EGDT_ONE_POINT,
    EGDT_TWO_POINTS,
    EGDT_TWO_POINTS_ORIENTED,
    EGDT_THREE_POINTS,
    EGDT_FOUR_POINTS,
    EGDT_LINE,
    EGDT_PLANE      // determined by point and normal vector
};

// -----------------------------------------------------------------------------

/// independent object describing geometry parametr of any object

class NEMESIS_CORE_PACKAGE CGeoDescriptor {
public:
    CGeoDescriptor(void);
    CGeoDescriptor(EGetDescriptorType type);

// setup methods --------------------------------------------------------------
    void SetType(EGetDescriptorType type);
    void SetPoint1(const CPoint& point);
    void SetPoint2(const CPoint& point);
    void SetPoint3(const CPoint& point);
    void SetPoint4(const CPoint& point);

// information methods --------------------------------------------------------
    EGetDescriptorType  GetType(void) const;
    const CPoint&       GetPoint1(void) const;
    const CPoint&       GetPoint2(void) const;
    const CPoint&       GetPoint3(void) const;
    const CPoint&       GetPoint4(void) const;

// section of private data ----------------------------------------------------
private:
    CPoint              Point1;
    CPoint              Point2;
    CPoint              Point3;
    CPoint              Point4;
    EGetDescriptorType  Type;
};

//------------------------------------------------------------------------------

#endif
