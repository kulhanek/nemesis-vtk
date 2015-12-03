#ifndef GraphicsH
#define GraphicsH
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
#include <ProObject.hpp>
#include <Manipulator.hpp>

//------------------------------------------------------------------------------

class CProject;
class CXMLElement;
class CGraphicsSetupList;
class CGraphicsProfileList;
class CGraphicsObjectList;
class CGraphicsViewList;
class CGraphicsView;
class CGraphicsObject;

//------------------------------------------------------------------------------

/// main graphics container

class NEMESIS_CORE_PACKAGE CGraphics : public CProObject {
public:
// constructors and destructors -----------------------------------------------
    CGraphics(CProject* p_owner,bool no_index);
    ~CGraphics(void);

    /// setup static indexes
    void SetStaticIndexes(void);

// methods with history -------------------------------------------------------
    /// clear all dynamics graphics objects
    void ClearGraphics(void);

// information methods --------------------------------------------------------
    /// get pointer to all objects
    CGraphicsObjectList*    GetObjects(void);

    /// get pointer to all profiles
    CGraphicsProfileList*   GetProfiles(void);

    /// get pointer to all setups
    CGraphicsSetupList*     GetSetups(void);

    /// get pointer to all views
    CGraphicsViewList*      GetViews(void);

    /// get pointer to the primary view
    CGraphicsView*          GetPrimaryView(void);

// input/output methods -------------------------------------------------------
    virtual void LoadData(CXMLElement* p_ele);
    virtual void SaveData(CXMLElement* p_ele);

// section of public data -----------------------------------------------------
public:
    CGraphicsObjectList*        Objects;
    CGraphicsProfileList*       Profiles;
    CGraphicsViewList*          Views;
    CGraphicsSetupList*          Setups;
};

//------------------------------------------------------------------------------

#endif
