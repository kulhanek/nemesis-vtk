#ifndef ProObjectHistoryH
#define ProObjectHistoryH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <SmallString.hpp>
#include <ProObject.hpp>

//------------------------------------------------------------------------------

class CPluginObject;
class CProject;
class CProObject;

//------------------------------------------------------------------------------

class CProObjectNameHI : public CHistoryItem {
public:
// constructors ----------------------------------------------------------------
    CProObjectNameHI(CProject* p_object);
    CProObjectNameHI(CProObject* p_object, const QString& newname);

// section of private data -----------------------------------------------------
private:
    int             ObjectIndex;    // object ID
    QString         OldName;        // old name
    QString         NewName;        // new name

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

class CProObjectDescrHI : public CHistoryItem {
public:
// constructors ----------------------------------------------------------------
    CProObjectDescrHI(CProject* p_object);
    CProObjectDescrHI(CProObject* p_object, const QString& newdescr);

// section of private data -----------------------------------------------------
private:
    int             ObjectIndex;    // object ID
    QString         NewDescr;   // old name
    QString         OldDescr;   // new name

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

class CProObjectFlagsHI : public CHistoryItem {
public:
// constructors ----------------------------------------------------------------
    CProObjectFlagsHI(CProject *p_object);
    CProObjectFlagsHI(CProObject* p_object, const CProObjectFlags& flags);

// section of private data -----------------------------------------------------
private:
    int                 ObjectIndex;    // object ID
    CProObjectFlags     OldFlags;        // old flags
    CProObjectFlags     NewFlags;        // new flags

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
