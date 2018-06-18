// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

#include <WorkPanelList.hpp>
#include <ContainerModel.hpp>

#include <TrajectoryList.hpp>
#include <TrajectorySegment.hpp>
#include <Snapshot.hpp>

#include "PODesignerDockingTabsResults.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPODesignerDockingTabsResults::CPODesignerDockingTabsResults(QWidget* p_owner, CTrajectorySegment *p_object,
                                                       CWorkPanel *p_workpanel)
        : QWidget(p_owner)
{
    DockingSegment = p_object;
    DockingTrajectory = p_object->GetTrajectory();
    if( DockingSegment == NULL ){
        INVALID_ARGUMENT("p_object is NULL");
    }
    WorkPanel = p_workpanel;

    // load graphics -----------------------------
    WidgetUI.setupUi(this);

    // init model which is determine by type of variable
    PosesModel = DockingSegment->GetContainerModel(this);

    // set model to QTreeView
    WidgetUI.posesOfResultsTV->setModel(PosesModel);
    WidgetUI.posesOfResultsTV->setDragDropMode(QAbstractItemView::DragOnly);
    WidgetUI.posesOfResultsTV->setDragEnabled(true);

    if( PosesModel != NULL ){
        for(int i=0; i < PosesModel->columnCount(); i++){
            // nice resize of columns
            WidgetUI.posesOfResultsTV->resizeColumnToContents(i);
        }
    }

    OffsetOfSnapshots = 0;
    SliderUpdating = false;
    SpinBoxUpdating = false;
    SliderMin = 0;
    SliderMax = 0;
    SpinBoxMin = 0;
    SpinBoxMax = 0;

    WidgetUI.WidgetTabOfResults->setEnabled(false);

    // add click on snapshot for move on them
    //------------------
    connect(WidgetUI.posesOfResultsTV,SIGNAL(clicked(const QModelIndex&)),
            this,SLOT(SnapshotsPosesClicked(const QModelIndex&)));
    // read and buttons activity response in snapshot view
    //------------------
    connect(DockingTrajectory,SIGNAL(OnSnapshotChanged(void)),
            this,SLOT(UpdateResultsMenu(void)));

    // prev and next button
    connect(WidgetUI.prevSnapshotPB, SIGNAL(clicked()),
        this, SLOT(PrevSnapshot()));
    //----------------
    connect(WidgetUI.nextSnapshotPB, SIGNAL(clicked()),
        this, SLOT(NextSnapshot()));

    // slider
    connect(WidgetUI.snapshotSL,SIGNAL(valueChanged(int)),
            this,SLOT(SliderValueChanged(int)));
    connect(WidgetUI.snapshotSL,SIGNAL(sliderMoved(int)),
            this,SLOT(SliderValueChanged(int)));

    // spin box
    connect(WidgetUI.snapshotSB,SIGNAL(valueChanged(int)),
            this,SLOT(SpinBoxValueChanged(int)));

    //------------------
    connect(DockingSegment,SIGNAL(destroyed(QObject*)),
            this,SLOT(SegmentDisconnect(QObject*)));

    UpdateResultsMenu();

    // do not init values here, the owner of this component is
    // responsible to call InitValues explicitly
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPODesignerDockingTabsResults::~CPODesignerDockingTabsResults()
{
    DockingSegment = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPODesignerDockingTabsResults::InitValues(void)
{
    // nothing to do here
}


//------------------------------------------------------------------------------

void CPODesignerDockingTabsResults::ApplyValues(void)
{
    // nothing to do here
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectorySegment* CPODesignerDockingTabsResults::GetTrajectorySegment(void) const
{
    return (DockingSegment);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPODesignerDockingTabsResults::UpdateResultsMenu(void)
{
    // update menu view of PODesignerDockingTabsResults with good values
    // in docking panel snapshot view focus on one of segments and is fill
    if( !DockingSegment || DockingTrajectory == NULL) {
        WidgetUI.WidgetTabOfResults->setEnabled(false);

        WidgetUI.posesOfResultsTV->setModel(NULL);

        WidgetUI.snapshotSB->setValue(0);
        WidgetUI.snapshotSB->setEnabled(false);

        WidgetUI.numOfSnapshotsSB->setValue(0);
        WidgetUI.numOfSnapshotsSB->setEnabled(false);

        WidgetUI.snapshotSL->setEnabled(false);
        WidgetUI.snapshotSL->setMinimum(0);
        WidgetUI.snapshotSL->setMaximum(0);
        return;
    }

    WidgetUI.WidgetTabOfResults->setEnabled(true);

    // update values from current segment of Results trajectory to menu view
    OffsetOfSnapshots = DockingTrajectory->GetNumberOfSnapshotsBefore(DockingSegment->GetSeqIndex());
    int nsnaps =  OffsetOfSnapshots + DockingSegment->GetNumberOfSnapshots();
    int current = OffsetOfSnapshots + DockingSegment->GetCurrentSnapshotIndex();

    bool set = (nsnaps != OffsetOfSnapshots);
    bool first = set && (current > OffsetOfSnapshots + 1);
    bool last = set && (current != nsnaps);

    if( set ) {
        if( SliderUpdating == false ){
            SliderUpdating = true;
            WidgetUI.snapshotSL->setMinimum(1);
            WidgetUI.snapshotSL->setMaximum(nsnaps - OffsetOfSnapshots);
            WidgetUI.snapshotSL->setValue(current - OffsetOfSnapshots);
            WidgetUI.snapshotSL->setEnabled(set);
            SliderUpdating = false;
        }

        if( SpinBoxUpdating == false ){
            SpinBoxUpdating = true;
            WidgetUI.snapshotSB->setMinimum(1);
            WidgetUI.snapshotSB->setMaximum(nsnaps);
            WidgetUI.snapshotSB->setValue(current);
            WidgetUI.snapshotSB->setEnabled(set);
            SpinBoxUpdating = false;
        }

        WidgetUI.numOfSnapshotsSB->setMinimum(1);
        WidgetUI.numOfSnapshotsSB->setMaximum(nsnaps);
        WidgetUI.numOfSnapshotsSB->setValue(nsnaps);
        WidgetUI.numOfSnapshotsSB->setEnabled(set);

    } else {
        if( SliderUpdating == false ){
            SliderUpdating = true;
            WidgetUI.snapshotSL->setMinimum(0);
            WidgetUI.snapshotSL->setMaximum(0);
            WidgetUI.snapshotSL->setValue(0);
            WidgetUI.snapshotSL->setEnabled(false);
            SliderUpdating = false;
        }

        if( SpinBoxUpdating == false ){
            SpinBoxUpdating = true;
            WidgetUI.snapshotSB->setMinimum(0);
            WidgetUI.snapshotSB->setMaximum(0);
            WidgetUI.snapshotSB->setValue(0);
            WidgetUI.snapshotSB->setEnabled(false);
            SpinBoxUpdating = false;
        }

        WidgetUI.numOfSnapshotsSB->setMinimum(0);
        WidgetUI.numOfSnapshotsSB->setMaximum(0);
        WidgetUI.numOfSnapshotsSB->setValue(0);
        WidgetUI.numOfSnapshotsSB->setEnabled(false);
    }

    WidgetUI.prevSnapshotPB->setEnabled(first);
    WidgetUI.nextSnapshotPB->setEnabled(last);

}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPODesignerDockingTabsResults::SnapshotsPosesClicked(const QModelIndex& index)
{
    if (!index.isValid()) {
        return;
    }
    if( (DockingSegment == NULL) || (DockingTrajectory == NULL)) return;
    // metod for move on clicked snapshot
    long int p_snap_indx = 0;
    long int indx_curr_snap = 0;

    // extract row form index
    p_snap_indx = index.row() + 1;

    indx_curr_snap = DockingTrajectory->GetCurrentSnapshotIndex() + OffsetOfSnapshots;

    if (indx_curr_snap != p_snap_indx)
    {
        DockingTrajectory->MoveToSnapshot(p_snap_indx + OffsetOfSnapshots);
    }

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPODesignerDockingTabsResults::SliderValueChanged(int pos)
{
    // slider movement
    if( SliderUpdating == true ) return;

    if( (DockingSegment == NULL) || (DockingTrajectory == NULL)) return;

    SliderUpdating = true;
    DockingTrajectory->MoveToSnapshot(pos + OffsetOfSnapshots);
    SliderUpdating = false;
}

//------------------------------------------------------------------------------

void CPODesignerDockingTabsResults::SpinBoxValueChanged(int pos)
{
    // spinbox changes transform to snapshot movement
    if( SpinBoxUpdating == true ) return;

    if( (DockingSegment == NULL) || (DockingTrajectory == NULL)) return;

    SpinBoxUpdating = true;
    DockingTrajectory->MoveToSnapshot(pos + OffsetOfSnapshots);
    SpinBoxUpdating = false;

}

//------------------------------------------------------------------------------

void CPODesignerDockingTabsResults::PrevSnapshot(void)
{
    // prev snapshot
    if( (DockingSegment == NULL) || (DockingTrajectory == NULL)) return;
    DockingTrajectory->PrevSnapshot();

}

//------------------------------------------------------------------------------

void CPODesignerDockingTabsResults::NextSnapshot(void)
{
    // next snapshot
    if( (DockingSegment == NULL) || (DockingTrajectory == NULL)) return;
    DockingTrajectory->NextSnapshot();

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPODesignerDockingTabsResults::SegmentDisconnect(QObject* qobject)
{
    DockingTrajectory = NULL;
    DockingSegment = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
