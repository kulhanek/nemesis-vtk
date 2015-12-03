// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <QtGui>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <vector>

#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <PluginObject.hpp>
#include <Project.hpp>
#include <ProjectList.hpp>
#include <HistoryList.hpp>
#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <Graphics.hpp>
#include <GraphicsView.hpp>
#include <GraphicsView.hpp>
#include <MainWindow.hpp>

#include <ImportJob.hpp>

#include <Structure.hpp>
#include <StructureList.hpp>
#include <TrajectoryList.hpp>
#include <Trajectory.hpp>

#include "PDBQTTrajSegment.hpp"
#include "DockingProject.hpp"
#include "DockingProjectResultsImportTool.hpp"
#include "DockingProjectResultsImportTool.moc"
#include "DockingProjectModule.hpp"
#include "OpenBabelUtils.hpp"

#include <openbabel/obconversion.h>

using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DockingProjectResultsImportToolCB(void* p_data);

CExtUUID        DockingProjectResultsImportToolID(
                    "{DOCKING_PROJECT_RESULTS_IMPORT_TOOL:7bda5a28-c66a-4c9e-b24e-71880651fbd7}",
                    "Results in PDBQT Import",
                    "Import tool using OpenBabel library.");

CPluginObject   DockingProjectResultsImportToolObject(&DockingProjectPlugin,
                    DockingProjectResultsImportToolID,IMPORT_STRUCTURE_CAT,
                    ":/images/OpenBabel/Openbabel.png",
                    QStringList() << "EPF_IMPORT_STRUCTURE",
                    DockingProjectResultsImportToolCB);

// -----------------------------------------------------------------------------

QObject* DockingProjectResultsImportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CDockingProjectResultsImportImportTool requires active project");
        return(NULL);
    }
    CDockingProjectResultsImportTool* p_object = new CDockingProjectResultsImportTool(p_project);
    p_object->ExecuteDialog();
    delete p_object;

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDockingProjectResultsImportTool::CDockingProjectResultsImportTool(CProject* p_project)
    : CProObject(&DockingProjectResultsImportToolObject,NULL,p_project,true)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectResultsImportTool::ExecuteDialog(void)
{
    // parse formats list ------------------------
    // results can be only in pdbqt
    QStringList filters;
    filters << "Autodock PDBQT format (*.pdbqt)";

    // open qfiledialog for file open with filters set correctly
    QFileDialog* p_dialog = new QFileDialog(GetProject()->GetMainWindow());

    // set window header
    if( GetProject()->property("impex.inject") == true ){
        p_dialog->setWindowTitle(tr("Inject PBDQT Results Trajectory Coordinates by OpenBabel"));
    } else {
        p_dialog->setWindowTitle(tr("Import PBDQT Results Trajectory by OpenBabel"));
    }

    // set filter to dialog window
    p_dialog->setNameFilters(filters);
    p_dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(DockingProjectResultsImportToolID)));
    p_dialog->setFileMode(QFileDialog::ExistingFile);
    p_dialog->setAcceptMode(QFileDialog::AcceptOpen);

    if( p_dialog->exec() == QDialog::Accepted ){
        LunchJob(p_dialog->selectedFiles().at(0));
    }

    delete p_dialog;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CDockingProjectResultsImportTool::LunchJob(const QString& file)
{
    GlobalSetup->SetLastOpenFilePathFromFile(file,DockingProjectResultsImportToolID);

    CProject* p_project = GetProject();
    CDockingProject* p_docproj = dynamic_cast<CDockingProject*>(p_project);

    // is project locked?
    if( (GetProject()->GetHistory()->IsLocked(EHCL_TRAJECTORIES)) && (p_docproj == NULL)){
        QMessageBox::information(GetProject()->GetMainWindow(),tr("Error"),
                                   tr("The current project is locked!"),
                                   QMessageBox::Abort, QMessageBox::Abort);
        return (false);
    }

    CTrajectoryList* p_tlist = NULL;

    if( p_docproj != NULL )
    {
        p_tlist =  p_docproj->GetTrajectories();

        if( p_tlist == NULL ) {
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                                  tr("The project does not provide any active trajectory thus the data cannot be imported!"),
                                  QMessageBox::Abort, QMessageBox::Abort);
            return (false);
        }

    } else {

        p_tlist = GetProject()->GetTrajectories();
        if( p_tlist == NULL ) {
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                                  tr("The project does not provide trajectories!"),
                                  QMessageBox::Abort, QMessageBox::Abort);
            return (false);
        }
    }

    CTrajectory* p_traj = p_tlist->GetActiveTrajectory();
    if( p_traj == NULL ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("The project does not have any active trajectory!"),
                              QMessageBox::Abort, QMessageBox::Abort);
        return (false);
    }

    CStructure* p_str = p_traj->GetStructure();
    if( p_str == NULL ){
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("The active trajectory does not have asigned structure!"),
                              QMessageBox::Abort, QMessageBox::Abort);
        return (false);
    }

    // Return only a file name
    QFileInfo file_res(file);
    QString name = file_res.fileName();
    CHistoryNode* p_history = p_traj->BeginChangeWH(EHCL_COMPOSITE,"import PBDQT Results " + name);
   // History = GetProject()->BeginChangeWH(EHCL_COMPOSITE,"import PBDQT Results " + name);
    if( p_history == NULL ) return (false);


    if( p_str->GetAtoms()->GetNumberOfAtoms() == 0 ){
        // structure does not contain any atoms - import initial structure
        if( ImportPDBQTStructure(p_str,file) == false ){
            return  (false);
        }
    }

    History = p_traj->BeginChangeWH(EHCL_TRAJECTORIES,"import PDBQT Trajectory " + name);

     // create segment
    CPDBQTTrajSegment* p_seg = new CPDBQTTrajSegment(p_traj);

    // setup segment
    p_seg->SetFileName(file);

    // register segment and create proper history records
    p_traj->RegisterSegment(p_seg,History);

    // end of trajectory change
    p_traj->EndChangeWH();

    // end of composite history change
    p_traj->EndChangeWH();

    // start data import
    p_seg->LoadTrajectoryData();

    // set first snapshot if necessary
    if( (p_traj->GetNumberOfSnapshots() > 0) && (p_traj->GetCurrentSnapshotIndex() == 0) ){
        p_traj->FirstSnapshot();
    }

    // only for adjust graphic
    CImportJob* p_job = new CImportJob(&DockingProjectResultsImportToolObject,GetProject());
    p_job->SetImportedStructure(p_str,file);
    p_job->AddGraphicModel(true);
    delete p_job;

    return (true);
}

//------------------------------------------------------------------------------

bool CDockingProjectResultsImportTool::ImportPDBQTStructure(CStructure* p_str,const QString& file)
{
    CHistoryNode* p_history_str_node = p_str->BeginChangeWH(EHCL_TOPOLOGY ,"import PDBQT Structure");

    p_str->BeginUpdate(p_history_str_node);

    try {
        std::ifstream   sin;

        sin.open(file.toLatin1());
        if( !sin ) {
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Unable to open file for reading!"),QMessageBox::Ok,QMessageBox::Ok);
            ES_ERROR("cannot open file to read");
            p_str->EndUpdate(true, p_history_str_node);
            return(false);
        }

        // read molecule from file to babel internal
        OBConversion    conv(&sin, &cout);
        OBFormat*       obFormat = conv.FormatFromExt(file.toStdString());
        OBMol           mol;

        if( obFormat == NULL ) {
            ES_ERROR("unsupported format");
            QString message(tr("Failed to set format for %1"));
            message = message.arg(QString(file));
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),message,QMessageBox::Ok,QMessageBox::Ok);
            p_str->EndUpdate(true,p_history_str_node);
            return(false);
        }

        if( ! conv.SetInFormat(obFormat) ) {
            ES_ERROR("Cannot set format");
            QString message(tr("Failed to set format for %1"));
            message = message.arg(QString(file));
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),message,QMessageBox::Ok,QMessageBox::Ok);
            p_str->EndUpdate(true,p_history_str_node);
            return(false);
        }

        if( ! conv.Read(&mol) ) {
            ES_ERROR("Cannot read molecule from file");
            QString message(tr("Failed to read molecule from file %1"));
            message = message.arg(QString(file));
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),message,QMessageBox::Ok,QMessageBox::Ok);
            p_str->EndUpdate(true,p_history_str_node);
            return(false);
        }

        mol.ConnectTheDots();
        mol.PerceiveBondOrders();

        // convert data and save structure to history for UNDO and REDO
        COpenBabelUtils::OpenBabel2Nemesis(mol, p_str, p_history_str_node);

    } catch(...) {
        ES_ERROR("an exception occured");
        QString message(tr("Failed to read molecule from file %1"));
        message = message.arg(QString(file));
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),message,QMessageBox::Ok,QMessageBox::Ok);
        p_str->EndUpdate(true,p_history_str_node);
        return(false);
    }
    // after load data to structure set it to active state
    GetProject()->GetStructures()->SetActiveStructure(p_str,p_history_str_node);

    // update trajectory indexes after load for right function
    p_str->UpdateAtomTrajIndexesWH();

    // we do not need to sort the lists
    p_str->EndUpdate(true,p_history_str_node);
    // topology change end
    p_str->EndChangeWH();
    /// auto fit scene
    if (GetProject()->GetGraphics()->GetPrimaryView()->IsOpenGLCanvasAttached()) {
        GetProject()->GetGraphics()->GetPrimaryView()->FitScene(false);
    }
    // dont end composite change here!

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
