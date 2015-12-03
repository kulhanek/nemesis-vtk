#ifndef TextObjectH
#define TextObjectH
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
class CTextSetup;
class CAtom;
class CStructure;

//------------------------------------------------------------------------------

extern CExtUUID TextObjectID;

//------------------------------------------------------------------------------

enum ETextObjectFlag {
    // user flags               = 0x00010000    // first user flag
    ETOF_GLOBAL_POSITION        = 0x00010000,
    ETOF_FACE_ORIENTATION       = 0x00020000
};

// -----------------------------------------------------------------------------

/// text graphics object

class NEMESIS_CORE_PACKAGE CTextObject : public CGraphicsObject {
public:
// constructors and destructors ------------------------------------------------
    CTextObject(CGraphicsObjectList* p_gl);

// setup methods ---------------------------------------------------------------
    /// set text
    bool SetTextWH(const QString& text);

    /// set position
    bool SetPositionWH(const CPoint& pos);

    /// set orientation
    bool SetOrientationWH(const CPoint& rot);

    /// set text
    void SetText(const QString& text,CHistoryNode* p_history=NULL);

    /// set position
    void SetPosition(const CPoint& pos,CHistoryNode* p_history=NULL);

    /// set orientation
    void SetOrientation(const CPoint& rot,CHistoryNode* p_history=NULL);

    /// get position
    const CPoint& GetPosition(void) const;

    /// get orientation
    const CPoint& GetOrientation(void) const;

    /// get text format
    const QString& GetText(void) const;

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

    /// save object specific data
    virtual void SaveData(CXMLElement* p_ele);

// section of public data ------------------------------------------------------
private:
    CPoint      Position;
    CPoint      Orientation;
    QString     Text;
};

//------------------------------------------------------------------------------

#endif
