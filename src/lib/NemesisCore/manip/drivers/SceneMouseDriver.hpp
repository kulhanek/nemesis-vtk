#ifndef SceneMouseDriverH
#define SceneMouseDriverH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <Point.hpp>
#include <MouseDriver.hpp>
#include <QCursor>

// -----------------------------------------------------------------------------

class CManipulator;

// -----------------------------------------------------------------------------

/// support of mouse (rotation, movement, selection ...)

class NEMESIS_CORE_PACKAGE CSceneMouseDriver : public CMouseDriver {
public:
// constructors and destructors -----------------------------------------------
    CSceneMouseDriver(CMouseHandler* p_handler);

// event receviers ------------------------------------------------------------
    virtual void MousePressEvent(QMouseEvent* p_event);
    virtual void MouseMoveEvent(QMouseEvent* p_event);
    virtual void MouseReleaseEvent(QMouseEvent* p_event);

    virtual void WheelEvent(QWheelEvent* p_event);

    virtual void KeyPressEvent(QKeyEvent* p_event);
    virtual void KeyReleaseEvent(QKeyEvent* p_event);

    virtual void LeaveEvent(QEvent* p_event);

    /// reset manipulation status
    virtual void ResetManipulation(void);

// section of private data ----------------------------------------------------
private:
    // -------------------------------------------
    enum EMouseAction {
        EMA_NOTHING,            // do nothing
        EMA_SELECT,             // select event
        EMA_MOVE,               // move manipulator
        EMA_ROTATE,             // rotate manipulator
        EMA_SCALE               // scale scene
    };

    // actions --------------------------
    EMouseAction    Action;

    // manipulators ---------------------
    int             StartX;
    int             StartY;
    int             MouseX;
    int             MouseY;

    void EncodeMouseButtonsPress(QMouseEvent* p_event);

    // -------------------------------------------
    // cursors
    QCursor DefaultCursor;
    QCursor SelectCursor;
    QCursor RotateYZCursor;
    QCursor RotateXCursor;
    QCursor TranslateYZCursor;
    QCursor TranslateXCursor;

    EMouseAction    CursorAction;
    EManipLevel     CursorManipLevel;

    void UpdateCursor(void);
};

// -----------------------------------------------------------------------------

#endif
