#ifndef CheminfoProjectStructureDesignerH
#define CheminfoProjectStructureDesignerH
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
#include "ui_CheminfoProjectStructureDesigner.h"
#include <QStandardItemModel>

//------------------------------------------------------------------------------

class CCheminfoProjectStructure;
class CPODesignerGeneral;
class CPODesignerFlags;
class CPODesignerRefBy;

//------------------------------------------------------------------------------

class CCheminfoProjectStructureDesigner : public CProObjectDesigner {
Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CCheminfoProjectStructureDesigner(CCheminfoProjectStructure *p_chinfo);

    /// Initialize visualization of properties
    void InitAllValues(void);

    /// Update object properties according to visual setup
    void ApplyAllValues(void);

// section of private data ----------------------------------------------------
private:
    CCheminfoProjectStructure*             Object;
    Ui::CheminfoProjectStructureDesigner   WidgetUI;
    CPODesignerGeneral*     General;
    CPODesignerFlags*       Flags;
    CPODesignerRefBy*       RefBy;

    CContainerModel*        StructureModel;

private slots:
    void InitValues(void);
    void ApplyValues(void);

    void OpenStructureDesigner(void);
    void ObjectTVDblClicked(const QModelIndex& index);
    void ButtonBoxClicked(QAbstractButton *);
    void UpdateStatistics(void);

};

//------------------------------------------------------------------------------

#endif
