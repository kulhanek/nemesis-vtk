// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ContainerModel.hpp>

#include <PODesignerGeneral.hpp>
#include <PDBQTTrajSegment.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <Trajectory.hpp>

#include <PDBQTTrajSegmentDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PDBQTTrajSegmentDesignerCB(void* p_data);

CExtUUID        PDBQTTrajSegmentDesignerID(
                    "{PDBQT_TRAJ_SEGMENT_DESIGNER:2f59da96-5788-4fa1-8f6b-a933c92cfdac}",
                    "PDBQT Trajectory Segment");

CPluginObject   PDBQTTrajSegmentDesignerObject(&NemesisCorePlugin,
                    PDBQTTrajSegmentDesignerID,
                    DESIGNER_CAT,
                    ":/images/NemesisCore/trajectory/segments/PDBQTTrajSegment.svg",
                    PDBQTTrajSegmentDesignerCB);

// -----------------------------------------------------------------------------

QObject* PDBQTTrajSegmentDesignerCB(void* p_data)
{
    CPDBQTTrajSegment* p_pro_object = static_cast<CPDBQTTrajSegment*>(p_data);
    QObject* p_object = new CPDBQTTrajSegmentDesigner(p_pro_object);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPDBQTTrajSegmentDesigner::CPDBQTTrajSegmentDesigner(CPDBQTTrajSegment* p_owner)
    : CProObjectDesigner(&PDBQTTrajSegmentDesignerObject,p_owner)
{   
    Object = p_owner;

    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);

    // static items
    WidgetUI.trajectoryLO->setObject(Object->GetTrajectory());
    QFileInfo fileinfo(Object->GetFileName());
    WidgetUI.fileNameLE->setText(fileinfo.fileName());
    WidgetUI.filePathLE->setText(fileinfo.path());

    // init model which is determine by type of variable
    PosesModel = Object->GetContainerModel(this);

    // set model to QTreeView
    WidgetUI.posesTV->setModel(PosesModel);
    WidgetUI.posesTV->setDragDropMode(QAbstractItemView::DragOnly);
    WidgetUI.posesTV->setDragEnabled(true);

    if( PosesModel != NULL ){
        for(int i=0; i < PosesModel->columnCount(); i++){
            // nice resize of columns
            WidgetUI.posesTV->resizeColumnToContents(i);
        }
    }

    // signals -----------------------------------
    connect(WidgetUI.seqIndexSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    //--------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));

    // external events ---------------------------
    connect(Object, SIGNAL(OnStatusChanged(EStatusChanged)),
            this, SLOT(InitValues()));
    //--------------
    connect(Object->GetTrajectory(), SIGNAL(OnSnapshotChanged(void)),
            this, SLOT(InitPointerValues()));
    //--------------
    connect(Object->GetTrajectory(), SIGNAL(OnTrajectorySegmentsChanged(void)),
            this, SLOT(InitPointerValues()));

    //--------------
    if( Object->GetProject() ){
        connect(Object->GetProject()->GetHistory(), SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
                this, SLOT(ProjectLockChanged(EHistoryChangeMessage)));
    }

    // init all values ---------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPDBQTTrajSegmentDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CPDBQTTrajSegmentDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_COMPOSITE,Object->GetType().GetName()) == false ) return;

    Changing = true;
        General->ApplyValues();
        ApplyValues();
    Changing = false;

    Object->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPDBQTTrajSegmentDesigner::ButtonBoxClicked(QAbstractButton* p_button)
{
    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Reset ) {
        InitAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Apply ) {
        ApplyAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Close ) {
        close();
        return;
    }
}

//------------------------------------------------------------------------------

void CPDBQTTrajSegmentDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.seqIndexSB->setValue(Object->GetSeqIndex());

    InitPointerValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CPDBQTTrajSegmentDesigner::InitPointerValues(void)
{
    WidgetUI.currentSnapshotSB->setValue(Object->GetCurrentSnapshotIndex());
    WidgetUI.numberOfSnapshotsSB->setValue(Object->GetNumberOfSnapshots());

    if( Object->GetTrajectory() ) {
        if( Object->GetTrajectory()->IsSegmentActive(Object) ){
            WidgetUI.isActiveSegmentLA->setText(tr("yes"));
        } else {
            WidgetUI.isActiveSegmentLA->setText(tr("no"));
        }
    }
}

//------------------------------------------------------------------------------

void CPDBQTTrajSegmentDesigner::ApplyValues(void)
{
    Object->SetSeqIndexWH(WidgetUI.seqIndexSB->value());
}

//------------------------------------------------------------------------------

void CPDBQTTrajSegmentDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================










