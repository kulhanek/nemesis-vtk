#ifndef CheminfoDatabaseWorkPanelH
#define CheminfoDatabaseWorkPanelH
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
#include <MainWindow.hpp>
#include "CheminfoProject.hpp"

#include "ui_CheminfoDatabaseWorkPanel.h"

// -----------------------------------------------------------------------------

extern CExtUUID CheminfoProjectDatabaseStructureID;

//------------------------------------------------------------------------------

class CCheminfoProject;
class CContainerModel;

//------------------------------------------------------------------------------

class CCheminfoDatabaseWorkPanel : public CWorkPanel {
    Q_OBJECT
public:
    // constructor and destructors ------------------------------------------------
    CCheminfoDatabaseWorkPanel(CProject* p_project);
    ~CCheminfoDatabaseWorkPanel();

    // section of private data ----------------------------------------------------
private:
    CHistoryNode*                      History;
    Ui::CheminfoDatabaseWorkPanel      WidgetUI;
    CCheminfoProject*                  CheminfoProject;
    CCheminfoProjectDatabase*          Database;
    CCheminfoProjectDatabaseStructure* DatabaseStructure;

private slots:
    void UpdateValues(void);
    void ConnectionPBClicked(bool checked);
    void BrowseDBStructurePBClicked(bool checked);
    void BrowseReceptorPBClicked(bool checked);
    void DBApplyPBClicked(bool checked);
    void DBStructApplyPBClicked(bool checked);

};

// -----------------------------------------------------------------------------

#endif
