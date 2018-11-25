#ifndef BoxObjectH
#define BoxObjectH
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

class CBoxSetup;
class CStructure;

//------------------------------------------------------------------------------

extern CExtUUID BoxObjectID;

//------------------------------------------------------------------------------

enum EBoxObjectFlag {
    // user flags               = 0x00010000    // first user flag
    EBCBOF_SHOW_CENTER          = 0x00010000,
    EBCBOF_ORIGIN_IN_CENTER     = 0x00020000    // box position points to box center
};

//------------------------------------------------------------------------------

/// draw box

class NEMESIS_CORE_PACKAGE CBoxObject : public CGraphicsObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CBoxObject(CGraphicsObjectList* p_gl);

// setup methods ---------------------------------------------------------------
    /// set position
    bool SetPositionWH(const CPoint& pos);

    /// set position
    bool SetPositionWH(CProObject* p_posobj);

    /// set dimension
    bool SetDimensionsWH(const CPoint& dim);

    /// set direction
    bool SetDirectionWH(const CPoint& direction);

    /// set direction
    bool SetDirectionWH(CProObject* p_direction);

    /// set rotation
    bool SetRotationWH(double rotation);

    /// set position
    void SetPosition(const CPoint& pos,CHistoryNode* p_history=NULL);

    /// set position
    void SetPosition(CProObject* p_posobj,CHistoryNode* p_history=NULL);

    /// set dimensions
    void SetDimensions(const CPoint& dim,CHistoryNode* p_history=NULL);

    // set direction
    void SetDirection(const CPoint& direction,CHistoryNode* p_history=NULL);

        /// set direction
    void SetDirection(CProObject* p_direction,CHistoryNode* p_history=NULL);

    /// set rotation
    void SetRotation(double rotation,CHistoryNode* p_history=NULL);

// information methods ---------------------------------------------------------

    /// copy setup from the other box
    void CopyBoxObject(CBoxObject* p_box);

    /// geometry descriptor
    virtual CGeoDescriptor GetGeoDescriptor(void);

    /// get position
    CPoint GetPosition(void);

    /// get position object
    CProObject* GetPositionObject(void);

    /// get dimensions
    const CPoint& GetDimensions(void);

    /// get direction
    CPoint GetDirection(void);

    /// return direction object
    CProObject* GetDirectionObject(void);

    /// get rotation
    double GetRotation(void);

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

// section of private data ----------------------------------------------------
private:
    CBoxSetup*  Setup;
    CPoint      Position;
    CProObject* PositionObject;
    CPoint      Direction;
    CProObject* DirectionObject;
    CPoint      Dimensions;
    double      Rotation;

    /// setup was changed
    virtual void SetupChanged(void);

    /// draw box
    void DrawBox(void);

    /// draw center
    void DrawCenter(void);

    /// handle history related things
    virtual void RemoveFromRegistered(CProObject* p_object,
                                      CHistoryNode* p_history);
};

//---------------------------------------------------------------------------

#endif
