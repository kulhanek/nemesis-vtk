// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>

#include <vector>

#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <WorkPanel.hpp>
#include <WorkPanelList.hpp>
#include <ErrorSystem.hpp>
#include <QMessageBox>
#include <ContainerModel.hpp>
#include <MainWindow.hpp>
#include <GlobalSetup.hpp>

#include "CheminfoProject.hpp"
#include "CheminfoProjectModule.hpp"
#include "CheminfoProjectDatabaseStructure.hpp"
#include "CheminfoProjectSetDatabaseStructure.hpp"
#include "CheminfoProjectSetDatabaseStructure.moc"

#include <openbabel/obconversion.h>

using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CCheminfoProjectSetDatabaseStructureCB(void* p_data);

CExtUUID        CCheminfoProjectSetDatabaseStructureID(
                    "{CHEMINFO_PROJECT_SET_STRUCTURE_DB:06b310b0-7a82-44c2-ab36-a50d2bf38e18}",
                    "Cheminfo Structure database open");

CPluginObject   CCheminfoProjectSetDatabaseStructureObject(&CheminfoProjectPlugin,
                    CCheminfoProjectSetDatabaseStructureID,CHEMINFO_CAT,
                    CCheminfoProjectSetDatabaseStructureCB);

// -----------------------------------------------------------------------------

QObject* CCheminfoProjectSetDatabaseStructureCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CCCheminfoProjectSetDatabaseStructure requires active project");
        return(NULL);
    }
    CCheminfoProjectSetDatabaseStructure* p_object = new CCheminfoProjectSetDatabaseStructure(p_project);
    p_object->ExecuteDialog();
    delete p_object;

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectSetDatabaseStructure::CCheminfoProjectSetDatabaseStructure(CProject* p_project)
    : CProObject(&CCheminfoProjectSetDatabaseStructureObject,NULL,p_project,true)
{
    CheminfoProject = dynamic_cast<CCheminfoProject*>(p_project);
    DatabaseStructure = CheminfoProject->GetDatabaseStructure();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectSetDatabaseStructure::ExecuteDialog(void)
{

    // dialog for open the directory and set the path
    QFileDialog* p_dialog = new QFileDialog(GetProject()->GetMainWindow(),tr("Cheminfo - Browse Structure database directory"));
    p_dialog->setDirectory (QString(GlobalSetup->GetLastOpenFilePath(CCheminfoProjectSetDatabaseStructureID)));
    p_dialog->setFileMode(QFileDialog::DirectoryOnly);
    p_dialog->setAcceptMode(QFileDialog::AcceptOpen);

    // setting checkbox on bottom of QFileDiqlox p_dialoq --------------------------------
    // setting label
    QLabel* p_settings_label = new QLabel(p_dialog);
    p_settings_label->setText("Settings:");
    QHBoxLayout *layout_horizontal40 = new QHBoxLayout;
    layout_horizontal40->addWidget(p_settings_label);

    // hierarchy
    QCheckBox* p_CB_hierarchy = new QCheckBox(p_dialog);
    p_CB_hierarchy->setCheckState(Qt::Checked);
    p_CB_hierarchy->setText(tr("Hierarchy"));
    p_CB_hierarchy->setToolTip(tr("Is structure database hierarchy ordered?\n(i.e. file ZINC00595260.mol2 id in folder DBpath/00/59/52/)") );

    // add checkbox to horizontal layout
    QHBoxLayout *layout_horizontal41 = new QHBoxLayout;
    layout_horizontal41->addWidget(p_CB_hierarchy);

    // prefix structure database
    QLineEdit* p_LE_db_prefix = new QLineEdit(p_dialog);
    p_LE_db_prefix->setPlaceholderText(tr("Structure files prefix. (e.g. ZINC)"));
    p_LE_db_prefix->setToolTip(tr("Structure database files prefix\n"\
                                      "(e.g. for file ZINC00595260.mol2 write ZINC)") );
    layout_horizontal41->addWidget(p_LE_db_prefix);

    // save settings
    QCheckBox* p_CB_saveSettings = new QCheckBox(p_dialog);
    p_CB_saveSettings->setCheckState(Qt::Unchecked);
    p_CB_saveSettings->setText(tr("Save settings"));
    p_CB_saveSettings->setToolTip(tr("Save settings into the file?\n(in next startup will be setting restore from the settings file)") );

    // add checkbox to horizontal layout
    QHBoxLayout *layout_horizontal42 = new QHBoxLayout;
    layout_horizontal42->addWidget(p_CB_saveSettings);

    // ligands type label
    QLabel* p_ligtype_label = new QLabel(p_dialog);
    p_ligtype_label->setText("Ligands type:");
    QHBoxLayout *layout_horizontal50 = new QHBoxLayout;
    layout_horizontal50->addWidget(p_ligtype_label);
    // ligands type combo box
    QComboBox* p_CB_ligtype = new QComboBox(p_dialog);
    p_CB_ligtype->addItems(DatabaseStructure->GetLigandFormats());
    QHBoxLayout *layout_horizontal51 = new QHBoxLayout;
    layout_horizontal51->addWidget(p_CB_ligtype);

    // results type label
    QHBoxLayout *layout_horizontal60 = new QHBoxLayout;
    QLabel* p_restype_label = new QLabel(p_dialog);
    p_restype_label->setText("Results type:");
    layout_horizontal60->addWidget(p_restype_label);
    // results type combo box
    QComboBox* p_CB_restype = new QComboBox(p_dialog);
    p_CB_restype->addItems(DatabaseStructure->GetResultsFormats());
    QHBoxLayout *layout_horizontal61 = new QHBoxLayout;
    layout_horizontal61->addWidget(p_CB_restype);

    // set horizontal layouts into QFileDialog layout
    QGridLayout *layout = (QGridLayout*)p_dialog->layout();
    // under labels for files
    layout->addLayout(layout_horizontal40,4,0);
    layout->addLayout(layout_horizontal41,4,1);
    layout->addLayout(layout_horizontal42,4,2);
    layout->addLayout(layout_horizontal50,5,0);
    layout->addLayout(layout_horizontal51,5,1);
    layout->addLayout(layout_horizontal60,6,0);
    layout->addLayout(layout_horizontal61,6,1);

    if(DatabaseStructure->IsLoaded()) {
        p_LE_db_prefix->setText(DatabaseStructure->GetPrefix().GetBuffer());
        if (DatabaseStructure->IsHierarchy()) {
            p_CB_hierarchy->setCheckState(Qt::Checked);
        } else {
            p_CB_hierarchy->setCheckState(Qt::Unchecked);
        }
        p_CB_ligtype->setCurrentIndex(DatabaseStructure->GetLigandTypeNum());
        p_CB_restype->setCurrentIndex(DatabaseStructure->GetResultsTypeNum());
    }

    // under QTextEdit for filenames
    // dialog window will be open
    if( p_dialog->exec() == QDialog::Rejected ){
        return;
    }
    CSmallString db_strpath =  p_dialog->selectedFiles().at(0);
    CSmallString db_strprefix = p_LE_db_prefix->text();
    CSmallString lig_type_text = p_CB_ligtype->currentText();
    CSmallString res_type_text = p_CB_restype->currentText();

    if(db_strprefix.IsEmpty() ) {
        QMessageBox::critical(NULL,"Note: Structure database values missing","Note: Structure database init values was not fully filled!\n"\
                              "The Structure files prefix was not filled.\nThis can be problem for openning files.",QMessageBox::Ok,QMessageBox::Ok);
    }
    // file format seem to be "typeFormat -- description" so if i find first occurence
    // of ' ' -> i have type
    int pos_l = lig_type_text.IndexOf(' ');
    CSmallString lig_type = lig_type_text.GetSubStringFromTo(0,pos_l-1);
    int pos_r = res_type_text.IndexOf(' ');
    CSmallString res_type = res_type_text.GetSubStringFromTo(0,pos_r-1);

    DatabaseStructure->SetPath(db_strpath);
    DatabaseStructure->SetPrefix(db_strprefix);
    DatabaseStructure->SetLigandType(lig_type,p_CB_ligtype->currentIndex());
    DatabaseStructure->SetResultsType(res_type,p_CB_restype->currentIndex());

    if (p_CB_saveSettings->checkState() == Qt::Checked) {
        CheminfoProject->GetDatabaseStructure()->SaveDatabaseStructureSettings();
    }

    CFileName dirpath(db_strpath);
    // set the last opened
    GlobalSetup->SetLastOpenFilePath(dirpath,CCheminfoProjectSetDatabaseStructureID);

    delete p_dialog;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCheminfoProjectSetDatabaseStructure::LunchJob(const QString& file)
{

    return (true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
