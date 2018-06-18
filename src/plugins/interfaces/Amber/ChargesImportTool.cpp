// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2014 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include "AmberModule.hpp"

#include "ChargesImportTool.hpp"

#include "ChargesImportJob.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* ChargesImportToolCB(void* p_data);

CExtUUID        ChargesImportToolID(
                    "{CHARGES_IMPORT_TOOL:77412bd8-5006-4786-bc61-b3db045cc8fa}",
                    "Point Charges (*qout *QOUT)");

CPluginObject   ChargesImportToolObject(&AmberPlugin,
                    ChargesImportToolID,IMPORT_PROPERTY_CAT,
                    QStringList(),
                    ChargesImportToolCB);

// -----------------------------------------------------------------------------

QObject* ChargesImportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CChargesImportTool requires active project");
        return(NULL);
    }

    CChargesImportTool* p_dialog = new CChargesImportTool(p_project);
    p_dialog->ExecuteDialog();
    delete p_dialog;

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CChargesImportTool::CChargesImportTool(CProject* p_project)
    : CProObject(&ChargesImportToolObject,NULL,p_project,true)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CChargesImportTool::ExecuteDialog(void)
{
    // set formats
    QStringList filters;
    filters << "Point charges (*qout *QOUT)";
    filters << "All files (*)";

    // set file dialog
    QFileDialog* p_dialog = new QFileDialog(GetProject()->GetMainWindow());
    p_dialog->setNameFilters(filters);
    p_dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(ChargesImportToolID)));
    p_dialog->setFileMode(QFileDialog::ExistingFile);

    if( p_dialog->exec() == QDialog::Accepted ) {
        LunchJob(p_dialog->selectedFiles().at(0));
    }

    delete p_dialog;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CChargesImportTool::LunchJob(const QString& file)
{
    GlobalSetup->SetLastOpenFilePathFromFile(file,ChargesImportToolID);

    // do we have structure?
    CStructure* p_str =  GetProject()->GetActiveStructure();
    if( p_str == NULL ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("The project does not provide any active structure thus the data cannot be imported!"),
                              QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    // is structure empty?
    if( p_str->IsEmpty() == true ){
        QMessageBox::information(GetProject()->GetMainWindow(),tr("Error"),
                                   tr("The active structure is empty!"),
                                   QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    // is project locked?
    if( GetProject()->GetHistory()->IsLocked(EHCL_TOPOLOGY) ){
        QMessageBox::information(GetProject()->GetMainWindow(),tr("Error"),
                                   tr("The current project is locked!"),
                                   QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    // create job
    CChargesImportJob* p_job = new CChargesImportJob(GetProject());
    p_job->SetImportedStructure(p_str,file);
    if( p_job->SubmitJob() == false ){
        delete p_job;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



