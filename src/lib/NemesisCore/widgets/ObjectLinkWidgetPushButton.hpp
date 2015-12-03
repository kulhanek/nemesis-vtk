#ifndef ObjectLinkWidgetPushButtonH
#define ObjectLinkWidgetPushButtonH
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

#include <QPushButton>

//------------------------------------------------------------------------------

class QObjectLinkWidget;

//------------------------------------------------------------------------------

/// handle drop actions for QObjectLinkWidget
class QObjectLinkWidgetPushButton : public QPushButton {
public:
    QObjectLinkWidgetPushButton(QWidget* p_parent);

// section of private data -----------------------------------------------------
private:
    QObjectLinkWidget* MasterWidget;

    /// can we accept the event
    virtual void dragEnterEvent(QDragEnterEvent *event);

    /// process drop event
    virtual void dropEvent(QDropEvent *event);
};

//------------------------------------------------------------------------------

#endif
