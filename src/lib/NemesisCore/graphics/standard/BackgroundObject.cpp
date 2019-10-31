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

#include <PluginObject.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <GraphicsObjectList.hpp>
#include <Project.hpp>

#include <NemesisCoreModule.hpp>
#include "BackgroundObject.hpp"
#include "BackgroundSetup.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* BackgroundObjectCB(void* p_data);

CExtUUID        BackgroundObjectID(
                    "{BACKGROUND_OBJECT:f646b31e-e0f8-406d-a9ea-f7f9df0349d3}",
                    "Background");

CPluginObject   BackgroundObject(&NemesisCorePlugin,
                     BackgroundObjectID,GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/objects/Background.svg",
                     BackgroundObjectCB);

// -----------------------------------------------------------------------------

QObject* BackgroundObjectCB(void* p_data)
{
    return(new CBackgroundObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBackgroundObject::CBackgroundObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&BackgroundObject,p_gl)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBackgroundObject::Draw(void)
{
    CBackgroundSetup* p_setup = GetSetup<CBackgroundSetup>();
    if( p_setup == NULL ) {
        ES_ERROR("setup is null");
        return;
    }

    // backup current color mode from anaglyph rendering if enabled
    GLboolean data[4];
    glGetBooleanv(GL_COLOR_WRITEMASK,data);

    // clear entire buffer only for red component and normal mode
    if( data[2] == GL_FALSE ){
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
    glClearColor(p_setup->Color.R(),p_setup->Color.G(),p_setup->Color.B(),1);
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // restore color mode
    glColorMask(data[0], data[1], data[2], data[3]);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

