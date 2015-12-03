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

#include <QtGui>
#include <QMessageBox>
#include <QCheckBox>
#include <QFileDialog>

#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <WorkPanel.hpp>
#include <WorkPanelList.hpp>
#include <ErrorSystem.hpp>
#include <QMessageBox>
#include <ContainerModel.hpp>
#include <GlobalSetup.hpp>

#include "SmallString.hpp"

#include <openbabel/obconversion.h>

#include <vector>

#include "CheminfoProjectModule.hpp"
#include "CheminfoProjectDatabase.hpp"
#include "CheminfoProjectDatabaseStructure.hpp"
#include "CheminfoProjectWindow.hpp"
#include "CheminfoProject.hpp"

#include "CheminfoDatabaseWorkPanel.moc"
#include "CheminfoDatabaseWorkPanel.hpp"

using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CheminfoDatabaseWorkPanelCB(void* p_data);

CExtUUID        CheminfoDatabaseWorkPanelID(
                    "{CHEMINFO_DATABASE_WORK_PANEL:ca096980-262c-423e-8d5f-4bc8667e847f}",
                    "Cheminfo Database Panel");

CPluginObject   CheminfoDatabaseWorkPanelObject(&CheminfoProjectPlugin,
                     CheminfoDatabaseWorkPanelID,WORK_PANEL_CAT,
                     ":../images/CheminfoProject/CheminfoProject.svg",
                     CheminfoDatabaseWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* CheminfoDatabaseWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CCheminfoDatabaseWorkPanel requires active project");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(CheminfoDatabaseWorkPanelID,p_project);
    if( p_wp == NULL ) {
        p_wp = new CCheminfoDatabaseWorkPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoDatabaseWorkPanel::CCheminfoDatabaseWorkPanel(CProject* p_project)
    : CWorkPanel(&CheminfoDatabaseWorkPanelObject,p_project,EWPR_TOOL)
{
    // set up ui file
    WidgetUI.setupUi(this);
    WidgetUI.labelConnect->setStyleSheet("QLabel { background-color : Tomato ;}");
    WidgetUI.labelConnectStructure->setStyleSheet("QLabel { background-color : Tomato ;}");


    CheminfoProject = dynamic_cast<CCheminfoProject*>(p_project);
    if (CheminfoProject == NULL) {
        QMessageBox::critical(NULL,"Error","Project is not Cheminfo Project!",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
    Database = CheminfoProject->GetDatabase();
    DatabaseStructure = CheminfoProject->GetDatabaseStructure();
    WidgetUI.ligTypeCB->addItems(DatabaseStructure->GetLigandFormats());
    WidgetUI.resTypeCB->addItems(DatabaseStructure->GetResultsFormats());

    //------------------
    connect(Database,SIGNAL(OnDatabaseConnectChanged(void)),
            this,SLOT(UpdateValues(void)));
    //------------------
    connect(DatabaseStructure,SIGNAL(OnDatabaseFillChanged(void)),
            this,SLOT(UpdateValues(void)));
    //------------------
    connect(WidgetUI.connectionPB,SIGNAL(clicked(bool)),
            this,SLOT(ConnectionPBClicked(bool)));
    //------------------
    connect(WidgetUI.browseDBStructurePB,SIGNAL(clicked(bool)),
            this,SLOT(BrowseDBStructurePBClicked(bool)));
    //------------------
    connect(WidgetUI.browseReceptorPB,SIGNAL(clicked(bool)),
            this,SLOT(BrowseReceptorPBClicked(bool)));
    //------------------
    connect(WidgetUI.dbApplyPB,SIGNAL(clicked(bool)),
            this,SLOT(DBApplyPBClicked(bool)) );
    //------------------
    connect(WidgetUI.dbStructApplyPB,SIGNAL(clicked(bool)),
            this,SLOT(DBStructApplyPBClicked(bool)) );

    UpdateValues();

    // load work panel setup
    LoadWorkPanelSetup();

}

// -----------------------------------------------------------------------------

void CCheminfoDatabaseWorkPanel::UpdateValues(void)
{
    if (Database->GetFirebird()->IsLogged()) {
        WidgetUI.labelConnect->setText("Connected");
        WidgetUI.labelConnect->setStyleSheet("QLabel { background-color : LimeGreen ;}");
        WidgetUI.connectionPB->setText("Disconnect");
    } else {
        WidgetUI.labelConnect->setText("Disconnected");
        WidgetUI.labelConnect->setStyleSheet("QLabel { background-color : Tomato ;}");
        WidgetUI.connectionPB->setText("Connect");
    }

    if (DatabaseStructure->IsLoaded() ) {
        WidgetUI.labelConnectStructure->setText("Found");
        WidgetUI.labelConnectStructure->setStyleSheet("QLabel { background-color : LimeGreen ;}");
        WidgetUI.browseDBStructurePB->setText("Clear");
    } else {
        WidgetUI.labelConnectStructure->setText("Not found");
        WidgetUI.labelConnectStructure->setStyleSheet("QLabel { background-color : Tomato ;}");
        WidgetUI.browseDBStructurePB->setText("Browse");
    }

    if (Database->AreInitValuesFill())
    {
        QFileInfo dbFile(Database->GetDatabaseName().GetBuffer());
        QString name = dbFile.fileName();
        QString path = dbFile.path();
        WidgetUI.dbTypeCB->setCurrentIndex(Database->GetDatabaseType());
        WidgetUI.dbNameLE->setText(name);
        WidgetUI.dbPathLE->setText(path);
        WidgetUI.dbUserLE->setText(Database->GetDatabaseUserName().GetBuffer());
        WidgetUI.dbPassLE->setText(Database->GetDatabasePassword().GetBuffer());
    }

    WidgetUI.structPathLE->setText(DatabaseStructure->GetPath().GetBuffer());
    WidgetUI.structPrefixLE->setText(DatabaseStructure->GetPrefix().GetBuffer());
    WidgetUI.ligTypeCB->setCurrentIndex(DatabaseStructure->GetLigandTypeNum() );
    WidgetUI.resTypeCB->setCurrentIndex(DatabaseStructure->GetResultsTypeNum());

    if (DatabaseStructure->IsHierarchy()) {
        WidgetUI.hierarchyCB->setCheckState(Qt::Checked);
    } else {
        WidgetUI.hierarchyCB->setCheckState(Qt::Unchecked);
    }
}

// -----------------------------------------------------------------------------

CCheminfoDatabaseWorkPanel::~CCheminfoDatabaseWorkPanel(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoDatabaseWorkPanel::ConnectionPBClicked(bool checked)
{
    if (Database->IsLogged()) {
        Database->DisconnectDatabaseFirebird();
    }
    else if (Database->AreInitValuesFill()) {
        Database->ConnectDatabaseFirebird();
    } else {
        CExtUUID mp_uuid;
        mp_uuid.LoadFromString("{CHEMINFO_PROJECT_SET_DATABASE:f61eebaf-f15b-4358-8fb7-096d79542fb2}");
        // open dialog
        CheminfoProject->GetMainWindow()->ExecuteObject(mp_uuid,CheminfoProject);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoDatabaseWorkPanel::BrowseDBStructurePBClicked(bool checked)
{
    if (DatabaseStructure->IsLoaded() ) {
        CSmallString empty;
        DatabaseStructure->SetPath(empty);
        DatabaseStructure->SetPrefix(empty);
        DatabaseStructure->SetLigandType(empty,0);
        DatabaseStructure->SetResultsType(empty,0);
        DatabaseStructure->Unload();
    } else {
        CExtUUID mp_uuid;
        mp_uuid.LoadFromString("{CHEMINFO_PROJECT_SET_STRUCTURE_DB:06b310b0-7a82-44c2-ab36-a50d2bf38e18}");
        // open dialog
        CheminfoProject->GetMainWindow()->ExecuteObject(mp_uuid,CheminfoProject);
        if (DatabaseStructure->AreInitValuesFill()) {
            DatabaseStructure->Load();
        }
    }
}

//------------------------------------------------------------------------------

void CCheminfoDatabaseWorkPanel::BrowseReceptorPBClicked(bool checked)
{
    // get list of supported babel formats -------
    OBConversion co;
    vector<string> formats = co.GetSupportedInputFormat();

    // parse formats list ------------------------
    QStringList filters;
    filters << "pdb, xyz, pdbqt -- Common formats & Autodock PDBQT format (*.pdb *.xyz *.pdbqt)";
    filters << "*.* -- All files (*.*)";

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
    QFileDialog* p_dialog = new QFileDialog(CheminfoProject->GetMainWindow());

    p_dialog->setWindowTitle(tr("Select for Firebird Database File"));

    p_dialog->setNameFilters(filters);
    p_dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(CheminfoDatabaseWorkPanelID)));
    p_dialog->setFileMode(QFileDialog::ExistingFile);
    p_dialog->setAcceptMode(QFileDialog::AcceptOpen);

    if( p_dialog->exec() == QDialog::Accepted ){
        WidgetUI.receptorLE->setText(p_dialog->selectedFiles().at(0));
        CheminfoProject->SetReceptorFile(p_dialog->selectedFiles().at(0));
        GlobalSetup->SetLastOpenFilePathFromFile(p_dialog->selectedFiles().at(0),CheminfoDatabaseWorkPanelID);
    }

    delete p_dialog;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoDatabaseWorkPanel::DBApplyPBClicked(bool checked)
{
    int type = WidgetUI.dbTypeCB->currentIndex();
    CSmallString name = WidgetUI.dbNameLE->text();
    CSmallString path = WidgetUI.dbPathLE->text();
    CSmallString user = WidgetUI.dbUserLE->text();
    CSmallString pass = WidgetUI.dbPassLE->text();

    unsigned int path_l = path.GetLength();

    if (path.GetSubStringFromTo(path_l-1,path_l) != "/") {
        path += "/";
    }

    CSmallString dbname = path + name;

    // we need only path other is on user
    if(dbname.IsEmpty() ) {
        QMessageBox::critical(NULL,"Error","Database init values was not fully filled!"\
                              "\nObligatory is db name and path.",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }

    bool changed = false;

    if (Database->GetDatabaseType() != TypeDBMS(type)) {
        Database->SetDatabaseType(TypeDBMS(type));
        changed = true;
    }

    if (Database->GetDatabaseName() != dbname) {
        Database->SetDatabaseName(dbname);
        changed = true;
    }

    if (Database->GetDatabaseUserName() != user) {
        Database->SetDatabaseUserName(user);
        changed = true;
    }

    if (Database->GetDatabasePassword() != pass) {
        Database->SetDatabasePassword(pass);
        changed = true;
    }
    if (Database->IsLogged() && changed) {
        Database->DisconnectDatabaseFirebird();
        Database->ConnectDatabaseFirebird();
    } else if (changed) {
        Database->ConnectDatabaseFirebird();
    }

    if (WidgetUI.saveDatabaseCB->checkState() == Qt::Checked) {
        CheminfoProject->GetDatabase()->SaveDatabaseSettings();
    }

}

//------------------------------------------------------------------------------

void CCheminfoDatabaseWorkPanel::DBStructApplyPBClicked(bool checked)
{
    // every set in structure database will call update, so i must save fields first
    CSmallString path_str = WidgetUI.structPathLE->text();
    CSmallString prefix_str = WidgetUI.structPrefixLE->text();
    CSmallString lig_type_text = WidgetUI.ligTypeCB->currentText();
    int lig_type_index = WidgetUI.ligTypeCB->currentIndex();
    CSmallString res_type_text = WidgetUI.resTypeCB->currentText();
    int res_type_index = WidgetUI.resTypeCB->currentIndex();

    if(path_str.IsEmpty() ) {
        QMessageBox::critical(NULL,"Error: Structure database values missing","Structure database init values was not fully filled!\n"\
                              "Please fill structure files path and obligatory with prefix.",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }

    // file format seem to be "typeFormat -- description" so if i find first occurence
    // of ' ' -> i have type
    int pos_l = lig_type_text.IndexOf(' ');
    CSmallString lig_type = lig_type_text.GetSubStringFromTo(0,pos_l-1);
    int pos_r = res_type_text.IndexOf(' ');
    CSmallString res_type = res_type_text.GetSubStringFromTo(0,pos_r-1);

    if(WidgetUI.hierarchyCB->checkState() == Qt::Checked) {
        DatabaseStructure->SetHierarchy(true);
    } else {
        DatabaseStructure->SetHierarchy(false);
    }

    if( path_str != DatabaseStructure->GetPath() ) {
        DatabaseStructure->SetPath(path_str);
    }

    if( prefix_str != DatabaseStructure->GetPrefix() ) {
        DatabaseStructure->SetPrefix(prefix_str);
    }

    if( lig_type != DatabaseStructure->GetLigandType() ) {
        DatabaseStructure->SetLigandType(lig_type,lig_type_index);
    }

    if( res_type != DatabaseStructure->GetResultsType() ) {
        DatabaseStructure->SetResultsType(res_type,res_type_index);
    }
    if (WidgetUI.saveDatabaseStructureCB->checkState() == Qt::Checked) {
        CheminfoProject->GetDatabaseStructure()->SaveDatabaseStructureSettings();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
