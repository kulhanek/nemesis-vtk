#ifndef GraphicsObjectHistoryH
#define GraphicsObjectHistoryH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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
#include <HistoryNode.hpp>
#include <SmallString.hpp>
#include <XMLDocument.hpp>

//------------------------------------------------------------------------------

class CProject;
class CProObject;
class CGraphicsObject;
class CStructure;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

class CGraphicsObjectHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CGraphicsObjectHI(CProject *p_object);
    CGraphicsObjectHI(CGraphicsObject* p_go,EHistoryItemDirection change);

// section of private data -----------------------------------------------------
private:
    CXMLDocument    GraphicsObjectData;

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

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

class CGraphicsObjectAddProObjectHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CGraphicsObjectAddProObjectHI(CProject *p_object);
    CGraphicsObjectAddProObjectHI(CGraphicsObject* p_go,CProObject* p_po,
                                           EHistoryItemDirection change);

// section of private data -----------------------------------------------------
private:
    int GraphicsObjectIndex;
    int ProObjectIndex;

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

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



#endif
