#ifndef OpenBabelOptimizerSetupDesignerH
#define OpenBabelOptimizerSetupDesignerH
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

#include <ExtComObjectDesigner.hpp>
#include "ui_OpenBabelOptimizerSetupDesigner.h"

//------------------------------------------------------------------------------

class COpenBabelOptimizerSetup;

//------------------------------------------------------------------------------

class COpenBabelOptimizerSetupDesigner : public CExtComObjectDesigner {
    Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    COpenBabelOptimizerSetupDesigner(COpenBabelOptimizerSetup* p_bo);

// section of private data ----------------------------------------------------
private:
    Ui::OpenBabelOptimizerSetupDesigner WidgetUI;
    COpenBabelOptimizerSetup*           Object;

private slots:
    void ButtonBoxClicked(QAbstractButton*);

    /// initialize visualization of properties
    void InitValues(void);

    /// update object properties according to visual setup
    void ApplyValues(void);
};

//------------------------------------------------------------------------------

#endif
