#ifndef ObjManipMouseDriverH
#define ObjManipMouseDriverH
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
#include <Manipulator.hpp>

// -----------------------------------------------------------------------------

class CSelectionRequest;

// -----------------------------------------------------------------------------

/// support for bond or direction manipulation

class NEMESIS_CORE_PACKAGE CObjManipMouseDriver :
                                public CMouseDriver, public CManipulator {
Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CObjManipMouseDriver(CMouseHandler* p_handler);
    ~CObjManipMouseDriver(void);

// event receviers ------------------------------------------------------------
    virtual void MousePressEvent(QMouseEvent* p_event);
    virtual void MouseMoveEvent(QMouseEvent* p_event);
    virtual void MouseReleaseEvent(QMouseEvent* p_event);

    virtual void KeyPressEvent(QKeyEvent* p_event);
    virtual void KeyReleaseEvent(QKeyEvent* p_event);

    virtual void LeaveEvent(QEvent* p_event);

    /// reset manipulation status
    virtual void ResetManipulation(void);

// section of private data ----------------------------------------------------
private:
    // -------------------------------------------
    enum EMouseAction {
        EMA_NOTHING,
        EMA_SELECT,     // select direction to manipulate
        EMA_ROTATE,     // rotate selected atoms
        EMA_MOVE        // move selected atoms
    };

    // selection request ----------------
    CSelectionRequest*  SelRequest;
    bool                Completed;

    // actions --------------------------
    EMouseAction        Action;

    // manipulators ---------------------
    int             StartX;
    int             StartY;
    int             MouseX;
    int             MouseY;

    /// return active manipulator
    CManipulator* GetManipulator(void);

    void EncodeMouseButtonsPress(QMouseEvent* p_event);

    // manipulator
    /// begin manipulation - return false to disable manipulation
    virtual bool StartManipulation(void);

    /// move by object/scene
    virtual void Move(const CPoint& dmov,CGraphicsView* p_view);

    /// rotate with object/scene
    virtual void Rotate(const CPoint& drot,CGraphicsView* p_view);

public slots:
    void RequestInitialized(void);
    void SelectionChanged(void);
    void RequestDetached(void);
};

// -----------------------------------------------------------------------------

#endif
