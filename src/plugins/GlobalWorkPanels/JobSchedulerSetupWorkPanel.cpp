// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <Job.hpp>
#include <JobScheduler.hpp>
#include <Project.hpp>
#include <ContainerModel.hpp>

#include "JobSchedulerSetupWorkPanel.moc"
#include "JobSchedulerSetupWorkPanel.hpp"
#include "GlobalWorkPanelsModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* JobSchedulerSetupWorkPanelCB(void* p_data);

CExtUUID        JobSchedulerSetupWorkPanelID(
                    "{JOB_SCHEDULER_SETUP:ae11c09c-1b53-4ac7-826e-2a19a3471681}",
                    "Job Scheduler Setup");

CPluginObject   JobSchedulerSetupWorkPanelObject(&GlobalWorkPanelsPlugin,
                    JobSchedulerSetupWorkPanelID,WORK_PANEL_CAT,
                    ":images/GlobalWorkPanels/JobSchedulerSetup.svg",
                    JobSchedulerSetupWorkPanelCB);

// -----------------------------------------------------------------------------

CJobSchedulerSetupWorkPanel* p_s7_window = NULL;

// -----------------------------------------------------------------------------

QObject* JobSchedulerSetupWorkPanelCB(void* p_data)
{
    if( p_s7_window != NULL ) return(p_s7_window);
    p_s7_window = new CJobSchedulerSetupWorkPanel;
    return(p_s7_window);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CJobSchedulerSetupWorkPanel::CJobSchedulerSetupWorkPanel(void)
    : CWorkPanel(&JobSchedulerSetupWorkPanelObject,NULL,EWPR_WINDOW)
{
    WidgetUI.setupUi(this);
    SchedulerModel = JobScheduler->GetContainerModel(this);
    if( SchedulerModel ) {
        WidgetUI.JobListTV->setModel(SchedulerModel);
    }

    // load job scheduler setup
    WidgetUI.MaxRunJobSB->setValue(JobScheduler->GetMaxNumberOfRunningJobs());

    // connect slots
    connect(WidgetUI.TerminateBU,SIGNAL(clicked(bool)),
            this,SLOT(TerminateJobs(void)));
    //--------------
    connect(WidgetUI.ButtonBox, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CJobSchedulerSetupWorkPanel::~CJobSchedulerSetupWorkPanel()
{
    SaveWorkPanelSetup();
    p_s7_window = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CJobSchedulerSetupWorkPanel::TerminateJobs(void)
{
    QModelIndexList selected_rows = WidgetUI.JobListTV->selectionModel()->selectedRows();
    for(int i=0; i < selected_rows.count(); i++){
        CJob* p_job = dynamic_cast<CJob*>(SchedulerModel->GetItem(selected_rows.at(i)));
        if( p_job != NULL ){
            // FIXME - possible race condition
            p_job->TerminateJob();
        }
    }
}

//------------------------------------------------------------------------------

void CJobSchedulerSetupWorkPanel::ButtonBoxClicked(QAbstractButton *button)
{
    if(button == NULL) return;

    // apply changes -------------
    if(WidgetUI.ButtonBox->standardButton(button) == QDialogButtonBox::Apply) {
        JobScheduler->SetMaxNumberOfRunningJobs(WidgetUI.MaxRunJobSB->value());
        return;
    }
    // save changes -------------
    if(WidgetUI.ButtonBox->standardButton(button) == QDialogButtonBox::Save) {
        JobScheduler->SetMaxNumberOfRunningJobs(WidgetUI.MaxRunJobSB->value());
        JobScheduler->SaveUserConfig();
        return;
    }
    // close changes -------------
    if(WidgetUI.ButtonBox->standardButton(button) == QDialogButtonBox::Close) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

