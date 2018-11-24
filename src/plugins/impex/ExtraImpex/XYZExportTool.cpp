// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2018 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <MainWindow.hpp>
#include <QFileDialog>
#include <QMessageBox>

#include "ExtraImpexModule.hpp"

#include "XYZExportTool.hpp"

#include "XYZExportJob.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* XYZExportToolCB(void* p_data);

CExtUUID        XYZExportToolID(
                    "{XYZ_EXPORT_TOOL:cb973697-9ccd-4ca1-b123-849dcb86770f}",
                    "XYZ File (*.xyz)",
                    "Export XYZ file");

CPluginObject   XYZExportToolObject(&ExtraImpexPlugin,
                    XYZExportToolID,EXPORT_STRUCTURE_CAT,
                    XYZExportToolCB);

// -----------------------------------------------------------------------------

QObject* XYZExportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CXYZExportTool requires active project");
        return(NULL);
    }

    CXYZExportTool* p_object = new CXYZExportTool(p_project);
    p_object->ExecuteDialog();
    delete p_object;

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CXYZExportTool::CXYZExportTool(CProject* p_project)
    : CProObject(&XYZExportToolObject,NULL,p_project,true)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CXYZExportTool::ExecuteDialog(void)
{
    // parse formats list ------------------------
    QStringList filters;
    filters << "XYZ Files (*.xyz)";
    filters << "All Files (*)";

    // open qfiledialog for file open with filters set correctly
    QFileDialog* p_dialog = new QFileDialog(GetProject()->GetMainWindow());
    p_dialog->setWindowTitle(XYZExportToolObject.GetDescription());

    p_dialog->setNameFilters(filters);
    p_dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(XYZExportToolID)));
    p_dialog->setFileMode(QFileDialog::AnyFile);
    p_dialog->setAcceptMode(QFileDialog::AcceptSave);
    p_dialog->setDefaultSuffix("xyz");

    if( p_dialog->exec() == QDialog::Accepted ){
        QString file = p_dialog->selectedFiles().at(0);
        QFileInfo finfo(file);
        if( finfo.suffix().isEmpty() ){
            file += ".xyz";
        }
        LaunchJob(file);
    }

    delete p_dialog;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CXYZExportTool::LaunchJob(const QString& file)
{
    GlobalSetup->SetLastOpenFilePathFromFile(file,XYZExportToolID);

    // get active structure to export
    CStructure* p_str = GetProject()->GetActiveStructure();
    if( p_str == NULL ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("There is no molecule in the active project!"),QMessageBox::Ok, QMessageBox::Ok);
        ES_ERROR("No molecule in project");
        return;
    }

    // create job
    CXYZExportJob* p_job = new CXYZExportJob(p_str,file);
    if( p_job->SubmitJob() == false ){
        delete p_job;
    }

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



