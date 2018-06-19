// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <Job.hpp>
#include <JobList.hpp>
#include <JobScheduler.hpp>
#include <Project.hpp>
#include <ProjectList.hpp>
#include <ContainerModel.hpp>
#include <WorkPanelList.hpp>

#include "JobWorkPanel.hpp"
#include "StandardWPModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* JobWorkPanelCB(void* p_data);

CExtUUID        JobWorkPanelID(
                    "{JOB_WORK_PANEL:69932eda-d93f-4dda-ae40-4695da5dda96}",
                    "Jobs");

CPluginObject   JobWorkPanelObject(&StandardWPPlugin,
                    JobWorkPanelID,WORK_PANEL_CAT,
                    ":/images/StandardWP/Jobs.svg",
                    JobWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* JobWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CJobWorkPanel requires active project");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(JobWorkPanelID,p_project);
    if( p_wp == NULL ) {
        p_wp = new CJobWorkPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CJobWorkPanel::CJobWorkPanel(CProject* p_project)
    : CWorkPanel(&JobWorkPanelObject,p_project,EWPR_TOOL)
{
    WidgetUI.setupUi(this);

    SchedulerModel = JobScheduler->GetContainerModel(this);
    WidgetUI.schedulerListTV->setModel(SchedulerModel);

    if( SchedulerModel ) {
        for(int i=0; i < SchedulerModel->columnCount(); i++) {
            WidgetUI.schedulerListTV->resizeColumnToContents(i);
        }
    }

    connect(Projects,SIGNAL(OnProjectNameChanged(CProject*)),
            this,SLOT(ProjectNameChanged(CProject*)));
    //--------------
    connect(WidgetUI.projectPB,SIGNAL(clicked(bool)),
            this,SLOT(OpenProjectDesigner(void)));
    //--------------
    connect(WidgetUI.jobListTV,SIGNAL(clicked(const QModelIndex&)),
            this,SLOT(TreeViewClicked(const QModelIndex&)));
    connect(WidgetUI.schedulerListTV,SIGNAL(clicked(const QModelIndex&)),
            this,SLOT(TreeViewClicked(const QModelIndex&)));
    //--------------
    connect(SchedulerModel,SIGNAL(modelReset(void)),
            this,SLOT(ModelReset(void)));
    //--------------
    connect(WidgetUI.terminateTB,SIGNAL(clicked(bool)),
            this,SLOT(TerminateJob(void)));

    // set project ------------------------
    Project = p_project;
    ProjectNameChanged(Project);
    JobModel = Project->GetJobs()->GetContainerModel(this);
    WidgetUI.jobListTV->setModel(JobModel);

    if( JobModel != NULL ) {
        connect(JobModel,SIGNAL(modelReset(void)),
                this,SLOT(ModelReset(void)));
        for(int i=0; i < JobModel->columnCount(); i++) {
            WidgetUI.jobListTV->resizeColumnToContents(i);
        }
    }

    ModelReset();
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CJobWorkPanel::~CJobWorkPanel()
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CJobWorkPanel::ProjectNameChanged(CProject* p_project)
{
    if( p_project != Project ) return;
    if( p_project != NULL ) {
        WidgetUI.projectPB->setText(p_project->GetName());
    } else {
        WidgetUI.projectPB->setText("");
    }
}

//------------------------------------------------------------------------------

void CJobWorkPanel::OpenProjectDesigner(void)
{
    if( Project == NULL ) return;
    Project->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CJobWorkPanel::TreeViewClicked(const QModelIndex& index)
{
    Job = NULL;
    if( WidgetUI.tabWidget->currentWidget() == WidgetUI.jobListTV ) {
        if( JobModel != NULL ) Job = dynamic_cast<CJob*>(JobModel->GetItem(index));
    } else {
        Job = dynamic_cast<CJob*>(SchedulerModel->GetItem(index));
    }
    WidgetUI.terminateTB->setEnabled(Job != NULL);
}

//------------------------------------------------------------------------------

void CJobWorkPanel::ModelReset(void)
{
    QString tmp = tr("Number of jobs: %1");
    QString text;
    int     num = 0;

    if( JobModel != NULL ) {
        num = JobModel->rowCount();
    }
    text = tmp.arg(num);
    WidgetUI.jobCountLabel->setText(text);

    num = SchedulerModel->rowCount();
    text = tmp.arg(num);
    WidgetUI.schedulerCountLabel->setText(text);

    TreeViewClicked(QModelIndex());
}

//------------------------------------------------------------------------------

void CJobWorkPanel::TerminateJob(void)
{
    if( Job == NULL ) return;
    Job->TerminateJob();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

