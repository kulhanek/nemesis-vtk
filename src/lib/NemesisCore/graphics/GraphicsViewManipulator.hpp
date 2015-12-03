#ifndef GraphicsViewManipulatorH
#define GraphicsViewManipulatorH
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
#include <Point.hpp>
#include <Transformation.hpp>
#include <Manipulator.hpp>

//------------------------------------------------------------------------------

/// base class for all object that can be manipulated

class NEMESIS_CORE_PACKAGE CGraphicsViewManipulator : public CManipulator {
public:
// constructors and destructors ------------------------------------------------
    CGraphicsViewManipulator(void);
    virtual   ~CGraphicsViewManipulator(void);

// executive methods -----------------------------------------------------------
    /// move scene
    virtual void Move(const CPoint& dmov,CGraphicsView* p_view);

    /// scale scene
    virtual void ChangeScale(double increment,CGraphicsView* p_view);

    /// move scene centerum
    virtual void MoveCentrum(const CPoint& dmov,CGraphicsView* p_view);

    /// rotate scene
    virtual void Rotate(const CPoint& drot,CGraphicsView* p_view);

    /// manip draw
    virtual void ManipDraw(void);

    /// draw scene in local frame
    virtual void RawDraw(void);

    /// set scene rotation
    void SetRotation(const CPoint& rot);

    /// set scene position
    void SetPos(const CPoint& pos);

    /// set scale
    void SetScale(double scale);

    /// set scene transformation matrix (rotation)
    void SetTrans(const CTransformation& tran);

    /// set scene fake rotation point
    void SetCentrum(const CPoint& centrum);

    /// aply global view tranformation matrix to OpenGL subsystem
    void ApplyGlobalViewTransformation(void);

    /// aply face transformation
    void ApplyFaceTransformation(void);

    /// reset manipulator
    void ResetManip(void);

// informational methods ------------------------------------------------------
    const CPoint&           GetPos(void) const;
    const CTransformation&  GetTrans(void) const;
    const CPoint&           GetCentrum(void) const;
    double                  GetScale(void) const;
    const CPoint            GetEulerAngles(void) const;

// input/output methods --------------------------------------------------------
    /// load manipulator data
    virtual void LoadManip(CXMLElement* p_ele);

    /// save manipulator data
    virtual void SaveManip(CXMLElement* p_ele);

// signals ---------------------------------------------------------------------
public:
    /// begin large data change
    void BeginUpdate(void);

    /// end update
    void EndUpdate(void);

    /// should somewhere emit OnManipulatorChanged
    /** it cannot be done here as the object does not inherit QObject */
    virtual void EmitManipulatorChanged(void);

// section of private data -----------------------------------------------------
private:
    CTransformation Trans;                  // natoceni objektu
    CPoint          Pos;                    // position of object
    CPoint          Centrum;                // fake center of object
    double          ScaleFac;               // scale factor for orthographic projection
    GLdouble        GlobalModelMatrix[16];  // backup of transformation matrix
    bool            Changed;                // data was changed
    int             UpdateLevel;            // begin/end level

    /// data were updated
    void DataUpdated(void);
};

//------------------------------------------------------------------------------

#endif

