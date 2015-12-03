#ifndef ResidueHistoryH
#define ResidueHistoryH
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
#include <SmallString.hpp>
#include <Residue.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

class CResidueHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CResidueHI(CProject* p_object);
    CResidueHI(CResidue* p_res,EHistoryItemDirection change);

// section of private data -----------------------------------------------------
private:
    int             MoleculeIndex;
    CXMLDocument    ResidueData;

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

class CResidueAddAtomHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CResidueAddAtomHI(CProject* p_object);
    CResidueAddAtomHI(CResidue* p_res,CAtom* p_atom,
                        EHistoryItemDirection change);

// section of private data -----------------------------------------------------
private:
    int NewResidueIndex;
    int OldResidueIndex;
    int AtomIndex;

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

class CResidueChainHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CResidueChainHI(CProject* p_object);
    CResidueChainHI(CResidue* p_res,const QString& newchain);

// section of private data -----------------------------------------------------
private:
    int     ResidueIndex;
    QString NewChain;
    QString OldChain;

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

class CResidueTypeHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CResidueTypeHI(CProject* p_object);
    CResidueTypeHI(CResidue* p_res,const QString& newType);

// section of private data -----------------------------------------------------
private:
    int     ResidueIndex;
    QString NewType;
    QString OldType;

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

class CResidueSeqIndexHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CResidueSeqIndexHI(CProject* p_object);
    CResidueSeqIndexHI(CResidue* p_res,int newseqidx);

// section of private data -----------------------------------------------------
private:
    int ResidueIndex;
    int NewSeqIndex;
    int OldSeqIndex;

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
