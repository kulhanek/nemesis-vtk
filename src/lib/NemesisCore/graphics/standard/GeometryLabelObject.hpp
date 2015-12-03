#ifndef GeometryLabelObjectH
#define GeometryLabelObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <GraphicsObject.hpp>

//------------------------------------------------------------------------------

class CGraphicsObjectList;
class CGeometryLabelSetup;

//------------------------------------------------------------------------------

extern CExtUUID GeometryLabelObjectID;

//------------------------------------------------------------------------------

enum EStandardModelObjectFlag {
    // user flags               = 0x00010000    // first user flag
    ESMOF_SHOW_TEXT             = 0x00010000,
    ESMOF_RELATIVE_TEXT_POS     = 0x00020000,
    ESMOF_SHOW_QUOTATION        = 0x00040000
};

// -----------------------------------------------------------------------------

enum EGeometryLabelType{
        ESGLT_NONE,
        ESGLT_POINT,
        ESGLT_DISTANCE,
        ESGLT_ANGLE,
        ESGLT_DIHEDRAL
};

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CGeometryLabelObject : public CGraphicsObject {
public:
// constructors and destructors ------------------------------------------------
    CGeometryLabelObject(CGraphicsObjectList* p_gl);

// setup methods ---------------------------------------------------------------
    /// set lable type
    bool SetTypeWH(EGeometryLabelType type);

    /// set lable type
    void SetType(EGeometryLabelType type,CHistoryNode* p_history=NULL);

    /// set offset
    bool SetOffsetWH(const CPoint& offset);

    /// set offset
    void SetOffset(const CPoint& offset,CHistoryNode* p_history=NULL);

// information method ----------------------------------------------------------
    /// get type
    EGeometryLabelType GetType(void);

    /// get custom format
    const CPoint& GetOffset(void) const;

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

// section of public data ------------------------------------------------------
private:
    CGeometryLabelSetup*    Setup;
    EGeometryLabelType      Type;
    CPoint                  Offset;

    /// label position
    void LabelPosition(void);

    /// label distance
    void LabelDistance(void);

    /// label angle
    void LabelAngle(void);

    /// label dihedral
    void LabelDihedral(void);

    /// position metrics
    void GetObjectMetricsPosition(CObjMetrics& metrics);

    /// distance metrics
    void GetObjectMetricsDistance(CObjMetrics& metrics);

    /// angle metrics
    void GetObjectMetricsAngle(CObjMetrics& metrics);

    /// dihedral metrics
    void GetObjectMetricsDihedral(CObjMetrics& metrics);

    /// draw label
    void DrawText(const CSimplePoint<float>& pos,const QString& label);

    /// draw quotation line
    void DrawQuotationLine(CSimplePoint<float>& p1,CSimplePoint<float>& p2);
};

//------------------------------------------------------------------------------

#endif
