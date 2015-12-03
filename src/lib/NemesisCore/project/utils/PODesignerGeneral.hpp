#ifndef PODesignerGeneralH
#define PODesignerGeneralH
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

#include <QWidget>
#include "ui_PODesignerGeneral.h"
#include <HistoryList.hpp>

//------------------------------------------------------------------------------

class CProObjectDesigner;
class CProObject;

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CPODesignerGeneral : public QWidget {
Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    CPODesignerGeneral(QWidget* p_owner,CProObject* p_object,CProObjectDesigner* p_designer);

// executive methods -----------------------------------------------------------
    /// initialize visualization of general properties
    void InitValues(void);

    /// update object general properties according to visual setup
    void ApplyValues(void);

// section of private data -----------------------------------------------------
private:
    Ui::PODesignerGeneral   WidgetUI;
    CProObject*             Object;
    CProObjectDesigner*     Designer;

private slots:
    void ProjectStatusChanged(EStatusChanged status);
    void ObjectStatusChanged(EStatusChanged status);
    void ProjectLockChanged(EHistoryChangeMessage message);
    void TryToSelect(void);
};

//------------------------------------------------------------------------------

#endif
