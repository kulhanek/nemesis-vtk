#ifndef NewPropertyWorkPanelH
#define NewPropertyWorkPanelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <WorkPanel.hpp>
#include "ui_NewPropertyWorkPanel.h"
#include <QStandardItemModel>

// -----------------------------------------------------------------------------

/// create new property

class CNewPropertyWorkPanel : public CWorkPanel {
Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CNewPropertyWorkPanel(CProject* p_project);
    ~CNewPropertyWorkPanel();

// section of private data ----------------------------------------------------
private:
    Ui::NewPropertyWorkPanel    WidgetUI;
    QStandardItemModel*         Model;

    virtual void LoadWorkPanelSpecificData(CXMLElement* p_ele);
    virtual void SaveWorkPanelSpecificData(CXMLElement* p_ele);

private slots:
    void ListItemChanged(const QModelIndex & index);
    void CreateObject(void);
    void CreateObject(const QModelIndex & index);
};

// -----------------------------------------------------------------------------

#endif
