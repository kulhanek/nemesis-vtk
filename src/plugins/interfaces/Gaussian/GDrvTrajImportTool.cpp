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
#include <QString>
#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <Project.hpp>
#include <HistoryList.hpp>
#include <TrajectoryList.hpp>
#include <Trajectory.hpp>
#include <Structure.hpp>
#include <AtomList.hpp>
#include <Graphics.hpp>
#include <GraphicsView.hpp>
#include <fstream>
#include <QMessageBox>
#include <QFileDialog>
#include <MainWindow.hpp>
#include <BondList.hpp>

#include "GaussianUtils.hpp"
#include "GaussianModule.hpp"
#include "GDrvTrajImportTool.hpp"
#include "GDrvTrajSegment.hpp"

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GDrvTrajImportToolCB(void* p_data);

CExtUUID        GDrvTrajImportToolID(
                    "{GDRV_TRAJ_IMPORT_TOOL:85b168fd-3aad-4bc7-ac01-271d2d5a7ff0}",
                    "Geometry 1D Driving File (*.log)",
                    "Import Gaussian single coordinate driving");

CPluginObject   GDrvTrajImportToolObject(&GaussianPlugin,
                    GDrvTrajImportToolID,IMPORT_TRAJECTORY_CAT,
                    QStringList() << "FORMAT=gdrv",
                    GDrvTrajImportToolCB);

// -----------------------------------------------------------------------------

QObject* GDrvTrajImportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CGDrvTrajImportTool requires active project");
        return(NULL);
    }


    CGDrvTrajImportTool* p_object = new CGDrvTrajImportTool(p_project);
    if( p_project->property("impex.direct") == false ){
        p_object->ExecuteDialog();
        delete p_object;
        return(NULL);
    } else {
        return(p_object);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGDrvTrajImportTool::CGDrvTrajImportTool(CProject* p_project)
    : CImportTrajectory(&GDrvTrajImportToolObject,p_project)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGDrvTrajImportTool::ExecuteDialog(void)
{
    // parse formats list ------------------------
    QStringList filters;
    filters << "Geometry Optimization Files (*.log)";
    filters << "All Files (*)";

    // open qfiledialog for file open with filters set correctly
    QFileDialog* p_dialog = new QFileDialog(GetProject()->GetMainWindow());
    p_dialog->setNameFilters(filters);
    p_dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath()));
    p_dialog->setFileMode(QFileDialog::ExistingFile);
    p_dialog->setAcceptMode(QFileDialog::AcceptOpen);

    if( p_dialog->exec() == QDialog::Accepted ){
        LaunchJob(p_dialog->selectedFiles().at(0));
    }

    delete p_dialog;

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGDrvTrajImportTool::LaunchJob(const QString& file)
{
    // does file exist?
    if( QFile::exists(file) == false ){
        QMessageBox::information(GetProject()->GetMainWindow(),tr("Error"),
                                   tr("The file does not exist!"),
                                   QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    GlobalSetup->SetLastOpenFilePathFromFile(file,GDrvTrajImportToolID);

    // is project locked?
    if( GetProject()->GetHistory()->IsLocked(EHCL_TRAJECTORIES) ){
        QMessageBox::information(GetProject()->GetMainWindow(),tr("Error"),
                                   tr("The current project is locked!"),
                                   QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    CTrajectoryList* p_tlist = GetProject()->GetTrajectories();
    if( p_tlist == NULL ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("The project does not provide trajectories!"),
                              QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    CTrajectory* p_traj = p_tlist->GetActiveTrajectory();
    if( p_traj == NULL ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("The project does not have any active trajectory!"),
                              QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    CStructure* p_str = p_traj->GetStructure();
    if( p_str == NULL ){
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("The active trajectory does not have asigned structure!"),
                              QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    bool imported_str = false;

    if( p_str->GetAtoms()->GetNumberOfAtoms() == 0 ){
        // structure does not contain any atoms - import initial structure
        if( ImportFirstStructure(p_str,file) == false ){
            return;
        }
        imported_str = true;
    }

    // update trajectory indexes
    p_str->SetTrajIndexes();

    CHistoryNode* p_history = p_traj->BeginChangeWH(EHCL_TRAJECTORIES,"import Gaussian geometry optimization trajectory");
    if( p_history == NULL ) return;

    // create segment
    CGDrvTrajSegment* p_seg = new CGDrvTrajSegment(p_traj);

    // setup segment
    p_seg->SetFileName(file);

    // register segment and create proper history records
    p_traj->RegisterSegment(p_seg,p_history);

    // end of change
    p_traj->EndChangeWH();

    // start data import
    p_seg->LoadTrajectoryData();

    // set last snapshot if necessary
    if( (p_traj->GetNumberOfSnapshots() > 0) && (p_traj->GetCurrentSnapshotIndex() == 0) ){
        p_traj->LastSnapshot();
        if( imported_str ){
            // recreate  bonds for the final structure
            p_str->GetBonds()->RecreateBonds();
        }
    }

    // create viasual apparance of CV
    p_seg->CreateGeoProperty();

    // and finaly open object designer
    p_seg->OpenObjectDesigner();

    // does file exist?
    if( CGaussianUtils::IsNormalTermination(file) == false ){
        int result = QMessageBox::warning(GetProject()->GetMainWindow(),tr("Warning"),
                                   tr("Normal termination of Gaussian not detected! Would you like to inspect the file?"),
                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if( result == QMessageBox::Yes ){
            QUrl url( "file://" + file);
            QDesktopServices::openUrl(url);
        }
    }
}

//------------------------------------------------------------------------------

bool CGDrvTrajImportTool::ImportFirstStructure(CStructure* p_str,const QString& file)
{
    p_str->BeginUpdate();

    try {

        std::ifstream   sin;

        sin.open(file.toLatin1());
        if( !sin ) {
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Unable to open file for reading!"),QMessageBox::Ok,QMessageBox::Ok);
            ES_ERROR("cannot open file to read");
            p_str->EndUpdate(true);
            return(false);
        }

        // read the first snapshot
        std::vector<CGAtom> atoms;
        int                 lineno = 0;

        // find geometry
        if( CGaussianUtils::FindGeometry(sin,lineno) == false ){
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Unable to read any structure from the gaussian output file"),QMessageBox::Ok,QMessageBox::Ok);
            ES_ERROR("unable to read any structure from the gaussian output file");
            p_str->EndUpdate(true);
            return(false);
        }

        if( CGaussianUtils::ReadGeometry(sin,lineno,atoms) == false ){
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Unable to read any structure from the gaussian output file"),QMessageBox::Ok,QMessageBox::Ok);
            ES_ERROR("unable to read any structure from the gaussian output file");
            p_str->EndUpdate(true);
            return(false);
        }

        // convert geometry to structure
        if( CGaussianUtils::ConvertToStructure(atoms,p_str) == false ){
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Unable to convert geometry to Nemesis structure"),QMessageBox::Ok,QMessageBox::Ok);
            ES_ERROR("unable to convert geometry to Nemesis structure");
            p_str->EndUpdate(true);
            return(false);
        }

    } catch(...) {
        ES_ERROR("an exception occured");
        QString message(tr("Failed to read molecule from file %1"));
        message = message.arg(QString(file));
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),message,QMessageBox::Ok,QMessageBox::Ok);
        p_str->EndUpdate(true);
        return(false);
    }

    // we do not need to sort the lists
    p_str->EndUpdate(true);

    /// auto fit scene
    GetProject()->GetGraphics()->GetPrimaryView()->FitScene(false);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



