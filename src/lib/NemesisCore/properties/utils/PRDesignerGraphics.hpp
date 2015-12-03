#ifndef PRDesignerGraphicsH
#define PRDesignerGraphicsH
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
#include "ui_PRDesignerGraphics.h"
#include <HistoryList.hpp>

//------------------------------------------------------------------------------

class CGraphicsProperty;
class CContainerModel;
class CProObjectDesigner;

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CPRDesignerGraphics : public QWidget {
Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    CPRDesignerGraphics(QWidget* p_owner,CGraphicsProperty* p_prop,CProObjectDesigner* p_designer);

// executive methods -----------------------------------------------------------
public slots:
    /// initialize visualization of general properties
    void InitValues(void);

public:
    /// load designer setup
    void LoadDesignerSetup(CXMLElement* p_ele);

    /// save designer setup
    void SaveDesignerSetup(CXMLElement* p_ele);

// section of private data ----------------------------------------------------
private:
    Ui::PRDesignerGraphics  WidgetUI;
    CGraphicsProperty*      Property;
    CProObjectDesigner*     Designer;

private slots:
    void CreateLink(void);
    void ShowHideLink(void);
    void RemoveLink(void);
    void LinkObjectStatusChanged(EStatusChanged status);
    void ProjectLockChanged(EHistoryChangeMessage message);
};

//------------------------------------------------------------------------------

#endif
