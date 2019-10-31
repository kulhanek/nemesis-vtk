#ifndef LightObjectH
#define LightObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <GraphicsObject.hpp>
#include <GraphicsUtil.hpp>

//------------------------------------------------------------------------------

class CLightSetup;
class CGraphicsObjectList;

//------------------------------------------------------------------------------

extern CExtUUID LightObjectID;

//------------------------------------------------------------------------------

enum ELightObjectFlag {
    // user flags               = 0x00010000    // first user flag
    ELOF_GLOBAL_POSITION        = 0x00010000,
    ELOF_SHOW_COVER             = 0x00020000,
    ELOF_ATTENUATION_ENABLED    = 0x00040000,
    ELOF_SPOT_ENABLED           = 0x00080000
};

//------------------------------------------------------------------------------

/// draw light

class NEMESIS_CORE_PACKAGE CLightObject : public CGraphicsObject {
public:
// constructors and destructors ------------------------------------------------
    CLightObject(CGraphicsObjectList* p_gl);

// setup methods ---------------------------------------------------------------
    /// set position
    bool SetPositionWH(const CPoint& pos);

    /// set orientation
    bool SetOrientationWH(const CPoint& rot);

    /// set position
    void SetPosition(const CPoint& pos,CHistoryNode* p_history=NULL);

    /// set orientation
    void SetOrientation(const CPoint& rot,CHistoryNode* p_history=NULL);

    /// get position
    const CPoint& GetPosition(void) const;

    /// get orientation
    const CPoint& GetOrientation(void) const;

// executive methods -----------------------------------------------------------
    /// draw object
    virtual void Draw(void);

    /// calculate visible object metrics
    virtual void GetObjectMetrics(CObjMetrics& metrics);

// input/outpu methods ---------------------------------------------------------
    /// load object specific data
    virtual void LoadData(CXMLElement* p_ele);

    /// save object specific data
    virtual void SaveData(CXMLElement* p_ele);

// section of public data ------------------------------------------------------
private:
    CLightSetup*    Setup;
    int             LightIndex;
    CPoint          Position;
    CPoint          Orientation;
    CCylinder       Cylinder;

    GLenum          GetLightEnumeration(int index);
    void            InitLight(void);
    void            MakeLight(void);
    void            DrawCover(void);

    /// update Cylinder and Cone
    virtual void SetupChanged(void);
};

//------------------------------------------------------------------------------

#endif
