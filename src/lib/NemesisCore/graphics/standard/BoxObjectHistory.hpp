#ifndef BoxObjectHistoryH
#define BoxObjectHistoryH
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

class CBoxObject;
class CProObject;

//------------------------------------------------------------------------------

class CBoxObjectChangePositionHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CBoxObjectChangePositionHI(CProject *p_object);
    CBoxObjectChangePositionHI(CBoxObject* p_obj,
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

class CBoxObjectChangePositionObjHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CBoxObjectChangePositionObjHI(CProject *p_object);
    CBoxObjectChangePositionObjHI(CBoxObject* p_obj,CProObject* p_newobj);

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

class CBoxObjectChangeDimensionsHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CBoxObjectChangeDimensionsHI(CProject *p_object);
    CBoxObjectChangeDimensionsHI(CBoxObject* p_obj,
                                        const CPoint& newDimensions);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    CPoint      OldDimensions;
    CPoint      NewDimensions;

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

class CBoxObjectChangeDirectionHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CBoxObjectChangeDirectionHI(CProject *p_object);
    CBoxObjectChangeDirectionHI(CBoxObject* p_obj,
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


class CBoxObjectChangeDirectionObjHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CBoxObjectChangeDirectionObjHI(CProject *p_object);
    CBoxObjectChangeDirectionObjHI(CBoxObject* p_obj,CProObject* p_newobj);

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

class CBoxObjectChangeRotationHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CBoxObjectChangeRotationHI(CProject *p_object);
    CBoxObjectChangeRotationHI(CBoxObject* p_obj,double newRotation);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    double      OldRotation;
    double      NewRotation;

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

class CBoxObjectBackupGeoHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CBoxObjectBackupGeoHI(CProject *p_object);
    CBoxObjectBackupGeoHI(CBoxObject* p_obj);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    CPoint      Position;
    CPoint      Dimensions;

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
