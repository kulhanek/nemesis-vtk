#ifndef PointObjectH
#define PointObjectH
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

//------------------------------------------------------------------------------

class CGraphicsObjectList;
class CPointSetup;

//------------------------------------------------------------------------------

enum EPointObjectFlag {
    // user flags               = 0x00010000    // first user flag
    ECPOF_SHOW_QUOTATION        = 0x00010000,
    ECPOF_SHOW_COG              = 0x00020000    // center of geometry
};

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CPointObject : public CGraphicsObject {
public:
// constructors and destructors -----------------------------------------------
    CPointObject(CGraphicsObjectList* p_gl);

    /// set point position with history
    bool SetPositionWH(const CPoint& pos);

    /// set point position
    void SetPosition(const CPoint& pos,CHistoryNode* p_history=NULL);

    /// get point position
    const CPoint& GetPosition(void);

    /// is autonomous point?
    bool IsAutonomousPoint(void);

    /// geometry descriptor
    virtual CGeoDescriptor GetGeoDescriptor(void);

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

    /// end manipulation
    virtual void EndManipulation(void);

    /// respond to manipulation event
    virtual void RespondToEvent(void);

// input/outpu methods ---------------------------------------------------------
    /// load object specific data
    virtual void LoadData(CXMLElement* p_ele);
    /// save object specific data
    virtual void SaveData(CXMLElement* p_ele);

// section of public data -----------------------------------------------------
private:
    CPointSetup*    Setup;
    CPoint          Point;

    /// update point position
    void UpdatePosition(void);

    /// draw position
    void DrawPosition(void);

    /// draw quotation towards individual atoms
    void DrawQuotation(void);

    /// add object to the list
    virtual void AddObject(CProObject* p_object,CHistoryNode* p_history=NULL);

    /// remove object from the list
    virtual void RemoveObject(CProObject* p_object,CHistoryNode* p_history=NULL);
};

//------------------------------------------------------------------------------

#endif
