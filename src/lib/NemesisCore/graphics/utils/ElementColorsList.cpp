// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <ElementColorsList.hpp>
#include <GlobalSetup.hpp>
#include <FileSystem.hpp>
#include <XMLParser.hpp>
#include <XMLPrinter.hpp>
#include <ErrorSystem.hpp>
//------------------------------------------------------------------------------

CElementColorsList ColorsList;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CElementColorsList::CElementColorsList(void)
{
    MaterialMode = EECM_NONE;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CElementColorsList::LoadColors(void)
{
    // config name
    CFileName      config_name;

    config_name = GlobalSetup->GetUserGraphicsSetupPath() / "colors.xml"; // first user setup
    if( CFileSystem::IsFile(config_name) == false ) {
        // use system config
        config_name = GlobalSetup->GetSystemGraphicsSetupPath() / "colors.xml";
    }

    if( CFileSystem::IsFile(config_name) == false ) {
        // no setup is available
        ES_ERROR("Cannot open any color setup file");
        return(false);
    }

    bool result = LoadColorsFile(config_name);
    return(result);
}

//------------------------------------------------------------------------------

bool CElementColorsList::LoadSystemColors(void)
{
    // config name
    CFileName      config_name;
    config_name = GlobalSetup->GetSystemGraphicsSetupPath() / "colors.xml";

    bool result = LoadColorsFile(config_name);
    return(result);
}

//------------------------------------------------------------------------------

bool CElementColorsList::LoadColorsFile(const CFileName& config_name)
{
    CXMLDocument ColorsData;

    // open config file
    CXMLParser xml_parser;
    xml_parser.SetOutputXMLNode(&ColorsData);

    if( xml_parser.Parse(config_name) == false ) {
        CSmallString error;
        error <<  "unable to parse configuration file (" << config_name << ")";
        ES_ERROR(error);
        return(false);
    }

    // load global colors and material
    CXMLElement* p_ele;
    if((p_ele = ColorsData.GetFirstChildElement("colors")) == NULL) {
        ES_ERROR("Cannot load root colors element");
        return(false);
    }

    bool result = true;

    result &= p_ele->GetAttribute("material_mode", MaterialMode);

    CXMLElement* p_gel = p_ele->GetFirstChildElement("global");
    result &= GlobalMaterial.Load(p_gel);

    CXMLElement* p_nel = p_ele->GetFirstChildElement("selection");
    result &= SelectionMaterial.Load(p_nel);

    CXMLElement* p_sel = p_ele->GetFirstChildElement("element");

    while( p_sel != NULL ) {
        int element;
        if( p_sel->GetAttribute("id",element) == false ) {
            ES_ERROR("ELEMENT id attribute is mandatory");
            return(false);
        }
        if( (element < 0) || (element >= CPT_NUM_OF_ELEMENTS) ) {
            ES_ERROR("ELEMENT id attribute is out of range");
            return(false);
        }
        if( Elements[element].Load(p_sel) == false ) {
            ES_ERROR("unable to load element color setup");
            return(false);
        }
        p_sel = p_sel->GetNextSiblingElement("element");
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CElementColorsList::SaveUserColors(void)
{
    // root document
    CXMLDocument    ColorsData;
    ColorsData.CreateChildDeclaration();
    ColorsData.CreateChildComment("NEMESIS element colors setup");

    CXMLElement* p_ele;
    if((p_ele = ColorsData.CreateChildElement("colors")) == NULL) {
        ES_ERROR("Cannot create root colors element");
        return(false);
    }

    bool result = true;

    p_ele->SetAttribute("material_mode", MaterialMode);

    CXMLElement* p_gel = p_ele->CreateChildElement("global");
    result &= GlobalMaterial.Save(p_gel);

    CXMLElement* p_nel = p_ele->CreateChildElement("selection");
    result &= SelectionMaterial.Save( p_nel);

    if(result == false) {
        ES_ERROR("Cannot save global color");
        return(false);
    }

    // save all elementcolors
    for(int i=0; i<CPT_NUM_OF_ELEMENTS; i++) {
        CXMLElement* p_sel = p_ele->CreateChildElement("element");
        if( p_sel == NULL ) {
            ES_ERROR("unable to create ELEMENT element");
            return(false);
        }
        p_sel->SetAttribute("id",i);
        if( Elements[i].Save(p_sel) == false ) {
            ES_ERROR("unable to save element color setup");
            return(false);
        }
    }

    // config name
    CFileName config_name;
    config_name = GlobalSetup->GetUserGraphicsSetupPath() / "colors.xml";

    // open config file
    CXMLPrinter xml_printer;
    xml_printer.SetPrintedXMLNode(&ColorsData);

    if( xml_printer.Print(config_name) == false ) {
        CSmallString error;
        error <<  "unable to save configuration file (" << config_name << ")";
        ES_ERROR(error);
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const CElementColors& CElementColorsList::GetElementColors(int z)
{
    return(Elements[z]);
}

//------------------------------------------------------------------------------

CElementColors* CElementColorsList::GetElementColorPointer(int z)
{
    return(&Elements[z]);
}

//------------------------------------------------------------------------------

void CElementColorsList::SetElementColors(int z,const CElementColors& color)
{
    Elements[z] = color;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

