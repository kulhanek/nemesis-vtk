#ifndef WorkPanelDialogWidgetH
#define WorkPanelDialogWidgetH
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
#include <QDialog>

//------------------------------------------------------------------------------

class CWorkPanel;

//------------------------------------------------------------------------------

/// wrapper for WorkPanel dialog container

class CWorkPanelDialogWidget : public QDialog {
public:
    CWorkPanelDialogWidget(CWorkPanel* p_child,QWidget* p_parent);

    /// save dialog geometry - bugfixed version of QWidget::saveGeometry()
    QByteArray  saveGeometry(void) const;

    /// restore dialog geometry - bugfixed version of QWidget::restoreGeometry()
    bool        restoreGeometry(const QByteArray &geometry);

// section of private data ----------------------------------------------------
private:
    CWorkPanel*     Child;
};

//------------------------------------------------------------------------------

#endif
