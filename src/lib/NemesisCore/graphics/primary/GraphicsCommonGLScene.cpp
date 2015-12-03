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

#include <GraphicsCommonGLScene.hpp>
#include <GL/gl.h>
#include <ErrorSystem.hpp>
#include <GraphicsView.hpp>
#include <QPainter>
#include <QPaintEngine>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsCommonGLScene::CGraphicsCommonGLScene(QObject* p_parent)
    : QGraphicsScene(p_parent)
{
    GraphicsView = NULL;
}

//------------------------------------------------------------------------------

void CGraphicsCommonGLScene::drawBackground(QPainter* p_painter, const QRectF &)
{
    if( (p_painter->paintEngine()->type() != QPaintEngine::OpenGL) &&
        (p_painter->paintEngine()->type() != QPaintEngine::OpenGL2) ) {
        ES_ERROR("not OpenGL viewport");
        return;
    }

    if( GraphicsView != NULL ) {
        GraphicsView->DrawGL();
    } else {
        // use this if GraphicsView is not assigned
        glViewport( 0, 0, (GLint)width(), (GLint)height() );
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 15.0 );
        glMatrixMode( GL_MODELVIEW );

        // use this if GraphicsView is not assigned
        glClearColor(0.0,0.0,0.0,1.0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

