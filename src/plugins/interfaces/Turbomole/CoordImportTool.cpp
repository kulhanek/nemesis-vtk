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

#include <Structure.hpp>
#include <Project.hpp>
#include <QMessageBox>
#include <QVBoxLayout>

#include "TurbomoleModule.hpp"
#include "CoordImportTool.hpp"
#include "CoordImportJob.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CoordImportToolCB(void* p_data);

CExtUUID        CoordImportToolID(
                    "{COORD_IMPORT_TOOL:237924cf-9209-444e-8a0c-ee61f220ea87}",
                    "Turbomole coord");

CPluginObject   CoordImportToolObject(&TurbomolePlugin,
                    CoordImportToolID,IMPORT_STRUCTURE_CAT,
                    QStringList() << "EPF_ALLOW_IMP_TO_EMPTY_STR",
                    CoordImportToolCB);

// -----------------------------------------------------------------------------

QObject* CoordImportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CCoordImportTool requires active project");
        return(NULL);
    }

    QObject* p_build_wp = new CCoordImportTool(p_project);
    return(p_build_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCoordImportTool::CCoordImportTool(CProject* p_project)
    : CWorkPanel(&CoordImportToolObject,p_project,EWPR_DIALOG)
{
    WidgetUI.setupUi(this);

    // set up import and export widgets
    InitInternalDialog();

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CCoordImportTool::~CCoordImportTool()
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCoordImportTool::InitInternalDialog(void)
{
    // parse formats list ------------------------
    QStringList filters;
    filters << "Turbomole coordinates (coord)";
    filters << "All files (*)";

    // open qfiledialog for file open with filters set correctly
    Dialog = new QFileDialog();
    Dialog->setNameFilters(filters);
    Dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(CoordImportToolID)));
    Dialog->setFileMode(QFileDialog::ExistingFile);
    Dialog->setAcceptMode(QFileDialog::AcceptOpen);

    QVBoxLayout* p_layout = new QVBoxLayout;
    p_layout->addWidget(Dialog);
    setLayout(p_layout);

    connect(Dialog, SIGNAL(rejected()), this, SLOT(close()));
    connect(Dialog, SIGNAL(accepted()), this, SLOT(LunchJob()));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCoordImportTool::LunchJob(void)
{
    // we must always close work panel
    // since internal file dialog is already closed
    close();

    QString file = Dialog->selectedFiles().at(0);
    GlobalSetup->SetLastOpenFilePathFromFile(file,CoordImportToolID);

    // do we have structure?
    CStructure* p_str =  GetProject()->GetActiveStructure();
    if( p_str == NULL ) {
        QMessageBox::critical(this,tr("Error"),
                              tr("The project does not provide any active structure thus the data cannot be imported!"),
                              QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    // is project locked?
    if( GetProject()->IsFlagSet<EProjecFlag>(EPF_ALLOW_IMP_TO_EMPTY_STR) == false ){
        if( GetProject()->GetHistory()->IsLocked(EHCL_TOPOLOGY) ){
            QMessageBox::information(this,tr("Error"),
                                       tr("The current project is locked!"),
                                       QMessageBox::Abort, QMessageBox::Abort);
            return;
        }
    } else {
        if( p_str->IsEmpty() == false ){
            if( GetProject()->GetHistory()->IsLocked(EHCL_TOPOLOGY) ){
                QMessageBox::information(this,tr("Error"),
                                           tr("The active structure is not empty and project is locked!"),
                                           QMessageBox::Abort, QMessageBox::Abort);
                return;
            }
        }
    }

    // create job and sutup job
    CCoordImportJob* p_job = new CCoordImportJob(GetProject());
    p_job->SetImportedStructure(p_str,file);

    // submit job
    if( p_job->SubmitJob() == false ){
        delete p_job;
    }

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



