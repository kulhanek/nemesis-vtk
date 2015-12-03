#ifndef DesktopSystemH
#define DesktopSystemH
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

#include <NemesisCoreMainHeader.hpp>
#include <XMLDocument.hpp>
#include <UUID.hpp>
#include <ExtComObject.hpp>

//---------------------------------------------------------------------------

class CXMLElement;
class CFileName;

//------------------------------------------------------------------------------

class CDesktopSystem : public CExtComObject {
public:
    CDesktopSystem(CPluginObject* p_pob,QObject* p_parent);

// desktop management ----------------------------------------------------------
    /// load desktop
    bool LoadDesktop(const CFileName& configname);

    /// save desktop
    bool SaveDesktop(const CFileName& configname);

    /// load desktop
    void LoadData(CXMLElement* p_ele);

    /// save desktop
    void SaveData(CXMLElement* p_ele);

// information methods ---------------------------------------------------------
    /// get all work panels configuration
    CXMLElement* GetWorkPanelsSetup(void);

    /// get work panel configuration data
    CXMLElement* GetWorkPanelSetup(const CExtUUID& uuid);

    /// get object designer configuration data
    CXMLElement* GetObjectDesignerSetup(const CExtUUID& uuid);

// private data -------------------------------------------------------------
protected:
    CXMLDocument    DesktopData;
};

//---------------------------------------------------------------------------
#endif
