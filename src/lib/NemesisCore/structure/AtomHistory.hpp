#ifndef AtomHistoryH
#define AtomHistoryH
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
#include <Point.hpp>
#include <Transformation.hpp>

//------------------------------------------------------------------------------

class CProject;
class CAtom;
class CStructure;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

class CAtomHI: public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CAtomHI(CProject* p_object);
    CAtomHI(CAtom* p_atom,EHistoryItemDirection change);

// section of private data -----------------------------------------------------
private:
    int             MoleculeIndex;
    CXMLDocument    AtomData;

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

class CAtomSerIndexHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CAtomSerIndexHI(CProject* p_object);
    CAtomSerIndexHI(CAtom* p_atom,int ser_idx);

// section of private data -----------------------------------------------------
private:
    int             AtomIndex;
    int             NewSerIndex;
    int             OldSerIndex;

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

class CAtomLocIndexHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CAtomLocIndexHI(CProject* p_object);
    CAtomLocIndexHI(CAtom* p_atom,int loc_idx);

// section of private data -----------------------------------------------------
private:
    int             AtomIndex;
    int             NewLocIndex;
    int             OldLocIndex;

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

class CAtomTypeHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CAtomTypeHI(CProject* p_object);
    CAtomTypeHI(CAtom* p_atom,const QString& newtype);

// section of private data -----------------------------------------------------
private:
    int             AtomIndex;
    QString         NewAtomType;
    QString         OldAtomType;

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

class CAtomZHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CAtomZHI(CProject* p_object);
    CAtomZHI(CAtom* p_atom,int newz);

// section of private data -----------------------------------------------------
private:
    int             MoleculeIndex;
    int             AtomIndex;
    int             NewZ;
    int             OldZ;

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

class CAtomChargeHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CAtomChargeHI(CProject* p_object);
    CAtomChargeHI(CAtom* p_atom,double newcharge);

// section of private data -----------------------------------------------------
private:
    int             AtomIndex;

    double          NewCharge;
    double          OldCharge;

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

class CAtomPosHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CAtomPosHI(CProject* p_object);
    CAtomPosHI(CAtom* p_atom,const CPoint& newpos);

// section of private data -----------------------------------------------------
private:
    int             AtomIndex;

    CPoint          NewPos;
    CPoint          OldPos;

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

class CAtomVelHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CAtomVelHI(CProject* p_object);
    CAtomVelHI(CAtom* p_atom,const CPoint& newvel);

// section of private data -----------------------------------------------------
private:
    int             AtomIndex;

    CPoint          NewVel;
    CPoint          OldVel;

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

class CAtomTrajIndexHI : public CHistoryItem {
public:
// constructors and destructors ------------------------------------------------
    CAtomTrajIndexHI(CProject* p_object);
    CAtomTrajIndexHI(CAtom* p_atom,int traj_idx);

// section of private data -----------------------------------------------------
private:
    int             AtomIndex;
    int             NewTrajIndex;
    int             OldTrajIndex;

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
