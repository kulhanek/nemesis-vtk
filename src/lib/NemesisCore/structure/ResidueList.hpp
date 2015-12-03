#ifndef ResidueListH
#define ResidueListH
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
#include <ProObject.hpp>

// -----------------------------------------------------------------------------

class CStructure;
class CProject;
class CResidue;
class CXMLElement;

// ----------------------------------------------------------------------------

///  declare residues list

class NEMESIS_CORE_PACKAGE CResidueList: public CProObject {
Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CResidueList(CStructure *p_str);

// executive methods ----------------------------------------------------------
    /// create residue
    CResidue*   CreateResidueWH(const QString& name,const QString& chain = QString(),
                                int seqindex=-1);

    /// create residue
    CResidue*   CreateResidueFromSelectedAtomsWH(const QString& name,
                                const QString& chain = QString(),
                                int seqindex=-1);

    /// delete selected residues
    bool DeleteSelectedResiduesWH(bool keep_connectors=true);

    /// delete empty residues (e.g. residues without any atoms)
    bool DeleteEmptyResiduesWH(void);

    /// update sequence index by its order (e.g. it will remove gaps)
    bool SequenceIndexByOrderWH(void);

    /// update sequence index by its reverse order (e.g. it will remove gaps)
    bool SequenceIndexByReverseOrderWH(void);

    /// update sequence index by chain orders
    bool SequenceIndexByChainsWH(void);

    /// set chain names
    bool SetChainNamesWH(void);

// executive methods ----------------------------------------------------------
    /// create residue
    CResidue*   CreateResidue(const QString& name,CHistoryNode* p_history=NULL);

    /// create residue
    CResidue*   CreateResidue(const QString& name,const QString& chain = QString(),
                              int seqindex=0,CHistoryNode* p_history=NULL);

    /// create residue
    CResidue*   CreateResidue(CXMLElement* p_ele,CHistoryNode* p_history=NULL);

    /// sort residues by sequence index
    void SortResidues(void);

    /// delete selected residues
    void DeleteSelectedResidues(bool keep_connectors=true,CHistoryNode* p_history=NULL);

    /// delete empty residues, e.g. residues that does not contain any atom
    void DeleteEmptyResidues(CHistoryNode* p_history=NULL);

    /// remove registrations for all residues
    void UnregisterAllRegisteredResidues(CHistoryNode* p_history=NULL);

    /// change parentship of all residues
    void MoveAllResiduesFrom(CResidueList* p_source,CHistoryNode* p_history=NULL);

// informational methods ------------------------------------------------------
    /// get molecule pointer
    CStructure* GetStructure(void) const;

    /// find an residue by the sequence index
    CResidue*   SearchBySeqIndex(int seqidx);

    /// get low sequence index
    int         GetLowSeqIndex(void);

    /// get top sequence index
    int         GetTopSeqIndex(void);

    /// get number of residues
    int         GetNumberOfResidues(void);

    /// find an residue by the index
    CResidue*   FindResidueByIndex(int index);

// input/output methods --------------------------------------------------------
    /// load all residues with history recording
    void LoadData(CXMLElement* p_ele,CHistoryNode* p_history);

    /// load all residues
    virtual void LoadData(CXMLElement* p_ele);

    /// save all residues
    void SaveData(CXMLElement* p_ele,bool selected);

    /// save all residues
    virtual void SaveData(CXMLElement* p_ele);

// external notification -------------------------------------------------------
    /// block signals for massive update
    void BeginUpdate(void);

    /// end of massive update
    void EndUpdate(bool do_not_sort=false);

    /// is updating?
    bool IsUpdating(void);

    /// emit OnResidueListChanged signal
    void EmitOnResidueListChanged(void);

    /// list size changed
    void ListSizeChanged(bool do_not_sort=false);

// -----------------------------------------------------------------------------
signals:
    /// emited when list is changed (order,residue added or removed)
    void OnResidueListChanged(void);

// section of private data ----------------------------------------------------
private:
    bool    Changed;
    int     UpdateLevel;
    bool    ForceSorting;

    /// used by SortResidues
    static bool LessThanBySeqIndex(CResidue* p_left,CResidue* p_right);

    /// set seq indexes
    void SetSeqIndexesForChain(QList<CResidue*>& chain,int& seq_index,CHistoryNode* p_history);
    void SetSeqIndexesForChain(CResidue* p_res,int& seq_index,CHistoryNode* p_history);

    friend class CResidue;
};

// -----------------------------------------------------------------------------

#endif

