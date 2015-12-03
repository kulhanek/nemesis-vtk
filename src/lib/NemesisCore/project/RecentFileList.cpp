// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012        Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <RecentFileList.hpp>
#include <XMLElement.hpp>
#include <XMLParser.hpp>
#include <XMLPrinter.hpp>
#include <XMLDocument.hpp>
#include <FileSystem.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <GlobalSetup.hpp>
#include <QMessageBox>
#include <PluginDatabase.hpp>

#include "RecentFileList.moc"

//------------------------------------------------------------------------------

using namespace std;

//------------------------------------------------------------------------------

CRecentFileList* RecentFiles = NULL;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRecentFileList::CRecentFileList(QObject* p_parent)
    : QObject(p_parent)
{

}

//------------------------------------------------------------------------------

bool CRecentFileList::Load(void)
{
    CFileName full_file_name;
    full_file_name = GlobalSetup->GetUserSetupPath() / "recent.xml";

    if( CFileSystem::IsFile(full_file_name) == false ) return(false);

    CXMLDocument xml_doc;
    CXMLParser   xml_parser;
    xml_parser.SetOutputXMLNode(&xml_doc);

    if( xml_parser.Parse(full_file_name) == false ) {
        ES_ERROR("unable to open recent file list");
        return(false);
    }

    CXMLElement* p_ele = xml_doc.GetFirstChildElement("list");
    if( p_ele == NULL ) {
        ES_ERROR("unable to open list element");
        return(false);
    }

    // load recent file names
    CXMLElement* p_tele = p_ele->GetFirstChildElement("file");
    while(p_tele != NULL) {
        CRecentFilePtr p_item = CRecentFilePtr(new CRecentFile);
        if( p_item->Load(p_tele) == true ){
            RecentFiles.push_back(p_item);
        }
        p_tele = p_tele->GetNextSiblingElement("file");
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CRecentFileList::Save(void)
{
    CXMLDocument xml_doc;
    xml_doc.CreateChildDeclaration();
    xml_doc.CreateChildComment("NEMESIS recent files");

    CXMLElement* p_ele = xml_doc.CreateChildElement("list");

    std::deque<CRecentFilePtr>::iterator it = RecentFiles.begin();
    std::deque<CRecentFilePtr>::iterator ie = RecentFiles.end();

    while (it != ie) {
        CXMLElement* p_tele = p_ele->CreateChildElement("file");
        (*it)->Save(p_tele);
        it++;
    }

    CFileName full_file_name;
    full_file_name = GlobalSetup->GetUserSetupPath() / "recent.xml";

    CXMLPrinter  xml_printer;
    xml_printer.SetPrintedXMLNode(&xml_doc);

    if( xml_printer.Print(full_file_name) == false ) {
        ES_ERROR("unable to save user recent files");
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CRecentFileList::GetFileName(unsigned int index, QString& name, QString& importer) const
{
    if(index < 0 || RecentFiles.size() <= index) {
        return(false);
    }
    CRecentFilePtr p_item = RecentFiles.at(index);
    name = p_item->GetName();
    if( p_item->GetProcessor().IsValidUUID() ){
        CExtUUID processor;
        PluginDatabase.GetObjectExtType(p_item->GetProcessor(),processor);
        importer = processor.GetName();
    } else {
        importer = "";
    }
    return(true);
}

//------------------------------------------------------------------------------

bool CRecentFileList::OpenFile(unsigned int index,CProject* p_project)
{
    if(index < 0 || RecentFiles.size() <= index) {
        return(false);
    }
    CRecentFilePtr p_item = RecentFiles.at(index);

    if( CFileSystem::IsFile(p_item->GetName()) == false ) {
        ES_ERROR("Recent file was moved or deleted");
        QMessageBox::critical(NULL, tr("Open Recent"),
                              tr("The file not found, it has probably been moved or deleted."),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        RemoveFile(p_item->GetName());
        return(false);
    }

    if( p_item->OpenFile(p_project) == false ){
        // remove the file from the stack
        RemoveFile(p_item->GetName());
    }
    // file is added on the stack by
    //    CImportJob::SetImportedStructure
    // or CProject::PushProjectFile
    return(true);
}

//------------------------------------------------------------------------------

void CRecentFileList::PushProjectFile(const CFileName& file)
{
    // try to remove the file from the list
    RemoveFile(file);

    // put the item to the stack
    CRecentFilePtr p_item = CRecentFilePtr(new CRecentFile(file));
    RecentFiles.push_front(p_item);

    // keep only recent files on the stack
    if(RecentFiles.size() >= 10) {
        // erase the last element
        RecentFiles.pop_back();
    }

    emit OnRecentFileListUpdate();
}

//---------------------------------------------------------------------------

void CRecentFileList::PushFile(const CFileName& file,const CExtUUID& processor)
{
    // try to remove the file from the list
    RemoveFile(file);

    // put the item to the stack
    CRecentFilePtr p_item = CRecentFilePtr(new CRecentFile(file,processor));
    RecentFiles.push_front(p_item);

    // keep only recent files on the stack
    if(RecentFiles.size() >= 10) {
        // erase the last element
        RecentFiles.pop_back();
    }

    emit OnRecentFileListUpdate();
}

//---------------------------------------------------------------------------

void CRecentFileList::RemoveFile(const CFileName& file)
{
    std::deque<CRecentFilePtr>::iterator it = RecentFiles.begin();
    std::deque<CRecentFilePtr>::iterator ie = RecentFiles.end();

    while (it != ie) {
        if((*it)->GetName() == file) {
            RecentFiles.erase(it);
            emit OnRecentFileListUpdate();
            break;
        }
        it++;
    }
}

//---------------------------------------------------------------------------

void CRecentFileList::ClearList(void)
{
    RecentFiles.clear();
    emit OnRecentFileListUpdate();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



