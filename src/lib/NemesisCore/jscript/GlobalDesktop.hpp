#ifndef GlobalDesktopH
#define GlobalDesktopH
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

class CXMLElement;

//---------------------------------------------------------------------------

enum EDesktopProjectRestoreMode {
    EDPRM_NEW_ROJECT_WP,
    EDPRM_USER
};

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CGlobalDesktop : public CDesktopSystem {
Q_OBJECT
public:
    CGlobalDesktop(QObject* p_parent);

// desktop management ----------------------------------------------------------
    /// load default user desktop
    bool LoadDefaultDesktop(void);

    /// save default user desktop
    bool SaveDefaultDesktop(void);

// initial project -------------------------------------------------------------
    /// set project restoration mode
    void SetProjectRestoreMode(EDesktopProjectRestoreMode mode);

    /// get project restore mode
    EDesktopProjectRestoreMode GetProjectRestoreMode(void);

    /// set initial project type
    void SetInitialProjectType(const CUUID& uuid);

    /// get initial project type
    const CUUID GetInitialProjectType(void);

    /// restore initial project
    void RestoreInitialProject(void);

// global work panels ----------------------------------------------------------
    /// enable/disable desktop restoration
    void EnableWPRestoration(bool enable);

    /// returns if desktop restore is enabled
    bool IsWPRestorationEnabled(void);

    /// enable/disable desktop restoration
    void EnableInitialProjectAfterLastIsClosed(bool enable);

    /// returns if desktop restore is enabled
    bool IsInitialProjectEnabledAfterLastIsClosed(void);

    /// restore global workpanels
    void RestoreWorkPanels(void);

// notify project main windows that they should update its setup
    void NotifyAllMainWindows(const QString sextuuid);

// signals ---------------------------------------------------------------------
signals:
    ///
    void OnMainWindowSetupChanged(const QString sextuuid);
};

//------------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE CGlobalDesktop*      GlobalDesktop;

//---------------------------------------------------------------------------
#endif
