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

#include <TerminalEdit.hpp>
#include <TerminalWidget.hpp>
#include <QKeyEvent>


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTerminalEdit::CTerminalEdit(QWidget* p_parent)
    : QTextEdit(p_parent)
{

}

// -----------------------------------------------------------------------------

CTerminalEdit::~CTerminalEdit(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTerminalEdit::keyPressEvent(QKeyEvent* p_event)
{
    CTerminalWidget* p_parent = dynamic_cast<CTerminalWidget*>(parent());
    if( p_parent == NULL ) return;

    if( p_parent->KeyPressEvent(p_event) ) return;

    QTextEdit::keyPressEvent(p_event);
}

//------------------------------------------------------------------------------

void CTerminalEdit::mouseReleaseEvent(QMouseEvent* p_event)
{
    CTerminalWidget* p_parent = dynamic_cast<CTerminalWidget*>(parent());
    if( p_parent == NULL ) return;

    // let parent check cursor position
    if(! p_parent->MouseReleaseEvent(p_event)){
        QTextEdit::mouseReleaseEvent(p_event);
    }
}

//------------------------------------------------------------------------------

void CTerminalEdit::dropEvent(QDropEvent* p_event)
{
    CTerminalWidget* p_parent = dynamic_cast<CTerminalWidget*>(parent());
    if( p_parent == NULL ) return;

    // let parent handle it
    if(! p_parent->DragDropEvent(p_event)){
        QTextEdit::dropEvent(p_event);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


