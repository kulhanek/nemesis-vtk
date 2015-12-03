// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <Structure.hpp>
#include <Project.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <MainWindow.hpp>

#include "DockingProject.hpp"
#include "DockingProjectReceptorImportJob.hpp"
#include "DockingProjectReceptorImportTool.hpp"
#include "DockingProjectReceptorImportTool.moc"
#include "DockingProjectModule.hpp"
#include "OpenBabelUtils.hpp"

#include <openbabel/obconversion.h>

#include <vector>

using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DockingProjectReceptorImportToolCB(void* p_data);

CExtUUID        DockingProjectReceptorImportToolID(
                    "{DOCKING_PROJECT_RECEPTOR_IMPORT_TOOL:e7db12dc-ac60-4b3f-a712-32c1db10153f}",
                    "Receptor Import",
                    "Import tool using OpenBabel library.");

CPluginObject   DockingProjectReceptorImportToolObject(&DockingProjectPlugin,
                    DockingProjectReceptorImportToolID,IMPORT_STRUCTURE_CAT,
                    ":/images/OpenBabel/impex/import.svg",
                    QStringList() << "EPF_IMPORT_STRUCTURE",
                    DockingProjectReceptorImportToolCB);

// -----------------------------------------------------------------------------

QObject* DockingProjectReceptorImportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CDockingProjectReceptorImportTool requires active project");
        return(NULL);
    }
    CDockingProjectReceptorImportTool* p_object = new CDockingProjectReceptorImportTool(p_project);
    p_object->ExecuteDialog();
    delete p_object;

    return(NULL);

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDockingProjectReceptorImportTool::CDockingProjectReceptorImportTool(CProject* p_project)
    : CProObject(&DockingProjectReceptorImportToolObject,NULL,p_project,true)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectReceptorImportTool::ExecuteDialog(void)
{
    // get list of supported babel formats -------
    OBConversion co;
    vector<string> formats = co.GetSupportedInputFormat();

    // parse formats list ------------------------
    QStringList filters;
    //    filters << "Common formats (*.pdb *.xyz)";
    //    filters << "Autodock PDBQT format (*.pdbqt)";
    filters << "Common formats & Autodock PDBQT format (*.pdb *.xyz *.pdbqt)";
    filters << "All files (*.*)";

    // generate filters for all of supported openbabel formats

    for(unsigned int i = 0; i < formats.size(); i++) {
        CSmallString formatString, extension;
        formatString = formats.at(i).c_str();
        int pos = formatString.FindSubString("--");
        if( pos != -1 ) {
            extension = formatString.GetSubString(0, pos-1);
            formatString << " (*." << extension << ")";
            filters << formatString.GetBuffer();
        }
    }
    // open qfiledialog for file open with filters set correctly
    QFileDialog* p_dialog = new QFileDialog(GetProject()->GetMainWindow());

    // set window header
    if( GetProject()->property("impex.inject") == true ){
        p_dialog->setWindowTitle(tr("Inject Receptor Coordinates by OpenBabel"));
    } else {
        p_dialog->setWindowTitle(tr("Import Receptor Structure by OpenBabel"));
    }

    p_dialog->setNameFilters(filters);
    p_dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(DockingProjectReceptorImportToolID)));
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

void CDockingProjectReceptorImportTool::LunchJob(const QString& file)
{

    GlobalSetup->SetLastOpenFilePathFromFile(file,DockingProjectReceptorImportToolID);

    CProject* p_project = GetProject();
    CDockingProject* p_docproj = dynamic_cast<CDockingProject*>(p_project);

    if( p_docproj != NULL )
    {
        CStructure* p_str =  p_docproj->GetReceptorStructure();

        if( p_str == NULL ) {
          QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
             tr("The project does not provide any active structure thus the data cannot be imported!"),
             QMessageBox::Abort, QMessageBox::Abort);
             return;
         }
         // create job and setup job
              CDockingProjectReceptorImportJob* p_job = new CDockingProjectReceptorImportJob(p_docproj);
              p_job->SetImportedStructure(p_str,file);

         // submit job
              if( p_job->SubmitJob() == false ){
                delete p_job;
              }
    } else {
        // if project isn't DockingProject so is project locked?
        if( GetProject()->GetHistory()->IsLocked(EHCL_TOPOLOGY) || GetProject()->GetHistory()->IsLocked(EHCL_TOPOLOGY)
            || GetProject()->GetHistory()->IsLocked(EHCL_STRUCTURES))
        {
            QMessageBox::information(GetProject()->GetMainWindow(),tr("Error"),
                                  tr("The current project is locked!"),
                                  QMessageBox::Abort, QMessageBox::Abort);
            return;
        }
        CStructure* p_str =  GetProject()->GetActiveStructure();
        if( p_str == NULL ) {
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("The project does not provide any active structure thus the data cannot be imported!"),
                              QMessageBox::Abort, QMessageBox::Abort);
            return;
        }
        // create job and setup job
         CDockingProjectReceptorImportJob* p_job = new CDockingProjectReceptorImportJob(GetProject());
         p_job->SetImportedStructure(p_str,file);

        // submit job
        if( p_job->SubmitJob() == false ){
            delete p_job;
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



