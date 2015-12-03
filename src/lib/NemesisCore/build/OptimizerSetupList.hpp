#ifndef OptimizerSetupListH
#define OptimizerSetupListH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <OptimizerSetup.hpp>

//------------------------------------------------------------------------------

class CStructure;
class COptimizerSetup;
class COptimizer;

//------------------------------------------------------------------------------

/// base class for molecule geometry optimizer

class NEMESIS_CORE_PACKAGE COptimizerSetupList : public CExtComObject {
public:
// constructor and destructor --------------------------------------------------
    COptimizerSetupList(CExtComObject* p_parent);

// setup -----------------------------------------------------------------------
    /// load user or optionaly system optimizer setup list
    bool LoadConfig(void);

    /// save optimizer setup list
    bool SaveConfig(void);

    /// close all setup designers
    void CloseAllObjectDesigners(void);

// executive methods -----------------------------------------------------------
    /// create default optimizer
    COptimizer* CreateDefaultOptimizerJob(CStructure* p_mol);

    /// open default optimizer setup
    void OpenDefaultOptimizerSetupDesigner(void);

// section of private data ----------------------------------------------------
private:
    COptimizerSetup* DefaultSetup;

    /// init default optimizer setup
    bool InitDefaultSetup(void);

    /// create setup from XML stream
    COptimizerSetup* CreateSetup(CXMLElement* p_data);
};

//------------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE COptimizerSetupList*  OptimizerSetups;

//------------------------------------------------------------------------------

#endif

