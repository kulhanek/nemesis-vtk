#ifndef SpecAxesObjectDesignerH
#define SpecAxesObjectDesignerH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include "ui_SpecAxesObjectDesigner.h"
#include <HistoryList.hpp>

//------------------------------------------------------------------------------

class CSpecAxesObject;
class CPODesignerGeneral;
class CGODesignerObjects;
class CGODesignerSetup;
class CGODesignerRefBy;

//------------------------------------------------------------------------------

/// special axes designer

class CSpecAxesObjectDesigner : public CProObjectDesigner {
Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    CSpecAxesObjectDesigner(CSpecAxesObject* p_go);

    /// initialize visualization of properties
    void InitAllValues(void);

    /// update object properties according to visual setup
    void ApplyAllValues(void);

// section of private data -----------------------------------------------------
private:
    Ui::SpecAxesObjectDesigner  WidgetUI;
    CSpecAxesObject*            Object;
    CPODesignerGeneral*         General;
    CGODesignerObjects*         Objects;
    CGODesignerSetup*           Setup;
    CGODesignerRefBy*           RefBy;

private slots:
    void InitValues(void);
    void ApplyValues(void);
    void ButtonBoxClicked(QAbstractButton*);
    void ProjectLockChanged(EHistoryChangeMessage message);
    void ShowCOMAsPoint(void);
    void ShowAAsVector(void);
    void ShowBAsVector(void);
    void ShowCAsVector(void);
};

//------------------------------------------------------------------------------

#endif
