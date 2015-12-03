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

#include <DesktopSystem.hpp>
#include <XMLIterator.hpp>
#include <XMLParser.hpp>
#include <XMLPrinter.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <WorkPanel.hpp>
#include <WorkPanelList.hpp>
#include <CategoryUUID.hpp>
#include <FileSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDesktopSystem::CDesktopSystem(CPluginObject* p_pob,QObject* p_parent)
    : CExtComObject(p_pob,p_parent)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CDesktopSystem::LoadDesktop(const CFileName& configname)
{
    DesktopData.RemoveAllChildNodes();

    CXMLParser xml_parser;
    xml_parser.SetOutputXMLNode(&DesktopData);

    if( CFileSystem::IsFile(configname) == false ){
        CSmallString warning;
        warning << "desktop setup does not exist (" << configname << ")";
        ES_WARNING(warning);
        return(true);
    }

    if( xml_parser.Parse(configname) == false ) {
        ES_ERROR("unable to parse user desktop setup");

        DesktopData.CreateChildDeclaration();
        DesktopData.CreateChildComment("NEMESIS desktop setup");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CDesktopSystem::SaveDesktop(const CFileName& configname)
{
    CXMLPrinter xml_printer;
    xml_printer.SetPrintedXMLNode(&DesktopData);

    if( xml_printer.Print(configname) == false ) {
        ES_ERROR("unable to save user desktop setup");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

void CDesktopSystem::LoadData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "desktop" ) {
        LOGIC_ERROR("element is not desktop");
    }

    DesktopData.RemoveAllChildNodes();
    CXMLElement* p_root = DesktopData.GetChildElementByPath("desktop",true);
    p_root->CopyContentsFrom(p_ele);
}

//------------------------------------------------------------------------------

void CDesktopSystem::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "desktop" ) {
        LOGIC_ERROR("element is not desktop");
    }

   CXMLElement* p_root = DesktopData.GetChildElementByPath("desktop",true);
   p_ele->CopyContentsFrom(p_root);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CXMLElement* CDesktopSystem::GetWorkPanelsSetup(void)
{
    CXMLElement* p_root = DesktopData.GetChildElementByPath("desktop/workpanels",true);
    return(p_root);
}

//------------------------------------------------------------------------------

CXMLElement* CDesktopSystem::GetWorkPanelSetup(const CExtUUID& uuid)
{
    CXMLElement* p_root = DesktopData.GetChildElementByPath("desktop/workpanels",true);

    // find item in the list
    CXMLIterator I(p_root);
    CXMLElement* p_ele;

    while( (p_ele = I.GetNextChildElement("item")) != NULL ) {
        CExtUUID item_uuid;
        item_uuid.GetValue(p_ele,"uuid");
        if( item_uuid == uuid ) return(p_ele);
    }

    // item does not exist - create it
    CXMLElement* p_fele = p_root->CreateChildElement("item");
    uuid.SetValue(p_fele,"uuid");
    return(p_fele);
}

//------------------------------------------------------------------------------

CXMLElement* CDesktopSystem::GetObjectDesignerSetup(const CExtUUID& uuid)
{
    CXMLElement* p_root = DesktopData.GetChildElementByPath("desktop/odesigners",true);

    // find item in the list
    CXMLIterator I(p_root);
    CXMLElement* p_ele;

    while( (p_ele = I.GetNextChildElement("item")) != NULL ) {
        CExtUUID item_uuid;
        item_uuid.GetValue(p_ele,"uuid");
        if( item_uuid == uuid ) return(p_ele);
    }

    // item does not exist - create it
    CXMLElement* p_fele = p_root->CreateChildElement("item");
    uuid.SetValue(p_fele,"uuid");
    return(p_fele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

