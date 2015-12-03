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

#include <QMessageBox>

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
#include "CheminfoProjectDatabaseStructure.hpp"
#include "CheminfoProjectDatabaseStructure.moc"

#include <openbabel/obconversion.h>

#include <vector>

using namespace std;
using namespace OpenBabel;

//------------------------------------------------------------------------------

QObject* CheminfoProjectDatabaseStructureCB(void* p_data);

CExtUUID        CheminfoProjectDatabaseStructureID(
                    "{CHEMINFO_DATABASE_STRUCTURE:b6acc4b0-f170-492d-a24c-a3156a9ba286}",
                    "Cheminfo Database Structure");

CPluginObject   CheminfoProjectDatabaseStructureObject(&CheminfoProjectPlugin,
                    CheminfoProjectDatabaseStructureID,CHEMINFO_CAT,
                    ":/images/CheminfoProject/12.cheminfo/DatabaseStructure.svg",
                    CheminfoProjectDatabaseStructureCB);

// -----------------------------------------------------------------------------

QObject* CheminfoProjectDatabaseStructureCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CCheminfoProjectDatabaseStructure requires active project");
        return(NULL);
    }

    QObject* p_obj = new CCheminfoProjectDatabaseStructure(p_project);
    return(p_obj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectDatabaseStructure::CCheminfoProjectDatabaseStructure(CProject *p_project)
    : CProObject(&CheminfoProjectDatabaseStructureObject,p_project,p_project)
{
    // default set is hierarchy structure database
    Hierarchy = true;
    Loaded = false;
//    DBLigandType = NULL;
//    DBResultsType = NULL;
    DBLigandTypeNumber = 0;
    DBResultsTypeNumber = 0;

    // get list of supported babel formats -------
    OBConversion co;
    vector<string> formats = co.GetSupportedInputFormat();

    // parse formats list ------------------------
    QStringList supported_formats;

    // generate filters for all of supported openbabel formats

    for(unsigned int i = 0; i < formats.size(); i++) {
        CSmallString formatString, extension;
        formatString = formats.at(i).c_str();
        int pos = formatString.FindSubString("--");
        if( pos != -1 ) {
            extension = formatString.GetSubString(0, pos-1);
            formatString << " (*." << extension << ")";
            supported_formats << formatString.GetBuffer();
        }
    }

    DBLigandFormats << "mol2 -- Sybyl Mol2 format (*.mol2)";
    DBLigandFormats << "pdb -- Protein Data Bank format (*.pdb)";
    DBLigandFormats << supported_formats;
    DBResultsFormats << "pdbqt -- Autodock PDBQT format (*.pdbqt)";
    DBResultsFormats << supported_formats;
}

//------------------------------------------------------------------------------

CCheminfoProjectDatabaseStructure::~CCheminfoProjectDatabaseStructure(void)
{

}
//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectDatabaseStructure::Load(void)
{
    Loaded = true;
    /// TODO check if directory contain files with choosen prefix
    EmitOnDatabaseFillChanged();
}

//------------------------------------------------------------------------------

void CCheminfoProjectDatabaseStructure::Unload(void)
{
    Loaded = false;
    EmitOnDatabaseFillChanged();

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCheminfoProjectDatabaseStructure::AreInitValuesFill(void)
{
    // DB prefix may be empty
    if (DBPath.IsEmpty() || DBLigandType.IsEmpty() || DBResultsType.IsEmpty())
    {
        return (false);
    }
    else
    {
        return(true);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSmallString CCheminfoProjectDatabaseStructure::GetPath(void)
{
    return(DBPath);
}

//------------------------------------------------------------------------------

CSmallString CCheminfoProjectDatabaseStructure::GetPrefix(void)
{
    return(DBPrefix);
}

//------------------------------------------------------------------------------

CSmallString CCheminfoProjectDatabaseStructure::GetLigandType(void)
{
    return(DBLigandType);
}

//------------------------------------------------------------------------------

CSmallString CCheminfoProjectDatabaseStructure::GetResultsType(void)
{
    return(DBResultsType);
}

//------------------------------------------------------------------------------

const QStringList CCheminfoProjectDatabaseStructure::GetLigandFormats(void)
{
    return(DBLigandFormats);
}

//------------------------------------------------------------------------------

const QStringList CCheminfoProjectDatabaseStructure::GetResultsFormats(void)
{
    return(DBResultsFormats);
}

//------------------------------------------------------------------------------

int CCheminfoProjectDatabaseStructure::GetLigandTypeNum(void) const
{
    return(DBLigandTypeNumber);
}

//------------------------------------------------------------------------------

int CCheminfoProjectDatabaseStructure::GetResultsTypeNum(void) const
{
    return(DBResultsTypeNumber);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectDatabaseStructure::SetPath(CSmallString dbpath)
{
    DBPath = dbpath;
    EmitOnDatabaseFillChanged();
}

//------------------------------------------------------------------------------

void CCheminfoProjectDatabaseStructure::SetPrefix(CSmallString dbprefix)
{
    DBPrefix = dbprefix;
    EmitOnDatabaseFillChanged();
}

//------------------------------------------------------------------------------

void CCheminfoProjectDatabaseStructure::SetLigandType(CSmallString dbligtype, int typenum)
{
    DBLigandType = dbligtype;
    DBLigandTypeNumber = typenum;
    EmitOnDatabaseFillChanged();
}

//------------------------------------------------------------------------------

void CCheminfoProjectDatabaseStructure::SetResultsType(CSmallString dbrestype, int typenum)
{
    DBResultsType = dbrestype;
    DBResultsTypeNumber = typenum;
    EmitOnDatabaseFillChanged();
}

//------------------------------------------------------------------------------

void CCheminfoProjectDatabaseStructure::SetHierarchy(bool dbhierarchy)
{
    Hierarchy = dbhierarchy;
    EmitOnDatabaseFillChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCheminfoProjectDatabaseStructure::SaveDatabaseStructureSettings(void)
{
    CFileName     config_name;

    config_name = GlobalSetup->GetUserSetupPath() / "cheminfo_database_structure.xml";

    CXMLDocument xml_doc;

    xml_doc.CreateChildDeclaration();
    xml_doc.CreateChildComment("Cheminfo Structure database settings");

    CXMLElement* p_ele = xml_doc.CreateChildElement("setup");

    // setup goes here
    p_ele->SetAttribute("dbpath",DBPath);
    p_ele->SetAttribute("dbprefix",DBPrefix);
    p_ele->SetAttribute("dbligandtype",DBLigandType);
    p_ele->SetAttribute("dbresultstype",DBResultsType);
    p_ele->SetAttribute("dbligandtypenum",DBLigandTypeNumber);
    p_ele->SetAttribute("dbresultstypenum",DBResultsTypeNumber);
    p_ele->SetAttribute("dbhierarchy",Hierarchy);

    CXMLPrinter xml_printer;
    xml_printer.SetPrintedXMLNode(&xml_doc);

    if( xml_printer.Print(config_name) == false ) {
        QMessageBox::critical(NULL,"Error","Structure database init values cannot be saved!",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CCheminfoProjectDatabaseStructure::LoadDatabaseStructureSettings(void)
{
    // load config
    CFileName     config_name;

    config_name = GlobalSetup->GetUserSetupPath() / "cheminfo_database_structure.xml"; // first user setup
    if( CFileSystem::IsFile(config_name) == false ) {
        // if not present - try to load system config
        config_name = GlobalSetup->GetSystemSetupPath() / "cheminfo_database_structure.xml"; // system setup
    }

    if( CFileSystem::IsFile(config_name) == false ){
        return(false);
    }

    CXMLDocument xml_doc;

    CXMLParser xml_parser;
    xml_parser.SetOutputXMLNode(&xml_doc);

    if( xml_parser.Parse(config_name) == false ) {
        ES_ERROR("Unable to load Structure database setup profile!");
        return(false);
    }

    CXMLElement* p_ele = xml_doc.GetFirstChildElement("setup");
    if( p_ele == NULL ) {
         ES_ERROR("Unable to open Structure database setup element!");
        return(false);
    }

    CSmallString dbpath;
    CSmallString dbprefix;
    CSmallString dbligandtype;
    CSmallString dbresultstype;
    CSmallString dbligandtypenum, dbresultstypenum;
    CSmallString dbhierarchy;

    p_ele->GetAttribute("dbpath",dbpath);
    p_ele->GetAttribute("dbprefix",dbprefix);
    p_ele->GetAttribute("dbligandtype",dbligandtype);
    p_ele->GetAttribute("dbresultstype",dbresultstype);
    p_ele->GetAttribute("dbligandtypenum",dbligandtypenum);
    p_ele->GetAttribute("dbresultstypenum",dbresultstypenum);
    p_ele->GetAttribute("dbhierarchy",dbhierarchy);

    SetHierarchy(dbhierarchy.ToBool());
    SetPath(dbpath);
    SetPrefix(dbprefix);
    SetLigandType(dbligandtype,dbligandtypenum.ToInt());
    SetResultsType(dbresultstype,dbresultstypenum.ToInt());

    Load();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCheminfoProjectDatabaseStructure::IsHierarchy(void)
{
    return(Hierarchy);
}

//------------------------------------------------------------------------------

bool CCheminfoProjectDatabaseStructure::IsLoaded(void)
{
    return(Loaded);
}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectDatabaseStructure::EmitOnDatabaseFillChanged(void)
{
    emit OnDatabaseFillChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
