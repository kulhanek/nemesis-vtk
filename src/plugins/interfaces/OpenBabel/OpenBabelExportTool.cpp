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

#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <BondList.hpp>
#include <MainWindow.hpp>
#include <QMessageBox>
#include <QFileDialog>
#include "OpenBabelUtils.hpp"

#include "OpenBabelModule.hpp"
#include "OpenBabelExportTool.hpp"

#include <iostream>
#include <vector>
#include <stdio.h>

#include "openbabel/mol.h"
#include "openbabel/obconversion.h"

using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* OpenBabelExportToolCB(void* p_data);

CExtUUID    OpenBabelExportToolID(
                    "{OPEN_BABEL_EXPORT_TOOL:33ff5076-ea4c-4d04-9c44-92c6b90a6172}",
                    "OpenBabel",
                    "OpenBabel export");

CPluginObject   OpenBabelExportToolObject(&OpenBabelPlugin,
                    OpenBabelExportToolID,EXPORT_STRUCTURE_CAT,
                    ":/images/OpenBabel/Openbabel.png",
                    OpenBabelExportToolCB);

// -----------------------------------------------------------------------------

QObject* OpenBabelExportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("COpenBabelExportTool requires active project");
        return(NULL);
    }

    COpenBabelExportTool* p_object = new COpenBabelExportTool(p_project);
    p_object->ExecuteDialog();
    delete p_object;

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

COpenBabelExportTool::COpenBabelExportTool(CProject* p_project)
    : CProObject(&OpenBabelExportToolObject,NULL,p_project,true)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void COpenBabelExportTool::ExecuteDialog(void)
{
    OpenBabel::OBConversion     co;
    std::vector< std::string>   formats = co.GetSupportedOutputFormat ();

    // parse formats list ------------------------
    QStringList filters;
    // Extract format from mouse click in Menu->File/export structure as... /OpenBabel/ THIS
    QVariant choosenFormatOpenBabelExport(GetProject()->property("impex.format"));

    // Send format to dialog
    filters << choosenFormatOpenBabelExport.toString();
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

    // --------------------------------
    QFileDialog* p_dialog = new QFileDialog(GetProject()->GetMainWindow());
    p_dialog->setDirectory (QString(GlobalSetup->GetLastOpenFilePath(OpenBabelExportToolID)));
    p_dialog->setNameFilters(filters);
    p_dialog->setFileMode(QFileDialog::AnyFile);
    p_dialog->setAcceptMode(QFileDialog::AcceptSave);

    if( p_dialog->exec() == QDialog::Accepted ){
        LaunchJob(p_dialog->selectedFiles().at(0),p_dialog->selectedNameFilter().left((p_dialog->selectedNameFilter().indexOf("--"))-1));
    }

    delete p_dialog;
}

//------------------------------------------------------------------------------

void COpenBabelExportTool::LaunchJob(const QString& file,const QString& ext)
{
    QString myfile = file;

    GlobalSetup->SetLastOpenFilePath(myfile,OpenBabelExportToolID);

    CStructure* p_str = GetProject()->GetActiveStructure();
    if( p_str == NULL ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("There is no molecule in active project!"),QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    if( p_str->GetAtoms()->HasInvalidAtoms() ){
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("Structure contains invalid atoms!"),QMessageBox::Ok,QMessageBox::Ok);
        return;
    }

    if( p_str->GetBonds()->HasInvalidBonds() ){
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("Structure contains invalid bonds!"),QMessageBox::Ok,QMessageBox::Ok);
        return;
    }

    ofstream ofs;
    OpenBabel::OBConversion conv(&cin, &ofs);
    OpenBabel::OBFormat*    obFormat = conv.FormatFromExt(myfile.toLatin1().constData());
    if(obFormat == NULL) {
        // if there is wrong or no extension - set it from filter
        myfile = myfile + "." + ext;
        if( ! conv.SetOutFormat(ext.toLatin1()) ) {
            QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                                  tr("Unable to set export format!"),QMessageBox::Ok,QMessageBox::Ok);
            return;
        }
    }

    if (!conv.SetOutFormat(obFormat)) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("Unable to set export format!"),QMessageBox::Ok,QMessageBox::Ok);
        return;
    }

    // open myfile
    ofs.open(myfile.toLatin1().constData());
    if( !ofs ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("Unable to open file for writing!"),QMessageBox::Ok,QMessageBox::Ok);
        return;
    }

    OBMol mol;

    // convert data
    COpenBabelUtils::Nemesis2OpenBabel(p_str,mol);

    // write data
    if( ! conv.Write(&mol) ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),
                              tr("Unable to write file!"),QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
}

//-----------------------------------------------------------------------------
