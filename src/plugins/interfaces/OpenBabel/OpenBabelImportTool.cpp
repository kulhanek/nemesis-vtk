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

#include "OpenBabelModule.hpp"
#include "OpenBabelImportTool.hpp"
#include "OpenBabelUtils.hpp"
#include "OpenBabelImportJob.hpp"

#include <openbabel/obconversion.h>

#include <vector>

using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* OpenBabelImportToolCB(void* p_data);

CExtUUID        OpenBabelImportToolID(
                    "{OPEN_BABEL_IMPORT_TOOL:aca235a5-4e68-47a3-afab-1d2f75b60760}",
                    "OpenBabel",
                    "OpenBabel import structure & inject coordinates");

CPluginObject   OpenBabelImportToolObject(&OpenBabelPlugin,
                    OpenBabelImportToolID,IMPORT_STRUCTURE_CAT,
                    ":/images/OpenBabel/Openbabel.png",
                    QStringList() << "EPF_IMPORT_STRUCTURE",
                    OpenBabelImportToolCB);

// -----------------------------------------------------------------------------

QObject* OpenBabelImportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("COpenBabelImportTool requires active project");
        return(NULL);
    }

    COpenBabelImportTool* p_object = new COpenBabelImportTool(p_project);
    p_object->ExecuteDialog();
    delete p_object;

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

COpenBabelImportTool::COpenBabelImportTool(CProject* p_project)
    : CProObject(&OpenBabelImportToolObject,NULL,p_project,true)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void COpenBabelImportTool::ExecuteDialog(void)
{
    // get list of supported babel formats -------
    OBConversion co;
    vector<string> formats = co.GetSupportedInputFormat();

    // parse formats list ------------------------
    QStringList filters;
    // Extract format from mouse click in Menu->File/import structure from... /OpenBabel/ THIS
    QVariant choosenFormatOpenBabelImport(GetProject()->property("impex.format"));

    // Send format to dialog
    filters << choosenFormatOpenBabelImport.toString();
    filters << "Common formats (*.pdb *.xyz)";

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

    if( GetProject()->property("impex.inject") == true ){
        p_dialog->setWindowTitle(tr("Inject Coordinates by OpenBabel"));
    } else {
        p_dialog->setWindowTitle(tr("Import Structure by OpenBabel"));
    }

    p_dialog->setNameFilters(filters);
    p_dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(OpenBabelImportToolID)));
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

void COpenBabelImportTool::LunchJob(const QString& file)
{
    GlobalSetup->SetLastOpenFilePathFromFile(file,OpenBabelImportToolID);

    // is project locked?
    if( GetProject()->GetHistory()->IsLocked(EHCL_TOPOLOGY) ){
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
    COpenBabelImportJob* p_job = new COpenBabelImportJob(GetProject());
    p_job->SetImportedStructure(p_str,file);

    // submit job
    if( p_job->SubmitJob() == false ){
        delete p_job;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



