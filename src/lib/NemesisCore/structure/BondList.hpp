#ifndef BondListH
#define BondListH
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
#include <Bond.hpp>
#include <BondData.hpp>

// -----------------------------------------------------------------------------

class CStructure;
class CProject;
class CAtom;
class CXMLElement;

// ----------------------------------------------------------------------------

///  declare bonds list

class NEMESIS_CORE_PACKAGE CBondList: public CProObject {
    Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CBondList(CStructure *p_str);

//-----------------------------------------------------------------------------
    /// create new bond
    CBond* CreateBondWH(CAtom *p_a1,CAtom *p_a2,EBondOrder order);

    /// add missing bonds
    bool AddBondsWH(void);

    /// remove all bonds
    bool RemoveBondsWH(void);

    /// remove selected bonds
    bool RemoveSelectedBondsWH(void);

    /// remove invalid bonds
    bool RemoveInvalidBondsWH(void);

// executive methods --------------------------------------------------
    /// create bond
    CBond* CreateBond(CAtom *p_a1,CAtom *p_a2,
                      EBondOrder order,CHistoryNode* p_history=NULL);

    /// create bond
    CBond* CreateBond(CXMLElement* p_ele,CHistoryNode* p_history=NULL);

    /// create bond
    CBond* CreateBond(const CBondData& bond_data,CHistoryNode* p_history=NULL);

    /// remove registrations for all bonds
    void UnregisterAllRegisteredBonds(CHistoryNode* p_history=NULL);

    /// assign new PBC bond indexes - used by BuildSuperCellWH
    // return number of assigned indexes
    int AssignPBCBondIndexes(void);

    /// change parentship of all bonds
    void MoveAllBondsFrom(CBondList* p_source,CHistoryNode* p_history=NULL);

// informational methods -------------------------------------------------------
    /// get structure pointer
    CStructure*  GetStructure(void) const;

    /// get number of bonds
    int GetNumberOfBonds(void);

    /// get number of completed bonds, e.g. do not count invalid bonds
    int GetNumberOfCompletedBonds(void);

    /// has invalid bonds
    bool HasInvalidBonds(void);

// input/output methods --------------------------------------------------------
    /// load all bonds with history recording
    void LoadData(CXMLElement* p_ele,CHistoryNode* p_history);

    /// load all bonds
    virtual void LoadData(CXMLElement* p_ele);

    /// save all bonds
    void SaveData(CXMLElement* p_ele,bool selected);

    /// save all bonds
    virtual void SaveData(CXMLElement* p_ele);

// external notification -------------------------------------------------------
    /// block signals for massive update
    void BeginUpdate(void);

    /// end of massive update
    void EndUpdate(void);

    /// emit OnBondListChanged signal
    void EmitOnBondListChanged(void);

    /// list size changed
    void ListSizeChanged(void);

// -----------------------------------------------------------------------------
signals:
    /// emmited when bond list is changed
    void OnBondListChanged(void);

// section of private data ----------------------------------------------------
private:
    bool    Changed;
    int     UpdateLevel;
};

// -----------------------------------------------------------------------------

#endif

