#ifndef CheminfoProjectWindowH
#define CheminfoProjectWindowH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Tomas Marek, xmarek11@stud.fit.vutbr.cz,
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

#include <MainWindow.hpp>
#include "ui_CheminfoProjectWindow.h"

//------------------------------------------------------------------------------

class CCheminfoProject;
class CProjectStatusBar;
class CGraphicsPrimaryView;

//------------------------------------------------------------------------------

class CCheminfoProjectWindow : public CMainWindow {
    Q_OBJECT
public:
// constructor and destructors -------------------------------------------------
    CCheminfoProjectWindow(CCheminfoProject* p_project);
    virtual ~CCheminfoProjectWindow(void);

    /// connect all menu - do not call it in constructor
    // it requires restored desktop
    void ConnectAllMenu(void);

public slots:
    // panels
    void CheminfoWP(void);
    void CheminfoDatabaseWP(void);
    void CheminfoStructuresWP(void);

    // set database
    void SetDatabase(void);
    void SetStructureDatabase(void);

// section of private data -----------------------------------------------------
private:
    CCheminfoProject*           Project;
    CGraphicsPrimaryView*       OpenGL;
    CProjectStatusBar*          ProjectStatusBar;
    Ui::CheminfoProjectWindow   WidgetUI;

    // connect cheminfo menu
    void ConnectCheminfoMenu(void);

    // panel
    QAction* actionCheminfoWP;
    QAction* actionCheminfoDatabaseWP;
    QAction* actionCheminfoStructuresWP;
    QAction* actionSetStructureDatabase;

    // set database connection
    QAction* actionSetDatabase;


// events -----------------------------------
    // process ESC key to stop project running jobs
    virtual void keyPressEvent(QKeyEvent* p_event);
};

//------------------------------------------------------------------------------

#endif
