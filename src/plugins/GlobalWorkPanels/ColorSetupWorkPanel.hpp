#ifndef PeriodicTablePanelH
#define PeriodicTablePanelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <WorkPanel.hpp>
#include "PeriodicTableWidget.hpp"
#include "ui_ColorSetupWorkPanel.h"

// -----------------------------------------------------------------------------

class CColorSetupWorkPanel : public CWorkPanel {
    Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CColorSetupWorkPanel();
    ~CColorSetupWorkPanel();

// section of private data ----------------------------------------------------
private slots:
    void UseValues(void);
    void ReInitValues(void);
    void ReInitElementColors(void);
    void SetNewElement(int z);
    void ButtonBoxClicked(QAbstractButton* p_button);

private:
    Ui::ColorSetupWorkPanel WidgetUI;
    CPeriodicTableWidget*   PeriodicTableWidget;
    int                     ActiveZ;
};

// -----------------------------------------------------------------------------

#endif
