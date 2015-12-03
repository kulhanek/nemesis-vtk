#ifndef ProjectDesktopH
#define ProjectDesktopH
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

#include <NemesisCoreMainHeader.hpp>
#include <DesktopSystem.hpp>

//---------------------------------------------------------------------------

class CProject;

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CProjectDesktop : public CDesktopSystem {
Q_OBJECT
public:
    CProjectDesktop(CProject* p_parent);

// desktop management ----------------------------------------------------------
    /// load default user desktop
    void LoadDefaultDesktop(void);

    /// save default user desktop
    void SaveDefaultDesktop(void);

    /// load main window setup - toolbars and shortcuts
    /// it is called by LoadDefaultDesktop
    void LoadMainWindowSetup(void);

    /// save main window user setup - toolbars and shortcuts
    void SaveMainWindowSetup(void);

// information methods ---------------------------------------------------------
    /// get main window configuration data
    CXMLElement* GetMainWindowSetup(void);

// main window shortcuts and toolbars ------------------------------------------
    /// get main window shortcuts
    CXMLElement* GetMainWindowShortcuts(bool system=false);

    /// get main window toolbars
    CXMLElement* GetMainWindowToolbars(bool system=false);

// section of private data -----------------------------------------------------
private:
    CProject*       Project;
    // extra main window setup that is not saved together with other desktop data
    // for example shortcuts and toolbars
    CXMLDocument    MainWindowExtra;

    // system configuration
    CXMLDocument    MainWindowExtraDefault;

    /// load user desktop
    void LoadDesktop(const QString& name);

    /// save  user desktop
    void SaveDesktop(const QString& name);
};

//------------------------------------------------------------------------------
#endif
