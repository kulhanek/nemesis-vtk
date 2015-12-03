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

#include <GraphicsSetupProfile.hpp>
#include <GlobalSetup.hpp>
#include <ErrorSystem.hpp>
#include <PluginDatabase.hpp>
#include <XMLDocument.hpp>
#include <XMLElement.hpp>
#include <XMLPrinter.hpp>
#include <XMLParser.hpp>
#include <FileSystem.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        GraphicsSetupProfileID(
                    "{GRAPHICS_SETUP_PROFILE:7fcbc9f0-3ede-43ef-9ad1-132ed3bef3ac}",
                    "Graphics setup");

CPluginObject   GraphicsSetupProfileObject(&NemesisCorePlugin,
                    GraphicsSetupProfileID,GRAPHICS_CAT,
                    ":/images/NemesisCore/graphics/SetupList.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsSetupProfile* GraphicsSetupProfile = NULL;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsSetupProfile::CGraphicsSetupProfile(CExtComObject* p_parent)
    : CGraphicsSetupList(&GraphicsSetupProfileObject,p_parent)
{
    MultiSamplingEnabled = true;
    QuadStereoEnabled = false;
}

//------------------------------------------------------------------------------

CGraphicsSetupProfile::~CGraphicsSetupProfile(void)
{
  //int count = children().count();
}

//------------------------------------------------------------------------------

void CGraphicsSetupProfile::InitAllSetupObjects(void)
{
    // create all global setup objects
    CSimpleIteratorC<CPluginObject> I(PluginDatabase.GetObjectList());
    CPluginObject* p_object;

    // loop over objects
    while ((p_object = I.Current()) != NULL) {
        I++;
        if (p_object->GetCategoryUUID() == GRAPHICS_SETUP_CAT){
            CGraphicsSetup* p_setup = static_cast<CGraphicsSetup*>(p_object->CreateObject(this));
            if( p_setup == NULL ) {
                CSmallString error;
                error << "unable to create setup object for " << p_object->GetObjectUUID().GetFullStringForm();
                ES_ERROR(error);
                continue;
            }
        }
    }
}

//------------------------------------------------------------------------------

void CGraphicsSetupProfile::CloseAllObjectDesigners(void)
{
    foreach(QObject* p_qobj,children()) {
        // close object designer
        CProObject* p_obj = static_cast<CProObject*>(p_qobj);
        p_obj->CloseObjectDesigner(true);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CGraphicsSetupProfile::LoadDefaultConfig(void)
{
    return( LoadConfig("default.xml") );
}

//------------------------------------------------------------------------------

bool CGraphicsSetupProfile::SaveDefaultConfig(void)
{
    return( SaveConfig("default.xml") );
}

//------------------------------------------------------------------------------

bool CGraphicsSetupProfile::LoadConfig(const CFileName& name)
{
    // load config
    CFileName     config_name;

    config_name = GlobalSetup->GetUserGraphicsSetupPath() / name; // first user setup
    if( CFileSystem::IsFile(config_name) == false ) {
        // if not present - try to load system config
        config_name = GlobalSetup->GetSystemGraphicsSetupPath() / name; // system setup
    }

    if( CFileSystem::IsFile(config_name) == false ) return(true);

    CXMLDocument xml_doc;

    CXMLParser xml_parser;
    xml_parser.SetOutputXMLNode(&xml_doc);

    if( xml_parser.Parse(config_name) == false ) {
        ES_ERROR("unable to load setup profile");
        return(false);
    }

    CXMLElement* p_ele = xml_doc.GetFirstChildElement("setups");
    if( p_ele == NULL ) {
        ES_ERROR("unable to open setup element");
        return(false);
    }

    // load config for individual setup objects
    try{
        foreach(QObject* p_obj,children()){
            CGraphicsSetup* p_setup = static_cast<CGraphicsSetup*>(p_obj);
            CXMLElement*    p_sele = FindSetupXMLData(p_ele,p_setup->GetType());
            if( p_sele ){
                p_setup->Load(p_sele);
            }
        }
    } catch(std::exception& e) {
        ES_ERROR_FROM_EXCEPTION("unable to load",e);
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CGraphicsSetupProfile::SaveConfig(const CFileName& name)
{
    CFileName     config_name;

    config_name = GlobalSetup->GetUserGraphicsSetupPath() / name;

    CXMLDocument xml_doc;

    xml_doc.CreateChildDeclaration();
    xml_doc.CreateChildComment("user graphics setup profile");

    CXMLElement* p_ele = xml_doc.CreateChildElement("setups");

    try {
        Save(p_ele);
    } catch(std::exception& e) {
        ES_ERROR_FROM_EXCEPTION("unable to save",e);
        return(false);
    }

    CXMLPrinter xml_printer;
    xml_printer.SetPrintedXMLNode(&xml_doc);

    if( xml_printer.Print(config_name) == false ) {
        ES_ERROR("unable to save setup profile");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CGraphicsSetupProfile::LoadUserDefault(CGraphicsSetup* p_setup)
{
    CFileName     config_name;
    config_name = GlobalSetup->GetUserGraphicsSetupPath() / "default.xml";
    if( CFileSystem::IsFile(config_name) == false ) return(false);

    CXMLDocument xml_doc;

    CXMLParser xml_parser;
    xml_parser.SetOutputXMLNode(&xml_doc);

    if( xml_parser.Parse(config_name) == false ) {
        ES_ERROR("unable to load setup profile");
        return(false);
    }

    CXMLElement* p_ele = xml_doc.GetFirstChildElement("setups");
    if( p_ele == NULL ) {
        ES_ERROR("unable to open setup element");
        return(false);
    }

    try{
        CXMLElement* p_sele = FindSetupXMLData(p_ele,p_setup->GetType());
        if( p_sele ) p_setup->Load(p_sele);
    } catch(std::exception& e) {
        ES_ERROR_FROM_EXCEPTION("unable to load",e);
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CGraphicsSetupProfile::LoadSystemDefault(CGraphicsSetup* p_setup)
{
    CFileName     config_name;
    config_name = GlobalSetup->GetSystemGraphicsSetupPath() / "default.xml";
    if( CFileSystem::IsFile(config_name) == false ) return(false);

    CXMLDocument xml_doc;

    CXMLParser xml_parser;
    xml_parser.SetOutputXMLNode(&xml_doc);

    if( xml_parser.Parse(config_name) == false ) {
        ES_ERROR("unable to load setup profile");
        return(false);
    }

    CXMLElement* p_ele = xml_doc.GetFirstChildElement("setups");
    if( p_ele == NULL ) {
        ES_ERROR("unable to open setup element");
        return(false);
    }

    try{
        CXMLElement* p_sele = FindSetupXMLData(p_ele,p_setup->GetType());
        if( p_sele ) p_setup->Load(p_sele);
    } catch(std::exception& e) {
        ES_ERROR_FROM_EXCEPTION("unable to load",e);
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CXMLElement* CGraphicsSetupProfile::FindSetupXMLData(CXMLElement* p_ele,const CExtUUID& uuid)
{
    // check input data -----------------------------
    if( p_ele == NULL ){
       INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "setups" ){
       LOGIC_ERROR("p_ele is not setups");
    }

    // load data ------------------------------------
    CXMLElement* p_ael;
    p_ael = p_ele->GetFirstChildElement("setup");

    while( p_ael != NULL ){
       CExtUUID  ext_uuid;
       ext_uuid.GetValue(p_ael,"uuid");

       if( uuid == ext_uuid ) return(p_ael);

       p_ael = p_ael->GetNextSiblingElement("setup");
    }

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================






