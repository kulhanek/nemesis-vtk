#ifndef AutoRotateWorkPanelH
#define AutoRotateWorkPanelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include "ui_AutoRotateWorkPanel.h"

//------------------------------------------------------------------------------

/// auto rotate graphics view

class CAutoRotateWorkPanel : public CWorkPanel {
Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CAutoRotateWorkPanel(CProject* p_project);
    ~CAutoRotateWorkPanel(void);

// section of private data ----------------------------------------------------
private:
    Ui::AutoRotateWorkPanel WidgetUI;
    QTimer*                 RotateTimer;

    /// raised when window is docked/undocked from the main window
    virtual void WindowDockStatusChanged(bool docked);

private slots:
    void ViewChanged(void);
    void Play(void);
    void Stop(void);
    void UpdateTimer(void);
    void RotateTick(void);
};

//------------------------------------------------------------------------------

#endif
