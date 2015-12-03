#ifndef GODesignerFlagsH
#define GODesignerFlagsH
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

#include <QWidget>
#include "ui_GODesignerFlags.h"
#include <HistoryList.hpp>

//------------------------------------------------------------------------------

class CGraphicsObject;
class CProObjectDesigner;

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CGODesignerFlags : public QWidget {
Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    CGODesignerFlags(QWidget* p_owner,CGraphicsObject* p_object,CProObjectDesigner* p_designer);

// executive methods -----------------------------------------------------------
public slots:
    /// initialize visualization of general properties
    void InitValues(void);

public:
    /// update object general properties according to visual setup
    void ApplyValues(void);

    /// load designer setup
    void LoadDesignerSetup(CXMLElement* p_ele);

    /// save designer setup
    void SaveDesignerSetup(CXMLElement* p_ele);

// section of private data -----------------------------------------------------
private:
    Ui::GODesignerFlags WidgetUI;
    CGraphicsObject*    Object;
    CProObjectDesigner* Designer;

private slots:
    void ProjectLockChanged(EHistoryChangeMessage message);
};

//------------------------------------------------------------------------------

#endif
