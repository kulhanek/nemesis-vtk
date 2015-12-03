#ifndef GraphicsObjectLinkH
#define GraphicsObjectLinkH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
class CGraphicsObjectEnd;

//------------------------------------------------------------------------------

extern CExtUUID GraphicsObjectLinkID;

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CGraphicsObjectLink : public CGraphicsObject {
public:
// constructors and destructors ------------------------------------------------
    CGraphicsObjectLink(CGraphicsObjectList* p_gl);
    CGraphicsObjectLink(CGraphicsObjectList* p_gl,CGraphicsObjectEnd* p_end);
    virtual ~CGraphicsObjectLink(void);

// information methods ---------------------------------------------------------
    /// get graphic object type - it returns type of end object
    virtual const CExtUUID& GetGraphicsType(void);

    /// get end object
    CProObject* GetEndObject(void);

// executive methods -----------------------------------------------------------
    /// draw object
    virtual void Draw(void);

    /// calculate visible object metrics
    virtual void GetObjectMetrics(CObjMetrics& metrics);

// input/output methods --------------------------------------------------------
    /// load object data
    virtual void LoadData(CXMLElement* p_ele);

    /// link object with given role - for delayed load
    virtual void LinkObject(const QString& role,CProObject* p_object);

    /// save object data
    virtual void SaveData(CXMLElement* p_ele);

// registered objects  ---------------------------------------------------------
public:
    virtual void RemoveFromRegistered(CProObject* p_object,
                                      CHistoryNode* p_history);

// section of private data -----------------------------------------------------
private:
    CGraphicsObjectEnd* ObjectEnd;
    friend class CGraphicsObjectEnd;
};

//------------------------------------------------------------------------------

#endif
