#ifndef FragmentH
#define FragmentH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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
#include <AtomData.hpp>
#include <BondData.hpp>
#include <SimpleVector.hpp>
#include <QIcon>

//------------------------------------------------------------------------------

class CXMLElement;

//------------------------------------------------------------------------------

enum EConnectorMode {
    ECM_MANUAL=0,
    ECM_AUTO
};

//------------------------------------------------------------------------------

/// contains static informations about fragment and connectors

class NEMESIS_CORE_PACKAGE CFragment {
public:
// constructor and destructor -------------------------------------------------
    CFragment(void);
    ~CFragment(void);

// -----------------------------------------------------------------------------
    /// return name of structure
    const QString& GetName(void) const;

    /// delete all data in the structure
    void Clear(void);

    /// check structure if it is empty
    bool IsEmpty(void) const;

// connector methods ----------------------------------------------------------
    void            SetFirstConnector(void);
    void            SetNextConnector(void);
    bool            SetBestConnector(EBondOrder order);
    void            SetConnectorMode(EConnectorMode mode);

    int             GetNumOfUserConnectors(void);
    int             GetConnectorIndex(void);
    QIcon           GetConnectorIcon(void);
    QIcon           GetConnectorIconByIndex(int index);

    /// connector is atom_id
    int             GetCurrentConnector(void);
    int             GetConnector(int index);
    EConnectorMode  GetConnectorMode(void);

// accessing data -------------------------------------------------------------
    int                 GetNumOfAtoms(void) const;
    const CAtomData&    GetAtomData(int index) const;

    int                 GetNumOfBonds(void) const;
    const CBondData&    GetBondData(int index) const;

// helper methods -------------------------------------------------------------
    /// set master atom proton number
    void SetMasterAtomZ(int z);

    /// get master atom proton number
    int GetMasterAtomZ(void) const;

    /// return opposite atom index for bond with bond_index connected to atom atom_index
    int GetOppositeAtom(int atom_id,int bond_index=1);

    /// return bond_index of the first bond connected to atom_index
    int GetConnectBond(int atom_id);

    /// return atom_index from atom_id
    int GetAtomDataFromIndex(int atom_id);

// input/output methods -------------------------------------------------------
    /// load the structure from a file
    bool Load(const CSmallString& name);

    /// save the structure to a file
    bool Save(const CSmallString& name);

    /// load the structure from a XML element
    void Load(CXMLElement* p_element);

    /// save the structure to a XML element
    void Save(CXMLElement* p_element);

// section of private data ----------------------------------------------------
private:
    QString                     Name;               // structure name
    CSimpleVector<CAtomData>    Atoms;              // atoms
    CSimpleVector<CBondData>    Bonds;              // bonds
    int                         MasterAtomZ;        // Z of master atom
    CSimpleVector<int>          Connectors;         // field of available connectors
    CSimpleVector<QIcon>        ConnectorIcons;     // field of available connectors (images)
    EConnectorMode              ConnectorMode;      // connector mode
    int                         ActiveConnector;    // active connector

    QIcon TryToLoadIcon(CXMLElement* p_ele);
};

//------------------------------------------------------------------------------

#endif
