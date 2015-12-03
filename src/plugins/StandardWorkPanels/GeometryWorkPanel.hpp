#ifndef GeometryWorkPanelH
#define GeometryWorkPanelH
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

#include <WorkPanel.hpp>
#include "ui_GeometryWorkPanel.h"
#include <HistoryList.hpp>

// -----------------------------------------------------------------------------

enum EGeometryWorkPanelAction {
    EGWPA_NONE      =  0,
    EGWPA_POSITION  =  1,
    EGWPA_DISTANCE  =  2,
    EGWPA_ANGLE     =  3,
    EGWPA_TORSION   =  4
};

// -----------------------------------------------------------------------------

class CSelectionRequest;
class CAtomList;

// -----------------------------------------------------------------------------

/// geometry measurement work panel

class CGeometryWorkPanel : public CWorkPanel {
Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CGeometryWorkPanel(CProject* p_project);
    ~CGeometryWorkPanel(void);

// section of private data ----------------------------------------------------
private:
    Ui::GeometryWorkPanel       WidgetUI;
    QButtonGroup*               ButtonGroup;
    CSelectionRequest*          SelRequest;
    CContainerModel*            Model;
    CAtomList*                  Atoms;

private slots:
    void SetAction(int id);
    void SelectionInitialized(void);
    void SelectionCompleted(void);
    void SelectionDetached(void);
    void SelectionChanged(void);
    void PhysicalQuantityUnitChange(void);
    void ModelDblClick(const QModelIndex& index);
    void LabelDAT(void);
    void RestrainDAT(void);
    void PropertyDAT(void);
    void ProjectLockChanged(EHistoryChangeMessage message);
};

// -----------------------------------------------------------------------------

#endif

