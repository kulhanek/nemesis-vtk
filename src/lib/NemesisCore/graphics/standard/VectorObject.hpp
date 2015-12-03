#ifndef VectorObjectH
#define VectorObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2011 Jiri Klusak, anim4tor@mail.muni.cz
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

class CVectorSetup;

//------------------------------------------------------------------------------

extern CExtUUID VectorObjectID;

//------------------------------------------------------------------------------

enum EVectorObjectFlag {
    // user flags               = 0x00010000    // first user flag
    EVOF_FREE_SIZE              = 0x00010000
};

//------------------------------------------------------------------------------

/// vector object

class NEMESIS_CORE_PACKAGE CVectorObject : public CGraphicsObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CVectorObject(CGraphicsObjectList* p_gl);

// setup methods ---------------------------------------------------------------
    /// set position
    bool SetPositionWH(const CPoint& position);

    /// set position
    bool SetPositionWH(CProObject* p_position);

    /// set direction
    bool SetDirectionWH(const CPoint& direction);

    /// set direction
    bool SetDirectionWH(CProObject* p_direction);

    /// set offset to position
    bool SetOffsetWH(double offset);

    /// set vector size
    bool SetLengthWH(double size);

// setup methods ---------------------------------------------------------------
    /// set position
    void SetPosition(const CPoint& position,CHistoryNode* p_history=NULL);

    /// set position
    void SetPosition(CProObject* p_position,CHistoryNode* p_history=NULL);

    /// set direction
    void SetDirection(const CPoint& direction,CHistoryNode* p_history=NULL);

    /// set direction
    void SetDirection(CProObject* p_direction,CHistoryNode* p_history=NULL);

    /// set offset to position
    void SetOffset(double offset,CHistoryNode* p_history=NULL);

    /// set vector length
    void SetLength(double length,CHistoryNode* p_history=NULL);

// information methods ---------------------------------------------------------
    /// get position
    CPoint GetPosition(void);

    /// return position object
    CProObject* GetPositionObject(void);

    /// get direction
    CPoint GetDirection(void);

    /// return direction object
    CProObject* GetDirectionObject(void);

    /// is in free size mode
    bool IsInFreeSizeMode(void);

    /// get offset
    double GetOffset(void);

    /// get length
    double GetLength(void);

    /// return geometry descriptor
    virtual CGeoDescriptor  GetGeoDescriptor(void);

// executive methods -----------------------------------------------------------
    /// draw object
    virtual void Draw(void);

    /// calculate visible object metrics
    virtual void GetObjectMetrics(CObjMetrics& metrics);

// manipulation ----------------------------------------------------------------
    /// begin manipulation
    virtual void StartManipulation(CHistoryNode* p_history);

    /// move by object/scene
    virtual void Move(const CPoint& dmov,CGraphicsView* p_view);

    /// rotate with object/scene
    virtual void Rotate(const CPoint& drot,CGraphicsView* p_view);

    /// end manipulation
    virtual void EndManipulation(void);

    /// respond to manipulation event
    virtual void RespondToEvent(void);

// input/outpu methods ---------------------------------------------------------
    /// load object specific data
    virtual void LoadData(CXMLElement* p_ele);

    /// link object with given role - for delayed load
    virtual void LinkObject(const QString& role,CProObject* p_object);

    /// save object specific data
    virtual void SaveData(CXMLElement* p_ele);

// section of private data -----------------------------------------------------
private:
    CVectorSetup*   Setup;
    CCylinder       Cylinder;
    CCylinder       Cone;
    CPoint          Position;
    CProObject*     PositionObject;
    CPoint          Direction;
    CProObject*     DirectionObject;
    double          Offset;
    double          Length;

    /// helper method
    void SetInitialColorScheme(void);
    void DrawVector(void);

    /// update Cylinder and Cone
    virtual void SetupChanged(void);

    /// handle history related things
    virtual void RemoveFromRegistered(CProObject* p_object,
                                      CHistoryNode* p_history);
};

//---------------------------------------------------------------------------

#endif
