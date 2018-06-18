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
#include <XYZTrajSegment.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <Trajectory.hpp>

#include <XYZTrajSegmentDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* XYZTrajSegmentDesignerCB(void* p_data);

CExtUUID        XYZTrajSegmentDesignerID(
                    "{XYZ_TRAJ_SEGMENT_DESIGNER:583c944e-1403-461d-8268-1fa82404e80a}",
                    "XYZ Trajectory Segment");

CPluginObject   XYZTrajSegmentDesignerObject(&NemesisCorePlugin,
                    XYZTrajSegmentDesignerID,
                    DESIGNER_CAT,
                    ":/images/NemesisCore/trajectory/segments/XYZTrajSegment.svg",
                    XYZTrajSegmentDesignerCB);

// -----------------------------------------------------------------------------

QObject* XYZTrajSegmentDesignerCB(void* p_data)
{
    CXYZTrajSegment* p_pro_object = static_cast<CXYZTrajSegment*>(p_data);
    QObject* p_object = new CXYZTrajSegmentDesigner(p_pro_object);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CXYZTrajSegmentDesigner::CXYZTrajSegmentDesigner(CXYZTrajSegment* p_owner)
    : CProObjectDesigner(&XYZTrajSegmentDesignerObject,p_owner)
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

void CXYZTrajSegmentDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CXYZTrajSegmentDesigner::ApplyAllValues(void)
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

void CXYZTrajSegmentDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CXYZTrajSegmentDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.seqIndexSB->setValue(Object->GetSeqIndex());

    InitPointerValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CXYZTrajSegmentDesigner::InitPointerValues(void)
{
    WidgetUI.currentSnapshotSB->setValue(Object->GetCurrentSnapshotIndex());
    WidgetUI.numberOfSnapshotsSB->setValue(Object->GetNumberOfSnapshots());

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

void CXYZTrajSegmentDesigner::ApplyValues(void)
{
    Object->SetSeqIndexWH(WidgetUI.seqIndexSB->value());
}

//------------------------------------------------------------------------------

void CXYZTrajSegmentDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================










