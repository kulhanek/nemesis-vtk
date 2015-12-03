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
#include <Graphics.hpp>
#include <GraphicsView.hpp>
#include <QMessageBox>
#include <QVBoxLayout>

#include "GaussianUtils.hpp"
#include "GVibTrajImportTool.hpp"
#include "GVibTrajSegment.hpp"
#include "GaussianModule.hpp"
#include "GVibTrajImportTool.moc"

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
                    GVibTrajImportToolCB);

// -----------------------------------------------------------------------------

QObject* GVibTrajImportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CGVibTrajImportTool requires active project");
        return(NULL);
    }

    QObject* p_build_wp = new CGVibTrajImportTool(p_project);
    return(p_build_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGVibTrajImportTool::CGVibTrajImportTool(CProject* p_project)
    : CWorkPanel(&GVibTrajImportToolObject,p_project,EWPR_DIALOG)
{
    WidgetUI.setupUi(this);

    // set up import and export widgets
    InitInternalDialog();

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CGVibTrajImportTool::~CGVibTrajImportTool()
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGVibTrajImportTool::InitInternalDialog(void)
{
    // parse formats list ------------------------
    QStringList filters;
    filters << "Gaussian Vibrations (*.log)";

    // open qfiledialog for file open with filters set correctly
    Dialog = new QFileDialog();
    Dialog->setNameFilters(filters);
    Dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(GVibTrajImportToolID)));
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

void CGVibTrajImportTool::ReadData(void)
{
    // we must always close work panel
    // since internal file dialog is already closed
    close();

    QString file = Dialog->selectedFiles().at(0);
    GlobalSetup->SetLastOpenFilePathFromFile(file,GVibTrajImportToolID);

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
    p_str->UpdateAtomTrajIndexes();

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
            QMessageBox::critical(this,tr("Error"),tr("Unable to open file for reading!"),QMessageBox::Ok,QMessageBox::Ok);
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
                QMessageBox::critical(this,tr("Error"),tr("Premature end of file!"),QMessageBox::Ok,QMessageBox::Ok);
                ES_ERROR("premature end of file");
                p_str->EndUpdate(true);
                return(false);
            }
            lineno++;

            // did we found frequencies?
            if( line.find(" Harmonic frequencies (cm**-1)") == 0 ) break;

            // is it geometry?
            if( CGaussianUtils::IsGeometry(line) ) {
                if( CGaussianUtils::ReadGeometry(sin,lineno,atoms,true) == false ){
                    QMessageBox::critical(this,tr("Error"),tr("Unable to read any structure from the gaussian output file"),QMessageBox::Ok,QMessageBox::Ok);
                    ES_ERROR("unable to read any structure from the gaussian output file");
                    p_str->EndUpdate(true);
                    return(false);
                }
            }
        }

        // convert last loaded geometry to structure
        if( CGaussianUtils::ConvertToStructure(atoms,p_str) == false ){
            QMessageBox::critical(this,tr("Error"),tr("Unable to convert geometry to Nemesis structure"),QMessageBox::Ok,QMessageBox::Ok);
            ES_ERROR("unable to convert geometry to Nemesis structure");
            p_str->EndUpdate(true);
            return(false);
        }

    } catch(...) {
        ES_ERROR("an exception occured");
        QString message(tr("Failed to read molecule from file %1"));
        message = message.arg(QString(file));
        QMessageBox::critical(this,tr("Error"),message,QMessageBox::Ok,QMessageBox::Ok);
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



