#ifndef GeoMeasurementH
#define GeoMeasurementH
/*! \file GeoMeasurement.hpp
*/
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

#include <NemesisCoreMainHeader.hpp>
#include <Point.hpp>

// -----------------------------------------------------------------------------

class CSelectionList;
class CAtom;

//------------------------------------------------------------------------------

enum EComparisonOperator {
    ECO_LT,
    ECO_LE,
    ECO_EQ,
    ECO_NE,
    ECO_GE,
    ECO_GT
};

// -----------------------------------------------------------------------------

/// calculate geometry values from list of geometry descriptors

class NEMESIS_CORE_PACKAGE CGeoMeasurement {
// User data ------------------------------------------------------------------
public:
    static CPoint GetPosition(CSelectionList* p_selection);
    static double GetDistance(CSelectionList* p_selection);
    static double GetAngle(CSelectionList* p_selection);
    static double GetTorsion(CSelectionList* p_selection);

    static double GetDistance(const CPoint& p1,const CPoint& p2);
    static double GetAngle(const CPoint& p1,const CPoint& p2,const CPoint& p3);
    static double GetTorsion(const CPoint& p1,const CPoint& p2,
                             const CPoint& p3,const CPoint& p4);
};

// -----------------------------------------------------------------------------

/// cartesian gradient of atom

class CAtomGrad {
public:
    CAtomGrad(void);
public:
    CAtom*  Atom;
    CPoint  Grad;
};

//---------------------------------------------------------------------------
#endif
