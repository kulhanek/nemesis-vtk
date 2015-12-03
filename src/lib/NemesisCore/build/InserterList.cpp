// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <InserterList.hpp>
#include <Structure.hpp>
#include <ErrorSystem.hpp>
#include <DefaultInserter.hpp>
#include <CategoryUUID.hpp>
#include <NemesisCoreModule.hpp>
#include <Project.hpp>
#include <PluginDatabase.hpp>
#include <GlobalSetup.hpp>
#include <FileSystem.hpp>
#include <XMLParser.hpp>
#include <XMLPrinter.hpp>
#include <Atom.hpp>
#include <Fragment.hpp>
#include <AtomList.hpp>

#include "InserterList.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        InserterListID(
                    "{INSERT_ADAPTER_LIST:f69db352-72d5-47a3-8542-f972daf18455}",
                    "Inserters");

CPluginObject   InserterListObject(&NemesisCorePlugin,
                    InserterListID,GENERIC_CAT,
                    ":/images/NemesisCore/build/InserterList.svg",
                    NULL);

CInserterList*  Inserters = NULL;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CInserterList::CInserterList(CExtComObject* p_parent)
    : CExtComObject(&InserterListObject,p_parent)
{
    ActiveInserter = NULL;
    AutoCenter = false;
    AllowNewStructure = false;
}

//------------------------------------------------------------------------------

CInserterList::~CInserterList(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CInserterList::LoadConfig(void)
{
    // make list of all available inserters
    InsertAllInserters();

    // load config
    CFileName     config_name;

    config_name = GlobalSetup->GetUserSetupPath() / "inserters.xml"; // first user setup
    if( CFileSystem::IsFile(config_name) == false ) {
        // if not present - try to load system config
        config_name = GlobalSetup->GetSystemSetupPath() / "inserters.xml"; // system setup
    }

    if( CFileSystem::IsFile(config_name) == false ) return(true);

    CXMLDocument xml_doc;

    CXMLParser xml_parser;
    xml_parser.SetOutputXMLNode(&xml_doc);

    if( xml_parser.Parse(config_name) == false ) {
        ES_ERROR("unable to load inserters setup profile");
        return(false);
    }

    CXMLElement* p_ele = xml_doc.GetFirstChildElement("setup");
    if( p_ele == NULL ) {
        ES_ERROR("unable to open setup element");
        return(false);
    }

    // load setup
    p_ele->GetAttribute("ac",AutoCenter);
    // do not save this option - it should be always off as default
    // p_ele->GetAttribute("ns",AllowNewStructure);

    return(true);
}

//------------------------------------------------------------------------------

bool CInserterList::SaveConfig(void)
{
    CXMLDocument xml_doc;

    CXMLElement* p_ele = xml_doc.CreateChildElement("setup");

    // save setup
    p_ele->SetAttribute("ac",AutoCenter);
    // do not save this option - it should be always off as default
    // p_ele->SetAttribute("ns",AllowNewStructure);

    // load config
    CFileName     config_name;

    config_name = GlobalSetup->GetUserSetupPath() / "inserters.xml";

    CXMLPrinter xml_printer;
    xml_printer.SetPrintedXMLNode(&xml_doc);

    if( xml_printer.Print(config_name) == false ) {
        ES_ERROR("unable to save inserters setup profile");
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CInserter* CInserterList::GetActiveInserter(void)
{
    return(ActiveInserter);
}

//------------------------------------------------------------------------------

bool CInserterList::IsAutoCenterEnabled(void)
{
    return(AutoCenter);
}

//------------------------------------------------------------------------------

bool CInserterList::IsNewStructureAllowed(void)
{
    return(AllowNewStructure);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CInserterList::SetAutoCenter(bool set)
{
    if( AutoCenter == set ) return;
    AutoCenter = set;
    emit SetupChanged();
}

//------------------------------------------------------------------------------

void CInserterList::SetNewStructureAllowed(bool set)
{
    if( AllowNewStructure == set ) return;
    AllowNewStructure = set;
    emit SetupChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CInserterList::InsertStructureWH(CFragment* p_str,CAtom* p_atom)
{
    if( p_str == NULL ) {
        INVALID_ARGUMENT("p_str is NULL");
        return(false);
    }

    if( p_atom == NULL ) {
        INVALID_ARGUMENT("p_atom is NULL");
    }

    if( ActiveInserter == NULL ) {
        ES_ERROR("ActiveInserter is NULL");
        return(false);
    }

    CStructure*  p_mol = p_atom->GetStructure();

    CHistoryNode* p_history = p_mol->BeginChangeWH(EHCL_TOPOLOGY,"add structure",p_str->GetName());
    if( p_history == NULL ) return (false);

    bool result = ActiveInserter->InsertStructure(p_str,p_atom,p_history);

    if( AutoCenter ) {
        p_mol->GetAtoms()->MoveCOMToWH(EGO_BOX_CENTER,EGS_ALL_ATOMS,true);
    }

    p_mol->EndChangeWH();
    return(result);
}

//---------------------------------------------------------------------------

bool CInserterList::InsertStructureWH(CFragment* p_str,CStructure* p_mol,const CPoint& pos)
{
    if( p_str == NULL ) {
        INVALID_ARGUMENT("p_str is NULL");
        return(false);
    }

    if( p_mol == NULL ) {
        INVALID_ARGUMENT("p_mol is NULL");
    }

    if( AllowNewStructure == false ) {
        if( p_mol->IsEmpty() == false ) {
            QString warn = "unable to add structure to non-empty molecule";
            p_mol->GetProject()->TextNotification(ETNT_WARNING,warn,2000);
            return(false);
        }
    }

    if( ActiveInserter == NULL ) {
        ES_ERROR("ActiveInserter is NULL");
        return(false);
    }

    CHistoryNode* p_history = p_mol->BeginChangeWH(EHCL_TOPOLOGY,"add structure",p_str->GetName());
    if( p_history == NULL ) return (false);

    bool result = ActiveInserter->InsertStructure(p_str,p_mol,pos,p_history);

    if( AutoCenter ) {
        p_mol->GetAtoms()->MoveCOMToWH(EGO_BOX_CENTER,EGS_ALL_ATOMS,true);
    }

    p_mol->EndChangeWH();

    if( AllowNewStructure ) {
        AllowNewStructure = false;
        emit SetupChanged();
    }

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CInserterList::InsertAllInserters(void)
{
    CSimpleIteratorC<CPluginObject> I(PluginDatabase.GetObjectList());
    CPluginObject* p_object;

    bool result = true;

    // loop over objects
    while ((p_object = I.Current()) != NULL) {
        I++;
        if (p_object->GetCategoryUUID() != INSERTER_CAT) continue;

        CInserter* p_setup = static_cast<CInserter*>(p_object->CreateObject(this));
        if( p_setup == NULL ) {
            CSmallString error;
            error << "unable to create setup object for " << p_object->GetObjectUUID().GetFullStringForm();
            ES_ERROR(error);
            result = false;
            continue;
        }
    }

    ActiveInserter = static_cast<CInserter*>(children().first());
    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




