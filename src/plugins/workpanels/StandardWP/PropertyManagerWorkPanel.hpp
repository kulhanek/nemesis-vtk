#ifndef PropertyManagerWorkPanelH
#define PropertyManagerWorkPanelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <HistoryList.hpp>
#include "ui_PropertyManagerWorkPanel.h"

//------------------------------------------------------------------------------

class CPropertyManagerWorkPanel : public CWorkPanel {
    Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CPropertyManagerWorkPanel(CProject* p_proj);
    ~CPropertyManagerWorkPanel(void);

// section of private data ----------------------------------------------------
private:
    Ui::PropertyManagerWorkPanel     WidgetUI;
    CContainerModel*                 PropertiesModel;

    /// raised when window is docked/undocked from the main window
    virtual void WindowDockStatusChanged(bool docked);

private slots:
    void OpenProjectDesigner(void);
    void ProjectNameChanged(CProject* p_project);

    void PropertiesTVSelectionChanged(void);
    void PropertiesTVDblClicked(const QModelIndex& index);

    void PropertyNew(void);
    void PropertyDelete(void);
    void PropertyInfo(void);

    void ProjectLockChanged(EHistoryChangeMessage message);
};

//------------------------------------------------------------------------------

#endif
