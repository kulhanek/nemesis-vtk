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

#include <ObjMetrics.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CObjMetrics::CObjMetrics(void)
{
    Valid = false;
}

//------------------------------------------------------------------------------

void CObjMetrics::CompareWith(const CPoint& pos)
{
    if( Valid ){
        if( Low.x > pos.x  ) Low.x = pos.x;
        if( Low.y > pos.y  ) Low.y = pos.y;
        if( Low.z > pos.z  ) Low.z = pos.z;

        if( High.x < pos.x  ) High.x = pos.x;
        if( High.y < pos.y  ) High.y = pos.y;
        if( High.z < pos.z  ) High.z = pos.z;
    } else {
        Low = pos;
        High = pos;
        Valid = true;
    }
}

//------------------------------------------------------------------------------

bool CObjMetrics::IsValid(void)
{
    return(Valid);
}

//------------------------------------------------------------------------------

const CPoint& CObjMetrics::GetLowPoint(void)
{
    return(Low);
}

//------------------------------------------------------------------------------

const CPoint& CObjMetrics::GetHighPoint(void)
{
    return(High);
}

//------------------------------------------------------------------------------

void CObjMetrics::operator *= (double fac)
{
    Low *= fac;
    High *= fac;
}

//------------------------------------------------------------------------------

