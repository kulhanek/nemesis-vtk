// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <HistoryItem.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CHistoryItem::CHistoryItem(CPluginObject* p_objectinfo,
                           CProject* p_project,
                           EHistoryItemDirection create_direction)
    : CComObject(p_objectinfo)
{
    CurrentDirection = create_direction;
    Project = p_project;
}

// -----------------------------------------------------------------------------

CHistoryItem::CHistoryItem(CPluginObject* p_objectinfo, CProject* p_project)
    : CComObject(p_objectinfo)
{
    CurrentDirection = EHID_FORWARD;
    Project = p_project;
}

// -----------------------------------------------------------------------------

CHistoryItem::~CHistoryItem(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CHistoryItem::MakeChange(void)
{
    switch(CurrentDirection) {
        case EHID_FORWARD:
            Backward();
            CurrentDirection = EHID_BACKWARD;
            break;

        case EHID_BACKWARD:
            Forward();
            CurrentDirection = EHID_FORWARD;
            break;
    };
}

//------------------------------------------------------------------------------

void CHistoryItem::ReverseDirection(void)
{
    switch(CurrentDirection) {
        case EHID_FORWARD:
            CurrentDirection = EHID_BACKWARD;
            break;

        case EHID_BACKWARD:
            CurrentDirection = EHID_FORWARD;
            break;
    };
}

//------------------------------------------------------------------------------

EHistoryItemDirection CHistoryItem::GetCurrentDirection(void) const
{
    return(CurrentDirection);
}

//------------------------------------------------------------------------------

CProject* CHistoryItem::GetProject(void) const
{
    return(Project);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CHistoryItem::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // restore item direction
    p_ele->GetAttribute<EHistoryItemDirection>("dir",CurrentDirection);

}

//------------------------------------------------------------------------------

void CHistoryItem::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }
    // save item data
    p_ele->SetAttribute("uuid",GetType().GetFullStringForm());
    p_ele->SetAttribute("dir",CurrentDirection);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




