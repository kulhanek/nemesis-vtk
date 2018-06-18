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
#include <Structure.hpp>
#include <MainWindow.hpp>
#include <QFileDialog>
#include <QMessageBox>

#include "ExtraImpexModule.hpp"

#include "NStrExportTool.hpp"

#include "NStrExportJob.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* NStrExportToolCB(void* p_data);

CExtUUID        NStrExportToolID(
                    "{NSTR_EXPORT_TOOL:40ae078b-5b48-48da-90bc-7b88e5986a14}",
                    "Nemesis Structure File (*.str)",
                    "Export Nemesis structure file");

CPluginObject   NStrExportToolObject(&ExtraImpexPlugin,
                    NStrExportToolID,EXPORT_STRUCTURE_CAT,
                    NStrExportToolCB);

// -----------------------------------------------------------------------------

QObject* NStrExportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CNStrExportTool requires active project");
        return(NULL);
    }

    CNStrExportTool* p_object = new CNStrExportTool(p_project);
    p_object->ExecuteDialog();
    delete p_object;

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNStrExportTool::CNStrExportTool(CProject* p_project)
    : CProObject(&NStrExportToolObject,NULL,p_project,true)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CNStrExportTool::ExecuteDialog(void)
{
    // parse formats list ------------------------
    QStringList filters;
    filters << "Nemesis Structure File (*.str)";
    filters << "All files (*)";

    // --------------------------------
    QFileDialog* p_dialog = new QFileDialog(GetProject()->GetMainWindow());

    p_dialog->setWindowTitle(NStrExportToolObject.GetDescription());

    p_dialog->setDirectory (QString(GlobalSetup->GetLastOpenFilePath(NStrExportToolID)));
    p_dialog->setNameFilters(filters);
    p_dialog->setFileMode(QFileDialog::AnyFile);
    p_dialog->setAcceptMode(QFileDialog::AcceptSave);
    p_dialog->setDefaultSuffix("str");

    if( p_dialog->exec() == QDialog::Accepted ){
        LunchJob(p_dialog->selectedFiles().at(0));
    }

    delete p_dialog;
}

//------------------------------------------------------------------------------

void CNStrExportTool::LunchJob(const QString& file)
{
    GlobalSetup->SetLastOpenFilePathFromFile(file,NStrExportToolID);

    // get active structure to export
    CStructure* p_str = GetProject()->GetActiveStructure();
    if( p_str == NULL ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("There is no molecule in the active project!"),QMessageBox::Ok, QMessageBox::Ok);
        ES_ERROR("No molecule in project");
        return;
    }

    // create job
    CNStrExportJob* p_job = new CNStrExportJob(p_str,file);
    if( p_job->SubmitJob() == false ){
        delete p_job;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
