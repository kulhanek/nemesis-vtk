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

#include <PODesignerGeneral.hpp>
#include <VibTrajSegment.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <Trajectory.hpp>
#include <NormalVibMode.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>

#include <VibTrajSegmentDesigner.hpp>
#include "VibTrajSegmentDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* VibTrajSegmentDesignerCB(void* p_data);

CExtUUID        VibTrajSegmentDesignerID(
                    "{VIB_TRAJ_SEGMENT_DESIGNER:02d468c7-8cc7-4295-92cf-eeae2ae9c653}",
                    "Vibrations");

CPluginObject   VibTrajSegmentDesignerObject(&NemesisCorePlugin,
                    VibTrajSegmentDesignerID,
                    DESIGNER_CAT,
                    ":/images/NemesisCore/trajectory/segments/VibTrajSegment.svg",
                    VibTrajSegmentDesignerCB);

// -----------------------------------------------------------------------------

QObject* VibTrajSegmentDesignerCB(void* p_data)
{
    CVibTrajSegment* p_pro_object = static_cast<CVibTrajSegment*>(p_data);
    QObject* p_object = new CVibTrajSegmentDesigner(p_pro_object);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CVibTrajSegmentDesigner::CVibTrajSegmentDesigner(CVibTrajSegment* p_owner)
    : CProObjectDesigner(&VibTrajSegmentDesignerObject,p_owner)
{   
    Object = p_owner;

    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);

    // static items
    WidgetUI.segmentTipeLA->setText(Object->GetType().GetName());
    WidgetUI.trajectoryLO->setObject(Object->GetTrajectory());
    QFileInfo fileinfo(Object->GetFileName());
    WidgetUI.fileNameLE->setText(fileinfo.fileName());
    WidgetUI.filePathLE->setText(fileinfo.path());

    // vibrations cannot change
    UpdateAll = false;
    VibModel = new QStandardItemModel(this);
    InitVibModel();
    WidgetUI.vibrationsTV->setModel(VibModel);
    connect(VibModel,SIGNAL(itemChanged(QStandardItem *)),
            this,SLOT(UpdateActiveVibrations(QStandardItem*)));

    // signals -----------------------------------
    connect(WidgetUI.seqIndexSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    //--------------
    connect(WidgetUI.numOfSnapshotsSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    //--------------
    connect(WidgetUI.amplitudeSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    //--------------
    connect(WidgetUI.buttonBox1, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));
    //--------------
    connect(WidgetUI.buttonBox2, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));
    //--------------
    connect(WidgetUI.deactivateAllPB,SIGNAL(clicked(bool)),
            this, SLOT(DeactivateAllModes(void)));
    //--------------
    connect(WidgetUI.activateImaginaryModesPB,SIGNAL(clicked(bool)),
            this, SLOT(ActivateImaginaryModes(void)));

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

//------------------------------------------------------------------------------

void CVibTrajSegmentDesigner::InitVibModel(void)
{
    WidgetUI.numOfVibrationsSB->setValue(Object->GetNumberOfModes());

    // set header
    QStringList header;
    header << tr("ID") << tr("Frequency") << tr("IR Intensity") << tr("Scope") << tr("Symmetry");
    VibModel->setHorizontalHeaderLabels(header);
    VibModel->setSortRole(Qt::UserRole+1);

    // load items
    for(int i=0; i < Object->GetNumberOfModes(); i++){
        CNormalVibMode* p_mode = Object->NormalModes[i];

        QStandardItem*          p_item;
        QList<QStandardItem*>   items;

        p_item = new QStandardItem(QString("%1").arg(i+1));
        p_item->setCheckable(true);
        p_item->setCheckState(p_mode->IsActive ? Qt::Checked : Qt::Unchecked);
        p_item->setData(i);
        p_item->setTextAlignment(Qt::AlignRight);
        items << p_item;

        p_item = new QStandardItem(PQ_FREQUENCY->GetRealValueText(p_mode->Frequency));
        p_item->setTextAlignment(Qt::AlignRight);
        p_item->setData(p_mode->Frequency);
        items << p_item;

        p_item = new QStandardItem(PQ_IR_INTENSITY->GetRealValueText(p_mode->Intensity));
        p_item->setTextAlignment(Qt::AlignRight);
        p_item->setData(p_mode->Intensity);
        items << p_item;

        QString scope;
        if( p_mode->IsRaman ){
            scope = "RAMAN";
        }
        if( p_mode->IsIR ){
            scope = "IR";
        }
        p_item = new QStandardItem(scope);
        p_item->setTextAlignment(Qt::AlignRight);
        p_item->setData(scope);
        items << p_item;

        p_item = new QStandardItem(QString::fromStdString(p_mode->Symmetry));
        p_item->setTextAlignment(Qt::AlignRight);
        p_item->setData(QString::fromStdString(p_mode->Symmetry));
        items << p_item;

        VibModel->appendRow(items);
    }

    WidgetUI.vibrationsTV->sortByColumn(0,Qt::AscendingOrder);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CVibTrajSegmentDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CVibTrajSegmentDesigner::ApplyAllValues(void)
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

void CVibTrajSegmentDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CVibTrajSegmentDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.seqIndexSB->blockSignals(true);
    WidgetUI.seqIndexSB->setValue(Object->GetSeqIndex());
    WidgetUI.seqIndexSB->blockSignals(false);

    WidgetUI.numOfSnapshotsSB->blockSignals(true);
    WidgetUI.numOfSnapshotsSB->setValue(Object->GetNumberOfSnapshots());
    WidgetUI.numOfSnapshotsSB->blockSignals(false);

    WidgetUI.amplitudeSB->blockSignals(true);
    WidgetUI.amplitudeSB->setValue(Object->GetAmplitude());
    WidgetUI.amplitudeSB->blockSignals(false);

    InitPointerValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CVibTrajSegmentDesigner::InitPointerValues(void)
{
    WidgetUI.currentSnapshotSB->setValue(Object->GetCurrentSnapshotIndex());
    //WidgetUI.numberOfSnapshotsSB->setValue(Object->GetNumberOfSnapshots());

    // this might be called during the segment destuction
    // in CYXZTrajSegment::~CYXZTrajSegment
    if( Object->GetTrajectory() ) {
        if( Object->GetTrajectory()->IsSegmentActive(Object) ){
            WidgetUI.isActiveSegmentLA->setText(tr("yes"));
        } else {
            WidgetUI.isActiveSegmentLA->setText(tr("no"));
        }
    }
}

//------------------------------------------------------------------------------

void CVibTrajSegmentDesigner::ApplyValues(void)
{
    Object->SetSeqIndexWH(WidgetUI.seqIndexSB->value());
    Object->SetAmplitude(WidgetUI.amplitudeSB->value());
}

//------------------------------------------------------------------------------

void CVibTrajSegmentDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CVibTrajSegmentDesigner::UpdateActiveVibrations(QStandardItem* p_item)
{
    if( UpdateAll ) return;
    int nmodes = Object->GetNumberOfModes();
    int mode = p_item->data().toInt();
    if( (mode < 0) || (mode > nmodes) ) return;

    Object->NormalModes[mode]->IsActive = p_item->checkState() != Qt::Unchecked;

    int active = 0;
    for(int i=0; i < nmodes; i++){
        if( Object->NormalModes[i]->IsActive ){
            active++;
        }
    }
    WidgetUI.numOfActiveVibrationsSB->setValue(active);
    Object->EmitOnSnapshotChanged();
}

//------------------------------------------------------------------------------

void CVibTrajSegmentDesigner::ActivateImaginaryModes(void)
{
    int nmodes = Object->GetNumberOfModes();
    UpdateAll = true;
    int active = 0;
    for(int i=0; i < nmodes; i++){
        if( Object->NormalModes[i]->Frequency < 0 ){
            Object->NormalModes[i]->IsActive = true;
            QStandardItem* p_item = VibModel->item(i);
            p_item->setCheckState(Qt::Checked);
            active++;
        } else {
            Object->NormalModes[i]->IsActive = false;
            QStandardItem* p_item = VibModel->item(i);
            p_item->setCheckState(Qt::Unchecked);
        }
    }
    UpdateAll = false;

    WidgetUI.numOfActiveVibrationsSB->setValue(active);
    Object->EmitOnSnapshotChanged();
}

//------------------------------------------------------------------------------

void CVibTrajSegmentDesigner::DeactivateAllModes(void)
{
    int nmodes = Object->GetNumberOfModes();
    UpdateAll = true;
    for(int i=0; i < nmodes; i++){
        Object->NormalModes[i]->IsActive = false;
        QStandardItem* p_item = VibModel->item(i);
        p_item->setCheckState(Qt::Unchecked);
    }
    UpdateAll = false;

    WidgetUI.numOfActiveVibrationsSB->setValue(0);
    Object->EmitOnSnapshotChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================










