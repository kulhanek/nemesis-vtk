// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

#include <vector>
#include <string>
#include <utility>
#include <fstream>

#include <QMessageBox>
#include <QtGui>
#include <QFileInfo>
#include <QFileDialog>

#include <HistoryList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <PluginDatabase.hpp>
#include <PluginObject.hpp>
#include <WorkPanelList.hpp>
#include <ErrorSystem.hpp>
#include <ContainerModel.hpp>
#include <GlobalSetup.hpp>

#include <Graphics.hpp>
#include <GraphicsObjectList.hpp>
#include <BoxObject.hpp>

#include <PhysicalQuantity.hpp>
#include <PhysicalQuantities.hpp>

#include <Structure.hpp>
#include <StructureList.hpp>
#include <Trajectory.hpp>
#include <TrajectoryList.hpp>

#include "DockingProject.hpp"
#include "DockingProjectModule.hpp"
#include "DockingProjectInputExportTool.hpp"

#include "DockingProjectDockVina.hpp"

#include "DockingProjectDockPanel.moc"
#include "DockingProjectDockPanel.hpp"
#include <PODesignerDockingTabsResults.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DockingProjectDockPanelCB(void* p_data);

CExtUUID        DockingProjectDockPanelID(
                    "{DOCKING_DOCK_PANEL:71573814-d613-4664-a6cd-beb8b4ee9229}",
                    "Dock Panel");

CPluginObject   DockingProjectDockPanelObject(&DockingProjectPlugin,
                     DockingProjectDockPanelID,WORK_PANEL_CAT,
                     ":/images/DockingProject/13.docking/Dock.svg",
                     DockingProjectDockPanelCB);

// -----------------------------------------------------------------------------

QObject* DockingProjectDockPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CDockingProjectDockPanel requires active project");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(DockingProjectDockPanelID,p_project);
    if( p_wp == NULL ) {
        p_wp = new CDockingProjectDockPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDockingProjectDockPanel::CDockingProjectDockPanel(CProject* p_project)
    : CWorkPanel(&DockingProjectDockPanelObject,p_project,EWPR_TOOL)
{

    DockingProject = dynamic_cast<CDockingProject*>(p_project);

    if( DockingProject == NULL )
    {
        QMessageBox::critical(NULL,tr("Error"),
                              tr("Run Docking is enable only in Docking Project!"),
                              QMessageBox::Ok,QMessageBox::Ok);
        ES_ERROR("Cannot open file to write");
    }

    // set up ui file
    WidgetUI.setupUi(this);
    WidgetUI.LigandImg->setStyleSheet("background-image: url(:/images/DockingProject/13.docking/DockRunLigand.svg); background-repeat: none;");
    WidgetUI.ReceptorImg->setStyleSheet("background-image: url(:/images/DockingProject/13.docking/DockRunReceptor.svg); background-repeat: none;");

    SetupVina = new CDockingProjectInputVinaExportTool(this);
    LoadingNewStructures = false;

    // set units
    WidgetUI.BoxXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.BoxYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.BoxZSB->setPhysicalQuantity(PQ_DISTANCE);

    WidgetUI.SizeXSB->setPhysicalQuantity(PQ_DIMENSION);
    WidgetUI.SizeYSB->setPhysicalQuantity(PQ_DIMENSION);
    WidgetUI.SizeZSB->setPhysicalQuantity(PQ_DIMENSION);

    // Reset the form to defaults
    WidgetUI.BoxXSB->setInternalValue(0);
    WidgetUI.BoxYSB->setInternalValue(0);
    WidgetUI.BoxZSB->setInternalValue(0);
    WidgetUI.SizeXSB->setInternalValue(25);
    WidgetUI.SizeYSB->setInternalValue(25);
    WidgetUI.SizeZSB->setInternalValue(25);

    WidgetUI.modesSB->setValue(9);
    WidgetUI.seedSB->setValue(0);
    WidgetUI.energyRangeSB->setValue(4);
    WidgetUI.procSB->setValue(1);
    WidgetUI.exhausSB->setValue(8);

    WidgetUI.lineEditOutFile->setText("out.01.pdbqt");

    //----------------
    connect(WidgetUI.RunDockPB, SIGNAL(clicked()),
            this, SLOT(StartPrepareDock(void)));
    //----------------
    connect(DockingProject->GetStructures(), SIGNAL(OnStructureListChanged()),
            this, SLOT(InitTabs()));
    //----------------
    connect(DockingProject->GetGraphics()->GetObjects(), SIGNAL(OnGraphicsObjectListChanged(void)),
            this, SLOT(InitValues()));

    // load work panel setup
    LoadWorkPanelSetup();
}

// -----------------------------------------------------------------------------

CDockingProjectDockPanel::~CDockingProjectDockPanel(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectDockPanel::InitTabs(void)
{
    QString file_ligand = DockingProject->GetLigandFileName().fileName();
    QString file_receptor = DockingProject->GetReceptorFileName().fileName();

    WidgetUI.lineEditLigand->setText(file_ligand);
    WidgetUI.lineEditReceptor->setText(file_receptor);
}

//------------------------------------------------------------------------------

void CDockingProjectDockPanel::InitValues(void)
{
    CPoint pos;
    CPoint dim;
    bool was_connected_box = SetupVina->GetBoxState();

    CBoxObject* p_box = SetupVina->GetBox();
    // When i found box i need to set values to widget window
    if (p_box != NULL) {
        if (was_connected_box == false) {
            //----------------
            connect(p_box, SIGNAL(OnStatusChanged(EStatusChanged)),
                    this, SLOT(InitValues(void)));
        }

        /// get position and dimensions
        pos = p_box->GetPosition();
        dim = p_box->GetDimensions();
        WidgetUI.BoxXSB->setInternalValue(pos.x);
        WidgetUI.BoxYSB->setInternalValue(pos.y);
        WidgetUI.BoxZSB->setInternalValue(pos.z);
        WidgetUI.SizeXSB->setInternalValue(dim.x);
        WidgetUI.SizeYSB->setInternalValue(dim.y);
        WidgetUI.SizeZSB->setInternalValue(dim.z);

    }
    else {
        WidgetUI.BoxXSB->setInternalValue(0);
        WidgetUI.BoxYSB->setInternalValue(0);
        WidgetUI.BoxZSB->setInternalValue(0);
        WidgetUI.SizeXSB->setInternalValue(25);
        WidgetUI.SizeYSB->setInternalValue(25);
        WidgetUI.SizeZSB->setInternalValue(25);
    }

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CDockingProjectDockPanel::SetStructures(void)
{
    CStructure* p_lig = DockingProject->GetLigandStructure();
    CStructure* p_rec = DockingProject->GetReceptorStructure();

    QString edit_ligand = WidgetUI.lineEditLigand->text();
    QString edit_receptor = WidgetUI.lineEditReceptor->text();

    bool have_lig = true;
    bool have_rec = true;

    if( (p_lig == NULL) || (edit_ligand.isEmpty()) ){
        QMessageBox::critical(NULL,tr("Error"),
                              tr("Load or determine Ligand first!\nFor docking must be opened some Ligand file or set his filename in Dock Panel."),
                              QMessageBox::Abort, QMessageBox::Abort);
        return (false);
    } else if( p_lig->IsEmpty() && (edit_ligand.isEmpty()) ) {
        QMessageBox::critical(NULL,tr("Error"),
                              tr("Load or determine Ligand first!\nFor docking must be opened some Ligand file or set his filename in Dock Panel."),
                              QMessageBox::Abort, QMessageBox::Abort);
        return (false);
    } else if (p_lig->IsEmpty() && (!edit_ligand.isEmpty())){
        have_lig = false;
    }

    if( (p_rec == NULL) || (edit_receptor.isEmpty()) ){
        QMessageBox::critical(NULL,tr("Error"),
                              tr("Load or determine Receptor first!\nFor docking must be opened some Receptor file or set his filename in Dock Panel."),
                              QMessageBox::Abort, QMessageBox::Abort);
        return (false);
    } else if( p_rec->IsEmpty() && (edit_receptor.isEmpty()) ) {
        QMessageBox::critical(NULL,tr("Error"),
                              tr("Load or determine Receptor first!\nFor docking must be opened some Receptor file or set his filename in Dock Panel."),
                              QMessageBox::Abort, QMessageBox::Abort);
        return (false);
    } else if (p_rec->IsEmpty() && (!edit_receptor.isEmpty())){
        have_rec = false;
    }

    /// This section is for adding structures only based on filled names of ligand and receptor
    /// and choosen directory where they are and where will be compute realized
    // dialog for open the directory and set the path
    QFileDialog* p_dialog = new QFileDialog();
    p_dialog->setDirectory (QString(GlobalSetup->GetLastOpenFilePath(DockingProjectDockPanelID)));
    p_dialog->setFileMode(QFileDialog::DirectoryOnly);
    p_dialog->setAcceptMode(QFileDialog::AcceptOpen);

    // dialog window will be open
    if( p_dialog->exec() == QDialog::Rejected ){
        return (false);
    }
    SetupVina->SetHadPath(true);
    SetupVina->SetPath(p_dialog->selectedFiles().at(0));

    if (!have_lig) {
        // if have not ligand structure and
        // is ligand name only write in line edit of Dock Panel we try open it
        CSmallString lig_path = SetupVina->GetPath();
        unsigned int path_l = lig_path.GetLength();

        if (lig_path.GetSubStringFromTo(path_l-1,path_l) != "/") {
            lig_path += "/";
        }
        QString lig_whole = lig_path.GetBuffer();
        lig_whole += edit_ligand;
        std::ifstream  sin;

        sin.open(lig_whole.toLatin1());
        if( !sin ) {
            QString message = tr("Unable to open file for reading!\nCannot open file to read: ");
            message += lig_whole;
            QMessageBox::critical(NULL, tr("New Run Docking in Docking Project"),
                                  message,
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
            ES_ERROR(message);
            return(false);
        }
        sin.close();
        DockingProject->LoadLigandStructure(lig_whole);
    }
    if (!have_rec) {
        // if have not receptor structure and
        // is receptor name only write in line edit of Dock Panel we try open it
        CSmallString rec_path = SetupVina->GetPath();
        unsigned int path_l = rec_path.GetLength();

        if (rec_path.GetSubStringFromTo(path_l-1,path_l) != "/") {
            rec_path += "/";
        }
        QString rec_whole = rec_path.GetBuffer();
        rec_whole += edit_receptor;
        std::ifstream  sin;

        sin.open(rec_whole.toLatin1());
        if( !sin ) {
            QString message = tr("Unable to open file for reading!\nCannot open file to read: ");
            message += rec_whole;
            QMessageBox::critical(NULL, tr("New Run Docking in Docking Project"),
                                  message,
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
            ES_ERROR(message);
            return(false);
        }
        sin.close();
        DockingProject->LoadReceptorStructure(rec_whole);
    }
    if (!have_lig || !have_rec) {
        if (DockingProject->GetJobs()->HasChildren()) {
            //------------
            connect(DockingProject->GetJobs(), SIGNAL(OnJobRemoved(CJob*)),
                    this,SLOT(StartRunDock(void)));
            LoadingNewStructures = true;
        }
    } else {
        StartRunDock();
    }

    return (true);
}

//------------------------------------------------------------------------------

bool CDockingProjectDockPanel::SetSettings(void)
{
    // method handle with adding filled fields in GUI into system
    if (WidgetUI.flexibleLigandCB->checkState() == Qt::Checked) {
        SetupVina->SetFlexibleLigand(true);
    } else {
        SetupVina->SetFlexibleLigand(false);
    }
    if (WidgetUI.combineLigandCB->checkState() == Qt::Checked) {
        SetupVina->SetCombineLigand(true);
    } else {
        SetupVina->SetCombineLigand(false);
    }
    if (WidgetUI.renumberLigandCB->checkState() == Qt::Checked) {
        SetupVina->SetRenumberLigand(true);
    } else {
        SetupVina->SetRenumberLigand(false);
    }

    if (WidgetUI.flexibleReceptorCB->checkState() == Qt::Checked) {
        SetupVina->SetFlexibleReceptor(true);
    } else {
        SetupVina->SetFlexibleReceptor(false);
    }
    if (WidgetUI.combineReceptorCB->checkState() == Qt::Checked) {
        SetupVina->SetCombineReceptor(true);
    } else {
        SetupVina->SetCombineReceptor(false);
    }
    if (WidgetUI.renumberReceptorCB->checkState() == Qt::Checked) {
        SetupVina->SetRenumberReceptor(true);
    } else {
        SetupVina->SetRenumberReceptor(false);
    }

    if (WidgetUI.logCB->checkState() == Qt::Checked) {
        SetupVina->SetSaveLogOutput(true);
    } else {
        SetupVina->SetSaveLogOutput(false);
    }

    if (WidgetUI.configCB->checkState() == Qt::Checked) {
        SetupVina->SetSaveConfigFile(true);
    } else {
        SetupVina->SetSaveConfigFile(false);
    }

    CStructure* p_lig = DockingProject->GetLigandStructure();
    CStructure* p_rec = DockingProject->GetReceptorStructure();

    SetupVina->SetLigandInFormat(SetupVina->GenerateCoordinatesInPdbqt(p_lig,true));
    SetupVina->SetReceptorInFormat(SetupVina->GenerateCoordinatesInPdbqt(p_rec,false));
    CFileName ligand_name = DockingProject->GetLigandFileName().completeBaseName() + ".pdbqt";
    CFileName receptor_name = DockingProject->GetReceptorFileName().completeBaseName() + ".pdbqt";
    // write into lineEdit Ligand and Receptor "in fact" opened files
    WidgetUI.lineEditLigand->setText(ligand_name.GetBuffer());
    SetupVina->SetLigandFileName(ligand_name);
    WidgetUI.lineEditReceptor->setText(receptor_name.GetBuffer());
    SetupVina->SetReceptorFileName(receptor_name);

    SetupVina->SetConfigFileName("config.cfg");
    SetupVina->SetOutputFileName(WidgetUI.lineEditOutFile->text().toStdString().c_str() );
    SetupVina->SetLigandFileName(WidgetUI.lineEditLigand->text().toStdString().c_str() );
    SetupVina->SetReceptorFileName(WidgetUI.lineEditReceptor->text().toStdString().c_str() );

    SetupVina->SetNumberOfModes(WidgetUI.modesSB->value());
    SetupVina->SetSeed(WidgetUI.seedSB->value());
    SetupVina->SetEnergyRange(WidgetUI.energyRangeSB->value());
    SetupVina->SetNumberOfProcessors(WidgetUI.procSB->value());
    SetupVina->SetExhaustiveness(WidgetUI.exhausSB->value());

    CPoint pos;
    CPoint dim;

    pos.x = WidgetUI.BoxXSB->getInternalValue();
    pos.y = WidgetUI.BoxYSB->getInternalValue();
    pos.z = WidgetUI.BoxZSB->getInternalValue();
    dim.x = WidgetUI.SizeXSB->getInternalValue();
    dim.y = WidgetUI.SizeYSB->getInternalValue();
    dim.z = WidgetUI.SizeZSB->getInternalValue();

    SetupVina->SetBoxPosition(pos);
    SetupVina->SetBoxDimension(dim);

    SetupVina->SetConfig(SetupVina->GenerateConfigText());

    SetupVina->SetWriteConfig(true);
    SetupVina->SetWriteLigand(true);
    SetupVina->SetWriteReceptor(true);

    return (true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectDockPanel::StartPrepareDock(void)
{
    if (SetStructures() == false) {
        ES_ERROR("unable to set structures for new dock in docking project");
        return;
    }
}

//------------------------------------------------------------------------------

void CDockingProjectDockPanel::StartRunDock(void)
{
    if (LoadingNewStructures) {
        if (DockingProject->GetJobs()->HasChildren()) {
            return;
        }
        //disconnect(myObject, SIGNAL(mySignal()), 0, 0);
        disconnect(DockingProject->GetJobs(), 0, this, 0);
        LoadingNewStructures = false;
    }
    if (SetSettings() == false ) {
        ES_ERROR("unable to set settings for new dock in docking project");
        QMessageBox::critical(NULL, tr("New Run Docking in Docking Project"),
                              tr("An error occurred during set settings for Docking!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        return;
    }
    if (!SetupVina->SaveInputFiles()) {
        ES_ERROR("unable to save files for new dock in docking project");
        QMessageBox::critical(NULL, tr("New Run Docking in Docking Project"),
                              tr("An error occurred during saving files for Docking!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        return;
    }

    CDockingProjectDockVina* p_job_dock = new CDockingProjectDockVina(DockingProject);
    if ((p_job_dock) == NULL ) {
        ES_ERROR("unable to open new dock in docking project");
        QMessageBox::critical(NULL, tr("New Run Docking in Docking Project"),
                              tr("An error occurred during starting Docking!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
    } else {
        // create job and setup job
        p_job_dock->SetComputePath(SetupVina->GetPath());
        p_job_dock->SetConfigFile(SetupVina->GetConfigFileName());
        p_job_dock->SetResultsFile(SetupVina->GetOutputFileName());
        p_job_dock->SetSaveLogOutput(SetupVina->GetSaveLogOutput());
        p_job_dock->SetSaveConfigFile(SetupVina->GetSaveConfigFile());
        // submit job
        if( p_job_dock->SubmitJob() == false ){
            delete p_job_dock;
        }
    }

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
