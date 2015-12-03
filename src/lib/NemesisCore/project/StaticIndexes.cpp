// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012        Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <StaticIndexes.hpp>
#include <SmallString.hpp>
#include <ErrorSystem.hpp>

//------------------------------------------------------------------------------

CExtUUID    _StructureListID(
                "{STRUCTURE_LIST:caf85f99-147c-4eb4-a7ba-15edc7e47b53}",
                "Structures");

CExtUUID    _PropertyListID(
                "{PROPERTY_LIST:c760d6ae-74af-4abd-a192-c4ba8ac21255}",
                "Properties");

CExtUUID    _GraphicsID(
                "{GRAPHICS:32bbf75b-edc0-4321-a188-a7dd37cd034f}",
                "Graphics");

CExtUUID    _GraphicsObjectListID(
                "{GRAPHICS_OBJECT_LIST:07c60ac0-2919-45f6-ad87-5b7e43b1aa38}",
                "Objects");

CExtUUID    _GraphicsProfileListID(
                "{GRAPHICS_PROFILE_LIST:b2a17719-cc6c-4850-97fe-e216b56387ba}",
                "Profiles");

CExtUUID    _GraphicsViewListID(
                "{GRAPHICS_VIEW_LIST:55610ded-9278-47ea-80ff-af7a6a879661}",
                "Views");

CExtUUID    _GraphicsViewID(
                "{GRAPHICS_VIEW:ec35dc44-7f02-4814-b3b2-c712f258aec6}",
                "View");

CExtUUID    _SelectionListID(
                "{SELECTION_LIST:f3f06587-a9f4-4e81-ac09-14035ae1544d}",
                "Selection");

CExtUUID   _GraphicSetupListID(
                "{GRAPHICS_SETUP_LIST:553dd8be-3ee0-4db0-97c7-7b28af1a1968}",
                "Setup");

CExtUUID    _HistoryListID(
                "{HISTORY_LIST:dfaa255c-6858-46b1-aa60-0517acaf21b1}",
                "History");

CExtUUID    _MouseHandlerID(
                "{MOUSE_HANDLER:79ff0534-3a9b-4546-b88d-c0269044683e}",
                "Mouse Handler");

CExtUUID    _JobListID(
                "{JOB_LIST:dc2ff03c-6907-4faa-b21c-01e5c7ecfe19}",
                "Jobs");

CExtUUID    _TrajectoryListID(
                "{TRAJECTORY_LIST:2b2a4e86-223a-433a-bcb9-93c5d2b3c717}",
                "Trajectories");

CExtUUID    _CheminfoProjectInfoListID(
                "{CHEMINFO_INFO_LIST:b169b023-ffd4-4afe-acc3-f061de216f74}",
                "Cheminfo Projects List");

CExtUUID    _CheminfoProjectStructureListProjectID(
                "{CHEMINFO_STRUCTURE_LIST_PROJECT:b2a0edc4-e0fd-40d0-a6b7-a379b23a1a66}",
                "Cheminfo Structure List Projects");

//------------------------------------------------------------------------------

CStaticIndexList StaticIndexes;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CStaticIndexList::CStaticIndexList(void)
{
    RegIndexes[STRUCTURES_INDEX]                = _StructureListID;
    RegIndexes[PROPERTIES_INDEX]                = _PropertyListID;
    RegIndexes[GRAPHICS_INDEX]                  = _GraphicsID;
    RegIndexes[GRAPHICS_OBJECTS_INDEX]          = _GraphicsObjectListID;
    RegIndexes[GRAPHICS_PROFILES_INDEX]         = _GraphicsProfileListID;
    RegIndexes[GRAPHICS_VIEWS_INDEX]            = _GraphicsViewListID;
    RegIndexes[GRAPHICS_PRIMARY_VIEW_INDEX]     = _GraphicsViewID;
    RegIndexes[GRAPHICS_SETUPS_INDEX]           = _GraphicSetupListID;
    RegIndexes[SELECTION_LIST_INDEX]            = _SelectionListID;
    RegIndexes[HISTORY_LIST_INDEX]              = _HistoryListID;
    RegIndexes[MOUSE_HANDLER_INDEX]             = _MouseHandlerID;
    RegIndexes[JOB_LIST_INDEX]                  = _JobListID;
    RegIndexes[TRAJECTORY_LIST_INDEX]           = _TrajectoryListID;
    RegIndexes[CHEMINFO_INFO_LIST_INDEX]        = _CheminfoProjectInfoListID;
    RegIndexes[CHEMINFO_STRUCTURE_LIST_PROJECT_INDEX] = _CheminfoProjectStructureListProjectID;
}

//------------------------------------------------------------------------------

void CStaticIndexList::CheckIndex(int index,const CExtUUID& type)
{
    if( index > LAST_USER_STATIC_INDEX ) return;
    if( index == PROJECT_INDEX ) return;
    if( index <= 0 ) {
        CSmallString error;
        error << "illegal static index (<<=0) for type '" << type.GetFullStringForm() << "'";
        ES_ERROR(error);
        return;
    }
    if( ! RegIndexes[index].IsValidUUID() ){
        CSmallString error;
        error << "static index '" << index << "' for '" << type.GetFullStringForm() << "' is not registered";
        ES_ERROR(error);
    }
    if( RegIndexes[index] != type ){
        CSmallString error;
        error << "static index '" << index << "' for '" << type.GetFullStringForm();
        error << "' is registered for '" << RegIndexes[index].GetFullStringForm() << "'";
        ES_ERROR(error);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



