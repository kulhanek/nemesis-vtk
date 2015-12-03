#ifndef ResidueWorkPanelH
#define ResidueWorkPanelH
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

#include <WorkPanel.hpp>
#include "ui_ResidueWorkPanel.h"
#include <HistoryList.hpp>
#include <RegisteredObject.hpp>
#include <vector>

//------------------------------------------------------------------------------

class CResidue;
class CSelectionRequest;
class CAtom;

//------------------------------------------------------------------------------

class CResidueWorkPanel : public CWorkPanel {
    Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CResidueWorkPanel(CProject* p_pro,CResidue* p_res);
    ~CResidueWorkPanel();

// section of private data ----------------------------------------------------
private:
    Ui::ResidueWorkPanel    WidgetUI;
    CResidue*               Residue;
    CContainerModel*        AtomsModel;
    bool                    UpdatingItems;
    std::vector<CAtom*>     LongestChain;
    std::vector<CAtom*>     ReorderedAtoms;
    bool                    HideOthersActive;
    bool                    ShowSelectedActive;

private slots:
    void ResidueChanged(void);
    void ResidueRegistered(CResidue* p_res);
    void ResidueUnregistered(void);
    void ProjectLockChanged(EHistoryChangeMessage message);
    void AtomsTVDblClicked(const QModelIndex& index);

    void UpdateEditTable(void);
    void EditTWChanged(QTableWidgetItem* p_item);
    void ItemSelectionChanged(void);

    // order
    void LocIndxByOrder(void);
    void LocIndxByMolecules(void);
    void AggregateHydrogens(void);
    void AggregateTerminals(void);
    void ReverseOrder(void);
    void SortAtoms(void);

    // build
    void AddHydrogens(void);
    void RemoveHydrogens(void);
    void NewResidue(void);
    void NamesByLocIndx(void);
    void NamesByHeavyAtoms(void);

    // show
    void ShowOthers(void);
    void HideOthers(void);
    void UpdateHighlightedAtoms(void);
    void ShowLabels(void);

    // nicemol
    void DetectHeadTail(void);
    void RearrangeAtomsAndNames(void);
    void RearrangeAtoms(void);
    void HeadAtomAboutToBeChanged(CProObject* p_obj);
    void TailAtomAboutToBeChanged(CProObject* p_obj);

    void FindLongestChain(void);
    void FindLongestChain(CAtom* p_atom,std::vector<CAtom*> path,
                          std::vector<CAtom*> &final_path,bool with_term);
    void RebuildSideChains(void);
    void FixTerminals(void);
    void RebuildHydrogens(void);
};

//------------------------------------------------------------------------------

#endif
