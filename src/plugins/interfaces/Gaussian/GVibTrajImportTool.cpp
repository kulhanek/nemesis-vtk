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
#include <fstream>
#include <QMessageBox>
#include <QFileDialog>
#include <Graphics.hpp>
#include <GraphicsView.hpp>
#include <MainWindow.hpp>

#include "GaussianUtils.hpp"
#include "GVibTrajImportTool.hpp"
#include "GVibTrajSegment.hpp"
#include "GaussianModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GVibTrajImportToolCB(void* p_data);

CExtUUID        GVibTrajImportToolID(
                    "{GVIB_TRAJ_IMPORT_TOOL:db41ebd2-4e2f-4ef6-9a83-06138fc034a1}",
                    "Vibrations File (*.log)",
                    "Import Gaussian vibrations");

CPluginObject   GVibTrajImportToolObject(&GaussianPlugin,
                    GVibTrajImportToolID,IMPORT_TRAJECTORY_CAT,
                    QStringList() << "FORMAT=gvib",
                    GVibTrajImportToolCB);

// -----------------------------------------------------------------------------

QObject* GVibTrajImportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CGVibTrajImportTool requires active project");
        return(NULL);
    }

    CGVibTrajImportTool* p_object = new CGVibTrajImportTool(p_project);
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

CGVibTrajImportTool::CGVibTrajImportTool(CProject* p_project)
    : CImportTrajectory(&GVibTrajImportToolObject,p_project)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGVibTrajImportTool::ExecuteDialog(void)
{
    // parse formats list ------------------------
    QStringList filters;
    filters << "Gaussian Vibrations (*.log)";

    // open qfiledialog for file open with filters set correctly
    QFileDialog* p_dialog = new QFileDialog();
    p_dialog->setNameFilters(filters);
    p_dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(GVibTrajImportToolID)));
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

void CGVibTrajImportTool::LaunchJob(const QString& file)
{
    // does file exist?
    if( QFile::exists(file) == false ){
        QMessageBox::information(GetProject()->GetMainWindow(),tr("Error"),
                                   tr("The file does not exist!"),
                                   QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    GlobalSetup->SetLastOpenFilePathFromFile(file,GVibTrajImportToolID);

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
                              tr("The active trajectory does not have assigned structure!"),
                              QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    if( p_str->GetAtoms()->GetNumberOfAtoms() == 0 ){
        // structure does not contain any atoms - import initial structure
        if( ImportLastStructure(p_str,file) == false ){
            return;
        }
    }

    // update trajectory indexes
    p_str->SetTrajIndexes();

    CHistoryNode* p_history = p_traj->BeginChangeWH(EHCL_TRAJECTORIES,"import Gaussian vibrations");
    if( p_history == NULL ) return;

    // create segment
    CTrajectorySegment* p_seg = new CGVibTrajSegment(p_traj);

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

//------------------------------------------------------------------------------

bool CGVibTrajImportTool::ImportLastStructure(CStructure* p_str,const QString& file)
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

        std::string line;

        for(;;){
            getline(sin,line);
            if( ! sin ){
                QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Premature end of file!"),QMessageBox::Ok,QMessageBox::Ok);
                ES_ERROR("premature end of file");
                p_str->EndUpdate(true);
                return(false);
            }
            lineno++;

            // did we found frequencies?
            if( line.find(" Harmonic frequencies (cm**-1)") == 0 ) break;

            // is it geometry?
            if( CGaussianUtils::IsGeometry(line) ) {
                if( CGaussianUtils::ReadGeometry(sin,lineno,atoms) == false ){
                    QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Unable to read any structure from the gaussian output file"),QMessageBox::Ok,QMessageBox::Ok);
                    ES_ERROR("unable to read any structure from the gaussian output file");
                    p_str->EndUpdate(true);
                    return(false);
                }
            }
        }

        // convert last loaded geometry to structure
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



