#ifndef GraphicsCommonGLSceneH
#define GraphicsCommonGLSceneH
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
#include <QGraphicsScene>

// -----------------------------------------------------------------------------

class CGraphicsView;

// -----------------------------------------------------------------------------

/// common graphics scene

class NEMESIS_CORE_PACKAGE CGraphicsCommonGLScene : public QGraphicsScene {
public:
// constructor and destructors -------------------------------------------------
    CGraphicsCommonGLScene(QObject* p_parent);

// section of provate data -----------------------------------------------------
private:
    CGraphicsView* GraphicsView;

    /// draw OpenGL scene as background
    virtual void drawBackground(QPainter* p_painter, const QRectF &);

    friend class CGraphicsCommonView;
};

// -----------------------------------------------------------------------------

#endif
