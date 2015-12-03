#ifndef StandardModelSetupDesignerH
#define StandardModelSetupDesignerH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <ProObjectDesigner.hpp>
#include "ui_StandardModelSetupDesigner.h"

//------------------------------------------------------------------------------

class CStandardModelSetup;
class CGSDesignerGeneral;
class CGSDesignerRefBy;

//------------------------------------------------------------------------------

class CStandardModelSetupDesigner : public CProObjectDesigner {
Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CStandardModelSetupDesigner(CStandardModelSetup* p_bo);

    /// initialize visualization of properties
    void InitAllValues(void);

    /// update object properties according to visual setup
    void ApplyAllValues(void);

// section of private data ----------------------------------------------------
private:
    Ui::StandardModelSetupDesigner  WidgetUI;
    CStandardModelSetup*            Setup;
    CGSDesignerGeneral*             General;
    CGSDesignerRefBy*               RefBy;
    int                             ModelIndex;

private slots:
    void InitValues(void);
    void ApplyValues(void);
    void ButtonBoxClicked(QAbstractButton *button);
    void ModelIndexChanged(int index);
};

//------------------------------------------------------------------------------

#endif
