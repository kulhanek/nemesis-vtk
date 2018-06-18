// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <WorkPanelDockWidget.hpp>
#include <WorkPanel.hpp>
#include <QCloseEvent>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CWorkPanelDockWidget::CWorkPanelDockWidget(CWorkPanel* p_child,QWidget* p_parent)
    : QDockWidget(p_child->windowTitle(),p_parent)
{
    Child = p_child;

    connect(this,SIGNAL(topLevelChanged(bool)),
            this,SLOT(TopLevelChanged(bool)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CWorkPanelDockWidget::closeEvent(QCloseEvent *event)
{
    if( Child == NULL ) return;
    Child->closeEvent(event);
    if( event->isAccepted() ) {
        Child->QWidget::setParent(NULL);
        delete Child;
    }
}

//------------------------------------------------------------------------------

void CWorkPanelDockWidget::TopLevelChanged(bool set)
{
    if( Child == NULL ) return;
    Child->WindowDockStatusChanged(!set);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

