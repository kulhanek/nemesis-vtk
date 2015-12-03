#ifndef TerminalEditH
#define TerminalEditH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QTextEdit>

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CTerminalEdit : public QTextEdit {
    Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CTerminalEdit(QWidget* p_parent);
    ~CTerminalEdit(void);

// section of private data ----------------------------------------------------
private:
    /// we need to override some key actions
    virtual void keyPressEvent(QKeyEvent* p_event);

    /// to handle wheel selection we need mouse actions
    virtual void mouseReleaseEvent(QMouseEvent* p_event);

    /// event to handle drags and drops
    virtual void dropEvent(QDropEvent* p_event);
};

// -----------------------------------------------------------------------------

#endif
