#ifndef PBCBoxSetupDesignerH
#define PBCBoxSetupDesignerH
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

#include <ProObjectDesigner.hpp>
#include "ui_PBCBoxSetupDesigner.h"

//------------------------------------------------------------------------------

class CPBCBoxSetup;
class CGSDesignerGeneral;
class CGSDesignerRefBy;

//------------------------------------------------------------------------------

class CPBCBoxSetupDesigner : public CProObjectDesigner {
Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CPBCBoxSetupDesigner(CPBCBoxSetup* p_setup);

    /// initialize visualization of properties
    void InitAllValues(void);

    /// update object properties according to visual setup
    void ApplyAllValues(void);

// section of private data ----------------------------------------------------
private:
    Ui::PBCBoxSetupDesigner WidgetUI;
    CPBCBoxSetup*           Setup;
    CGSDesignerGeneral*     General;
    CGSDesignerRefBy*       RefBy;

private slots:
    void InitValues(void);
    void ApplyValues(void);
    void ButtonBoxClicked(QAbstractButton *button);
};

//------------------------------------------------------------------------------

#endif
