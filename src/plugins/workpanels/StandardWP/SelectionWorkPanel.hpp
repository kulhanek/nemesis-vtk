#ifndef SelectionWorkPanelH
#define SelectionWorkPanelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <WorkPanel.hpp>
#include "ui_SelectionWorkPanel.h"

// -----------------------------------------------------------------------------

class CContainerModel;
class CSelectionList;

// -----------------------------------------------------------------------------

class CSelectionWorkPanel : public CWorkPanel {
    Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CSelectionWorkPanel(CProject* p_project);
    ~CSelectionWorkPanel();

// section of private data ----------------------------------------------------
private:
    Ui::SelectionWorkPanel      WidgetUI;
    CContainerModel*            Model;
    CSelectionList*             Selection;

    /// raised when window is docked/undocked from the main window
    virtual void WindowDockStatusChanged(bool docked);

private slots:
    void SetProject(CProject* p_project);
    void SelectionRequestChanged(void);
    void OpenProjectDesigner(void);
    void ListViewDoubleClicked(const QModelIndex& index);
    void UpdateNumberOfSelectedItems(void);
    void ProjectNameChanged(CProject* p_project);
};

// -----------------------------------------------------------------------------

#endif

