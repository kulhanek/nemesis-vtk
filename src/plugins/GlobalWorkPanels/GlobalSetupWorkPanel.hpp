#ifndef GlobalSetupWorkPanelH
#define GlobalSetupWorkPanelH
//==============================================================================
//    Copyright 1998,1999,2000,2001,2005 Petr Kulhanek
//
//    This file is part of HiPoLy library.
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor,
//    Boston, MA  02110-1301  USA
//==============================================================================

#include <QWidget>
#include <WorkPanel.hpp>
#include "ui_GlobalSetupWorkPanel.h"

// -----------------------------------------------------------------------------

class CGlobalSetupWorkPanel : public CWorkPanel {
    Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CGlobalSetupWorkPanel(void);
    ~CGlobalSetupWorkPanel();

// section of private data ----------------------------------------------------
private:
    Ui::GlobalSetupWorkPanel WidgetUI;
    QStandardItemModel*      ModulePaths;

    void InitStartupTab(void);
    void SaveStartupTab(void);

    void InitPathsTab(void);
    void SavePathsTab(void);

    void InitMouseTab(void);
    void SaveMouseTab(void);

    void InitGraphicsTab(void);
    void SaveGraphicsTab(void);

private slots:
    void buttonBoxClicked(QAbstractButton* p_button);
    void UpdateFixedPath(void);
    void RestoreMouseSystemDefaults(void);
    void RestoreMouseUserDefaults(void);
    void ApplyMouseSetup(void);
    void ClearModulePaths(void);
};

// -----------------------------------------------------------------------------

#endif
