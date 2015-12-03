// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <OptimizerSetupList.hpp>
#include <OptimizerSetup.hpp>
#include <CategoryUUID.hpp>
#include <NemesisCoreModule.hpp>
#include <PluginDatabase.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <Optimizer.hpp>
#include <JobList.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <XMLDocument.hpp>
#include <XMLParser.hpp>
#include <XMLPrinter.hpp>
#include <GlobalSetup.hpp>
#include <FileSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        OptimizerSetupListID(
                    "{OPTIMIZER_SETUP_LIST:77df2e28-eeb2-41b2-a6e3-f5fc4ec7e278}",
                    "Optimizers");

CPluginObject   OptimizerSetupListObject(&NemesisCorePlugin,
                    OptimizerSetupListID,GENERIC_CAT,
                     ":/images/NemesisCore/build/Optimizers.svg",
                     NULL);

COptimizerSetupList*  OptimizerSetups = NULL;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

COptimizerSetupList::COptimizerSetupList(CExtComObject* p_parent)
    : CExtComObject(&OptimizerSetupListObject,p_parent)
{
    DefaultSetup = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool COptimizerSetupList::LoadConfig(void)
{
    // load config
    CFileName     config_name;

    config_name = GlobalSetup->GetUserSetupPath() / "optimizers.xml"; // first user setup
    if( CFileSystem::IsFile(config_name) == false ) {
        // if not present - try to load system config
        config_name = GlobalSetup->GetSystemSetupPath() / "optimizers.xml"; // system setup
    }

    if( CFileSystem::IsFile(config_name) == false ){
        InitDefaultSetup();
        return(true);
    }

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

    CXMLElement* p_goele;
    p_goele = p_ele->GetFirstChildElement("setup");

    while( p_goele != NULL ) {
        COptimizerSetup* p_setup = CreateSetup(p_goele);
        bool default_setup = false;
        p_goele->GetAttribute("default",default_setup);
        if( default_setup ){
            DefaultSetup = p_setup;
        }
        p_goele = p_goele->GetNextSiblingElement("setup");
    }

    if( DefaultSetup == NULL ){
        // emergency setup
        InitDefaultSetup();
    }

    return(true);
}

//------------------------------------------------------------------------------

bool COptimizerSetupList::SaveConfig(void)
{
    CFileName     config_name;

    config_name = GlobalSetup->GetUserSetupPath() / "optimizers.xml";

    CXMLDocument xml_doc;

    xml_doc.CreateChildDeclaration();
    xml_doc.CreateChildComment("user optimizer setup profile");

    CXMLElement* p_ele = xml_doc.CreateChildElement("setups");

    foreach(QObject* p_qobj,children()) {
        COptimizerSetup* p_st = static_cast<COptimizerSetup*>(p_qobj);
        CXMLElement* p_stele = p_ele->CreateChildElement("setup");
        p_st->SaveData(p_stele);
        if( DefaultSetup == p_st ){
            p_stele->SetAttribute("default",true);
        }
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

void COptimizerSetupList::CloseAllObjectDesigners(void)
{
    foreach(QObject* p_qobj,children()) {
        // close object designer
        COptimizerSetup* p_obj = static_cast<COptimizerSetup*>(p_qobj);
        p_obj->CloseObjectDesigner(true);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

COptimizer* COptimizerSetupList::CreateDefaultOptimizerJob(CStructure* p_mol)
{
    if( DefaultSetup == NULL ) {
        ES_ERROR("no default optimizer setup");
        return(NULL);
    }

    if( p_mol == NULL ) {
        ES_ERROR("p_mol is NULL");
        return(NULL);
    }

    CProject* p_project = p_mol->GetProject();

    // create optimizer object
    COptimizer* p_opt;
    p_opt = static_cast<COptimizer*>(PluginDatabase.CreateObject(DefaultSetup->GetOptimizerUUID(),p_project));

    if( p_opt == NULL ) {
        ES_ERROR("unable to create object");
        return(NULL);
    }

    // setup optimizer
    p_opt->SetOptimizerSetup(DefaultSetup);
    p_opt->SetOptimizedMolecule(p_mol);

    // register job
    p_project->GetJobs()->RegisterJob(p_opt);

    return(p_opt);
}

//------------------------------------------------------------------------------

void COptimizerSetupList::OpenDefaultOptimizerSetupDesigner(void)
{
    if( DefaultSetup == NULL ) {
        ES_ERROR("no default optimizer setup");
        return;
    }

    DefaultSetup->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool COptimizerSetupList::InitDefaultSetup(void)
{
    CSimpleIteratorC<CPluginObject> I(PluginDatabase.GetObjectList());
    CPluginObject* p_object;

    bool result = true;

    // loop over objects
    while ((p_object = I.Current()) != NULL) {
        I++;
        if (p_object->GetCategoryUUID() != OPTIMIZER_SETUP_CAT) continue;

        COptimizerSetup* p_setup = static_cast<COptimizerSetup*>(p_object->CreateObject(this));
        if( p_setup == NULL ) {
            CSmallString error;
            error << "unable to create optimizer setup object for " << p_object->GetObjectUUID().GetFullStringForm();
            ES_ERROR(error);
            result = false;
            continue;
        } else {
            // only one object to create
            break;
        }
    }

    // set first optimizer setup as default
    if( children().size() > 0 ){
        DefaultSetup = static_cast<COptimizerSetup*>(children().first());
    } else {
        DefaultSetup = NULL;
    }

    return(result);
}

//------------------------------------------------------------------------------

COptimizerSetup* COptimizerSetupList::CreateSetup(CXMLElement* p_data)
{
    if( p_data == NULL ) return(NULL);

    QString suuid;
    if( p_data->GetAttribute("uuid",suuid) == false ) {
        ES_ERROR("unable to get uuid attribute");
        return(NULL);
    }
    CExtUUID  ext_uuid;
    if( ext_uuid.LoadFromString(suuid) == false ) {
        CSmallString error;
        error << "unable to decode uuid attribute " << suuid;
        ES_ERROR(error);
        return(NULL);
    }

    // create object
    COptimizerSetup* p_st = static_cast<COptimizerSetup*>(PluginDatabase.CreateObject(ext_uuid,this));

    if( p_st == NULL ) {
        CSmallString error;
        error << "unable to create optimizer setup " << ext_uuid.GetStringForm();
        ES_ERROR(error);
        return(NULL);
    }

    // load data
    p_st->LoadData(p_data);

    return(p_st);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


