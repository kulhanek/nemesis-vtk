#ifndef BoxObjectDesignerH
#define BoxObjectDesignerH
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
#include "ui_BoxObjectDesigner.h"
#include <HistoryList.hpp>

//------------------------------------------------------------------------------

class CBoxObject;
class CPODesignerGeneral;
class CGODesignerSetup;
class CGODesignerRefBy;

//------------------------------------------------------------------------------

class CBoxObjectDesigner : public CProObjectDesigner {
Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CBoxObjectDesigner(CBoxObject* p_bo);

    /// initialize visualization of properties
    void InitAllValues(void);

    /// update object properties according to visual setup
    void ApplyAllValues(void);

// section of private data ----------------------------------------------------
private:
    Ui::BoxObjectDesigner   WidgetUI;
    CBoxObject*             Object;
    CPODesignerGeneral*     General;
    CGODesignerSetup*       Setup;
    CGODesignerRefBy*       RefBy;

private slots:
    void InitValues(void);
    void ApplyValues(void);
    void ButtonBoxClicked(QAbstractButton*);
    void PositionObjectChanged(void);
    void DirectionObjectChanged(void);

    void ChangeToCartesian(void);
    void ChangeToSpherical(void);

    void ProjectLockChanged(EHistoryChangeMessage message);
};

//------------------------------------------------------------------------------

#endif
