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
#include <OpenBabelUtils.hpp>
#include <Graphics.hpp>
#include <GraphicsView.hpp>
#include <QFileDialog>
#include <MainWindow.hpp>
#include <QMessageBox>

#include "XYZTrajImportTool.hpp"
#include "XYZTrajSegment.hpp"
#include "ExtraImpexModule.hpp"

#include "openbabel/mol.h"
#include "openbabel/obconversion.h"

//------------------------------------------------------------------------------
using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* XYZTrajImportToolCB(void* p_data);

CExtUUID        XYZTrajImportToolID(
                    "{XYZ_TRAJ_IMPORT_TOOL:1a01e876-f6b4-4d71-a9a6-582272d0232a}",
                    "XYZ Trajectory File (*.xyz)",
                    "Import XYZ trajectory");

CPluginObject   XYZTrajImportToolObject(&ExtraImpexPlugin,
                    XYZTrajImportToolID,IMPORT_TRAJECTORY_CAT,
                    QStringList() << "FORMAT=xyz",
                    XYZTrajImportToolCB);

// -----------------------------------------------------------------------------

QObject* XYZTrajImportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CXYZTrajImportTool requires active project");
        return(NULL);
    }

    CXYZTrajImportTool* p_object = new CXYZTrajImportTool(p_project);
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

CXYZTrajImportTool::CXYZTrajImportTool(CProject* p_project)
    : CImportTrajectory(&XYZTrajImportToolObject,p_project)
{
}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CXYZTrajImportTool::ExecuteDialog(void)
{
    // parse formats list ------------------------
    QStringList filters;
    filters << "XYZ Trajectory Files (*.xyz)";
    filters << "All Files (*)";

    // open qfiledialog for file open with filters set correctly
    QFileDialog* p_dialog = new QFileDialog(GetProject()->GetMainWindow());

    p_dialog->setWindowTitle(XYZTrajImportToolObject.GetDescription());

    p_dialog->setNameFilters(filters);
    p_dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(XYZTrajImportToolID)));
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

void CXYZTrajImportTool::LaunchJob(const QString& file)
{
    // does file exist?
    if( QFile::exists(file) == false ){
        QMessageBox::information(GetProject()->GetMainWindow(),tr("Error"),
                                   tr("The file does not exist!"),
                                   QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    GlobalSetup->SetLastOpenFilePathFromFile(file,XYZTrajImportToolID);

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

    if( p_str->GetAtoms()->GetNumberOfAtoms() == 0 ){
        // structure does not contain any atoms - import initial structure
        if( ImportXYZStructure(p_str,file) == false ){
            return;
        }
    }

    // update trajectory indexes
    p_str->SetTrajIndexes();

    CHistoryNode* p_history = p_traj->BeginChangeWH(EHCL_TRAJECTORIES,"import XYZ trajectory");
    if( p_history == NULL ) return;

    // create segment
    CTrajectorySegment* p_seg = new CXYZTrajSegment(p_traj);

    // setup segment
    p_seg->SetFileName(file);

    // register segment and create proper history records
    p_traj->RegisterSegment(p_seg,p_history);

    // end of change
    p_traj->EndChangeWH();

    // start data import
    p_seg->LoadTrajectoryData();

    // set first snapshot if necessary
    if( (p_traj->GetNumberOfSnapshots() > 0) && (p_traj->GetCurrentSnapshotIndex() == 0) ){
        p_traj->FirstSnapshot();
    }
}

//------------------------------------------------------------------------------

bool CXYZTrajImportTool::ImportXYZStructure(CStructure* p_str,const QString& file)
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

        // read molecule from file to babel internal
        OBConversion    conv(&sin, &cout);
        OBFormat*       obFormat = conv.FormatFromExt(file.toStdString());
        OBMol           mol;

        if( obFormat == NULL ) {
            ES_ERROR("unsupported format");
            QString message(tr("Failed to set format for %1"));
            message = message.arg(QString(file));
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),message,QMessageBox::Ok,QMessageBox::Ok);
            p_str->EndUpdate(true);
            return(false);
        }

        if( ! conv.SetInFormat(obFormat) ) {
            ES_ERROR("Cannot set format");
            QString message(tr("Failed to set format for %1"));
            message = message.arg(QString(file));
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),message,QMessageBox::Ok,QMessageBox::Ok);
            p_str->EndUpdate(true);
            return(false);
        }

        if( ! conv.Read(&mol) ) {
            ES_ERROR("Cannot read molecule from file");
            QString message(tr("Failed to read molecule from file %1"));
            message = message.arg(QString(file));
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),message,QMessageBox::Ok,QMessageBox::Ok);
            p_str->EndUpdate(true);
            return(false);
        }

        mol.ConnectTheDots();
        mol.PerceiveBondOrders();

        // convert data
        COpenBabelUtils::OpenBabel2Nemesis(mol,p_str);
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



