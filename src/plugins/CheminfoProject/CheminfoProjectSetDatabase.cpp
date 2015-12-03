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

#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <XMLElement.hpp>
#include <GlobalSetup.hpp>
#include <list>
#include <sstream>

#include <PluginDatabase.hpp>
#include <ProObject.hpp>
#include <GraphicsObject.hpp>
#include <Project.hpp>
#include <Graphics.hpp>
#include <GraphicsObjectList.hpp>

#include <QFont>
#include <QString>
#include <MainWindow.hpp>

#include <WorkPanelList.hpp>

#include <ibase.h>
#include <SmallString.hpp>

#include <FirebirdDatabase.hpp>
#include <FirebirdTransaction.hpp>
#include <FirebirdQuerySQL.hpp>
#include <FirebirdItem.hpp>

#include "CheminfoProject.hpp"
#include "CheminfoProjectModule.hpp"
#include "CheminfoProjectSetDatabase.hpp"
#include "CheminfoProjectSetDatabase.moc"

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CheminfoProjectSetDatabaseCB(void* p_data);

CExtUUID        CheminfoProjectSetDatabaseID(
                    "{CHEMINFO_PROJECT_SET_DATABASE:f61eebaf-f15b-4358-8fb7-096d79542fb2}",
                    "Set Cheminfo database");

CPluginObject   CheminfoProjectSetDatabaseObject(&CheminfoProjectPlugin,
                    CheminfoProjectSetDatabaseID,WORK_PANEL_CAT,
                    CheminfoProjectSetDatabaseCB);

// -----------------------------------------------------------------------------

QObject* CheminfoProjectSetDatabaseCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CheminfoProjectSetDatabase requires active project");
        return(NULL);
    }
    // only one instance is allowed
    CWorkPanel* p_object = WorkPanels->Find(CheminfoProjectSetDatabaseID,p_project);
    if( p_object == NULL ) {
        p_object = new CCheminfoProjectSetDatabase(p_project);
        p_object->ShowAsDialog();
        delete p_object;
    }
    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectSetDatabase::CCheminfoProjectSetDatabase(CProject* p_project)
    : CWorkPanel(&CheminfoProjectSetDatabaseObject,p_project,EWPR_DIALOG)
{
    WidgetUI.setupUi(this);

    CheminfoProject = dynamic_cast<CCheminfoProject*>(p_project);
    CCheminfoProjectDatabase* p_database = CheminfoProject->GetDatabase();

    if (p_database->AreInitValuesFill())
    {
        QFileInfo dbFile(p_database->GetDatabaseName().GetBuffer());
        QString name = dbFile.fileName();
        QString path = dbFile.path();
        WidgetUI.dbTypeCB->setCurrentIndex(p_database->GetDatabaseType());
        WidgetUI.dbNameLine->setText(name);
        WidgetUI.dbPathLine->setText(path);
        WidgetUI.dbUserLine->setText(p_database->GetDatabaseUserName().GetBuffer());
        WidgetUI.dbPasswordLine->setText(p_database->GetDatabasePassword().GetBuffer());
    }


    // signals -----------------------------------
    connect(WidgetUI.connectPB, SIGNAL(clicked(bool)),
            this, SLOT(ConnectDatabaseClicked(bool)));
    // -------------
    connect(WidgetUI.loadPB, SIGNAL(clicked(bool) ),
            this, SLOT(BrowseForDBClicked(bool)) );
    // -------------
    connect(WidgetUI.dbPathLine, SIGNAL(editingFinished(void) ),
            this, SLOT(ChangeConnectPBFocus(void)));
    // -------------
    connect(WidgetUI.cancelPB, SIGNAL(clicked(bool)),
            this, SLOT(close(void)));
    // load work panel setup
    LoadWorkPanelSetup();


}

//------------------------------------------------------------------------------

CCheminfoProjectSetDatabase::~CCheminfoProjectSetDatabase(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectSetDatabase::ChangeConnectPBFocus(void)
{
    WidgetUI.connectPB->setDefault(true);
    //WidgetUI.connectPB->setFocus();
}

//------------------------------------------------------------------------------

void CCheminfoProjectSetDatabase::ConnectDatabaseClicked (bool checked)
{
    if(CheminfoProject == NULL) {
        QMessageBox::critical(NULL,"Error","Cheminfo project must be open!",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
    
    CSmallString    database,path,user,password;

    int dbtype = WidgetUI.dbTypeCB->currentIndex();
    database = WidgetUI.dbNameLine->text();
    path = WidgetUI.dbPathLine->text();
    user = WidgetUI.dbUserLine->text();
    password = WidgetUI.dbPasswordLine->text();
    
    // we need only path other is on user
    if(database.IsEmpty()) {
        QMessageBox::critical(NULL,"Error","Database init values was not fully filled!",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }

    unsigned int path_l = path.GetLength();
    
    if (path.GetSubStringFromTo(path_l-1,path_l) != "/") {
        path += "/";
    }

    switch(dbtype) {
    case 0:
        CheminfoProject->GetDatabase()->ConnectDatabaseFirebird(path+database,user,password);
        break;
    }

    if (WidgetUI.saveSettingsCB->checkState() == Qt::Checked) {
        CheminfoProject->GetDatabase()->SaveDatabaseSettings();
    }

    if (CheminfoProject->GetDatabase()->GetFirebird()->IsLogged()){
        this->close();
    }
}

//------------------------------------------------------------------------------

void CCheminfoProjectSetDatabase::BrowseForDBClicked (bool checked)
{
    QStringList filters;
    filters << "Firebird Database format (*.fdb)";
    filters << "All files (*.*)";

    // open qfiledialog for file open with filters set correctly
    QFileDialog* p_dialog = new QFileDialog(CheminfoProject->GetMainWindow());

    p_dialog->setWindowTitle(tr("Select for Firebird Database File"));

    p_dialog->setNameFilters(filters);
    p_dialog->setDirectory(QString(GlobalSetup->GetLastOpenFilePath(CheminfoProjectDatabaseID)));
    p_dialog->setFileMode(QFileDialog::ExistingFile);
    p_dialog->setAcceptMode(QFileDialog::AcceptOpen);

    if( p_dialog->exec() == QDialog::Accepted ){
        UpdateValues(p_dialog->selectedFiles().at(0));
        GlobalSetup->SetLastOpenFilePathFromFile(p_dialog->selectedFiles().at(0),CheminfoProjectDatabaseID);
    }

    delete p_dialog;
    ChangeConnectPBFocus();
    WidgetUI.dbUserLine->setFocus();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectSetDatabase::UpdateValues (const QString& file)
{
    QFileInfo selectedFile(file);
    QString name = selectedFile.fileName();
    QString path = selectedFile.path();
    WidgetUI.dbNameLine->setText(name);
    WidgetUI.dbPathLine->setText(path);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
