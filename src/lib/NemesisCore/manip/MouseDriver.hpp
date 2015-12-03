#ifndef MouseDriverH
#define MouseDriverH
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
#include <QKeyEvent>
#include <QMouseEvent>
#include <QObject>

// -----------------------------------------------------------------------------

class CMouseHandler;
class CSelectionList;
class CManipulator;
class CProject;
class CHistoryList;
class CGraphicsView;
class CGraphicsViewList;

// -----------------------------------------------------------------------------

/// manipulation directions

enum EManipLevel {
    EML_YZ,
    EML_X
};

// -----------------------------------------------------------------------------

/// this is the core class for Mouse Drivers

class NEMESIS_CORE_PACKAGE CMouseDriver : public QObject {
public:
// constructors and destructors ------------------------------------------------
    CMouseDriver(CMouseHandler* p_handler);

// event receviers -------------------------------------------------------------
    virtual void MousePressEvent(QMouseEvent* p_event);
    virtual void MouseMoveEvent(QMouseEvent* p_event);
    virtual void MouseReleaseEvent(QMouseEvent* p_event);

    virtual void WheelEvent(QWheelEvent* p_event);

    virtual void KeyPressEvent(QKeyEvent* p_event);
    virtual void KeyReleaseEvent(QKeyEvent* p_event);

    virtual void EnterEvent(QEvent* p_event);
    virtual void LeaveEvent(QEvent* p_event);

// information methods ---------------------------------------------------------
    /// get handler
    CMouseHandler*          GetHandler(void);

    /// get history list
    CHistoryList*           GetHistory(void);

    /// get selection list
    CSelectionList*         GetSelection(void);

    /// get graphics view list
    CGraphicsViewList*      GetViews(void);

    /// return active manipulator
    virtual CManipulator*   GetManipulator(void);

    /// get manipulation level
    EManipLevel             GetManipLevel(void);

    /// get graphics view
    CGraphicsView*          GetActiveView(void);

    /// reset manipulation status
    virtual void ResetManipulation(void);

// executive methods -----------------------------------------------------------
    /// select object
    void SelectObject(int x,int y);

    /// set active view
    void SetActiveView(CGraphicsView* p_view);

    /// set cursor for view
    void SetCursor(const QCursor& cursor);

// section of protected data ---------------------------------------------------
private:
    CMouseHandler*      Handler;
    CGraphicsView*      ActiveView;

protected:
    EManipLevel         ManipLevel;         // YZ or X movement?
};

// -----------------------------------------------------------------------------

#endif
