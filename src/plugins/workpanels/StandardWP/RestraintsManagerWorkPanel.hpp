#ifndef RestraintManagerWorkPanelH
#define RestraintManagerWorkPanelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2011 Jakub Stepan, xstepan3@chemi.muni.cz
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
#include <HistoryList.hpp>
#include "ui_RestraintsManagerWorkPanel.h"

// -----------------------------------------------------------------------------

class CStructure;

// -----------------------------------------------------------------------------

class CRestraintsManagerWorkPanel : public CWorkPanel {
Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CRestraintsManagerWorkPanel(CProject* p_project);
    ~CRestraintsManagerWorkPanel();

// section of private data ----------------------------------------------------
private:
    Ui::RestraintsManagerWorkPanel  WidgetUI;
    CContainerModel*                RestraintsModel;
    CStructure*                     ActiveStructure;

private slots:
    void UpdateStructureList(void);
    void StructureIndexChanged(int index);

    void StructureInfo(void);

    void RestraintsTVReset(void);
    void RestraintsTVClicked(const QModelIndex& index);
    void RestraintsTVDblClicked(const QModelIndex& index);
    void RestraintInfo(void);
    void RestraintNew(void);
    void RestraintToggleEnable(void);
    void RestraintDelete(void);
    void DeleteAllRestraints(void);
    void DeleteInvalidRestraints(void);

    void EnableRestraints(bool set);
    void UpdateRestraintListStatus(void);

    void ProjectLockChanged(EHistoryChangeMessage message);
};

// -----------------------------------------------------------------------------

#endif

