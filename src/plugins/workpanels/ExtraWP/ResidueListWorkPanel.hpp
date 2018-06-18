#ifndef ResidueListWorkPanelH
#define ResidueListWorkPanelH
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
#include "ui_ResidueListWorkPanel.h"
#include <HistoryList.hpp>
#include <RegisteredObject.hpp>

//------------------------------------------------------------------------------

class CResidue;
class CSelectionRequest;

//------------------------------------------------------------------------------

class CResidueListWorkPanel : public CWorkPanel {
    Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CResidueListWorkPanel(CProject* p_pro,CStructure* p_mol);
    ~CResidueListWorkPanel(void);

// section of private data ----------------------------------------------------
private:
    Ui::ResidueListWorkPanel    WidgetUI;
    CStructure*                 Structure;
    CContainerModel*            ResiduesModel;
    CContainerModel*            AtomsModel;
    bool                        UpdatingItems;

private slots:
    void StructureChanged(void);
    void StructureRegistered(void);
    void StructureUnregistered(void);
    void ProjectLockChanged(EHistoryChangeMessage message);

    void ListModelTVDblClicked(const QModelIndex& index);

    void UpdateEditTable(void);
    void EditTWChanged(QTableWidgetItem* p_item);
    void ItemSelectionChanged(void);

    // order
    void SeqIndxByOrder(void);
    void SeqIndxByReverseOrder(void);
    void SeqIndxByChains(void);
    void SortResidues(void);

    // build
    void EditResidues(void);
    void DeleteResidues(void);
    void DeleteEmptyStructures(void);
    void MergeResidues(void);

    // show
    void UpdateHighlightedResidues(void);

    // chains
    void SetChainNames(void);

    // atoms
    void SerialIndexBySeqAndLocIndexes(void);
};

//------------------------------------------------------------------------------

#endif
