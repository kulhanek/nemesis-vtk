#ifndef PlaneObjectHistoryH
#define PlaneObjectHistoryH
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

class CPlaneObject;
class CProObject;

//------------------------------------------------------------------------------

class CPlaneObjectChangePositionHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CPlaneObjectChangePositionHI(CProject *p_object);
    CPlaneObjectChangePositionHI(CPlaneObject* p_obj,
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

class CPlaneObjectChangeDirectionHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CPlaneObjectChangeDirectionHI(CProject *p_object);
    CPlaneObjectChangeDirectionHI(CPlaneObject* p_obj,
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

class CPlaneObjectChangeRotationHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CPlaneObjectChangeRotationHI(CProject *p_object);
    CPlaneObjectChangeRotationHI(CPlaneObject* p_obj,double newRotation);

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

class CPlaneObjectChangeSizeHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CPlaneObjectChangeSizeHI(CProject *p_object);
    CPlaneObjectChangeSizeHI(CPlaneObject* p_obj,double newSize);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    double      OldSize;
    double      NewSize;

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

class CPlaneObjectChangeClippingIDHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CPlaneObjectChangeClippingIDHI(CProject *p_object);
    CPlaneObjectChangeClippingIDHI(CPlaneObject* p_obj,int newID);

// section of private data -----------------------------------------------------
private:
    int ObjectID;
    int OldID;
    int NewID;

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

class CPlaneObjectChangePositionObjHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CPlaneObjectChangePositionObjHI(CProject *p_object);
    CPlaneObjectChangePositionObjHI(CPlaneObject* p_obj,CProObject* p_newobj);

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

class CPlaneObjectChangeDirectionObjHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CPlaneObjectChangeDirectionObjHI(CProject *p_object);
    CPlaneObjectChangeDirectionObjHI(CPlaneObject* p_obj,CProObject* p_newobj);

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

class CPlaneObjectBackupGeoHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CPlaneObjectBackupGeoHI(CProject *p_object);
    CPlaneObjectBackupGeoHI(CPlaneObject* p_obj);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    CPoint      Position;
    CPoint      Direction;
    double      Rotation;
    double      Size;

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
