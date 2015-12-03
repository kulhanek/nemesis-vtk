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
#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>

#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <MainWindow.hpp>

#include "DockingProject.hpp"
#include "DockingProjectModule.hpp"
#include "DockingProjectLigandExportTool.hpp"
#include "DockingProjectLigandExportTool.moc"

#include "DockingProjectLigandExportJob.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DockingProjectLigandExportToolCB(void* p_data);

CExtUUID        DockingProjectLigandExportToolID(
                    "{DOCKING_PROJECT_LIGAND_EXPORT_TOOL:af8f9bfe-4c54-4e43-878b-d383c84bf024}",
                    "Docking Ligand file (*.pdbqt)",
                    "Export PDBQT Ligand file");

CPluginObject   DockingProjectLigandExportToolObject(&DockingProjectPlugin,
                    DockingProjectLigandExportToolID,EXPORT_STRUCTURE_CAT,
                    DockingProjectLigandExportToolCB);

// -----------------------------------------------------------------------------

QObject* DockingProjectLigandExportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CDockingProjectLigandExportTool requires active project");
        return(NULL);
    }

    CDockingProjectLigandExportTool* p_object = new CDockingProjectLigandExportTool(p_project);
    p_object->ExecuteDialog();
    delete p_object;

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDockingProjectLigandExportTool::CDockingProjectLigandExportTool(CProject* p_project)
    : CProObject(&DockingProjectLigandExportToolObject,NULL,p_project,true)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectLigandExportTool::ExecuteDialog(void)
{
    CDockingProject* p_docproj = dynamic_cast<CDockingProject*>(GetProject());
    // parse formats list ------------------------
    QStringList filters;
    filters << "PDBQT File (*.pdbqt)";
    filters << "All files (*.*)";

    // --------------------------------
    QFileDialog* p_dialog = new QFileDialog(GetProject()->GetMainWindow());

    p_dialog->setWindowTitle(DockingProjectLigandExportToolObject.GetDescription());

    p_dialog->setDirectory (QString(GlobalSetup->GetLastOpenFilePath(DockingProjectLigandExportToolID)));
    p_dialog->setNameFilters(filters);
    p_dialog->setFileMode(QFileDialog::AnyFile);
    p_dialog->setAcceptMode(QFileDialog::AcceptSave);
    if( p_docproj != NULL ){
        // Return only a file base name without extension
        QFileInfo file( p_docproj->GetLigandFileName() );
        QString name = file.baseName();
        p_dialog->selectFile(name);
    }
    p_dialog->setDefaultSuffix("pdbqt");

    // setting checkbox on bottom of QFileDiqlox p_dialoq --------------------------------
    // settings label
    QLabel* p_settings_label = new QLabel(p_dialog);
    p_settings_label->setText("Settings:");

    // add label to horizontal layout
    QHBoxLayout *layout_horizontal = new QHBoxLayout;
    layout_horizontal->addWidget(p_settings_label);

    // flexible
    QCheckBox* p_checkbox_flexible = new QCheckBox(p_dialog);
    p_checkbox_flexible->setCheckState(Qt::Checked);
    p_checkbox_flexible->setText(tr("Flexible"));
    p_checkbox_flexible->setToolTip(tr("Uncheck to output as a rigid molecule\n(i.e. no branches or torsion tree)") );

    // combine
    QCheckBox* p_checkbox_combine = new QCheckBox(p_dialog);
    p_checkbox_combine->setCheckState(Qt::Unchecked);
    p_checkbox_combine->setText(tr("Combine"));
    p_checkbox_combine->setToolTip(tr("Combine separate molecular pieces of input into a single rigid molecule\n"\
                                      "(requires \"not Flexible\" option or will have no effect)") );

    // renumber
    QCheckBox* p_checkbox_renumber = new QCheckBox(p_dialog);
    p_checkbox_renumber->setCheckState(Qt::Checked);
    p_checkbox_renumber->setText(tr("Renumber"));
    p_checkbox_renumber->setToolTip(tr("Renumber atoms sequentially\n(uncheck is to preserve atom indices from input file)") );

    // add  3 checkboxs to horizontal layout
    QHBoxLayout *layout_horizontal_2 = new QHBoxLayout;
    layout_horizontal_2->addWidget(p_checkbox_flexible);
    layout_horizontal_2->addWidget(p_checkbox_combine);
    layout_horizontal_2->addWidget(p_checkbox_renumber);
    // add stretch to left-alignment
    layout_horizontal_2->addStretch();

    // set horizontal layouts into QFileDialog layout
    QGridLayout *layout = (QGridLayout*)p_dialog->layout();
    // under labels for files
    layout->addLayout(layout_horizontal,4,0);
    // under QTextEdit for filenames
    layout->addLayout(layout_horizontal_2,4,1);

    if( p_dialog->exec() == QDialog::Accepted ){
        bool flexible, combine, renumber;
        if (p_checkbox_flexible->checkState() == Qt::Checked) {
            flexible = true;
        } else {
            flexible = false;
        }
        if (p_checkbox_combine->checkState() == Qt::Checked) {
            combine = true;
        } else {
            combine = false;
        }
        if (p_checkbox_renumber->checkState() == Qt::Checked) {
            renumber = true;
        } else {
            renumber = false;
        }
        LunchJob(p_dialog->selectedFiles().at(0), flexible, combine, renumber);
    }

    delete p_dialog;
}

//------------------------------------------------------------------------------

void CDockingProjectLigandExportTool::LunchJob(const QString& file, bool flexible_state, bool combine_state, bool renumber_state)
{
    GlobalSetup->SetLastOpenFilePathFromFile(file,DockingProjectLigandExportToolID);

    // initialize topology change
    CDockingProject* p_docproj = dynamic_cast<CDockingProject*>(GetProject());
    CStructure* p_str = NULL;
    if( p_docproj != NULL ){
        p_str = p_docproj->GetLigandStructure();
    } else {
        p_str = GetProject()->GetActiveStructure();
    }
    if( p_str == NULL ) {
        QMessageBox::critical(GetProject()->GetMainWindow(),tr("Error"),tr("There is no molecule in the active project!"),QMessageBox::Ok, QMessageBox::Ok);
        ES_ERROR("No molecule in project");
        return;
    }

    // create job
    CDockingProjectLigandExportJob* p_job = new CDockingProjectLigandExportJob(p_str,file,flexible_state,combine_state,renumber_state);
    if( p_job->SubmitJob() == false ){
        delete p_job;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
