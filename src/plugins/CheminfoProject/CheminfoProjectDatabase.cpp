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

#include <QMessageBox>
#include <QFileInfo>

#include <ErrorSystem.hpp>
#include <CategoryUUID.hpp>
#include <XMLElement.hpp>
#include <XMLDocument.hpp>
#include <XMLParser.hpp>
#include <XMLPrinter.hpp>
#include <GlobalSetup.hpp>
#include <FileSystem.hpp>

#include "CheminfoProject.hpp"
#include "CheminfoProjectModule.hpp"
#include "CheminfoProjectDatabase.hpp"
#include "CheminfoProjectDatabase.moc"


//------------------------------------------------------------------------------

QObject* CheminfoProjectDatabaseCB(void* p_data);

CExtUUID        CheminfoProjectDatabaseID(
                    "{CHEMINFO_DATABASE:1644a4e6-06a9-4f07-9ea3-9d4ca4469f59}",
                    "Cheminfo database");

CPluginObject   CheminfoProjectDatabaseObject(&CheminfoProjectPlugin,
                    CheminfoProjectDatabaseID,CHEMINFO_CAT,
                    ":/images/CheminfoProject/12.cheminfo/Database.svg",
                    CheminfoProjectDatabaseCB);

// -----------------------------------------------------------------------------

QObject* CheminfoProjectDatabaseCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CCheminfoProjectDatabase requires active project");
        return(NULL);
    }

    QObject* p_obj = new CCheminfoProjectDatabase(p_project);
    return(p_obj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectDatabase::CCheminfoProjectDatabase(CProject* p_project)
    : CProObject(&CheminfoProjectDatabaseObject,p_project,p_project)
{
    FBdb = new CFirebirdDatabase();
}

//------------------------------------------------------------------------------

CCheminfoProjectDatabase::~CCheminfoProjectDatabase(void)
{
    DisconnectDatabaseFirebird();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectDatabase::EmitOnDatabaseConnectChanged(void)
{
    emit OnDatabaseConnectChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectDatabase::ConnectDatabaseFirebird (const CSmallString& dbname, const CSmallString& user,const CSmallString& password)
{
    if (FBdb->IsLogged()) {
        QString error;
        error = DBName.GetBuffer();
        error += " is already logged.\nLogout first to login another database!";
        QMessageBox::critical(NULL,"Error",error,QMessageBox::Ok,QMessageBox::Ok);
        return;
    }

    DBName = dbname;
    DBUser = user;
    DBPassword = password;

    FBdb->SetDatabaseName(DBName);

    if(FBdb->Login(DBUser,DBPassword) == false) {
        QString error;
        error = "Unable to login to database ";
        error += DBName.GetBuffer();
        QMessageBox::critical(NULL,"Error",error,QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
    DBType = FIREBIRD;
    EmitOnDatabaseConnectChanged();
}

//------------------------------------------------------------------------------

void CCheminfoProjectDatabase::ConnectDatabaseFirebird (void)
{
    if (FBdb->IsLogged()) {
        QString error;
        error = DBName.GetBuffer();
        error += " is already logged.\nLogout first to login another database!";
        QMessageBox::critical(NULL,"Error",error,QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
    if(!AreInitValuesFill()) {
        QMessageBox::critical(NULL,"Error","None database init values was filled!",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }

    FBdb->SetDatabaseName(DBName);

    if(FBdb->Login(DBUser,DBPassword) == false) {
        QString error;
        error = "Unable to login to database ";
        error += DBName.GetBuffer();
        QMessageBox::critical(NULL,"Error",error,QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
    DBType = FIREBIRD;
    EmitOnDatabaseConnectChanged();
}

//------------------------------------------------------------------------------

void CCheminfoProjectDatabase::DisconnectDatabaseFirebird (void)
{
    if (FBdb->IsLogged()) {
        FBdb->Logout();
    }
    EmitOnDatabaseConnectChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFirebirdDatabase* CCheminfoProjectDatabase::GetFirebird(void)
{
    return(FBdb);
}

//------------------------------------------------------------------------------

bool CCheminfoProjectDatabase::AreInitValuesFill(void)
{
    if (DBName.IsEmpty() || DBUser.IsEmpty() || DBPassword.IsEmpty())
    {
        return (false);
    }
    else
    {
        return(true);
    }
}

//------------------------------------------------------------------------------

bool CCheminfoProjectDatabase::IsLogged(void)
{
    return(FBdb->IsLogged());
}

//------------------------------------------------------------------------------

CSmallString CCheminfoProjectDatabase::GetDatabaseName(void)
{
    return(DBName);
}

//------------------------------------------------------------------------------

CSmallString CCheminfoProjectDatabase::GetDatabaseUserName(void)
{
    return(DBUser);
}

//------------------------------------------------------------------------------

CSmallString CCheminfoProjectDatabase::GetDatabasePassword(void)
{
    return(DBPassword);
}

//------------------------------------------------------------------------------

TypeDBMS CCheminfoProjectDatabase::GetDatabaseType(void)
{
    return(DBType);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


void CCheminfoProjectDatabase::SetDatabaseName(CSmallString dbname)
{
    DBName = dbname;
}

//------------------------------------------------------------------------------

void CCheminfoProjectDatabase::SetDatabaseUserName(CSmallString dbuser)
{
    DBUser = dbuser;
}

//------------------------------------------------------------------------------

void CCheminfoProjectDatabase::SetDatabasePassword(CSmallString dbpassword)
{
    DBPassword = dbpassword;
}

//------------------------------------------------------------------------------

void CCheminfoProjectDatabase::SetDatabaseType(TypeDBMS dbtype)
{
    DBType = dbtype;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCheminfoProjectDatabase::SaveDatabaseSettings(void)
{
    CFileName     config_name;

    config_name = GlobalSetup->GetUserSetupPath() / "cheminfo_database.xml";

    CXMLDocument xml_doc;

    xml_doc.CreateChildDeclaration();
    xml_doc.CreateChildComment("Cheminfo Database settings");

    CXMLElement* p_ele = xml_doc.CreateChildElement("setup");

    QFileInfo dbFile(DBName.GetBuffer());
    QString name = dbFile.fileName();
    QString path = dbFile.path();

    // setup goes here
    p_ele->SetAttribute("dbtype",DBType);
    p_ele->SetAttribute("dbname",name);
    p_ele->SetAttribute("dbpath",path);
    p_ele->SetAttribute("dbuser",DBUser);
    p_ele->SetAttribute("dbpassword",DBPassword);

    CXMLPrinter xml_printer;
    xml_printer.SetPrintedXMLNode(&xml_doc);

    if( xml_printer.Print(config_name) == false ) {
        QMessageBox::critical(NULL,"Error","Database init values cannot be saved!",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CCheminfoProjectDatabase::LoadDatabaseSettings(void)
{
    // load config
    CFileName     config_name;

    config_name = GlobalSetup->GetUserSetupPath() / "cheminfo_database.xml"; // first user setup
    if( CFileSystem::IsFile(config_name) == false ) {
        // if not present - try to load system config
        config_name = GlobalSetup->GetSystemSetupPath() / "cheminfo_database.xml"; // system setup
    }

    if( CFileSystem::IsFile(config_name) == false ){
        return(false);
    }

    CXMLDocument xml_doc;

    CXMLParser xml_parser;
    xml_parser.SetOutputXMLNode(&xml_doc);

    if( xml_parser.Parse(config_name) == false ) {
         ES_ERROR("Unable to load database setup profile!");
        return(false);
    }

    CXMLElement* p_ele = xml_doc.GetFirstChildElement("setup");
    if( p_ele == NULL ) {
        ES_ERROR("Unable to open database setup element!");
        return(false);
    }

    CSmallString dbtype;
    CSmallString name;
    CSmallString path;
    CSmallString dbuser;
    CSmallString dbpassword;

    p_ele->GetAttribute("dbtype",dbtype);
    p_ele->GetAttribute("dbname",name);
    p_ele->GetAttribute("dbpath",path);

    unsigned int path_l = path.GetLength();

    if (path.GetSubStringFromTo(path_l-1,path_l) != "/") {
        path += "/";
    }

    p_ele->GetAttribute("dbuser",dbuser);
    p_ele->GetAttribute("dbpassword",dbpassword);

    SetDatabaseName(path + name);
    SetDatabaseUserName(dbuser);
    SetDatabasePassword(dbpassword);
    SetDatabaseType(TypeDBMS(dbtype.ToInt()) );

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
