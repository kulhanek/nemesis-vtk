#ifndef MouseDriverSetupH
#define MouseDriverSetupH
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

#include <NemesisCoreMainHeader.hpp>
#include <Point.hpp>
#include <ExtComObject.hpp>

// -----------------------------------------------------------------------------

enum EMouseMode {
    EMM_SRT         = 0,
    EMM_SELECT      = 1,
    EMM_ROTATE      = 2,
    EMM_TRANSLATE   = 3,
    EMM_SCALE       = 4
};

// -----------------------------------------------------------------------------

/// setup for all common Mouse Drivers

class NEMESIS_CORE_PACKAGE CMouseDriverSetup : public CExtComObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CMouseDriverSetup(CExtComObject* p_parent);

// I/O methods -----------------------------------------------------------------
    /// load system setup if exists
    void LoadSystemSetup(void);

    /// load user setup if exist
    bool LoadUserSetup(void);

    /// save user setup
    bool SaveUserSetup(void);

// notification ----------------------------------------------------------------
    /// emit signal OnMouseSetupChanged
    void EmitOnMouseSetupChanged(void);

// signals ---------------------------------------------------------------------
signals:
    /// emited when setup is chenged
    void OnMouseSetupChanged(void);

// mouse preferences -----------------------------------------------------------
public:
    static CPoint       RSensitivity;       // sensitivity of rotation
    static CPoint       MSensitivity;       // sensitivity of movement
    static float        SSensitivity;       // scale sensitivity
    static float        WheelSensitivity;   // wheel sensitivity    
    static bool         ThreeButtonSimul;   // emulate three button mouse
    static EMouseMode   MouseMode;          // global mouse mode
};

// -----------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE class CMouseDriverSetup* MouseDriverSetup;

// -----------------------------------------------------------------------------

#endif
