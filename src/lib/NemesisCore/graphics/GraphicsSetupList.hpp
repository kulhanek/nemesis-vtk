#ifndef GraphicsSetupListH
#define GraphicsSetupListH
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
#include <GraphicsSetup.hpp>
#include <UUID.hpp>

// -----------------------------------------------------------------------------

class CGraphics;

// -----------------------------------------------------------------------------

///  list of all graphics setup objects

class NEMESIS_CORE_PACKAGE CGraphicsSetupList : public CProObject {
public:
// constructors and destructors -----------------------------------------------
    /// standard constructor
    CGraphicsSetupList(CGraphics* p_owner,bool no_index);

    /// alternative constructor used by CGraphicsSetupProfile
    CGraphicsSetupList(CPluginObject* p_object,CExtComObject* p_parent);

    ~CGraphicsSetupList(void);

// executive methods ----------------------------------------------------------
    /// find setup for object with id
    CGraphicsSetup* FindSetup(const CUUID& objecttype,int object_id=-1);

    /// create setup for object with id
    CGraphicsSetup* CreateSetup(const CUUID& objecttype,int object_id=-1);

    /// find setup type for given object type
    void FindSetupType(const CUUID& objecttype,CUUID& setuptype);

    /// remove setup
    void RemoveSetup(CGraphicsSetup* p_setup);

    /// clear all dynamics graphics setups
    void ClearSetups(void);

// input/output methods -------------------------------------------------------
    /// load setup objects
    void Load(CXMLElement* p_ele);

    /// save setup objects
    void Save(CXMLElement* p_ele);

// section of private data ----------------------------------------------------
private:
    CGraphicsSetup* FindSetupByType(const CUUID& setup_type);
};

// -----------------------------------------------------------------------------

#endif
