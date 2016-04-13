// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2016 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <GL/gl.h>
#include <GL/glu.h>
#include <Project.hpp>

#include "AmberModule.hpp"
#include "GESPGridObject.hpp"
#include "GESPGridSetup.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GESPGridObjectCB(void* p_data);

CExtUUID        GESPGridObjectID(
                    "{GESPGRID_OBJECT:d91e3a75-2453-453e-b601-2c495be44ac1}",
                    "GESPGrid");

CPluginObject   GESPGridObject(&AmberPlugin,
                     GESPGridObjectID,GRAPHICS_OBJECT_CAT,
                    ":/images/Amber/GESPGrid.svg",
                     GESPGridObjectCB);

// -----------------------------------------------------------------------------

QObject* GESPGridObjectCB(void* p_data)
{
    return(new CGESPGridObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGESPGridObject::CGESPGridObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&GESPGridObject,p_gl)
{
    MinESP = 0;
    MaxESP = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGESPGridObject::AddPoint(double x, double y, double z, double esp)
{
    CGESPGridPoint point;
    point.x = x;
    point.y = y;
    point.z = z;
    point.esp = esp;

    Data.push_back(point);

    if(Data.size() == 1){
        MinESP = esp;
        MaxESP = esp;
    }

    if(MinESP > esp){
        MinESP = esp;
    }
    if(MaxESP < esp){
        MaxESP = esp;
    }
}

//------------------------------------------------------------------------------

void CGESPGridObject::Draw(void)
{
    CGESPGridSetup* p_setup = GetSetup<CGESPGridSetup>();
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
    //glClearColor(p_setup->Color.R(),p_setup->Color.G(),p_setup->Color.B(),1);
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // restore color mode
    glColorMask(data[0], data[1], data[2], data[3]);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

