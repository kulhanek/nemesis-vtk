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
#include <Project.hpp>
#include <Structure.hpp>
#include <Trajectory.hpp>
#include <qcustomplot.h>
#include <PhysicalQuantity.hpp>
#include <PhysicalQuantities.hpp>

#include "GaussianModule.hpp"
#include "GOptTrajSegment.hpp"
#include "GOptTrajSegmentDesigner.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GOptTrajSegmentDesignerCB(void* p_data);

CExtUUID        GOptTrajSegmentDesignerID(
                    "{GOPT_TRAJ_SEGMENT_DESIGNER:1a5c5755-2b22-49d0-9c29-df2ac7f038ee}",
                    "Gaussian Geometry Optimization");

CPluginObject   GOptTrajSegmentDesignerObject(&GaussianPlugin,
                    GOptTrajSegmentDesignerID,
                    DESIGNER_CAT,
                    ":/images/Gaussian/GOptTrajSegment.svg",
                    GOptTrajSegmentDesignerCB);

// -----------------------------------------------------------------------------

QObject* GOptTrajSegmentDesignerCB(void* p_data)
{
    CGOptTrajSegment* p_pro_object = static_cast<CGOptTrajSegment*>(p_data);
    QObject* p_object = new CGOptTrajSegmentDesigner(p_pro_object);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGOptTrajSegmentDesigner::CGOptTrajSegmentDesigner(CGOptTrajSegment* p_owner)
    : CProObjectDesigner(&GOptTrajSegmentDesignerObject,p_owner)
{   
    Object = p_owner;
    Plot = NULL;

    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);

    // static items
    WidgetUI.trajectoryLO->setObject(Object->GetTrajectory());
    QFileInfo fileinfo(Object->GetFileName());
    WidgetUI.fileNameLE->setText(fileinfo.fileName());
    WidgetUI.filePathLE->setText(fileinfo.path());
    WidgetUI.methodLE->setText(Object->GetMethod());
    WidgetUI.methodLE->setCursorPosition(0);
    double fenergy = Object->GetFinalEnergy();
    WidgetUI.energyLE->setPhysicalQuantity(PQ_ABSOLUTE_ENERGY);
    WidgetUI.energyLE->setInternalValue(fenergy);

    // models
    WidgetUI.energyTV->setModel(Object->GetEnergyModel());

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
    connect( WidgetUI.energyTV, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(SnapshotClicked(const QModelIndex&)));

    //--------------
    if( Object->GetProject() ){
        connect(Object->GetProject()->GetHistory(), SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
                this, SLOT(ProjectLockChanged(EHistoryChangeMessage)));
    }

    // response to energy unit change
    connect(PQ_ENERGY,SIGNAL(OnUnitChanged(void)),
            this,SLOT(UpdateGraph(void)));

    // init graph
    CreateGraph();

    // init all values ---------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
}

//------------------------------------------------------------------------------

void CGOptTrajSegmentDesigner::CreateGraph(void)
{
    Plot = new QCustomPlot(this);
    Plot->setAutoAddPlottableToLegend(false);
    Plot->setInteraction(QCP::iSelectPlottables,true);
    Plot->setInteraction(QCP::iRangeZoom,true);
    Plot->setInteraction(QCP::iRangeDrag,true);

    connect( Plot, SIGNAL(selectionChangedByUser()),
            this, SLOT(GraphClicked()));

    QCPTextElement* p_title = new QCPTextElement(Plot, "Relative energy");
    Plot->plotLayout()->insertRow(0);
    Plot->plotLayout()->addElement(0, 0, p_title);

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    Plot->xAxis->setTicker(fixedTicker);
    fixedTicker->setTickStep(1.0);

    Plot->xAxis->setLabel("step");
    Plot->yAxis->setLabel("ΔEr [" + PQ_ENERGY->GetUnitName() + "]");

    QCPGraph* p_graph = Plot->addGraph();
    p_graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc));
    p_graph->selectionDecorator()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare),QCPScatterStyle::spAll);
    Object->PopulateGraphData(p_graph);
    Plot->rescaleAxes();

    p_graph->setSelectable(QCP::stSingleData);

    if( WidgetUI.graphTab->layout() ) delete WidgetUI.graphTab->layout();
    QVBoxLayout* p_layout = new QVBoxLayout();
    p_layout->setContentsMargins(0,0,0,0);
    p_layout->addWidget(Plot);
    WidgetUI.graphTab->setLayout(p_layout);
}

//------------------------------------------------------------------------------

void CGOptTrajSegmentDesigner::UpdateGraph(void)
{
    if( Plot == NULL ) return;
    Plot->yAxis->setLabel("ΔEr [" + PQ_ENERGY->GetUnitName() + "]");
    Object->PopulateGraphData(Plot->graph());
    Plot->replot();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGOptTrajSegmentDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGOptTrajSegmentDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_COMPOSITE,Object->GetType().GetName()) == NULL ) return;

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

void CGOptTrajSegmentDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CGOptTrajSegmentDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.seqIndexSB->setValue(Object->GetSeqIndex());

    InitPointerValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGOptTrajSegmentDesigner::InitPointerValues(void)
{
    WidgetUI.numberOfSnapshotsSB->setValue(Object->GetNumberOfSnapshots());
    WidgetUI.currentSnapshotSB->setValue(Object->GetCurrentSnapshotIndex());

    // this might be called during the segment destuction
    // in CYXZTrajSegment::~CYXZTrajSegment
    if( Object->GetTrajectory() ) {
        if( Object->GetTrajectory()->IsSegmentActive(Object) ){
            WidgetUI.isActiveSegmentLA->setText(tr("yes"));
        } else {
            WidgetUI.isActiveSegmentLA->setText(tr("no"));
        }
    }

    long int sidx = Object->GetTrajectory()->GetCurrentSnapshotIndex();
    long int segb = Object->GetBaseSnapshopIndex();
    long int sege = segb + Object->GetNumberOfSnapshots();

    if( (segb < sidx) && (sidx <= sege) ){
        // select snapshot
        QModelIndex idx = Object->GetEnergyModel()->index(Object->GetCurrentSnapshotIndex()-1,0);
        if( idx.isValid() ){
            WidgetUI.energyTV->selectionModel()->select(idx,QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        }
        // select graph
        QCPDataRange dr(Object->GetCurrentSnapshotIndex()-1,Object->GetCurrentSnapshotIndex());
        QCPDataSelection ds(dr);
        Plot->graph()->setSelection(ds);
    } else {
        // clear selection
        WidgetUI.energyTV->selectionModel()->clearSelection();
        QCPDataSelection ds;
        Plot->graph()->setSelection(ds);
        }
    Plot->replot();
}

//------------------------------------------------------------------------------

void CGOptTrajSegmentDesigner::ApplyValues(void)
{
    Object->SetSeqIndexWH(WidgetUI.seqIndexSB->value());
}

//------------------------------------------------------------------------------

void CGOptTrajSegmentDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
}

//------------------------------------------------------------------------------

void CGOptTrajSegmentDesigner::SnapshotClicked(const QModelIndex& index)
{
    long int idx = Object->GetBaseSnapshopIndex() + index.row() + 1;
    Object->GetTrajectory()->MoveToSnapshot(idx);
}

//------------------------------------------------------------------------------

void CGOptTrajSegmentDesigner::GraphClicked(void)
{
    QCPGraph* p_graph = Plot->graph();
    QCPDataSelection sd = p_graph->selection();

    int selected_id = -1;
    foreach (QCPDataRange dataRange, sd.dataRanges()) {
       selected_id = dataRange.begin() + 1;     // this should done only once
    }
    if( selected_id == -1 ) return;

    long int idx = Object->GetBaseSnapshopIndex() + selected_id;
    Object->GetTrajectory()->MoveToSnapshot(idx);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGOptTrajSegmentDesigner::keyPressEvent(QKeyEvent *event)
{
    if( WidgetUI.tabWidget->currentWidget() != WidgetUI.graphTab ){
        CProObjectDesigner::keyPressEvent(event);
        return;
    }

    if( event->key() == Qt::Key_Escape ){
        Plot->rescaleAxes();
        Plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        Plot->replot();
    }
    CProObjectDesigner::keyPressEvent(event);
    return;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================










