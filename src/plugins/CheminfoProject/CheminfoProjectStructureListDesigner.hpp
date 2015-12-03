#ifndef CheminfoProjectStructureListDesignerH
#define CheminfoProjectStructureListDesignerH
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

#include <ProObjectDesigner.hpp>
#include "ui_CheminfoProjectStructureListDesigner.h"

//------------------------------------------------------------------------------

class CCheminfoProjectStructureList;
class CPODesignerGeneral;
//class CPODesignerCheminfoProjectStructureList;

//------------------------------------------------------------------------------

class CCheminfoProjectStructureListDesigner : public CProObjectDesigner {
    Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CCheminfoProjectStructureListDesigner(CCheminfoProjectStructureList* p_owner);

    /// Initialize visualization of properties
    void InitAllValues(void);

    /// Update object properties according to visual setup
    void ApplyAllValues(void);

// section of private data ----------------------------------------------------
private:
    CCheminfoProjectStructureList*             StructureList;
    Ui::CheminfoProjectStructureListDesigner   WidgetUI;
    CPODesignerGeneral*                        General;

private slots:
    void ButtonBoxClicked(QAbstractButton* p_button);
};

//------------------------------------------------------------------------------

#endif
