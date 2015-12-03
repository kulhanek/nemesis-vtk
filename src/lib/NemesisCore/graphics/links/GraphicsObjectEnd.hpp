#ifndef GraphicsObjectEndH
#define GraphicsObjectEndH
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
#include <GraphicsObject.hpp>

// -----------------------------------------------------------------------------

class CGraphicsObjectLink;

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CGraphicsObjectEnd {
public:
// constructors and destructors ------------------------------------------------
    CGraphicsObjectEnd(CProObject* p_owner);
    ~CGraphicsObjectEnd(void);

// methods with changes registered into history list ---------------------------
    /// create graphics object
    bool CreateGraphicsObjectWH(void);

    /// toggle hide/show object
    bool ToggleShowHideFlagWH(void);

    /// remove graphics object
    bool RemoveGraphicsObjectWH(void);

// information methods ---------------------------------------------------------
    /// return end object
    CProObject* GetEndObject(void);

    /// get link object
    CGraphicsObjectLink* GetLinkObject(void);

// executive methods -----------------------------------------------------------
    /// create graphics object
    void CreateGraphicsObject(CHistoryNode* p_history=NULL);

    /// draw object
    virtual void Draw(void);

    /// access to object setup
    CGraphicsSetup* GetSetup(void) const;

    /// type
    template<class type>
    type* GetSetup(void) const;

    /// calculate visible object metrics
    virtual void GetObjectMetrics(CObjMetrics& metrics);

    /// called when link object is created/removed
    virtual void LinkObjectUpdated(void);

// local/global coordinate system ----------------------------------------------
    /// apply global view transformation matrix
    void ApplyGlobalViewTransformation(void);

    /// apply face transformation matrix
    void ApplyFaceTransformation(void);

// section of private data -----------------------------------------------------
private:
    CProObject*             Object;
    CGraphicsObjectLink*    Link;

    friend class CGraphicsObjectLink;
};

//------------------------------------------------------------------------------

template<class type>
type* CGraphicsObjectEnd::GetSetup(void) const
{
    return( dynamic_cast<type*>(GetSetup()) );
}

//------------------------------------------------------------------------------

#endif
