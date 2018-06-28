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

#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <Project.hpp>
#include <HistoryList.hpp>
#include <TrajectoryList.hpp>
#include <Trajectory.hpp>
#include <Structure.hpp>
#include <AtomList.hpp>
#include <QVBoxLayout>
#include <QMessageBox>

#include "TVibTrajImportTool.hpp"
#include "TVibTrajSegment.hpp"
#include "TurbomoleModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* TVibTrajImportToolCB(void* p_data);

CExtUUID        TVibTrajImportToolID(
                    "{TVIB_TRAJ_IMPORT_TOOL:e83bd4cf-df84-4504-b533-84126c915304}",
                    "Turbomole Vibrations");

CPluginObject   TVibTrajImportToolObject(&TurbomolePlugin,
                    TVibTrajImportToolID,IMPORT_TRAJECTORY_CAT,
                    TVibTrajImportToolCB);

// -----------------------------------------------------------------------------

QObject* TVibTrajImportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CTVibTrajImportTool requires active project");
        return(NULL);
    }

    QObject* p_build_wp = new CTVibTrajImportTool(p_project);
    return(p_build_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTVibTrajImportTool::CTVibTrajImportTool(CProject* p_project)
    : CWorkPanel(&TVibTrajImportToolObject,p_project,EWPR_DIALOG)
{
    WidgetUI.setupUi(this);

    // set up import and export widgets
    InitInternalDialog();

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CTVibTrajImportTool::~CTVibTrajImportTool()
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTVibTrajImportTool::InitInternalDialog(void)
{
    // parse formats list ------------------------
    QStringList filters;
    filters << "Turbomole Vibrations (control)";

    // open qfiledialog for file open with filters set correctly
    Dialog = new QFileDialog();
    Dialog->setNameFilters(filters);
    Dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(TVibTrajImportToolID)));
    Dialog->setFileMode(QFileDialog::ExistingFile);
    Dialog->setAcceptMode(QFileDialog::AcceptOpen);

    QVBoxLayout* p_layout = new QVBoxLayout;
    p_layout->addWidget(Dialog);
    setLayout(p_layout);

    connect(Dialog, SIGNAL(rejected()), this, SLOT(close()));
    connect(Dialog, SIGNAL(accepted()), this, SLOT(ReadData()));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTVibTrajImportTool::ReadData(void)
{
    // we must always close work panel
    // since internal file dialog is already closed
    close();

    QString file = Dialog->selectedFiles().at(0);
    GlobalSetup->SetLastOpenFilePathFromFile(file,TVibTrajImportToolID);

    // is project locked?
    if( GetProject()->GetHistory()->IsLocked(EHCL_TRAJECTORIES) ){
        QMessageBox::information(this,tr("Error"),
                                   tr("The current project is locked!"),
                                   QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    CTrajectoryList* p_tlist = GetProject()->GetTrajectories();
    if( p_tlist == NULL ) {
        QMessageBox::critical(this,tr("Error"),
                              tr("The project does not provide trajectories!"),
                              QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    CTrajectory* p_traj = p_tlist->GetActiveTrajectory();
    if( p_traj == NULL ) {
        QMessageBox::critical(this,tr("Error"),
                              tr("The project does not have any active trajectory!"),
                              QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    CStructure* p_str = p_traj->GetStructure();
    if( p_str == NULL ){
        QMessageBox::critical(this,tr("Error"),
                              tr("The active trajectory does not have asigned structure!"),
                              QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    if( p_str->GetAtoms()->GetNumberOfAtoms() == 0 ){
        QMessageBox::critical(this,tr("Error"),
                              tr("The active trajectory structure does not contain any atom!"),
                              QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    // update trajectory indexes
    p_str->SetTrajIndexes();

    CHistoryNode* p_history = p_traj->BeginChangeWH(EHCL_TRAJECTORIES,"import Turbomole vibrations");
    if( p_history == NULL ) return;

    // create segment
    CTrajectorySegment* p_seg = new CTVibTrajSegment(p_traj);

    // setup segment
    p_seg->SetFileName(file);

    // register segment and create proper history records
    p_traj->RegisterSegment(p_seg,p_history);

    // switch trajectory play mode to loop
    p_traj->SetPlayModeWH(ETPM_LOOP);

    // end of change
    p_traj->EndChangeWH();

    // start data import
    p_seg->LoadTrajectoryData();

    // set first snapshot if necessary
    if( (p_traj->GetNumberOfSnapshots() > 0) && (p_traj->GetCurrentSnapshotIndex() == 0) ){
        p_traj->FirstSnapshot();
    }
    // and finaly open object designer
    p_seg->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



