#ifndef ConfigMainWindowH
#define ConfigMainWindowH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Tomas Marek, xmarek11@stud.fit.vutbr.cz
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

#include <QDialog>
#include <QList>
#include <QAction>
#include <QToolBar>
#include <QMenu>
#include <QStandardItemModel>
#include <WorkPanel.hpp>

#include "ui_ConfigMainWindow.h"

// -----------------------------------------------------------------------------

class CMainWindow;

// -----------------------------------------------------------------------------

class CConfigMainWindow : public CWorkPanel {
Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CConfigMainWindow(CMainWindow* p_parent);
    ~CConfigMainWindow();

// section of private data ----------------------------------------------------
private slots:
    void ActionListClicked(const QModelIndex & index);

    void AddToolBar(void);
    void RemoveToolBar(void);
    void ToolBarListClicked(const QModelIndex & index);

    void AddAction(void);
    void RemoveAction(void);
    void MoveUpAction(void);
    void MoveDownAction(void);
    void AddSeparator(void);
    void ToolBarActionListClicked(const QModelIndex & index);

    void ButtonBoxClicked(QAbstractButton *);

    // shortcuts
    void SetShortcut(void);
    void ResetShortcut(void);

private:
    Ui::ConfigMainWindow    WidgetUI;
    CMainWindow*            Parent;
    QStandardItemModel*     ActionsModel;
    QStandardItemModel*     ToolBarsModel;
    QStandardItemModel*     ToolBarActionsModel;
    QAction*                ActiveAction;
    QToolBar*               ActiveToolBar;
    QAction*                ActiveToolBarAction;

    void UpdateActionsList(void);
    void UpdateMenuItems(QWidget* p_widget,QStandardItem* p_root);
    void UpdateMenuItemsShortcuts(void);
    void UpdateMenuItemsShortcuts(QStandardItem* p_item);
    void UpdateToolBarsList(void);
    void UpdateToolBarActionList(void);

    void RefreshWidgetLists(void);
    bool RestoreDefaultToolbar(void);

    void HandleKeyEvent(QKeyEvent *);
    bool eventFilter(QObject *, QEvent *);
};

// -----------------------------------------------------------------------------

#endif
