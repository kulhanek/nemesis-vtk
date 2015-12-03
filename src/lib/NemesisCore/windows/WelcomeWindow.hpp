#ifndef WelcomeWindowH
#define WelcomeWindowH
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

#include <QWidget>
#include "ui_WelcomeWindow.h"

//------------------------------------------------------------------------------

class CWelcomeWindow : public QWidget {
    Q_OBJECT

public:
// constructor and destructors ------------------------------------------------
    CWelcomeWindow(void);
    virtual ~CWelcomeWindow(void);

// setup methods ---------------------------------------------------------------
    /// determine number of initialization steps
    void SetMaxProgressSteps(int max_steps);

// public slots ----------------------------------------------------------------
public slots:
    /// increment counter of initializations steps
    void IncrementProgress(const QString& text);

    /// change only text, do not update progress
    void ChangeNotification(const QString& text);

// section of private data ----------------------------------------------------
private:
    Ui::WelcomeWindow   WidgetUI;           // ui of window
    int                 MaxProgessSteps;
    int                 CurrProgessStep;
};

//------------------------------------------------------------------------------

#endif
