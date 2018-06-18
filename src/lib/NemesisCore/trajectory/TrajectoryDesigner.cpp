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

#include <QtGui>

#include <NemesisCoreModule.hpp>
#include <Object.hpp>
#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>

#include <PODesignerGeneral.hpp>
#include <PODesignerRefBy.hpp>
#include <Trajectory.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <ContainerModel.hpp>
#include <TrajectoryList.hpp>
#include <TrajectorySegment.hpp>
#include <SnapshotFilter.hpp>
#include <MainWindow.hpp>

#include <TrajectoryDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* TrajectoryDesignerCB(void* p_data);

CExtUUID        TrajectoryDesignerID(
                    "{TRAJECTORY_DESIGNER:8470d302-d01d-4150-a419-a638d27b2906}",
                    "Trajectory");

CPluginObject   TrajectoryDesignerObject(&NemesisCorePlugin,
                    TrajectoryDesignerID,
                    DESIGNER_CAT,
                    ":/images/NemesisCore/trajectory/Trajectory.svg",
                    TrajectoryDesignerCB);

// -----------------------------------------------------------------------------

QObject* TrajectoryDesignerCB(void* p_data)
{
    CTrajectory* p_pro_object = static_cast<CTrajectory*>(p_data);
    QObject* p_object = new CTrajectoryDesigner(p_pro_object);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectoryDesigner::CTrajectoryDesigner(CTrajectory* p_owner)
    : CProObjectDesigner(&TrajectoryDesignerObject,p_owner)
{   
    Object = p_owner;

    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    RefBy = new CPODesignerRefBy(WidgetUI.refByTab,Object,this);

    // segments model
    SegmentsModel = Object->GetContainerModel(this,"segments");
    WidgetUI.segmentsTV->setModel(SegmentsModel);

    if( SegmentsModel != NULL ){
        for(int i=0; i < SegmentsModel->columnCount(); i++){
            WidgetUI.segmentsTV->resizeColumnToContents(i);
        }
        // connect signals ---------------------------
        connect(SegmentsModel,SIGNAL(modelReset()),
                this,SLOT(SegmentsTVSelectionChanged()));
    }

    // filters
    FiltersModel = Object->GetContainerModel(this,"filters");
    WidgetUI.filtersTV->setModel(FiltersModel);

    if( FiltersModel != NULL ){
        for(int i=0; i < FiltersModel->columnCount(); i++){
            WidgetUI.filtersTV->resizeColumnToContents(i);
        }
        // connect signals ---------------------------
        connect(FiltersModel,SIGNAL(modelReset()),
                this,SLOT(FiltersTVSelectionChanged()));
    }

    // setup structure link object
    WidgetUI.structureLO->setProject(Object->GetProject());
    WidgetUI.structureLO->setObjectBaseMIMEType("structure.indexes");
    WidgetUI.structureLO->setSelectionHandler(NULL);
    WidgetUI.structureLO->showRemoveButton(true);

    // signals -----------------------------------
    connect(WidgetUI.structureLO, SIGNAL(OnObjectChanged(void)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.playSpeedSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.playModeCB, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SetChangedFlagTrue()));

    connect(WidgetUI.segmentsTV->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this,SLOT(SegmentsTVSelectionChanged(void)));
    //------------------
    connect(WidgetUI.segmentsTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(SegmentsTVDblClicked(const QModelIndex&)));
    //------------------
    connect(WidgetUI.deleteSegmentPB,SIGNAL(clicked(bool)),
            this,SLOT(DeleteSegment(void)));
    //------------------
    connect(WidgetUI.moveUpSegmentPB,SIGNAL(clicked(bool)),
            this,SLOT(MoveUpSegment(void)));
    //------------------
    connect(WidgetUI.moveDownSegmentPB,SIGNAL(clicked(bool)),
            this,SLOT(MoveDownSegment(void)));
    //------------------
    connect(WidgetUI.infoSegmentPB,SIGNAL(clicked(bool)),
            this,SLOT(SegmentInfo(void)));

    connect(WidgetUI.filtersTV->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this,SLOT(FiltersTVSelectionChanged(void)));
    //------------------
    connect(WidgetUI.filtersTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(FiltersTVDblClicked(const QModelIndex&)));
    //------------------
    connect(WidgetUI.newFilterPB,SIGNAL(clicked(bool)),
            this,SLOT(NewFilter(void)));
    //------------------
    connect(WidgetUI.deleteFilterPB,SIGNAL(clicked(bool)),
            this,SLOT(DeleteFilter(void)));
    //------------------
    connect(WidgetUI.moveUpFilterPB,SIGNAL(clicked(bool)),
            this,SLOT(MoveUpFilter(void)));
    //------------------
    connect(WidgetUI.moveDownFilterPB,SIGNAL(clicked(bool)),
            this,SLOT(MoveDownFilter(void)));
    //------------------
    connect(WidgetUI.filterInfoPB,SIGNAL(clicked(bool)),
            this,SLOT(FilterInfo(void)));

    //--------------
    connect(WidgetUI.buttonBox1, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));
    connect(WidgetUI.buttonBox2, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));

    // external events ---------------------------
    connect(Object, SIGNAL(OnStatusChanged(EStatusChanged)),
            this, SLOT(InitValues()));
    //--------------
    connect(Object, SIGNAL(OnSnapshotChanged(void)),
            this, SLOT(InitValues()));
    //--------------
    connect(Object, SIGNAL(OnTrajectorySegmentsChanged(void)),
            this, SLOT(InitValues()));
    //--------------
    connect(Object->GetTrajectories(), SIGNAL(OnTrajectoryListChanged(void)),
            this, SLOT(InitValues()));
    //--------------
    connect(Object, SIGNAL(OnSnapshotFiltersChanged(void)),
            this, SLOT(InitValues()));
    //--------------
    if( Object->GetProject() ){
        connect(Object->GetProject()->GetHistory(), SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
                this, SLOT(ProjectLockChanged(EHistoryChangeMessage)));
    }

    // init all values ---------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
    SegmentsTVSelectionChanged();
    FiltersTVSelectionChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectoryDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_COMPOSITE,Object->GetType().GetName()) == false ) return;

    Changing = true;
        General->ApplyValues();
        ApplyValues();
        RefBy->ApplyValues();
    Changing = false;

    Object->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectoryDesigner::ButtonBoxClicked(QAbstractButton* p_button)
{
    if( WidgetUI.buttonBox1->standardButton(p_button) == QDialogButtonBox::Reset ) {
        InitAllValues();
        return;
    }
    if( WidgetUI.buttonBox1->standardButton(p_button) == QDialogButtonBox::Apply ) {
        ApplyAllValues();
        return;
    }
    if( WidgetUI.buttonBox1->standardButton(p_button) == QDialogButtonBox::Close ) {
        close();
        return;
    }

    if( WidgetUI.buttonBox2->standardButton(p_button) == QDialogButtonBox::Reset ) {
        InitAllValues();
        return;
    }
    if( WidgetUI.buttonBox2->standardButton(p_button) == QDialogButtonBox::Apply ) {
        ApplyAllValues();
        return;
    }
    if( WidgetUI.buttonBox2->standardButton(p_button) == QDialogButtonBox::Close ) {
        close();
        return;
    }
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;
    if(!Object->GetStructure()) return;

    WidgetUI.structureLO->blockSignals(true);
    WidgetUI.structureLO->setObject(Object->GetStructure());
    WidgetUI.structureLO->blockSignals(false);

    WidgetUI.currentSnapshotSB->setValue(Object->GetCurrentSnapshotIndex());
    WidgetUI.numberOfSnapshotsSB->setValue(Object->GetNumberOfSnapshots());
    WidgetUI.currentSegmentSB->setValue(Object->GetCurrentSegmentIndex());
    WidgetUI.numberOfSegmentsSB->setValue(Object->GetNumberOfSegments());

    if( Object->GetTrajectories()->GetActiveTrajectory() == Object ){
        WidgetUI.isActiveTrajectoryLA->setText(tr("yes"));
    } else {
        WidgetUI.isActiveTrajectoryLA->setText(tr("no"));
    }

    WidgetUI.playSpeedSB->setEnabled(!Object->IsPlaying());
    WidgetUI.playSpeedSB->blockSignals(true);
    WidgetUI.playSpeedSB->setValue(Object->GetPlayTickTime());
    WidgetUI.playSpeedSB->blockSignals(false);


    WidgetUI.playModeCB->setEnabled(!Object->IsPlaying());
    WidgetUI.playModeCB->blockSignals(true);
    WidgetUI.playModeCB->setCurrentIndex(Object->GetPlayMode());
    WidgetUI.playModeCB->blockSignals(false);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::ApplyValues(void)
{
    Object->SetStructureWH(WidgetUI.structureLO->getObject<CStructure*>());
    Object->SetPlayTickTimeWH(WidgetUI.playSpeedSB->value());
    Object->SetPlayModeWH(static_cast<ETrajectoryPlayMode>(WidgetUI.playModeCB->currentIndex()));
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    WidgetUI.structureGB->setEnabled(! Object->IsHistoryLocked(EHCL_TRAJECTORIES));
    WidgetUI.playSpeedSB->setEnabled(! Object->IsHistoryLocked(EHCL_TRAJECTORIES) && ! Object->IsPlaying());
    WidgetUI.playModeCB->setEnabled(! Object->IsHistoryLocked(EHCL_TRAJECTORIES) && ! Object->IsPlaying());
    WidgetUI.segmentsW->setEnabled(! Object->IsHistoryLocked(EHCL_TRAJECTORIES));
    WidgetUI.filtersW->setEnabled(! Object->IsHistoryLocked(EHCL_TRAJECTORIES));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectoryDesigner::SegmentsTVSelectionChanged(void)
{
    QModelIndexList selected_rows = WidgetUI.segmentsTV->selectionModel()->selectedRows();
    WidgetUI.deleteSegmentPB->setEnabled(selected_rows.count() > 0);
    WidgetUI.infoSegmentPB->setEnabled(selected_rows.count() > 0);

    if( selected_rows.count() == 1 ){
        CTrajectorySegment* p_seg = NULL;
        p_seg = dynamic_cast<CTrajectorySegment*>(SegmentsModel->GetItem(selected_rows.at(0)));
        if( p_seg != NULL ){
            WidgetUI.moveUpSegmentPB->setEnabled(! p_seg->IsFirstSegment());
            WidgetUI.moveDownSegmentPB->setEnabled(! p_seg->IsLastSegment());
        } else {
            WidgetUI.moveUpSegmentPB->setEnabled(false);
            WidgetUI.moveDownSegmentPB->setEnabled(false);
        }
    } else {
        WidgetUI.moveUpSegmentPB->setEnabled(false);
        WidgetUI.moveDownSegmentPB->setEnabled(false);
    }
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::SegmentsTVDblClicked(const QModelIndex& index)
{
    CProObject* p_obj = dynamic_cast<CProObject*>(SegmentsModel->GetItem(index));
    if( p_obj == NULL ) return;
    p_obj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::DeleteSegment(void)
{
    QModelIndexList selected_rows = WidgetUI.segmentsTV->selectionModel()->selectedRows();
    if( selected_rows.count() > 1 ){
        CHistoryNode* p_history = Object->BeginChangeWH(EHCL_TRAJECTORIES,"delete trajectory segments");
        if( p_history == NULL ) return;
        for(int i=0; i < selected_rows.count(); i++){
            CTrajectorySegment* p_seg = dynamic_cast<CTrajectorySegment*>(SegmentsModel->GetItem(selected_rows[i]));
            if( p_seg != NULL ){
                p_seg->RemoveFromBaseList(p_history);
            }
        }
        Object->EndChangeWH();
    } else {
        CTrajectorySegment* p_seg = dynamic_cast<CTrajectorySegment*>(SegmentsModel->GetItem(selected_rows[0]));
        if( p_seg == NULL ) return;
        p_seg->DeleteWH();
    }
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::MoveUpSegment(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.segmentsTV->selectionModel();
    if( p_selmodel->selectedRows().count() != 1 ) return;

    CTrajectorySegment* p_seg = NULL;
    p_seg = dynamic_cast<CTrajectorySegment*>(SegmentsModel->GetItem(p_selmodel->selectedRows().at(0)));
    if( p_seg == NULL ) return;

    p_seg->MoveUpWH();
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::MoveDownSegment(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.segmentsTV->selectionModel();
    if( p_selmodel->selectedRows().count() != 1 ) return;

    CTrajectorySegment* p_seg = NULL;
    p_seg = dynamic_cast<CTrajectorySegment*>(SegmentsModel->GetItem(p_selmodel->selectedRows().at(0)));
    if( p_seg == NULL ) return;

    p_seg->MoveDownWH();
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::SegmentInfo(void)
{
    QModelIndexList selected_rows = WidgetUI.segmentsTV->selectionModel()->selectedRows();
    for(int i=0; i < selected_rows.count(); i++){
        CTrajectorySegment* p_seg = dynamic_cast<CTrajectorySegment*>(SegmentsModel->GetItem(selected_rows[i]));
        if( p_seg != NULL ){
            p_seg->OpenObjectDesigner();
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectoryDesigner::FiltersTVSelectionChanged(void)
{
    QModelIndexList selected_rows = WidgetUI.filtersTV->selectionModel()->selectedRows();
    WidgetUI.deleteFilterPB->setEnabled(selected_rows.count() > 0);
    WidgetUI.filterInfoPB->setEnabled(selected_rows.count() > 0);

    if( selected_rows.count() == 1 ){
        CSnapshotFilter* p_flt = NULL;
        p_flt = dynamic_cast<CSnapshotFilter*>(FiltersModel->GetItem(selected_rows.at(0)));
        if( p_flt != NULL ){
            WidgetUI.moveUpFilterPB->setEnabled(! p_flt->IsFirstFilter());
            WidgetUI.moveDownFilterPB->setEnabled(! p_flt->IsLastFilter());
        } else {
            WidgetUI.moveUpFilterPB->setEnabled(false);
            WidgetUI.moveDownFilterPB->setEnabled(false);
        }
    } else {
        WidgetUI.moveUpFilterPB->setEnabled(false);
        WidgetUI.moveDownFilterPB->setEnabled(false);
    }
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::FiltersTVDblClicked(const QModelIndex& index)
{
    CProObject* p_obj = dynamic_cast<CProObject*>(FiltersModel->GetItem(index));
    if( p_obj == NULL ) return;
    p_obj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::NewFilter(void)
{
    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString("{NEW_SNAPSHOT_FILTER_WORK_PANEL:7b037a7d-4330-4631-b702-9c90a19512c6}");
    CMainWindow::ShowAsDialog(mp_uuid,Object);
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::DeleteFilter(void)
{
    QModelIndexList selected_rows = WidgetUI.filtersTV->selectionModel()->selectedRows();
    if( selected_rows.count() > 1 ){
        CHistoryNode* p_history = Object->BeginChangeWH(EHCL_TRAJECTORIES,"delete trajectory segments");
        if( p_history == NULL ) return;
        for(int i=0; i < selected_rows.count(); i++){
            CSnapshotFilter* p_flt = dynamic_cast<CSnapshotFilter*>(FiltersModel->GetItem(selected_rows[i]));
            if( p_flt != NULL ){
                p_flt->RemoveFromBaseList(p_history);
            }
        }
        Object->EndChangeWH();
    } else {
        CSnapshotFilter* p_flt = dynamic_cast<CSnapshotFilter*>(FiltersModel->GetItem(selected_rows[0]));
        if( p_flt == NULL ) return;
        p_flt->DeleteWH();
    }
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::MoveUpFilter(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.filtersTV->selectionModel();
    if( p_selmodel->selectedRows().count() != 1 ) return;

    CSnapshotFilter* p_flt = NULL;
    p_flt = dynamic_cast<CSnapshotFilter*>(FiltersModel->GetItem(p_selmodel->selectedRows().at(0)));
    if( p_flt == NULL ) return;

    p_flt->MoveUpWH();
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::MoveDownFilter(void)
{
    QItemSelectionModel* p_selmodel = WidgetUI.filtersTV->selectionModel();
    if( p_selmodel->selectedRows().count() != 1 ) return;

    CSnapshotFilter* p_flt = NULL;
    p_flt = dynamic_cast<CSnapshotFilter*>(FiltersModel->GetItem(p_selmodel->selectedRows().at(0)));
    if( p_flt == NULL ) return;

    p_flt->MoveDownWH();
}

//------------------------------------------------------------------------------

void CTrajectoryDesigner::FilterInfo(void)
{
    QModelIndexList selected_rows = WidgetUI.filtersTV->selectionModel()->selectedRows();
    for(int i=0; i < selected_rows.count(); i++){
        CSnapshotFilter* p_flt = dynamic_cast<CSnapshotFilter*>(FiltersModel->GetItem(selected_rows[i]));
        if( p_flt != NULL ){
            p_flt->OpenObjectDesigner();
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================










