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

#include <Manipulator.hpp>
#include <ErrorSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CManipulator::CManipulator(void)
{

}

//------------------------------------------------------------------------------

CManipulator::~CManipulator(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CManipulator::StartManipulation(void)
{
    return(true);
}

//------------------------------------------------------------------------------

void CManipulator::StartManipulation(CHistoryNode* p_history)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CManipulator::Move(const CPoint &dmov,CGraphicsView* p_view)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CManipulator::MoveCentrum(const CPoint& dmov,CGraphicsView* p_view)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CManipulator::Rotate(const CPoint &drot,CGraphicsView* p_view)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CManipulator::ChangeScale(double increment,CGraphicsView* p_view)
{
    // nothing to be here
}


//------------------------------------------------------------------------------

void CManipulator::EndManipulation(void)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CManipulator::LoadManip(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }
}

//------------------------------------------------------------------------------

void CManipulator::SaveManip(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



