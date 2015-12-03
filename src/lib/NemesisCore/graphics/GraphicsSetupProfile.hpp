#ifndef GraphicsSetupGlobalListH
#define GraphicsSetupGlobalListH
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
#include <GraphicsSetupList.hpp>
#include <SimpleList.hpp>
#include <UUID.hpp>
#include <DirectoryEnum.hpp>

// -----------------------------------------------------------------------------

class CProject;

// -----------------------------------------------------------------------------

///  list of all global setups

class NEMESIS_CORE_PACKAGE CGraphicsSetupProfile : public CGraphicsSetupList {
public:
// constructors and destructors ------------------------------------------------
    CGraphicsSetupProfile(CExtComObject* p_parent);
    ~CGraphicsSetupProfile(void);

// executive methods -----------------------------------------------------------
    /// init all setup objects
    void InitAllSetupObjects(void);

    /// close all setup designers
    void CloseAllObjectDesigners(void);

// global setup ----------------------------------------------------------------
    /// load default profile setup
    bool LoadDefaultConfig(void);

    /// save default profile setup
    bool SaveDefaultConfig(void);

    /// load specific profile setup
    bool LoadConfig(const CFileName& name);

    /// save specific profile setup
    bool SaveConfig(const CFileName& name);

    /// load user default setup
    bool LoadUserDefault(CGraphicsSetup* p_setup);

    /// load system default setup
    bool LoadSystemDefault(CGraphicsSetup* p_setup);

// section of public data ------------------------------------------------------
public:
    bool    MultiSamplingEnabled;   // enable multisampling antialiasing
    bool    QuadStereoEnabled;      // enable quad buffer stereo

// private data ----------------------------------------------------------------
private:
    /// find specific setup data
    CXMLElement* FindSetupXMLData(CXMLElement* p_ele,const CExtUUID& uuid);
};

// -----------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE CGraphicsSetupProfile* GraphicsSetupProfile;

// -----------------------------------------------------------------------------

#endif
