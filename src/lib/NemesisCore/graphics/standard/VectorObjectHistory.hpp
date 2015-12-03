#ifndef VectorObjectHistoryH
#define VectorObjectHistoryH
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
#include <Point.hpp>

//------------------------------------------------------------------------------

class CVectorObject;
class CProObject;

//------------------------------------------------------------------------------

class CVectorObjectChangePositionHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CVectorObjectChangePositionHI(CProject *p_object);
    CVectorObjectChangePositionHI(CVectorObject* p_obj,
                                 const CPoint& newPosition);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    CPoint      OldPosition;
    CPoint      NewPosition;

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

class CVectorObjectChangeDirectionHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CVectorObjectChangeDirectionHI(CProject *p_object);
    CVectorObjectChangeDirectionHI(CVectorObject* p_obj,
                                        const CPoint& newDirection);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    CPoint      OldDirection;
    CPoint      NewDirection;

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

class CVectorObjectChangeOffsetHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CVectorObjectChangeOffsetHI(CProject *p_object);
    CVectorObjectChangeOffsetHI(CVectorObject* p_obj,double newOffset);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    double      OldOffset;
    double      NewOffset;

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

class CVectorObjectChangeLengthHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CVectorObjectChangeLengthHI(CProject *p_object);
    CVectorObjectChangeLengthHI(CVectorObject* p_obj,double newLength);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    double      OldLength;
    double      NewLength;

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

class CVectorObjectChangePositionObjHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CVectorObjectChangePositionObjHI(CProject *p_object);
    CVectorObjectChangePositionObjHI(CVectorObject* p_obj,CProObject* p_newobj);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    int         OldPositionObject;
    int         NewPositionObject;

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

class CVectorObjectChangeDirectionObjHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CVectorObjectChangeDirectionObjHI(CProject *p_object);
    CVectorObjectChangeDirectionObjHI(CVectorObject* p_obj,CProObject* p_newobj);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    int         OldDirectionObject;
    int         NewDirectionObject;

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

class CVectorObjectBackupGeoHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CVectorObjectBackupGeoHI(CProject *p_object);
    CVectorObjectBackupGeoHI(CVectorObject* p_obj);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    CPoint      Position;
    CPoint      Direction;
    double      Offset;
    double      Length;

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
