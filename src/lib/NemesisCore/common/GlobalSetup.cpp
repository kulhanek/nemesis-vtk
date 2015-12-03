// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2013        Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010        Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2009        Petr Kulhanek, kulhanek@chemi.muni.cz,
//                              Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 2008        Petr Kulhanek, kulhanek@enzim.hu,
//                              Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004   Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <GlobalSetup.hpp>
#include <XMLElement.hpp>
#include <XMLParser.hpp>
#include <XMLPrinter.hpp>
#include <XMLDocument.hpp>
#include <FileSystem.hpp>
#include <ErrorSystem.hpp>
#include <SystemFS.hpp>
#include <QCoreApplication>
#include <Project.hpp>
#include <PluginDatabase.hpp>
#include <iomanip>

#include "GlobalSetup.moc"

#define VERSION "14.x"

//------------------------------------------------------------------------------

using namespace std;

//------------------------------------------------------------------------------

CGlobalSetup* GlobalSetup = NULL;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CGlobalSetup::Init(void)
{
    // QT returns path with / delimiter no matter, which OS is used
    AppDir.SetFromUNIX(QCoreApplication::applicationDirPath());

    // remove bin directory
    PackageDir = AppDir.GetFileDirectory();

#if defined WIN32
    QCoreApplication::addLibraryPath(QString(PackageDir / "lib" / "qtplugins"));
#endif

    // global setup
    SystemSetup                = PackageDir / "etc";
    PluginsSetup               = SystemSetup / "plugins";
    PluginsLocation            = PackageDir / "lib";
    SystemGraphicsSetupName    = SystemSetup / "graphics";
    SystemDesktopPath          = SystemSetup / "desktop";
    DocPath                    = PackageDir / "doc";
    SharePath                  = PackageDir / "share";
    FontDir                    = SharePath / "fonts";

    // user setup
    UserSetup = CSystemFS::GetUserSetupDir() / ".nemesis" / VERSION;

    if( CFileSystem::CreateDir(UserSetup) == false ) {
        ES_ERROR("unable to create user setup directory");
        return(false);
    }

    UserGraphicsSetupPath    = UserSetup / "graphics";
    if( CFileSystem::CreateDir(UserGraphicsSetupPath) == false ) {
        ES_ERROR("unable to create user graphics setup directory");
        return(false);
    }

    UserDesktopPath    = UserSetup / "desktop";
    if( CFileSystem::CreateDir(UserDesktopPath) == false ) {
        ES_ERROR("unable to create user desktop setup directory");
        return(false);
    }

    CXMLDocument xml_doc;
    CXMLParser   xml_parser;

    xml_parser.SetOutputXMLNode(&xml_doc);

    LastOpenFilePath = CSystemFS::GetUserDocumentsDir();
    FixedLastOpenFilePath = CSystemFS::GetUserDocumentsDir();
    LastOpenFileMode = ELOFM_USE_SINGLE_PATH;


    if( CFileSystem::IsFile(UserSetup / "paths.xml") == false ) {
        return(true); // no path setup
    }


    // try to load path setup
    if( xml_parser.Parse(UserSetup / "paths.xml") == false ) {
        ES_WARNING("unable to load user path setup");
        return(true);
    }

    CXMLElement* p_ele = xml_doc.GetChildElementByPath("paths");
    if( p_ele == NULL ) {
        ES_ERROR("unable to open path element");
        return(true);
    }

    p_ele->GetAttribute("lastopenfilepath",LastOpenFilePath);
    p_ele->GetAttribute("fixedlastopenfilepath",FixedLastOpenFilePath);
    p_ele->GetAttribute<ELastOpenFileMode>("lastopenfilemode",LastOpenFileMode);

    CXMLElement* p_iele = p_ele->GetChildElementByPath("module_paths/item");

    while( p_iele != NULL ){
        CSmallString suuid;
        CSmallString path;
        p_iele->GetAttribute("module",suuid);
        p_iele->GetAttribute("path",path);

        CUUID uuid(suuid);
        LastOpenFilePathPerModule[uuid] = path;

        p_iele = p_iele->GetNextSiblingElement("item");
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CGlobalSetup::Save(void)
{
    CXMLDocument xml_doc;
    CXMLPrinter  xml_printer;

    xml_printer.SetPrintedXMLNode(&xml_doc);

    xml_doc.CreateChildDeclaration();
    xml_doc.CreateChildComment("NEMESIS user path setup");

    CXMLElement* p_ele = xml_doc.CreateChildElement("paths");

    p_ele->SetAttribute("lastopenfilepath",LastOpenFilePath);
    p_ele->SetAttribute("fixedlastopenfilepath",FixedLastOpenFilePath);
    p_ele->SetAttribute("lastopenfilemode",LastOpenFileMode);

    CXMLElement* p_mele = p_ele->CreateChildElement("module_paths");

    std::map<CUUID,CFileName>::iterator it = LastOpenFilePathPerModule.begin();
    std::map<CUUID,CFileName>::iterator ie = LastOpenFilePathPerModule.end();

    while( it != ie ){
        CXMLElement* p_iele = p_mele->CreateChildElement("item");
        CUUID uuid = it->first;
        p_iele->SetAttribute("module",uuid);
        p_iele->SetAttribute("path",it->second);
        it++;
    }

    if( xml_printer.Print(UserSetup / "paths.xml") == false ) {
        ES_ERROR("unable to save user path setup");
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGlobalSetup::PrintSetup(ostream& vout)
{
    vout << endl;
    vout << "Nemesis global path setup:" << endl;
    vout << "   Application path          : " << AppDir << endl;
    vout << "   Package path              : " << PackageDir << endl;
    vout << "   Plugins setup             : " << PluginsSetup << endl;
    vout << "   Plugins location          : " << PluginsLocation << endl;
    vout << "   Graphics setup            : " << SystemGraphicsSetupName << endl;
    vout << "   Desktop setup             : " << SystemDesktopPath << endl;
    vout << "   Share path                : " << SharePath << endl;
    vout << "   FTGL Fonts                : " << FontDir << endl;
    vout << endl;
    vout << "User path setup:" << endl;
    vout << "   Graphics setup            : " << UserGraphicsSetupPath << endl;
    vout << "   Desktop setup             : " << UserDesktopPath << endl;
    switch(LastOpenFileMode){
        case ELOFM_USE_FIXED_PATH:
    vout << "   Last open file mode       : fixed path" << endl;
        break;
        case ELOFM_USE_SINGLE_PATH:
    vout << "   Last open file mode       : last single path" << endl;
        break;
        case ELOFM_USE_PATH_PER_MODULE:
    vout << "   Last open file mode       : last path per module" << endl;
        break;
    }

    vout << "   Fixed last open file path : " << FixedLastOpenFilePath << endl;
    vout << "   Last open file path       : " << LastOpenFilePath << endl;
    vout << "   Registered module paths" << endl;

    std::map<CUUID,CFileName>::iterator it = LastOpenFilePathPerModule.begin();
    std::map<CUUID,CFileName>::iterator ie = LastOpenFilePathPerModule.end();

    while( it != ie ){
        CUUID uuid = it->first;
        CExtUUID ext_uuid;
        PluginDatabase.GetObjectExtType(uuid,ext_uuid);
        vout << "   " << setw(15) << ext_uuid.GetName().toStdString() << " " << string(it->second) << endl;
        it++;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const CFileName& CGlobalSetup::GetSystemSetupPath(void) const
{
    return(SystemSetup);
}

//------------------------------------------------------------------------------

const CFileName& CGlobalSetup::GetPluginsLocation(void) const
{
    return(PluginsLocation);
}

//---------------------------------------------------------------------------

const CFileName& CGlobalSetup::GetPluginsSetup(void) const
{
    return(PluginsSetup);
}

//---------------------------------------------------------------------------

const CFileName& CGlobalSetup::GetDocPath(void) const
{
    return(DocPath);
}


//---------------------------------------------------------------------------

const CFileName& CGlobalSetup::GetSystemGraphicsSetupPath(void) const
{
    return(SystemGraphicsSetupName);
}

//---------------------------------------------------------------------------

const CFileName& CGlobalSetup::GetSystemDesktopPath(void) const
{
    return(SystemDesktopPath);
}

//---------------------------------------------------------------------------

const CFileName& CGlobalSetup::GetSystemSharePath(void) const
{
    return(SharePath);
}

//---------------------------------------------------------------------------

const CFileName CGlobalSetup::GetSystemDesktopPath(CProject* p_project,const CFileName& file) const
{
    CFileName fullfile;
    fullfile = SystemDesktopPath / p_project->GetType().GetMaskedStringForm() / file;
    return(fullfile);
}

//---------------------------------------------------------------------------

CFileName CGlobalSetup::GetWorkPanelSetupPath(const CExtUUID& type) const
{
    CFileName path;
    path = SystemSetup / "workpanels" / type.GetMaskedStringForm();
    return(path);
}

//------------------------------------------------------------------------------

const CFileName& CGlobalSetup::GetFontPath(void) const
{
    return(FontDir);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const CFileName& CGlobalSetup::GetUserSetupPath(void) const
{
    return(UserSetup);
}

//------------------------------------------------------------------------------

const CFileName CGlobalSetup::GetLastOpenFilePath(void) const
{
    CFileName last_file_path;

    switch(LastOpenFileMode){
        case ELOFM_USE_FIXED_PATH:
            last_file_path = FixedLastOpenFilePath;
            break;

        case ELOFM_USE_SINGLE_PATH:
            last_file_path = LastOpenFilePath;
            break;

        case ELOFM_USE_PATH_PER_MODULE:
            last_file_path = LastOpenFilePath;
            break;
    }

    if( CFileSystem::IsDirectory(last_file_path) == false ){
        last_file_path = CSystemFS::GetUserDocumentsDir();
    }
    return(last_file_path);
}

//------------------------------------------------------------------------------

const CFileName CGlobalSetup::GetLastOpenFilePath(const CUUID& module) const
{
    CFileName last_file_path;

    switch(LastOpenFileMode){
        case ELOFM_USE_FIXED_PATH:
            last_file_path = FixedLastOpenFilePath;
            break;

        case ELOFM_USE_SINGLE_PATH:
            last_file_path = LastOpenFilePath;
            break;

        case ELOFM_USE_PATH_PER_MODULE:
            last_file_path = LastOpenFilePath;
            if( LastOpenFilePathPerModule.find(module) != LastOpenFilePathPerModule.end() ){
                last_file_path = LastOpenFilePathPerModule.find(module)->second;
            }
            break;
    }

    if( CFileSystem::IsDirectory(last_file_path) == false ){
        last_file_path = CSystemFS::GetUserDocumentsDir();
    }
    return(last_file_path);
}

//---------------------------------------------------------------------------

void CGlobalSetup::SetLastOpenFilePath(const CFileName& path,const CUUID& module)
{
    LastOpenFilePath = path;
    LastOpenFilePathPerModule[module] = path;
}

//---------------------------------------------------------------------------

void CGlobalSetup::SetLastOpenFilePathFromFile(const QString& file,const CUUID& module)
{
    SetLastOpenFilePath(CFileName(file).GetFileDirectory(),module);
}

//---------------------------------------------------------------------------

const CFileName& CGlobalSetup::GetFixedLastOpenFilePath(void) const
{
    return(FixedLastOpenFilePath);
}

//---------------------------------------------------------------------------

void CGlobalSetup::SetFixedLastOpenFilePath(const CFileName& path)
{
    FixedLastOpenFilePath = path;
}

//---------------------------------------------------------------------------

std::map<CUUID,CFileName>& CGlobalSetup::GetLastFileModulePaths(void)
{
    return(LastOpenFilePathPerModule);
}

//---------------------------------------------------------------------------

void CGlobalSetup::ClearLastFileModelPaths(void)
{
    LastOpenFilePathPerModule.clear();
}

//---------------------------------------------------------------------------

ELastOpenFileMode CGlobalSetup::GetLastOpenFileMode(void) const
{
    return(LastOpenFileMode);
}

//---------------------------------------------------------------------------

void CGlobalSetup::SetLastOpenFileMode(ELastOpenFileMode set)
{
    LastOpenFileMode = set;
}

//---------------------------------------------------------------------------

const CFileName& CGlobalSetup::GetUserGraphicsSetupPath(void) const
{
    return(UserGraphicsSetupPath);
}

//---------------------------------------------------------------------------

const CFileName& CGlobalSetup::GetUserDesktopPath(void) const
{
    return(UserDesktopPath);
}

//---------------------------------------------------------------------------

const CFileName CGlobalSetup::GetUserDesktopPath(CProject* p_project,const CFileName& file) const
{
    CFileName dirfile;
    dirfile = UserDesktopPath / p_project->GetType().GetMaskedStringForm();
    CFileName fullfile;
    fullfile = dirfile / file;

    if( CFileSystem::CreateDir(dirfile) == false ) {
        ES_ERROR("unable to create user setup directory");
    }

    return(fullfile);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



