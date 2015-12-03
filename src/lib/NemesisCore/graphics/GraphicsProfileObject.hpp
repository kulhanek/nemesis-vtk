#ifndef GraphicsProfileObjectH
#define GraphicsProfileObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

// this is here because Windows has defined a macro for GetObject->GetObjectA
// substitution, we need to aply it on all GetObject occurences
#if defined _WIN32 || defined __CYGWIN__
#include <windows.h>
#endif

#include <ProObject.hpp>

//------------------------------------------------------------------------------

class CGraphicsProfile;
class CGraphicsObject;

//------------------------------------------------------------------------------

/// graphics profile object

class NEMESIS_CORE_PACKAGE CGraphicsProfileObject : public CProObject {
public:
// constructors and destructors ------------------------------------------------
    CGraphicsProfileObject(CGraphicsProfile* p_profile,CGraphicsObject* p_object);

// executive methods -----------------------------------------------------------
    /// toggle show/hide
    bool ToggleShowHideFlagWH(void);

    /// emit parent OnGraphicsProfileChanged
    virtual void SetFlags(const CProObjectFlags& flags,CHistoryNode* p_history=NULL);

// information methods ---------------------------------------------------------
    /// return graphics object
    CGraphicsObject*    GetObject(void) const;

    /// return profile
    CGraphicsProfile*   GetGraphicsProfile(void) const;

// input/output methods --------------------------------------------------------
    /// load object setup
    virtual void LoadData(CXMLElement* p_ele);

    /// save object setup
    virtual void SaveData(CXMLElement* p_ele);

// section of private data -----------------------------------------------------
private:
    CGraphicsObject*        Object;
    CGraphicsProfileObject* NextObject;

// private methods -------------------------------------------------------------
    virtual ~CGraphicsProfileObject(void);

    friend class CGraphicsProfile;
};

//------------------------------------------------------------------------------

#endif
