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
#include <MainWindow.hpp>
#include <QFileDialog>
#include <QMessageBox>

#include <Structure.hpp>
#include <Project.hpp>

#include "ExtraImpexModule.hpp"

#include "Mol2ImportTool.hpp"
#include "Mol2ImportTool.moc"

#include "Mol2ImportJob.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* Mol2ImportToolCB(void* p_data);

CExtUUID        Mol2ImportToolID(
                    "{MOL2_IMPORT_TOOL:5123167c-992e-4279-b4bf-0213366d8cfe}",
                    "Tripos Mol2 File (*.mol2)",
                    "Import Tripos Mol2 file");

CPluginObject   Mol2ImportToolObject(&ExtraImpexPlugin,
                    Mol2ImportToolID,IMPORT_STRUCTURE_CAT,
                    QStringList() << "EPF_IMPORT_STRUCTURE",
                    Mol2ImportToolCB);

// -----------------------------------------------------------------------------

QObject* Mol2ImportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CMol2ImportTool requires active project");
        return(NULL);
    }

    CMol2ImportTool* p_object = new CMol2ImportTool(p_project);
    p_object->ExecuteDialog();
    delete p_object;

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMol2ImportTool::CMol2ImportTool(CProject* p_project)
    : CProObject(&Mol2ImportToolObject,NULL,p_project,true)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMol2ImportTool::ExecuteDialog(void)
{
    // parse formats list ------------------------
    QStringList filters;
    filters << "Tripos Mol2 Files (*.mol2)";
    filters << "All files (*)";

    // open qfiledialog for file open with filters set correctly
    QFileDialog* p_dialog = new QFileDialog(GetProject()->GetMainWindow());

    p_dialog->setNameFilters(filters);
    p_dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(Mol2ImportToolID)));
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

void CMol2ImportTool::LunchJob(const QString& file)
{
    GlobalSetup->SetLastOpenFilePathFromFile(file,Mol2ImportToolID);

    // do we have structure?
    CStructure* p_str =  GetProject()->GetActiveStructure();
    if( p_str == NULL ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("The project does not provide any active structure thus the data cannot be imported!"),
                              QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    // is project locked?
    if( GetProject()->IsFlagSet<EProjecFlag>(EPF_ALLOW_IMP_TO_EMPTY_STR) == false ){
        if( GetProject()->GetHistory()->IsLocked(EHCL_TOPOLOGY) ){
            QMessageBox::information(GetProject()->GetMainWindow(),tr("Error"),
                                       tr("The current project is locked!"),
                                       QMessageBox::Abort, QMessageBox::Abort);
            return;
        }
    } else {
        if( p_str->IsEmpty() == false ){
            if( GetProject()->GetHistory()->IsLocked(EHCL_TOPOLOGY) ){
                QMessageBox::information(GetProject()->GetMainWindow(),tr("Error"),
                                           tr("The active structure is not empty and project is locked!"),
                                           QMessageBox::Abort, QMessageBox::Abort);
                return;
            }
        }
    }

    // create job and sutup job
    CMol2ImportJob* p_job = new CMol2ImportJob(GetProject());
    p_job->SetImportedStructure(p_str,file);

    // submit job
    if( p_job->SubmitJob() == false ){
        delete p_job;
    }

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



