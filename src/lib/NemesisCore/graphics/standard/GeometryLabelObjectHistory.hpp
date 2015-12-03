#ifndef GeometryLabelObjectHistoryH
#define GeometryLabelObjectHistoryH
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
#include <HistoryItem.hpp>
#include <GeometryLabelObject.hpp>

//------------------------------------------------------------------------------

class CGeometryLabelObject;

//------------------------------------------------------------------------------

class CGeometryLabelObjectChangeTypeHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CGeometryLabelObjectChangeTypeHI(CProject *p_object);
    CGeometryLabelObjectChangeTypeHI(CGeometryLabelObject* p_obj,
                                     EGeometryLabelType newtype);

// section of private data -----------------------------------------------------
private:
    int                 ObjectID;
    EGeometryLabelType  OldType;
    EGeometryLabelType  NewType;

// executive methods -----------------------------------------------------------
    /// perform the change in the forward direction
    virtual void Forward(void);

    /// perform the change in the backward direction
    virtual void Backward(void);

// input/output methods --------------------------------------------------------
    /// load data
    virtual void LoadData(CXMLElement* p_ele);

    /// save data
    virtual void SaveData(CXMLElement* p_ele);
};

//------------------------------------------------------------------------------

class CGeometryLabelObjectChangeOffsetHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CGeometryLabelObjectChangeOffsetHI(CProject *p_object);
    CGeometryLabelObjectChangeOffsetHI(CGeometryLabelObject* p_obj,
                                        const CPoint& newoffset);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    CPoint      OldOffset;
    CPoint      NewOffset;

// executive methods -----------------------------------------------------------
    /// perform the change in the forward direction
    virtual void Forward(void);

    /// perform the change in the backward direction
    virtual void Backward(void);

// input/output methods --------------------------------------------------------
    /// load data
    virtual void LoadData(CXMLElement* p_ele);

    /// save data
    virtual void SaveData(CXMLElement* p_ele);
};

//------------------------------------------------------------------------------

class CGeometryLabelObjectBackupGeoHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CGeometryLabelObjectBackupGeoHI(CProject *p_object);
    CGeometryLabelObjectBackupGeoHI(CGeometryLabelObject* p_obj);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    CPoint      Offset;

// executive methods -----------------------------------------------------------
    /// perform the change in the forward direction
    virtual void Forward(void);

    /// perform the change in the backward direction
    virtual void Backward(void);

// input/output methods --------------------------------------------------------
    /// load data
    virtual void LoadData(CXMLElement* p_ele);

    /// save data
    virtual void SaveData(CXMLElement* p_ele);
};

//------------------------------------------------------------------------------

#endif
