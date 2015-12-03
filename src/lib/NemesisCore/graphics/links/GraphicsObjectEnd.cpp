// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <NemesisCoreModule.hpp>
#include <GraphicsObjectEnd.hpp>
#include <Project.hpp>
#include <Graphics.hpp>
#include <GraphicsObjectList.hpp>
#include <GraphicsObjectLink.hpp>
#include <GraphicsProfileList.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsObjectEnd::CGraphicsObjectEnd(CProObject* p_owner)
{
    Object = p_owner;
    Link = NULL;
}

//------------------------------------------------------------------------------

CGraphicsObjectEnd::~CGraphicsObjectEnd(void)
{
    if( Link ){
        Link->UnregisterObject(Object);
        Link->ObjectEnd = NULL;
    }
    Link = NULL;
}

//------------------------------------------------------------------------------

bool CGraphicsObjectEnd::CreateGraphicsObjectWH(void)
{
    if( Link != NULL ) return(false); // already linked

    CHistoryNode* p_history = Object->BeginChangeWH(EHCL_GRAPHICS,Object->tr("create graphics representation of property"));
    if( p_history == NULL ) return (false);

    CreateGraphicsObject(p_history);

    Object->EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CGraphicsObjectEnd::ToggleShowHideFlagWH(void)
{
    if( Link == NULL ) return(true);
    return(Link->ToggleShowHideFlagWH());
}

//------------------------------------------------------------------------------

bool CGraphicsObjectEnd::RemoveGraphicsObjectWH(void)
{
    if( Link == NULL ) return(true);

    CHistoryNode* p_history = Object->BeginChangeWH(EHCL_GRAPHICS,Object->tr("remove graphics representation of property"));
    if( p_history == NULL ) return (false);

    Link->RemoveFromBaseList(p_history);

    Object->EndChangeWH();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CProObject* CGraphicsObjectEnd::GetEndObject(void)
{
    return(Object);
}

//------------------------------------------------------------------------------

CGraphicsObjectLink* CGraphicsObjectEnd::GetLinkObject(void)
{
    return(Link);
}

//------------------------------------------------------------------------------

void CGraphicsObjectEnd::CreateGraphicsObject(CHistoryNode* p_history)
{
    if( Link != NULL ) return; // already linked

    CGraphicsObject* p_gobj = Object->GetProject()->GetGraphics()->GetObjects()->CreateObject(this,p_history);
    if( p_gobj ){
        // add object to active profile
        CGraphicsProfile* p_gpro = Object->GetProject()->GetGraphics()->GetProfiles()->GetActiveProfile();
        if( p_gpro ){
            p_gpro->AddObject(p_gobj,-1,p_history);
        }
        LinkObjectUpdated();
    }
}

//------------------------------------------------------------------------------

void CGraphicsObjectEnd::Draw(void)
{
}

//------------------------------------------------------------------------------

CGraphicsSetup* CGraphicsObjectEnd::GetSetup(void) const
{
    if( Link ){
        return( Link->GetSetup() );
    }
    LOGIC_ERROR("Link is not ready so setup is NULL");
}

//------------------------------------------------------------------------------

void CGraphicsObjectEnd::GetObjectMetrics(CObjMetrics& metrics)
{
}

//------------------------------------------------------------------------------

void CGraphicsObjectEnd::LinkObjectUpdated(void)
{
    // nothing to do here
}

//------------------------------------------------------------------------------

void CGraphicsObjectEnd::ApplyGlobalViewTransformation(void)
{
    if( Link ) Link->ApplyGlobalViewTransformation();
}

//------------------------------------------------------------------------------

void CGraphicsObjectEnd::ApplyFaceTransformation(void)
{
    if( Link ) Link->ApplyFaceTransformation();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

