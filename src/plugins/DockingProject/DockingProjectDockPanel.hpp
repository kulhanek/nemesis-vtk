#ifndef DockingProjectDockPanelH
#define DockingProjectDockPanelH
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

#include <WorkPanel.hpp>

#include "ui_DockingProjectDockPanel.h"

// -----------------------------------------------------------------------------

class CTrajectorySegment;
class CContainerModel;
class CDockingProject;
class CDockingProjectInputVinaExportTool;

//------------------------------------------------------------------------------

class CDockingProjectDockPanel : public CWorkPanel {
    Q_OBJECT
public:
    // constructor and destructors ------------------------------------------------
    CDockingProjectDockPanel(CProject* p_project);
    ~CDockingProjectDockPanel();

    // section of private data ----------------------------------------------------
private:
    CHistoryNode*                       History;
    Ui::DockingProjectDockPanel         WidgetUI;
    CDockingProject*                    DockingProject;
    CDockingProjectInputVinaExportTool* SetupVina;

    /// True if structures arent load in Nemesis and are filled their names in GUI
    bool LoadingNewStructures;

    /// Methods for prepare env for dock
    /// Will fill structured loaded or names filled in GUI
    bool SetStructures(void);
    /// Method handle with adding filled fields in GUI into system
    bool SetSettings(void);

private slots:
    /// Init and add tabs
    void InitTabs(void);
    /// Changes after signals
    void InitValues(void);

    /// Start prepare for docking procedure
    void StartPrepareDock(void);
    /// Start docking procedure
    void StartRunDock(void);

};

// -----------------------------------------------------------------------------

#endif
