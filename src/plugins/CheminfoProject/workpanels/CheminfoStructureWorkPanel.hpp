#ifndef CheminfoStructureWorkPanelH
#define CheminfoStructureWorkPanelH
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
#include "CheminfoProject.hpp"

#include "ui_CheminfoStructureWorkPanel.h"

// -----------------------------------------------------------------------------

class CCheminfoProject;
class CContainerModel;
class CCheminfoProjectStructure;
class CCheminfoProjectStructureList;
class CPODesignerCheminfoProjectStructureList;

//------------------------------------------------------------------------------

class CCheminfoStructureWorkPanel : public CWorkPanel {
    Q_OBJECT
public:
    // constructor and destructors ------------------------------------------------
    CCheminfoStructureWorkPanel(CProject* p_project);
    ~CCheminfoStructureWorkPanel();

    void ClickedOnProjectStructure(CCheminfoProjectStructure* p_chemstr);

    // section of private data ----------------------------------------------------
private:
    Ui::CheminfoStructureWorkPanel  WidgetUI;
    CCheminfoProject*               CheminfoProject;
    CCheminfoProjectStructureListProject*  CheminfoStructureListProject;
    CCheminfoProjectStructureList*  CheminfoStructureList;

    void AddTab(void);

private slots:
    void UpdateValues(void);
    void InitTabs(void);
};

// -----------------------------------------------------------------------------

#endif
