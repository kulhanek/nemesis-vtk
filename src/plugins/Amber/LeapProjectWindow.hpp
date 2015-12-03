#ifndef LeapProjectWindowH
#define LeapProjectWindowH
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

#include <MainWindow.hpp>
#include <QToolBar>
#include <UUID.hpp>
#include <QClipboard>
#include <HistoryList.hpp>
#include <AtomList.hpp>
#include <StandardModelSetup.hpp>

#include "ui_LeapProjectWindow.h"

//------------------------------------------------------------------------------

class CProject;
class CLeapProject;
class CWorkPanel;
class CHistoryList;
class CTerminalWidget;

//------------------------------------------------------------------------------

class CLeapProjectWindow : public CMainWindow {
Q_OBJECT
public:
// constructor and destructors -------------------------------------------------
    CLeapProjectWindow(CLeapProject* p_project);
    virtual ~CLeapProjectWindow(void);

    /// connect all menu - do not call it in constructor
    // it requires restored desktop
    void ConnectAllMenu(void);

// EDIT ========================================================================
private slots:
    void UndoChange(void);
    void RedoChange(void);
    void UpdateEditMenu(void);

private:
    void ConnectEditMenu(void);

// section of private data ----------------------------------------------------
private:
    CLeapProject*           Project;
    Ui::LeapProjectWindow   WidgetUI;
    CTerminalWidget*        Terminal;

private slots:
    void NewLineEntered(const QString& line);
};

//------------------------------------------------------------------------------

#endif
