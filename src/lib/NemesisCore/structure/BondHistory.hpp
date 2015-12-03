#ifndef BondHistoryH
#define BondHistoryH
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
#include <XMLDocument.hpp>
#include <Bond.hpp>

//------------------------------------------------------------------------------

class CPluginObject;
class CProject;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

class CBondHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CBondHI(CProject* p_object);
    CBondHI(CBond* p_bond,EHistoryItemDirection change);

// section of private data -----------------------------------------------------
private:
    int             MoleculeIndex;
    CXMLDocument    BondData;

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

class CBondOrderHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CBondOrderHI(CProject* p_object);
    CBondOrderHI(CBond* p_bond,EBondOrder order);

// section of private data -----------------------------------------------------
private:
    int             BondIndex;

    EBondOrder      OldOrder;
    EBondOrder      NewOrder;

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

class CBondTypeHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CBondTypeHI(CProject* p_object);
    CBondTypeHI(CBond* p_bond,const QString& type);

// section of private data -----------------------------------------------------
private:
    int             BondIndex;

    QString         OldType;
    QString         NewType;

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

class CBondSetFirstAtomHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CBondSetFirstAtomHI(CProject* p_object);
    CBondSetFirstAtomHI(CBond* p_bond,CAtom* p_fa);

// section of private data -----------------------------------------------------
private:
    int BondIndex;
    int OldFirstAtomIndex;
    int NewFirstAtomIndex;

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

class CBondSetSecondAtomHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CBondSetSecondAtomHI(CProject* p_object);
    CBondSetSecondAtomHI(CBond* p_bond,CAtom* p_fa);

// section of private data -----------------------------------------------------
private:
    int BondIndex;
    int OldSecondAtomIndex;
    int NewSecondAtomIndex;

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
