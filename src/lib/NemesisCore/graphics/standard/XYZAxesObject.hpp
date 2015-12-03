#ifndef XYZAxesObjectH
#define XYZAxesObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008-2009 Petr Kulhanek, kulhanek@enzim.hu,
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
#include <GL/gl.h>
#include <GL/glu.h>
#include <GraphicsUtil.hpp>

//------------------------------------------------------------------------------

class CXYZAxesSetup;

//------------------------------------------------------------------------------

extern CExtUUID        XYZAxesObjectID;

//------------------------------------------------------------------------------

/// XYZ axes

class NEMESIS_CORE_PACKAGE CXYZAxesObject : public CGraphicsObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CXYZAxesObject(CGraphicsObjectList* p_gl);

// executive methods -----------------------------------------------------------
    /// draw object
    virtual void Draw(void);

    /// calculate visible object metrics
    virtual void GetObjectMetrics(CObjMetrics& metrics);

// section of private data -----------------------------------------------------
private:
    CCylinder       Cylinder;
    CCylinder       Cone;
    CXYZAxesSetup*  Setup;

    /// update color setup
    void SetInitialColorScheme(void);

    /// draw one axis
    void DrawAxis(const CPoint& vect,bool showm,
                  float pmag,float mmag,
                  CColor* npart,CColor* mpart,CColor* conec);

    /// update Cylinder and Cone
    virtual void SetupChanged(void);
};

//---------------------------------------------------------------------------

#endif
