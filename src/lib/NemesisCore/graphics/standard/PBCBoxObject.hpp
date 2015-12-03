#ifndef PBCBoxObjectH
#define PBCBoxObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

class CPBCBoxSetup;
class CStructure;

//------------------------------------------------------------------------------

extern CExtUUID PBCBoxObjectID;

//------------------------------------------------------------------------------

enum EPBCBoxObjectFlag {
    // user flags               = 0x00010000    // first user flag
    EPBCBOF_SHOW_CENTER         = 0x00010000,
    EPBCBOF_SHOW_FAMILIAR       = 0x00020000,
    EPBCBOF_CENTER_TO_ORIGIN    = 0x00040000
};

//------------------------------------------------------------------------------

/// draw PBC box

class NEMESIS_CORE_PACKAGE CPBCBoxObject : public CGraphicsObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CPBCBoxObject(CGraphicsObjectList* p_gl);

// setup methods ---------------------------------------------------------------
    /// set k-offsets
    bool SetKOffsetsWH(int ka,int kb,int kc);

    /// set k-offsets
    void SetKOffsets(int ka,int kb,int kc,CHistoryNode* p_history=NULL);

// information methods ---------------------------------------------------------
    /// geometry descriptor
    virtual CGeoDescriptor GetGeoDescriptor(void);

    /// get ka offset
    int GetKAOffset(void);

    /// get kb offset
    int GetKBOffset(void);

    /// get kc offset
    int GetKCOffset(void);

// executive methods -----------------------------------------------------------
    /// draw object
    virtual void Draw(void);

    /// calculate visible object metrics
    virtual void GetObjectMetrics(CObjMetrics& metrics);

    /// can object be added
    virtual bool CanBeObjectAdded(CProObject* p_object);

// input/outpu methods ---------------------------------------------------------
    /// load object specific data
    virtual void LoadData(CXMLElement* p_ele);

    /// save object specific data
    virtual void SaveData(CXMLElement* p_ele);

// section of private data ----------------------------------------------------
private:
    CPBCBoxSetup*   Setup;
    int             KA;
    int             KB;
    int             KC;
    CSphere         Sphere;
    CCylinder       Cylinder;

    /// setup was changed
    virtual void SetupChanged(void);

    /// draw box
    void DrawRegularBox(CStructure* p_str);

    /// draw box
    void DrawFamiliarBox(CStructure* p_str);

    /// draw center
    void DrawCenter(CStructure* p_str);

    /// set initial color scheme
    void SetInitialColorScheme(void);

    /// draw tube
    void DrawTube(CPoint& p1,CPoint& p2);

    /// draw sphere
    void DrawSphere(CPoint& p1);

    /// get regular box metrics
    void GetRegularBoxMetrics(CStructure* p_str,CObjMetrics& metrics);

    /// get familiar box metrics
    void GetFamiliarBoxMetrics(CStructure* p_str,CObjMetrics& metrics);
};

//---------------------------------------------------------------------------

#endif
