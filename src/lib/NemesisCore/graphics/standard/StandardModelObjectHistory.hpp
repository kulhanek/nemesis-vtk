#ifndef StandardModelHistoryH
#define StandardModelHistoryH
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
#include <HistoryNode.hpp>
#include <StandardModelSetup.hpp>

//------------------------------------------------------------------------------

class CStandardModelObject;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

class CStandardModelChangeModelHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CStandardModelChangeModelHI(CProject *p_object);
    CStandardModelChangeModelHI(CStandardModelObject* p_obj,EModel newmodel);

// section of private data -----------------------------------------------------
private:
    int         ObjectID;
    EModel      OldModel;
    EModel      NewModel;

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

class CStandardModelChangeKPosHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CStandardModelChangeKPosHI(CProject *p_object);
    CStandardModelChangeKPosHI(CStandardModelObject* p_obj,int ka,int kb,int kc);

// section of private data -----------------------------------------------------
private:
    int     ObjectID;
    int     OldKA;
    int     OldKB;
    int     OldKC;
    int     NewKA;
    int     NewKB;
    int     NewKC;

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
