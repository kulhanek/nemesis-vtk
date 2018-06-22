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

#include <GraphicsCommonView.hpp>
#include <GraphicsView.hpp>
#include <Project.hpp>
#include <GraphicsCommonGLScene.hpp>
#include <GraphicsSetupProfile.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsCommonView::CGraphicsCommonView(QWidget* p_widget)
    : QGraphicsView(p_widget)
{
    Project = NULL;
    GraphicsView = NULL;
    MouseHandler = NULL;

    QSurfaceFormat format;
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setStereo(GraphicsSetupProfile->QuadStereoEnabled);
    if( GraphicsSetupProfile->MultiSamplingEnabled ){
        // FIXME
        format.setSamples(2);
    }

    OpenGLViewport = new QOpenGLWidget();
    OpenGLViewport->setFormat(format);
    setViewport(OpenGLViewport); // it will take ownership

    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    Scene = new CGraphicsCommonGLScene(this);
    setScene(Scene);
}

//------------------------------------------------------------------------------

CGraphicsCommonView::~CGraphicsCommonView()
{
    // make OpenGL context active
    if( OpenGLViewport) OpenGLViewport->makeCurrent();
    if( GraphicsView ){
        GraphicsView->ShadowView = NULL;
        GraphicsView->DetachOpenGLCanvas();
        Project = NULL;
        GraphicsView = NULL;
        MouseHandler = NULL;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsCommonView::SetGraphicsView(CGraphicsView* p_view)
{
    GraphicsView = p_view;
    if( OpenGLViewport ) OpenGLViewport->makeCurrent();
    if( GraphicsView ){
        Project = GraphicsView->GetProject();
        MouseHandler = Project->GetMouseHandler();
        GraphicsView->AttachOpenGLCanvas(this);
    } else {
        MouseHandler = NULL;
    }
    if( Scene ) Scene->GraphicsView = GraphicsView;
}

//------------------------------------------------------------------------------

void CGraphicsCommonView::UpdateScene(void)
{
    if( scene() ) scene()->update();
}

//------------------------------------------------------------------------------

QOpenGLWidget* CGraphicsCommonView::GetOpenGL(void)
{
    return(OpenGLViewport);
}

//------------------------------------------------------------------------------

void CGraphicsCommonView::ActivateGLContext(void)
{
    if( OpenGLViewport ) OpenGLViewport->makeCurrent();
}

//------------------------------------------------------------------------------

QSurfaceFormat CGraphicsCommonView::GetFormat(void)
{
    if( OpenGLViewport ) return(OpenGLViewport->format());
    return(QSurfaceFormat());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsCommonView::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();
    if( scene() ) {
        scene()->setSceneRect( QRect(QPoint(0, 0),size));
    }
    QGraphicsView::resizeEvent(event);
}

//------------------------------------------------------------------------------

// resend mouse and keybord modifiers to MouseHandler if it is set

void CGraphicsCommonView::mousePressEvent(QMouseEvent* p_event)
{
    if( MouseHandler == NULL ) return;
    if( GraphicsView == NULL ) return;
    MouseHandler->MousePressEvent(p_event,GraphicsView);
    QGraphicsView::mousePressEvent(p_event);
}

//------------------------------------------------------------------------------

void CGraphicsCommonView::mouseMoveEvent(QMouseEvent* p_event)
{
    if( MouseHandler == NULL ) return;
    if( GraphicsView == NULL ) return;
    MouseHandler->MouseMoveEvent(p_event,GraphicsView);
    QGraphicsView::mouseMoveEvent(p_event);
}

//------------------------------------------------------------------------------

void CGraphicsCommonView::mouseReleaseEvent(QMouseEvent* p_event)
{
    if( MouseHandler == NULL ) return;
    if( GraphicsView == NULL ) return;
    MouseHandler->MouseReleaseEvent(p_event,GraphicsView);
    QGraphicsView::mouseReleaseEvent(p_event);
}

//------------------------------------------------------------------------------

void CGraphicsCommonView::wheelEvent(QWheelEvent* p_event)
{
    if( MouseHandler == NULL ) return;
    if( GraphicsView == NULL ) return;
    MouseHandler->WheelEvent(p_event,GraphicsView);
    QGraphicsView::wheelEvent(p_event);
}

//------------------------------------------------------------------------------

void CGraphicsCommonView::keyPressEvent(QKeyEvent* p_event)
{
    if( MouseHandler == NULL ) return;
    if( GraphicsView == NULL ) return;
    MouseHandler->KeyPressEvent(p_event,GraphicsView);
    QGraphicsView::keyPressEvent(p_event);
}

//------------------------------------------------------------------------------

void CGraphicsCommonView::keyReleaseEvent(QKeyEvent* p_event)
{
    if( MouseHandler == NULL ) return;
    if( GraphicsView == NULL ) return;
    MouseHandler->KeyReleaseEvent(p_event,GraphicsView);
    QGraphicsView::keyReleaseEvent(p_event);
}

//------------------------------------------------------------------------------

void CGraphicsCommonView::enterEvent(QEvent* p_event)
{
    if( MouseHandler == NULL ) return;
    if( GraphicsView == NULL ) return;
    MouseHandler->EnterEvent(p_event,GraphicsView);
    QGraphicsView::enterEvent(p_event);
}

//------------------------------------------------------------------------------

void CGraphicsCommonView::leaveEvent(QEvent* p_event)
{
    if( MouseHandler == NULL ) return;
    if( GraphicsView == NULL ) return;
    MouseHandler->LeaveEvent(p_event,GraphicsView);
    QGraphicsView::leaveEvent(p_event);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

