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

#include <GeoDescriptor.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGeoDescriptor::CGeoDescriptor(void)
{
    Type = EGDT_NILL;
}

//------------------------------------------------------------------------------

CGeoDescriptor::CGeoDescriptor(EGetDescriptorType type)
{
    Type = type;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeoDescriptor::SetType(EGetDescriptorType type)
{
    Type = type;
}

//------------------------------------------------------------------------------

void CGeoDescriptor::SetPoint1(const CPoint& point)
{
    Point1 = point;
}

//------------------------------------------------------------------------------

void CGeoDescriptor::SetPoint2(const CPoint& point)
{
    Point2 = point;
}

//------------------------------------------------------------------------------

void CGeoDescriptor::SetPoint3(const CPoint& point)
{
    Point3 = point;
}

//------------------------------------------------------------------------------

void CGeoDescriptor::SetPoint4(const CPoint& point)
{
    Point4 = point;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

EGetDescriptorType  CGeoDescriptor::GetType(void) const
{
    return(Type);
}

//------------------------------------------------------------------------------

const CPoint& CGeoDescriptor::GetPoint1(void) const
{
    return(Point1);
}

//------------------------------------------------------------------------------

const CPoint& CGeoDescriptor::GetPoint2(void) const
{
    return(Point2);
}

//------------------------------------------------------------------------------

const CPoint& CGeoDescriptor::GetPoint3(void) const
{
    return(Point3);
}

//------------------------------------------------------------------------------

const CPoint& CGeoDescriptor::GetPoint4(void) const
{
    return(Point4);
}

//------------------------------------------------------------------------------

