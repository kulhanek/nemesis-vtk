#ifndef PlaneObjectH
#define PlaneObjectH
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
#include <AtomList.hpp>
#include <GeoDescriptor.hpp>

#include <PlaneSetup.hpp>

//------------------------------------------------------------------------------

class CPlaneSetup;

//------------------------------------------------------------------------------

extern CExtUUID PlaneObjectID;

//------------------------------------------------------------------------------

enum EPlaneObjectFlag {
    // user flags               = 0x00010000    // first user flag
    EPOF_FIT_TO_ATOMS           = 0x00010000,
    EPOF_CLIP_PLANE             = 0x00020000,
    EPOF_DO_NOT_SHOW            = 0x00040000
};

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CPlaneObject : public CGraphicsObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CPlaneObject(CGraphicsObjectList* p_gl);

// setup methods ---------------------------------------------------------------
    /// set origin
    bool SetPositionWH(const CPoint& origin);

    /// set origin
    bool SetPositionWH(CProObject* p_origin);

    /// set direction
    bool SetDirectionWH(const CPoint& direction);

    /// set direction
    bool SetDirectionWH(CProObject* p_direction);

    /// set size
    bool SetSizeWH(double size);

    /// set rotation
    bool SetRotationWH(double rotation);

    /// set clipping plane ID (0...5)
    bool SetClippingPlaneIDWH(int id);

// setup methods ---------------------------------------------------------------
    /// set origin
    void SetPosition(const CPoint& origin,CHistoryNode* p_history=NULL);

    /// set origin
    void SetPosition(CProObject* p_origin,CHistoryNode* p_history=NULL);

    /// set direction
    void SetDirection(const CPoint& direction,CHistoryNode* p_history=NULL);

    /// set direction
    void SetDirection(CProObject* p_direction,CHistoryNode* p_history=NULL);

    /// set size
    void SetSize(double size,CHistoryNode* p_history=NULL);

    /// set rotation
    void SetRotation(double rotation,CHistoryNode* p_history=NULL);

    /// set clipping plane ID (0...5)
    void SetClippingPlaneID(int id,CHistoryNode* p_history=NULL);

// information methods ---------------------------------------------------------
    /// get origin
    CPoint GetPosition(void);

    /// return origin object
    CProObject* GetPositionObject(void);

    /// get direction
    CPoint GetDirection(void);

    /// return direction object
    CProObject* GetDirectionObject(void);

    /// get size
    double GetSize(void);

    /// get rotation
    double GetRotation(void);

    /// get clipping plane ID
    int GetClippingPlaneID(void);

    /// return geometry descriptor
    virtual CGeoDescriptor  GetGeoDescriptor(void);

// executive methods -----------------------------------------------------------
    /// draw object
    virtual void Draw(void);

    /// calculate visible object metrics
    virtual void GetObjectMetrics(CObjMetrics& metrics);

// input/outpu methods ---------------------------------------------------------
    /// load object specific data
    virtual void LoadData(CXMLElement* p_ele);

    /// link object with given role - for delayed load
    virtual void LinkObject(const QString& role,CProObject* p_object);

    /// save object specific data
    virtual void SaveData(CXMLElement* p_ele);

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

// section of private data -----------------------------------------------------
private:
    CPlaneSetup*        Setup;
    std::vector<CPoint> Vertices;
    CPoint              Position;
    CProObject*         PositionObject;
    CPoint              Direction;
    CProObject*         DirectionObject;
    double              Size;
    double              Rotation;
    int                 ClippingPlaneID;

    /// fit plane
    void FitPlane(void);

    /// draw plane
    void DrawPlane(void);

    /// clip plane
    void ClipPlane(void);

    /// update Cylinder and Cone
    virtual void SetupChanged(void);

    /// handle history related things
    virtual void RemoveFromRegistered(CProObject* p_object,
                                      CHistoryNode* p_history);
};


//------------------------------------------------------------------------------

#endif
