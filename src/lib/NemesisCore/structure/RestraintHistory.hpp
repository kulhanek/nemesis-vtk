#ifndef RestraintHistoryH
#define RestraintHistoryH
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
#include <HistoryNode.hpp>
#include <SmallString.hpp>
#include <XMLDocument.hpp>
#include <Restraint.hpp>

//------------------------------------------------------------------------------

class CPluginObject;
class CProject;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

class CRestraintHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CRestraintHI(CProject *p_object);
    CRestraintHI(CRestraint* p_res,EHistoryItemDirection change);

// section of private data -----------------------------------------------------
private:
    int             MoleculeIndex;
    CXMLDocument    RestraintData;

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

class CRestraintTargetHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CRestraintTargetHI(CProject* p_object);
    CRestraintTargetHI(CRestraint* p_rst,double target);

// section of private data -----------------------------------------------------
private:
    int     RstIndex;
    double  NewTargetValue;
    double  OldTargetValue;

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

class CRestraintForceHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CRestraintForceHI(CProject* p_object);
    CRestraintForceHI(CRestraint* p_rst,double target);

// section of private data -----------------------------------------------------
private:
    int     RstIndex;
    double  NewForceValue;
    double  OldForceValue;

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

class CRestraintPropertyHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CRestraintPropertyHI(CProject* p_object);
    CRestraintPropertyHI(CRestraint* p_rst,CProperty* p_prop);

// section of private data -----------------------------------------------------
private:
    int     RstIndex;
    int     NewPropertyIndex;
    int     OldPropertyIndex;

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
