// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2014 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <ErrorSystem.hpp>
#include <SmallString.hpp>
#include <SmallTime.hpp>
#include <XMLElement.hpp>

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <BatchJob.hpp>
#include <BatchJobList.hpp>
#include <BatchJobScheduler.hpp>
#include <Project.hpp>
#include <ProjectList.hpp>
#include <ContainerModel.hpp>
#include <WorkPanelList.hpp>

#include "BatchJobWorkPanel.moc"
#include "BatchJobWorkPanel.hpp"
#include "StandardWorkPanelsModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* BatchJobWorkPanelCB(void* p_data);

CExtUUID        BatchJobWorkPanelID(
                    "{BATCH_JOB_WORK_PANEL:b2afd501-90ad-4e59-94e3-bd8ad4ce7668}",
                    "Batch Jobs");

CPluginObject   BatchJobWorkPanelObject(&StandardWorkPanelsPlugin,
                    BatchJobWorkPanelID,WORK_PANEL_CAT,
                    ":/images/StandardWorkPanels/BatchJobs.svg",
                    BatchJobWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* BatchJobWorkPanelCB(void* p_data)
{
    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(BatchJobWorkPanelID,NULL);
    if( p_wp == NULL ) {
        p_wp = new CBatchJobWorkPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBatchJobWorkPanel::CBatchJobWorkPanel(void)
    : CWorkPanel(&BatchJobWorkPanelObject,p_project,EWPR_TOOL)
{
    WidgetUI.setupUi(this);

    BatchJobModel = BatchJobList->GetContainerModel(this);
    WidgetUI.batchJobListTV->setModel(SchedulerModel);

    if( BatchJobModel ) {
        for(int i=0; i < BatchJobModel->columnCount(); i++) {
            WidgetUI.batchJobListTV->resizeColumnToContents(i);
        }
    }

    //--------------
    connect(WidgetUI.batchJobListTV,SIGNAL(clicked(const QModelIndex&)),
            this,SLOT(TreeViewClicked(const QModelIndex&)));
    //--------------
    connect(SchedulerModel,SIGNAL(modelReset(void)),
            this,SLOT(ModelReset(void)));
    //--------------
    connect(WidgetUI.releaseTB,SIGNAL(clicked(bool)),
            this,SLOT(ReleaseBatchJob(void)));
    //--------------
    connect(WidgetUI.inspectTB,SIGNAL(clicked(bool)),
            this,SLOT(InspectBatchJob(void)));
    //--------------
    connect(WidgetUI.openTB,SIGNAL(clicked(bool)),
            this,SLOT(OpenBatchJob(void)));

    // set project ------------------------
    Project = p_project;
    ProjectNameChanged(Project);
    BatchJobModel = Project->GetBatchJobs()->GetContainerModel(this);
    WidgetUI.jobListTV->setModel(BatchJobModel);

    if( BatchJobModel != NULL ) {
        connect(BatchJobModel,SIGNAL(modelReset(void)),
                this,SLOT(ModelReset(void)));
        for(int i=0; i < BatchJobModel->columnCount(); i++) {
            WidgetUI.jobListTV->resizeColumnToContents(i);
        }
    }

    ModelReset();
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CBatchJobWorkPanel::~CBatchJobWorkPanel()
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBatchJobWorkPanel::TreeViewClicked(const QModelIndex& index)
{
    BatchJob = NULL;
    if( WidgetUI.tabWidget->currentWidget() == WidgetUI.jobListTV ) {
        if( BatchJobModel != NULL ) BatchJob = dynamic_cast<CBatchJob*>(BatchJobModel->GetItem(index));
    } else {
        BatchJob = dynamic_cast<CBatchJob*>(SchedulerModel->GetItem(index));
    }
    WidgetUI.terminateTB->setEnabled(BatchJob != NULL);
}

//------------------------------------------------------------------------------

void CBatchJobWorkPanel::ModelReset(void)
{
    QString tmp = tr("Number of jobs: %1");
    QString text;
    int     num = 0;

    if( BatchJobModel != NULL ) {
        num = BatchJobModel->rowCount();
    }
    text = tmp.arg(num);
    WidgetUI.jobCountLabel->setText(text);

    num = SchedulerModel->rowCount();
    text = tmp.arg(num);
    WidgetUI.schedulerCountLabel->setText(text);

    TreeViewClicked(QModelIndex());
}

//------------------------------------------------------------------------------

void CBatchJobWorkPanel::ReleaseBatchJob(void)
{
    if( BatchJob == NULL ) return;
    BatchJob->ReleaseBatchJob();
}

//------------------------------------------------------------------------------

void CBatchJobWorkPanel::InspectBatchJob(void)
{
    if( BatchJob == NULL ) return;
    BatchJob->InspectBatchJob();
}

//------------------------------------------------------------------------------

void CBatchJobWorkPanel::OpenBatchJob(void)
{
    if( BatchJob == NULL ) return;
    BatchJob->OpenBatchJob();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

