#ifndef ObjMetricsH
#define ObjMetricsH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

/// object metrics

class NEMESIS_CORE_PACKAGE CObjMetrics {
public:
    CObjMetrics(void);

// setup methods --------------------------------------------------------------
    /// compare with point
    void            CompareWith(const CPoint& pos);

// information methods --------------------------------------------------------
    /// is object metrics valid?
    bool            IsValid(void);

    /// get low point
    const CPoint&   GetLowPoint(void);

    /// get high point
    const CPoint&   GetHighPoint(void);

    /// operator
    void operator *= (double fac);

// section of private data ----------------------------------------------------
private:
    CPoint  Low;
    CPoint  High;
    bool    Valid;
};

//------------------------------------------------------------------------------

#endif
