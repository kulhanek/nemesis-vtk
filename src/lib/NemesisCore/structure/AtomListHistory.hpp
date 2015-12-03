#ifndef AtomListHistoryH
#define AtomListHistoryH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <HistoryNode.hpp>
#include <SmallString.hpp>
#include <Transformation.hpp>
#include <Point.hpp>

//------------------------------------------------------------------------------

class CPluginObject;
class CProject;
class CAtomList;
class CXMLElement;
class CStructure;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

class CAtomListTransHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CAtomListTransHI(CProject* p_object);
    CAtomListTransHI(CStructure* p_mol,
                              const CTransformation& trans);

// section of private data -----------------------------------------------------
private:
    int                 MoleculeIndex;
    CTransformation     NewTrans;
    CTransformation     OldTrans;
    CSimpleVector<int>  Indexes;

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

class NEMESIS_CORE_PACKAGE CAtomListCoordinatesHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CAtomListCoordinatesHI(CProject* p_object);
    CAtomListCoordinatesHI(CStructure* p_mol);
    CAtomListCoordinatesHI(CStructure* p_mol,bool selected);

// section of private data -----------------------------------------------------
private:
    int                     MoleculeIndex;
    CSimpleVector<CPoint>   Coordinates;
    CSimpleVector<int>      Indexes;

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

class CAtomListChangeParentHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CAtomListChangeParentHI(CProject* p_object);
    CAtomListChangeParentHI(CStructure* p_master,CStructure* p_source,
                            const QVector<int>& indexes,int topmindex,int lowsindex);

// section of private data -----------------------------------------------------
private:
    int                 MasterIndex;
    int                 SourceIndex;
    CSimpleVector<int>  Atoms;
    int                 TopMasterSerIndex;
    int                 LowSourceSerIndex;

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
