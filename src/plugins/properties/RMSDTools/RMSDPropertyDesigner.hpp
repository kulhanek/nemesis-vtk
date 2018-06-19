#ifndef RMSDPropertyDesignerH
#define RMSDPropertyDesignerH
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

#include <ProObjectDesigner.hpp>
#include "ui_RMSDPropertyDesigner.h"
#include <HistoryList.hpp>

//------------------------------------------------------------------------------

class CRMSDProperty;
class CPODesignerGeneral;
class CPRDesignerValue;
class CPRDesignerGraphics;
class CPRDesignerAtoms;
class CPODesignerRefBy;

//------------------------------------------------------------------------------

/// RMSDProperty designer

class CRMSDPropertyDesigner : public CProObjectDesigner {
Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CRMSDPropertyDesigner(CRMSDProperty* p_fmo);

    /// initialize visualization of properties
    void InitAllValues(void);

    /// update object properties according to visual setup
    void ApplyAllValues(void);

// section of private data ----------------------------------------------------
private:
    Ui::RMSDPropertyDesigner    WidgetUI;
    CRMSDProperty*              Property;
    CContainerModel*            MapModel;
    CPODesignerGeneral*         General;
    CPRDesignerValue*           Value;
    CPRDesignerGraphics*        Graphics;
    CPODesignerRefBy*           RefBy;
    CSelectionRequest*          SelRequest;

private slots:
    void InitValues(void);
    void ApplyValues(void);

    void TemplateObjectChanged(void);
    void TargetObjectChanged(void);

    void StartManualMapping(void);
    void StopManualMapping(void);
    void SelectionCompleted(void);

    void MapTVClicked(const QModelIndex& index);
    void MapTVReset(void);
    void RemoveSelectedCouple(void);
    void RemoveHydrogenCouples(void);
    void RemoveAllCouples(void);
    void MappingBySerIndx(void);
    void MappingByDistance(void);

    void AlignTarget(void);
    void HideAllMappedAtoms(void);
    void HideAllUnmappedAtoms(void);
    void ShowAllAtoms(void);
    void MapTargetPosToTemplate(void);
    void MapTemplatePosToTarget(void);
    void ImprintTemplateTopologyToTarget(void);
    void ImprintTargetTopologyToTemplate(void);

    void ButtonBoxClicked(QAbstractButton*);
    void ProjectLockChanged(EHistoryChangeMessage message);
};

//------------------------------------------------------------------------------

#endif
