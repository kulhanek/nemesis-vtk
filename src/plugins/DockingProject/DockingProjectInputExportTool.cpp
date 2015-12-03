// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
/**********************************************************************
  CDockingProjectInputExportTool - Dialog for generating Autodock Vina input decks

  Copyright (C) 2008 Marcus D. Hanwell
  Copyright (C) 2008 Michael Banck

  This file is part of the Avogadro molecular editor project.
  For more information, see <http://avogadro.openmolecules.net/>

  Avogadro is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Avogadro is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
 **********************************************************************/

#include <list>
#include <sstream>

#include <QFont>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <GlobalSetup.hpp>
#include <Project.hpp>
#include <ProObject.hpp>
#include <PluginDatabase.hpp>

#include <Graphics.hpp>
#include <GraphicsObject.hpp>
#include <GraphicsObjectList.hpp>
#include <BoxObject.hpp>

#include <Structure.hpp>
#include <AtomList.hpp>
#include <Atom.hpp>
#include <PeriodicTable.hpp>

#include <PhysicalQuantity.hpp>
#include <PhysicalQuantities.hpp>

#include "DockingProjectModule.hpp"
#include "DockingProject.hpp"
#include "DockingProjectInputExportTool.hpp"
#include "DockingProjectInputExportTool.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DockingProjectInputExportToolCB(void* p_data);

CExtUUID        DockingProjectInputExportToolID(
                    "{DOCKING_PROJECT_INPUT_EXPORT_TOOL:e777b111-7b54-4c11-8e8d-6d63ad2bfd80}",
                    "Docking Input (*.cfg, *.pdbqt)");

CPluginObject   DockingProjectInputExportToolObject(&DockingProjectPlugin,
                    DockingProjectInputExportToolID,EXPORT_STRUCTURE_CAT,
                    DockingProjectInputExportToolCB);

// -----------------------------------------------------------------------------

QObject* DockingProjectInputExportToolCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CDockingProjectInputExportTool requires active project");
        return(NULL);
    }

    CDockingProjectInputExportTool* p_object = new CDockingProjectInputExportTool(p_project);
    p_object->ShowAsDialog();
    delete p_object;
    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDockingProjectInputExportTool::CDockingProjectInputExportTool(CProject* p_project)
    : CWorkPanel(&DockingProjectInputExportToolObject,p_project,EWPR_DIALOG)
{
    WidgetUI.setupUi(this);

    SetupVina = new CDockingProjectInputVinaExportTool(this);

    DockingProject = dynamic_cast<CDockingProject*>(p_project);

    // set units
    WidgetUI.doubleSpinBoxX->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.doubleSpinBoxY->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.doubleSpinBoxZ->setPhysicalQuantity(PQ_DISTANCE);

    WidgetUI.spinBoxSizeX->setPhysicalQuantity(PQ_DIMENSION);
    WidgetUI.spinBoxSizeY->setPhysicalQuantity(PQ_DIMENSION);
    WidgetUI.spinBoxSizeZ->setPhysicalQuantity(PQ_DIMENSION);

    WidgetUI.InputImg->setStyleSheet("background-image: url(:/images/DockingProject/13.docking/InputExport.svg); background-repeat: none;");

    // reset setup
    ResetSetup();
    // update view - again with possibly new setup
   // UpdateGeneratedText();

    // Connect the GUI elements to the correct slots
    connect(WidgetUI.modesSB, SIGNAL(valueChanged(int)),
        this, SLOT(UpdateGeneratedText()));
    //----------------
    connect(WidgetUI.seedSB, SIGNAL(valueChanged(int)),
        this, SLOT(UpdateGeneratedText()));
    //----------------
    connect(WidgetUI.energyRangeSB, SIGNAL(valueChanged(int)),
        this, SLOT(UpdateGeneratedText()));
    //----------------
    connect(WidgetUI.procSB, SIGNAL(valueChanged(int)),
        this, SLOT(UpdateGeneratedText()));
    //----------------
    connect(WidgetUI.exhausSB, SIGNAL(valueChanged(int)),
        this, SLOT(UpdateGeneratedText()));
    //----------------
    connect(WidgetUI.procSB, SIGNAL(valueChanged(int)),
        this, SLOT(UpdateGeneratedText()));
    //----------------
    connect(WidgetUI.doubleSpinBoxX, SIGNAL(valueChanged(double)),
        this, SLOT(UpdateGeneratedText()));
    connect(WidgetUI.doubleSpinBoxY, SIGNAL(valueChanged(double)),
        this, SLOT(UpdateGeneratedText()));
    connect(WidgetUI.doubleSpinBoxZ, SIGNAL(valueChanged(double)),
        this, SLOT(UpdateGeneratedText()));
    connect(WidgetUI.spinBoxSizeX, SIGNAL(valueChanged(double)),
        this, SLOT(UpdateGeneratedText()));
    connect(WidgetUI.spinBoxSizeY, SIGNAL(valueChanged(double)),
        this, SLOT(UpdateGeneratedText()));
    connect(WidgetUI.spinBoxSizeZ, SIGNAL(valueChanged(double)),
        this, SLOT(UpdateGeneratedText()));

    //----------------
    connect(WidgetUI.lineEditOutFile, SIGNAL(textChanged(const QString &)),
        this, SLOT(UpdateGeneratedText()));
    connect(WidgetUI.lineEditLigand, SIGNAL(textChanged(const QString &)),
        this, SLOT(UpdateGeneratedText()));
    connect(WidgetUI.lineEditReceptor, SIGNAL(textChanged(const QString &)),
        this, SLOT(UpdateGeneratedText()));
    connect(WidgetUI.previewText, SIGNAL(textChanged()),
        this, SLOT(PreviewEdited()));
    connect(WidgetUI.previewTextLigand, SIGNAL(textChanged()),
        this, SLOT(PreviewEditedLigand()));
    connect(WidgetUI.previewTextReceptor, SIGNAL(textChanged()),
        this, SLOT(PreviewEditedReceptor()));
    //----------------
    connect(WidgetUI.flexibleLigandCB, SIGNAL(stateChanged(int)),
        this, SLOT(CheckBoxesEdited(int)));
    connect(WidgetUI.combineLigandCB, SIGNAL(stateChanged(int)),
        this, SLOT(CheckBoxesEdited(int)));
    connect(WidgetUI.renumberLigandCB, SIGNAL(stateChanged(int)),
        this, SLOT(CheckBoxesEdited(int)));
    connect(WidgetUI.flexibleReceptorCB, SIGNAL(stateChanged(int)),
        this, SLOT(CheckBoxesEdited(int)));
    connect(WidgetUI.combineReceptorCB, SIGNAL(stateChanged(int)),
        this, SLOT(CheckBoxesEdited(int)));
    connect(WidgetUI.renumberReceptorCB, SIGNAL(stateChanged(int)),
        this, SLOT(CheckBoxesEdited(int)));
    //----------------
    connect(WidgetUI.saveButton, SIGNAL(clicked()),
        this, SLOT(SaveInputFiles()));
    //----------------
    connect(WidgetUI.resetButton, SIGNAL(clicked()),
        this, SLOT(ResetSetup()));

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CDockingProjectInputExportTool::~CDockingProjectInputExportTool(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectInputExportTool::PreviewEdited(void)
{
    // determine if the preview text has changed from the form generated
    // and disable controls when was changed
    WidgetUI.controlW->setEnabled(WidgetUI.previewText->toPlainText() == SetupVina->GenerateConfigText() );
}

//------------------------------------------------------------------------------

void CDockingProjectInputExportTool::PreviewEditedLigand(void)
{
    WidgetUI.frameCheckBoxesLigand->setEnabled(WidgetUI.previewTextLigand->toPlainText() == SetupVina->GetLigandInFormat() );
}

//------------------------------------------------------------------------------

void CDockingProjectInputExportTool::PreviewEditedReceptor(void)
{
    WidgetUI.frameCheckBoxesReceptor->setEnabled(WidgetUI.previewTextReceptor->toPlainText() == SetupVina->GetReceptorInFormat());
}

//------------------------------------------------------------------------------

void CDockingProjectInputExportTool::CheckBoxesEdited(int value)
{
    // change one of checkbox to another value than default implies regenerate files
    // on save click, so we must disable changes on them
    UpdateInternalValuesFromCheckboxes();
    if (SetupVina->AreLigandCheckBoxesDefaults() )
    {
        WidgetUI.previewTextLigand->setEnabled(true);
    } else {
        WidgetUI.previewTextLigand->setEnabled(false);
    }
    if (SetupVina->AreReceptorCheckBoxesDefaults() )
    {
        WidgetUI.previewTextReceptor->setEnabled(true);
    } else {

        WidgetUI.previewTextReceptor->setEnabled(false);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


void CDockingProjectInputExportTool::ResetSetup(void)
{
    // first unlock user changes
    if( ! WidgetUI.controlW->isEnabled() ){
        WidgetUI.controlW->setEnabled(true);
        WidgetUI.saveButton->setEnabled(true);
        UpdateGeneratedText();
        return;
    }

    // Position and dimension of box
    CPoint pos;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;

    CPoint dim;
    dim.x = 25;
    dim.y = 25;
    dim.z = 25;

    CBoxObject* p_box = SetupVina->GetBox();
    if (p_box != NULL) {
        /// get position and dimensions
        pos = p_box->GetPosition();
        dim = p_box->GetDimensions();
    }

    WidgetUI.doubleSpinBoxX->setInternalValue(pos.x);
    WidgetUI.doubleSpinBoxY->setInternalValue(pos.y);
    WidgetUI.doubleSpinBoxZ->setInternalValue(pos.z);
    WidgetUI.spinBoxSizeX->setInternalValue(dim.x);
    WidgetUI.spinBoxSizeY->setInternalValue(dim.y);
    WidgetUI.spinBoxSizeZ->setInternalValue(dim.z);
    SetupVina->SetBoxPosition(pos);
    SetupVina->SetBoxDimension(dim);

    // Reset the form to defaults
    WidgetUI.modesSB->setValue(9);
    WidgetUI.seedSB->setValue(0);
    WidgetUI.energyRangeSB->setValue(4);
    WidgetUI.procSB->setValue(1);
    WidgetUI.exhausSB->setValue(8);
    SetupVina->SetNumberOfModes(9);
    SetupVina->SetSeed(0);
    SetupVina->SetEnergyRange(4);
    SetupVina->SetNumberOfProcessors(1);
    SetupVina->SetExhaustiveness(8);

    WidgetUI.lineEditConfig->setText("config.cfg");
    SetupVina->SetConfigFileName("config.cfg");
    WidgetUI.lineEditOutFile->setText("out.01.pdbqt");
    SetupVina->SetOutputFileName("out.01.pdbqt");
    CFileName ligand_name = "My_ligand";
    CFileName receptor_name = "My_receptor";

    // in Docking project set file names from path of opened files
    if( DockingProject != NULL ){
        // Return only a file name
        // save generated file - i dont have to regenerete in every repaint in UpdatePreviewText
        if ( (DockingProject->GetLigandStructure() != NULL) && (!DockingProject->GetLigandStructure()->IsEmpty()) ) {
            ligand_name = DockingProject->GetLigandFileName().completeBaseName();
            SetupVina->SetLigandInFormat( GenerateStrucureText(DockingProject->GetLigandStructure(), true) );
        }
        if ( (DockingProject->GetReceptorStructure() != NULL) && (!DockingProject->GetReceptorStructure()->IsEmpty()) ) {
            receptor_name = DockingProject->GetReceptorFileName().completeBaseName();
            SetupVina->SetReceptorInFormat( GenerateStrucureText(DockingProject->GetReceptorStructure(), false) );
        }
    }
    ligand_name += ".pdbqt";
    receptor_name += ".pdbqt";
    // write into lineEdit Ligand and Receptor "in fact" opened files
    WidgetUI.lineEditLigand->setText(ligand_name.GetBuffer());
    SetupVina->SetLigandFileName(ligand_name);
    WidgetUI.lineEditReceptor->setText(receptor_name.GetBuffer());
    SetupVina->SetReceptorFileName(receptor_name);

    WidgetUI.controlW->setEnabled(true);
    WidgetUI.saveButton->setEnabled(true);


    UpdateGeneratedText();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectInputExportTool::UpdateGeneratedText(void)
{    
    QFont font_bold = QFont ();
    font_bold.setBold (true);

    if(DockingProject != NULL) {
        // if ligand structure is empty
        if( DockingProject->GetLigandStructure()->IsEmpty()){
            WidgetUI.previewTextLigand->setText(tr("No ligand is in the project!\nNo Ligand structure will be saved\n"));
            WidgetUI.previewTextLigand->setFont(font_bold);
            WidgetUI.writeLigandCB->setCheckState(Qt::Unchecked);
        } else if( WidgetUI.controlsW->isEnabled() ){
            WidgetUI.previewTextLigand->setText(SetupVina->GetLigandInFormat());
        }

        // if i dont have receptor sctructure say it and preview of receptor file get gray
        if( DockingProject->GetReceptorStructure() == NULL ){
            WidgetUI.previewTextReceptor->setText(tr("No receptor is in the project!\nNo receptor structure will be saved!"));
            WidgetUI.previewTextReceptor->setFont(font_bold);
            WidgetUI.writeReceptorCB->setCheckState(Qt::Unchecked);
        } else if ( DockingProject->GetReceptorStructure()->IsEmpty()) {
            WidgetUI.previewTextReceptor->setText(tr("No receptor is in the project!\nNo receptor structure will be saved!"));
            WidgetUI.previewTextReceptor->setFont(font_bold);
            WidgetUI.writeReceptorCB->setCheckState(Qt::Unchecked);
        } else if( WidgetUI.controlsW->isEnabled() ){
            // if i have receptor i shall show PDBQT
            WidgetUI.previewTextReceptor->setText(SetupVina->GetReceptorInFormat());
        }
    } else {
        WidgetUI.previewTextReceptor->setText(tr("Open the Input Tool in the Docking project for Receptor support!\nNo receptor structure will be saved!"));
        WidgetUI.previewTextReceptor->setFont(font_bold);
        WidgetUI.writeReceptorCB->setCheckState(Qt::Unchecked);
    }
    UpdateInternalValuesFromFields();
    // Generate config file all ways
    WidgetUI.previewText->setText( SetupVina->GenerateConfigText() );
}

//------------------------------------------------------------------------------

QString CDockingProjectInputExportTool::GenerateStrucureText (CStructure* structure, bool is_ligand)
{
    UpdateInternalValuesFromCheckboxes();

    return (SetupVina->GenerateCoordinatesInPdbqt(structure,is_ligand) );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectInputExportTool::SaveInputFiles(void)
{
    UpdateInternalValuesFromCheckboxes();
    UpdateInternalValuesFromFields();

    SetupVina->SaveInputFiles();

    // should we close this window?
    if( WidgetUI.closeOnSaveCB->isChecked() ){
        close();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectInputExportTool::UpdateInternalValuesFromCheckboxes(void)
{
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

    /// write files only if i have their filename from widget and set checkbox
    if (WidgetUI.writeConfigCB->checkState() == Qt::Checked) {
        SetupVina->SetWriteConfig(true);
    } else {
        SetupVina->SetWriteConfig(false);
    }
    if (WidgetUI.writeLigandCB->checkState() == Qt::Checked) {
        SetupVina->SetWriteLigand(true);
    } else {
        SetupVina->SetWriteLigand(false);
    }
    if (WidgetUI.writeReceptorCB->checkState() == Qt::Checked) {
        SetupVina->SetWriteReceptor(true);
    } else {
        SetupVina->SetWriteReceptor(false);
    }

}

//------------------------------------------------------------------------------

void CDockingProjectInputExportTool::UpdateInternalValuesFromFields(void)
{
    CPoint pos;
    CPoint dim;

    pos.x = WidgetUI.doubleSpinBoxX->getInternalValue();
    pos.y = WidgetUI.doubleSpinBoxY->getInternalValue();
    pos.z = WidgetUI.doubleSpinBoxZ->getInternalValue();
    dim.x = WidgetUI.spinBoxSizeX->getInternalValue();
    dim.y = WidgetUI.spinBoxSizeY->getInternalValue();
    dim.z = WidgetUI.spinBoxSizeZ->getInternalValue();

    SetupVina->SetBoxPosition(pos);
    SetupVina->SetBoxDimension(dim);

    // Reset the form to defaults
    SetupVina->SetNumberOfModes( WidgetUI.modesSB->value() );
    SetupVina->SetSeed( WidgetUI.seedSB->value() );
    SetupVina->SetEnergyRange( WidgetUI.energyRangeSB->value() );
    SetupVina->SetNumberOfProcessors( WidgetUI.procSB->value() );
    SetupVina->SetExhaustiveness( WidgetUI.exhausSB->value() );

    // assign filename from lines in widget window
    SetupVina->SetConfigFileName( WidgetUI.lineEditConfig->text() );
    SetupVina->SetOutputFileName( WidgetUI.lineEditOutFile->text() );
    SetupVina->SetLigandFileName( WidgetUI.lineEditLigand->text() );
    SetupVina->SetReceptorFileName( WidgetUI.lineEditReceptor->text() );

    // assign config and structures from fields
    SetupVina->SetConfig( WidgetUI.previewText->toPlainText() );
    SetupVina->SetLigandInFormat( WidgetUI.previewTextLigand->toPlainText() );
    SetupVina->SetReceptorInFormat( WidgetUI.previewTextReceptor->toPlainText() );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
