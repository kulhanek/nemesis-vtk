// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <AboutWindow.hpp>
#include <ErrorSystem.hpp>
#include <MainWindow.hpp>
#include <QDesktopServices>
#include <QUrl>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ConnectHelpMenu(void)
{
    BIND_ACTION(actionNemesisHelp);
    BIND_ACTION(actionAboutNemesis);
    BIND_ACTION(actionAboutQt);

    //====================================
    connect(actionNemesisHelp,SIGNAL(triggered(bool)),
            this,SLOT(NemesisHelp(void)));
    //-----------------
    connect(actionAboutNemesis,SIGNAL(triggered(bool)),
            this,SLOT(AboutNemesis(void)));
    //-----------------
    connect(actionAboutQt, SIGNAL(triggered(bool)),
            qApp, SLOT(aboutQt()));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::AboutNemesis(void)
{
    // has Qt::WA_DeleteOnClose flag set
    new CAboutWindow();
}

//------------------------------------------------------------------------------

void CMainWindow::NemesisHelp(void)
{
    QDesktopServices::openUrl(QUrl("https://nemesis.ncbr.muni.cz"));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



