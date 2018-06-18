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
#include <QMessageBox>

#include <Structure.hpp>
#include <Project.hpp>
#include <QFileDialog>
#include <MainWindow.hpp>

#include "AmberModule.hpp"

#include "TopologyImportTool.hpp"
#include "TopologyImportTool.moc"

#include "TopologyImportJob.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* TopologyImportToolCB(void* p_data);

CExtUUID        TopologyImportToolID(
                    "{TOPOLOGY_IMPORT_TOOL:066140ae-4d97-433c-8e22-3e4c02f7282b}",
                    "Topology (*.parm7)",
                    "Amber Topology");

CPluginObject   TopologyImportToolObject(&AmberPlugin,
                    TopologyImportToolID,IMPORT_STRUCTURE_CAT,
                    QStringList() << "EPF_IMPORT_STRUCTURE",
                    TopologyImportToolCB);

// -----------------------------------------------------------------------------

QObject* TopologyImportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CTopologyImportTool requires active project");
        return(NULL);
    }

    CTopologyImportTool* p_object = new CTopologyImportTool(p_project);
    p_object->ExecuteDialog();
    delete p_object;

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopologyImportTool::CTopologyImportTool(CProject* p_project)
    : CProObject(&TopologyImportToolObject,NULL,p_project,true)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTopologyImportTool::ExecuteDialog(void)
{
    // parse formats list ------------------------
    QStringList filters;
    filters << "Topology files (*.parm7 *.top)";
    filters << "All files (*)";

    // construct dialog
    QFileDialog* p_dialog = new QFileDialog(GetProject()->GetMainWindow());
    p_dialog->setNameFilters(filters);
    p_dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(TopologyImportToolID)));
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

void CTopologyImportTool::LunchJob(const QString& file)
{
    GlobalSetup->SetLastOpenFilePathFromFile(file,TopologyImportToolID);

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

    // create job
    CTopologyImportJob* p_job = new CTopologyImportJob(GetProject());
    p_job->SetImportedStructure(p_str,file);
    if( p_job->SubmitJob() == false ){
        delete p_job;
    }

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



