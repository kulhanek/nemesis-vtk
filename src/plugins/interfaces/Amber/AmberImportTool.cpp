// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>

#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <ProjectList.hpp>
#include <HistoryList.hpp>
#include <QFileDialog>
#include <QMessageBox>

#include <Project.hpp>
#include <Structure.hpp>

#include <AmberTopology.hpp>
#include <AmberRestart.hpp>

#include "AmberImportJob.hpp"

#include "AmberModule.hpp"
#include "AmberImportTool.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* AmberImportToolCB(void* p_data);

CExtUUID        AmberImportToolID(
                    "{AMBER_IMPORT_OBJECT:35be4b76-9f1d-402c-81da-e5c6506c9046}",
                    "Topology and Restart (*.parm7 *.rst7)",
                    "Amber Topology and Restart");

CPluginObject   AmberImportToolObject(&AmberPlugin,
                    AmberImportToolID,IMPORT_STRUCTURE_CAT,
                    QStringList() << "EPF_IMPORT_STRUCTURE",
                    AmberImportToolCB);

// -----------------------------------------------------------------------------

QObject* AmberImportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CAmberImportTool requires active project");
        return(NULL);
    }

    CAmberImportTool* p_wp = new CAmberImportTool(p_project);
    p_wp->ShowAsDialog();
    delete p_wp;

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAmberImportTool::CAmberImportTool(CProject* p_project)
    : CWorkPanel(&AmberImportToolObject,p_project,EWPR_DIALOG)
{
    WidgetUI.setupUi(this);

    // connect slots
    connect(WidgetUI.loadTopologyTB, SIGNAL(clicked()),
            this, SLOT(LoadTopology()));
    connect(WidgetUI.deleteTopologyTB, SIGNAL(clicked()),
            this, SLOT(DeleteTopology()));

    connect(WidgetUI.loadCoordinatesTB, SIGNAL(clicked()),
            this, SLOT(LoadCoordinates()));
    connect(WidgetUI.deleteCoordinatesTB, SIGNAL(clicked()),
            this, SLOT(DeleteCoordinates()));

    connect(WidgetUI.buttonLoadAll, SIGNAL(clicked()),
            this,SLOT(LoadAllData()));

    ImportJob = new CAmberImportJob(GetProject());

    DeleteTopology();

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CAmberImportTool::~CAmberImportTool()
{
    // destroy import job if it was not used
    if( ImportJob ) delete ImportJob;
    ImportJob = NULL;
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CAmberImportTool::LoadTopology(void)
{
    QString filename = QFileDialog::getOpenFileName(NULL,
                       tr("Load topology"),
                       (const char*)GlobalSetup->GetLastOpenFilePath(AmberImportToolID),
                       tr("Topology file (*.top *.parm *.parm7);;All files (*)"));

    if( filename == NULL ) return; // no file was selected

    CFileName file_name(filename.toLatin1().constData());

    // update last open path
    GlobalSetup->SetLastOpenFilePath(file_name.GetFileDirectory(),AmberImportToolID);

    // load topology
    if( ImportJob->Topology.Load(file_name) == false) {
        ES_ERROR("unable to load topology");
        QMessageBox::critical(NULL, tr("Load topology"),
                              tr("An error occurred during topology opening!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        DeleteTopology();
        return;
    }

    QString num;
    num.setNum(ImportJob->Topology.AtomList.GetNumberOfAtoms());
    WidgetUI.numberOfAtomLE->setText(num);
    WidgetUI.groupBoxLoad2->setEnabled(true);
    WidgetUI.lineEdit_1->setText((const char*)file_name);
}

//------------------------------------------------------------------------------

void CAmberImportTool::DeleteTopology(void)
{
    WidgetUI.groupBoxLoad2->setEnabled(false);
    WidgetUI.numberOfAtomLE->setText("0");
    WidgetUI.lineEdit_1->setText("");
    WidgetUI.lineEdit_2->setText("");
    WidgetUI.buttonLoadAll->setEnabled(false);
    ImportJob->Topology.Clean();
}

//------------------------------------------------------------------------------

void CAmberImportTool::LoadCoordinates(void)
{
    QString qfilename = QFileDialog::getOpenFileName(NULL,
                        tr("Load restart coordinates"),
                        (const char*)GlobalSetup->GetLastOpenFilePath(AmberImportToolID),
                        tr("Restart file (*.crd *.rst *.rst7);;All files (*)"));

    if( qfilename == NULL ) return; // no file was selected

    CFileName file_name(qfilename.toLatin1().constData());

    // update last open path
    GlobalSetup->SetLastOpenFilePath(file_name.GetFileDirectory(),AmberImportToolID);

    // load restart
    ImportJob->Restart.AssignTopology(&ImportJob->Topology);
    if(ImportJob->Restart.Load(file_name) == false) {
        ES_ERROR("unable to load coordinates");
        QMessageBox::critical(NULL, tr("Load coordinate"),
                              tr("An error occurred during coordinate file opening!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        DeleteCoordinates();
        return;
    }
    WidgetUI.buttonLoadAll->setEnabled(true);
    WidgetUI.lineEdit_2->setText((const char*)file_name);
}

//------------------------------------------------------------------------------

void CAmberImportTool::DeleteCoordinates(void)
{
    WidgetUI.lineEdit_2->setText("");
    WidgetUI.buttonLoadAll->setEnabled(false);
}

//------------------------------------------------------------------------------

void CAmberImportTool::LoadAllData(void)
{   
    // is project locked?
    if( GetProject()->GetHistory()->IsLocked(EHCL_TOPOLOGY) ){
        QMessageBox::information(this,tr("Error"),
                                   tr("The current project is locked!"),
                                   QMessageBox::Abort | QMessageBox::Abort);
    }

    CStructure* p_str = GetProject()->GetActiveStructure();
    if( p_str == NULL ) {
        ES_ERROR("No molecule active in the project");
        QMessageBox::critical(this,tr("Error"),
                              tr("The project does not provide any active molecule thus the data cannot be imported!"),
                              QMessageBox::Abort | QMessageBox::Abort);
        return;
    }

    // close dialog
    close();

    // setup import job and lunch it
    ImportJob->SetImportedStructure(p_str,"");

    if( ImportJob->SubmitJob() == false ){
        delete ImportJob;
    }
    ImportJob = NULL;
}

//------------------------------------------------------------------------------


