// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <Structure.hpp>
#include <MainWindow.hpp>
#include <QFileDialog>
#include <QMessageBox>

#include "ExtraImpexModule.hpp"

#include "Mol2ExportTool.hpp"

#include "Mol2ExportJob.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* Mol2ExportToolCB(void* p_data);

CExtUUID        Mol2ExportToolID(
                    "{MOL2_EXPORT_TOOL:db1a5e79-8b76-4370-bcdf-9c53f2dd78ed}",
                    "Tripos Mol2 File (*.mol2)",
                    "Export Tripos Mol2 file");

CPluginObject   Mol2ExportToolObject(&ExtraImpexPlugin,
                    Mol2ExportToolID,EXPORT_STRUCTURE_CAT,
                    Mol2ExportToolCB);

// -----------------------------------------------------------------------------

QObject* Mol2ExportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CMol2ExportTool requires active project");
        return(NULL);
    }

    CMol2ExportTool* p_object = new CMol2ExportTool(p_project);
    p_object->ExecuteDialog();
    delete p_object;

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMol2ExportTool::CMol2ExportTool(CProject* p_project)
    : CProObject(&Mol2ExportToolObject,NULL,p_project,true)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMol2ExportTool::ExecuteDialog(void)
{
    // parse formats list ------------------------
    QStringList filters;
    filters << "Tripos Mol2 File (*.mol2)";
    filters << "All files (*)";

    // --------------------------------
    QFileDialog* p_dialog = new QFileDialog(GetProject()->GetMainWindow());

    p_dialog->setWindowTitle(Mol2ExportToolObject.GetDescription());

    p_dialog->setDirectory (QString(GlobalSetup->GetLastOpenFilePath(Mol2ExportToolID)));
    p_dialog->setNameFilters(filters);
    p_dialog->setFileMode(QFileDialog::AnyFile);
    p_dialog->setAcceptMode(QFileDialog::AcceptSave);
    p_dialog->setDefaultSuffix("mol2");

    if( p_dialog->exec() == QDialog::Accepted ){
        LunchJob(p_dialog->selectedFiles().at(0));
    }

    delete p_dialog;
}

//------------------------------------------------------------------------------

void CMol2ExportTool::LunchJob(const QString& file)
{
    GlobalSetup->SetLastOpenFilePathFromFile(file,Mol2ExportToolID);

    // get active structure to export
    CStructure* p_str = GetProject()->GetActiveStructure();
    if( p_str == NULL ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("There is no molecule in the active project!"),QMessageBox::Ok, QMessageBox::Ok);
        ES_ERROR("No molecule in project");
        return;
    }

    // create job
    CMol2ExportJob* p_job = new CMol2ExportJob(p_str,file);
    if( p_job->SubmitJob() == false ){
        delete p_job;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
