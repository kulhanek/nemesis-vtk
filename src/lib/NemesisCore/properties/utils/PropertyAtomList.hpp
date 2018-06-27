#ifndef PropertyAtomListH
#define PropertyAtomListH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ProObject.hpp>

// remove AddAtom macro defined on Windows
#if defined AddAtom
#undef AddAtom
#endif

// -----------------------------------------------------------------------------

class CProperty;
class CAtom;
class CStructure;

// -----------------------------------------------------------------------------

///  list of atoms used by properties
/**
     THIS OBJECT MUST HAVE INDEX - see CPropertyAtomListAddAtomHI
*/

class NEMESIS_CORE_PACKAGE CPropertyAtomList : public CProObject {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    CPropertyAtomList(CProperty* p_property);
    virtual ~CPropertyAtomList(void);

// methods with changes registered into history list ---------------------------
    /// add atom
    bool AddAtomWH(CAtom* p_atom);

    /// remove atom
    bool RemoveAtomWH(CAtom* p_atom);

    /// add selected atoms
    bool AddSelectedAtomsWH(void);

    /// remove selected atoms
    bool RemoveSelectedAtomsWH(void);

    /// remove all atoms
    bool RemoveAllAtomsWH(void);

    /// show as graphic point
    bool ShowAsPointWH(void);

// executive methods without change registration to history list ---------------
    /// add atom
    void AddAtom(CAtom* p_atom, CHistoryNode* p_history = NULL);

    /// remove atom
    void RemoveAtom(CAtom* p_atom, CHistoryNode* p_history = NULL);

// information methods ---------------------------------------------------------
    /// get number of atoms
    int GetNumberOfAtoms(void);

    /// does list contain atom
    bool ContainsAtom(CAtom* p_atom);

    /// contains any atom from the structure
    bool ContainsAnyAtomFrom(CStructure* p_str);

    /// conatins only atoms from given structure?
    bool IsFromStructure(CStructure* p_str);

    /// get atom
    CAtom* GetAtom(int index);

    /// get COM (center-of-mass)
    CPoint GetCOM(void);

    /// get COM (center-of-mass)
    CPoint GetCOM(double& totmass);

    /// get master property
    CProperty* GetProperty(void);

    /// get atom list
    const QList<CAtom*>& GetAtoms(void) const;

// input/output methods --------------------------------------------------------
    /// load all structures
    virtual void LoadData(CXMLElement* p_ele);

    /// save all structures
    virtual void SaveData(CXMLElement* p_ele);

// external notification -------------------------------------------------------
    /// emit OnPropertyAtomListChanged signal
    void EmitOnPropertyAtomListChanged(void);

// registered objects  ---------------------------------------------------------
public:
    virtual void RemoveFromRegistered(CProObject* p_object,
                                      CHistoryNode* p_history);

// -----------------------------------------------------------------------------
signals:
    /// emmited when property list is changed
    void OnPropertyAtomListChanged(void);

// section of private data ----------------------------------------------------
private:
    QList<CAtom*>   Atoms;      // list of atoms
};

// -----------------------------------------------------------------------------

#endif

