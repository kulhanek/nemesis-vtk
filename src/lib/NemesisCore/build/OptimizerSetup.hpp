#ifndef OptimizerSetupH
#define OptimizerSetupH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <NemesisCoreMainHeader.hpp>
#include <ExtComObject.hpp>

//------------------------------------------------------------------------------

class CXMLElement;

//------------------------------------------------------------------------------

/// base class for molecule geometry optimizer

class NEMESIS_CORE_PACKAGE COptimizerSetup : public CExtComObject {
Q_OBJECT
public:
// constructor and destructor -------------------
    COptimizerSetup(CPluginObject* p_objectinfo,CExtComObject* p_parent);

// information methods ---------------------------------------------------------
    /// return UUID of associated optimizer
    virtual const CUUID& GetOptimizerUUID(void);

    /// get notify tick interval
    virtual int GetNotifyTickInterval(void);

// executive methods -----------------------------------------------------------
    /// load optimizer setup
    virtual void LoadData(CXMLElement* p_ele);

    /// save optimizer setup
    virtual void SaveData(CXMLElement* p_ele);

// public signals --------------------------------------------------------------
signals:
    void OnSetupChanged(void);
};

//------------------------------------------------------------------------------

#endif
