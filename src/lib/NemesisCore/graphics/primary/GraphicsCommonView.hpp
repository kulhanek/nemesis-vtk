#ifndef GraphicsCommonViewH
#define GraphicsCommonViewH
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

#include <NemesisCoreMainHeader.hpp>
#include <QGraphicsView>
#include <QOpenGLWidget>
#include <SmallString.hpp>

// -----------------------------------------------------------------------------

class CProject;
class CGraphicsView;
class CMouseHandler;
class CGraphicsCommonGLScene;

// -----------------------------------------------------------------------------

/// common graphics view

class NEMESIS_CORE_PACKAGE CGraphicsCommonView : public QGraphicsView {
public:
// constructor and destructors -------------------------------------------------
    CGraphicsCommonView(QWidget* p_widget);
    ~CGraphicsCommonView();

// executive methods -----------------------------------------------------------
    /// set graphics view
    void SetGraphicsView(CGraphicsView* p_view);

    /// update scene
    void UpdateScene(void);

    /// get underline opengl widget
    QOpenGLWidget* GetOpenGL(void);

    /// get underline opengl context current
    void ActivateGLContext(void);

    /// finish underline opengl context current
    void DoneGLContext(void);

    /// get OpenGL format
    QSurfaceFormat GetFormat(void);

// section of private data -----------------------------------------------------
protected:
    CProject*               Project;
    CGraphicsView*          GraphicsView;
    CMouseHandler*          MouseHandler;

protected:
    QOpenGLWidget*          OpenGLViewport;
    CGraphicsCommonGLScene* Scene;

    bool            IntelGPU;
    CSmallString    GLVendor;
    CSmallString    GLRenderer;
    CSmallString    GLVersion;

protected:
    // events ------------------------------------
    virtual void resizeEvent(QResizeEvent *event);

    virtual void mousePressEvent(QMouseEvent* p_event);
    virtual void mouseMoveEvent(QMouseEvent* p_event);
    virtual void mouseReleaseEvent(QMouseEvent* p_event);

    virtual void wheelEvent(QWheelEvent* p_event);

    virtual void keyPressEvent(QKeyEvent* p_event);
    virtual void keyReleaseEvent(QKeyEvent* p_event);

    virtual void enterEvent(QEvent* p_event);
    virtual void leaveEvent(QEvent* p_event);

    virtual void showEvent(QShowEvent *event);
};

// -----------------------------------------------------------------------------

#endif
