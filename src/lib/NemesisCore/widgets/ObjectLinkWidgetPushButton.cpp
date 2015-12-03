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

#include <ObjectLinkWidgetPushButton.hpp>
#include <ObjectLinkWidget.hpp>
#include <QDragEnterEvent>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObjectLinkWidgetPushButton::QObjectLinkWidgetPushButton(QWidget* p_parent)
    : QPushButton(p_parent)
{
    MasterWidget = dynamic_cast<QObjectLinkWidget*>(p_parent);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QObjectLinkWidgetPushButton::dragEnterEvent(QDragEnterEvent *event)
{
    if( MasterWidget ){
        MasterWidget->PBDragEnterEvent(event);
    }
}

//------------------------------------------------------------------------------

void QObjectLinkWidgetPushButton::dropEvent(QDropEvent *event)
{
    if( MasterWidget ){
        MasterWidget->PBDropEvent(event);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


