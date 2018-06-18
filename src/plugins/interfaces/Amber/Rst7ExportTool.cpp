// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QMessageBox>
#include <MainWindow.hpp>

#include "AmberModule.hpp"

#include "Rst7ExportTool.hpp"
#include "Rst7ExportJob.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* Rst7ExportToolCB(void* p_data);

CExtUUID        Rst7ExportToolID(
                    "{RST7_EXPORT_TOOL:f96ffc6e-9d12-4d6f-aff8-d68a368553fe}",
                    "Amber Restart (*.rst7)");

CPluginObject   Rst7ExportToolObject(&AmberPlugin,
                    Rst7ExportToolID,EXPORT_STRUCTURE_CAT,
                    Rst7ExportToolCB);

// -----------------------------------------------------------------------------

QObject* Rst7ExportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CRst7ExportTool requires active project");
        return(NULL);
    }

    CRst7ExportTool* p_object = new CRst7ExportTool(p_project);
    p_object->ExecuteDialog();
    delete p_object;

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRst7ExportTool::CRst7ExportTool(CProject* p_project)
    : CProObject(&Rst7ExportToolObject,NULL,p_project,true)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRst7ExportTool::ExecuteDialog(void)
{
    // parse formats list ------------------------
    QStringList filters;
    filters << "Amber restart (*.rst7)";
    filters << "All files (*)";

    // --------------------------------
    QFileDialog* p_dialog = new QFileDialog(GetProject()->GetMainWindow());

    p_dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(Rst7ExportToolID)));
    p_dialog->setNameFilters(filters);
    p_dialog->setFileMode(QFileDialog::AnyFile);
    p_dialog->setAcceptMode(QFileDialog::AcceptSave);
    p_dialog->setDefaultSuffix("rst7");

    if( p_dialog->exec() == QDialog::Accepted ){
        LunchJob(p_dialog->selectedFiles().at(0));
    }

    delete p_dialog;
}

//------------------------------------------------------------------------------

void CRst7ExportTool::LunchJob(const QString& file)
{
    GlobalSetup->SetLastOpenFilePathFromFile(file,Rst7ExportToolID);

    // get active structure to export
    CStructure* p_str = GetProject()->GetActiveStructure();
    if( p_str == NULL ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("There is no molecule in the active project!"),
                              QMessageBox::Ok, QMessageBox::Ok);
        ES_ERROR("No molecule in project");
        return;
    }

    // create job
    CRst7ExportJob* p_job = new CRst7ExportJob(p_str,file);
    if( p_job->SubmitJob() == false ){
        delete p_job;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
