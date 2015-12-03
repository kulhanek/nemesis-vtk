#ifndef ResidueH
#define ResidueH
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
#include <Point.hpp>
#include <ProObject.hpp>

// remove AddAtom macro defined on Windows
#if defined AddAtom
#undef AddAtom
#endif

// -----------------------------------------------------------------------------

class CProject;
class CStructure;
class CAtom;
class CBond;
class CResidue;
class CResidueList;
class CXMLElement;

// ----------------------------------------------------------------------------

extern CExtUUID NEMESIS_CORE_PACKAGE ResidueID;

// ----------------------------------------------------------------------------

///  residue implementation

class NEMESIS_CORE_PACKAGE CResidue : public CProObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CResidue(CResidueList* p_bl);
    CResidue(CResidueList* p_bl,bool no_index);
    virtual ~CResidue(void);
    void    RemoveFromBaseList(CHistoryNode* p_history=NULL);

// methods with changes registered into history list ---------------------------
    /// delete residue with history
    bool DeleteWH(bool keep_connectors=true);

    /// set sequence index
    bool SetSeqIndexWH(int seqidx);

    /// set chain
    bool SetChainWH(const QString& chain);

    /// set type
    bool SetTypeWH(const QString& type);

    /// add hydrogens to residue
    bool AddHydrogensWH(bool polar_only);

    /// remove hydrogens from residue
    bool RemoveHydrogensWH(void);

    /// update local index by its order (e.g. it will remove gaps)
    bool LocalIndexByOrderWH(void);

    /// update local index by molecule index, usufull for residue spliting
    bool LocalIndexByMoleculesWH(void);

    /// reverse local index
    bool ReverseLocalIndexWH(void);

    /// aggregate hydrogens to heavy atoms
    bool AggregateHydrogensWH(void);

    /// aggregate terminals to heavy atoms
    bool AggregateTerminalsWH(void);

    /// rename by local index
    bool NamesByLocalIndexWH(void);

    /// rename by heavy atoms order (sorted by local index)
    bool NamesByHeavyAtomsWH(void);

    /// merge with other residue
    bool MergeWithWH(CResidue* p_res);

// informational methods -------------------------------------------------------
    /// get molecule owner
    CStructure*      GetStructure(void) const;

    /// get bond list owner
    CResidueList*   GetResidues(void) const;

    /// get number of atoms
    int             GetNumberOfAtoms(void) const;

    /// return top local index
    int             GetTopLocalIndex(void) const;

    /// return residue long name
    QString         GetLongName(void) const;

    /// return sequential index
    int             GetSeqIndex(void) const;

    /// get residue chain name
    const QString&  GetChain(void) const;

    /// get residue type
    const QString&  GetType(void) const;

    /// is atom owned by residue?
    bool            IsOwned(CAtom* p_atom);

    /// return atom list
    QList<CAtom*>&  GetAtoms();

    /// return atom list
    QList<CBond*>   GetBonds(bool include_connectors=false);

    /// get connector atoms
    QList<CAtom*>   GetConnectorAtoms(bool opposite=false);

    /// get connector bonds
    QList<CBond*>   GetConnectorBonds(void);

    /// find atom by index
    CAtom*          FindAtomByIndex(int index);

    /// is any atom from residue selected?
    bool            IsAnyAtomSelected(bool use_manip_flag=false);

// setup methods ---------------------------------------------------------------
    /// set name
    virtual void SetName(const QString& name,CHistoryNode* p_history=NULL);

    /// set description
    virtual void SetDescription(const QString& descrip,CHistoryNode* p_history=NULL);

    /// set sequential index
    void SetSeqIndex(int seqidx,CHistoryNode* p_history=NULL);

    /// set residue chain name
    void SetChain(const QString& chain,CHistoryNode* p_history=NULL);

    /// set residue type
    void SetType(const QString& type,CHistoryNode* p_history=NULL);

    /// merge residue
    void MergeWith(CResidue* p_res,CHistoryNode* p_history=NULL);

// executive methods -----------------------------------------------------------   
    /// add atom
    void AddAtom(CAtom* p_atom,CHistoryNode* p_history=NULL);

    /// remove atom
    void RemoveAtom(CAtom* p_atom,CHistoryNode* p_history=NULL);

    /// delete residue and its contents
    void Delete(bool keep_connectors=true,CHistoryNode* p_history=NULL);

    /// sort atoms according to local index
    void SortAtoms(void);

    /// change parent, see CResidueList::MoveAllResiduesFrom
    void ChangeParent(CResidueList* p_newparent);

// input/output methods --------------------------------------------------------
    /// load residue data
    virtual void LoadData(CXMLElement* p_ele);

    /// save residue data
    virtual void SaveData(CXMLElement* p_ele);

// external notification -------------------------------------------------------
    /// block signals for massive update
    void BeginUpdate(void);

    /// end of massive update
    void EndUpdate(bool do_not_sort=false);

    /// emit OnAtomListChanged signal
    void EmitOnAtomListChanged(void);

    /// emit OnOnStatusChanged signal
    void EmitOnStatusChanged(EStatusChanged status);

    /// list size changed
    void ListSizeChanged(bool do_not_sort=false);

// -----------------------------------------------------------------------------
signals:  
    /// emited when list is changed (order,atom added or removed)
    void OnAtomListChanged(void);

// section of private data -----------------------------------------------------
private:
    int             SeqIndex;
    QString         Chain;
    QString         Type;
    QList<CAtom*>   Atoms;

    bool                    Changed;
    QFlags<EStatusChanged>  ObjectChanged;
    int                     UpdateLevel;
    bool                    ForceSorting;

    /// used by SortAtoms
    static bool LessThanByLocalIndex(CAtom* p_left,CAtom* p_right);

    friend class CResidueModel;
    friend class CResidueAtomOrderHistoryNode;
};

// -----------------------------------------------------------------------------

#endif

