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

#include <MouseDriverSetup.hpp>
#include <XMLElement.hpp>
#include <XMLDocument.hpp>
#include <XMLParser.hpp>
#include <XMLPrinter.hpp>
#include <GlobalSetup.hpp>
#include <FileSystem.hpp>
#include <PluginObject.hpp>
#include <CategoryUUID.hpp>
#include <NemesisCoreModule.hpp>
#include <ErrorSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

// complement this setup with bool CMouseDriverSetup::LoadSystemSetup(void)!

CPoint      CMouseDriverSetup::RSensitivity(0.025,0.025,0.025);
CPoint      CMouseDriverSetup::MSensitivity(0.1,0.1,0.1);
float       CMouseDriverSetup::SSensitivity = 0.01;
float       CMouseDriverSetup::WheelSensitivity = 0.2;
bool        CMouseDriverSetup::ThreeButtonSimul = true;
EMouseMode  CMouseDriverSetup::MouseMode =  EMM_SRT;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        MouseDriverSetupID(
                    "{MOUSE_DRIVER_SETUP:7fee9b00-143a-4912-b12c-e7759c8d4fee}",
                    "Mouse setup");

CPluginObject   MouseDriverSetupObject(&NemesisCorePlugin,
                    MouseDriverSetupID,GENERIC_CAT,NULL);

//------------------------------------------------------------------------------

CMouseDriverSetup* MouseDriverSetup = NULL;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMouseDriverSetup::CMouseDriverSetup(CExtComObject* p_parent)
    : CExtComObject(&MouseDriverSetupObject,p_parent)
{
    ConFlags |= EECOF_HIDDEN;
}

//------------------------------------------------------------------------------

void CMouseDriverSetup::EmitOnMouseSetupChanged(void)
{
    emit OnMouseSetupChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMouseDriverSetup::LoadSystemSetup(void)
{
    RSensitivity = CPoint(0.025,0.025,0.025);
    MSensitivity = CPoint(0.1,0.1,0.1);
    WheelSensitivity = 0.2;
    ThreeButtonSimul = true;
    MouseMode =  EMM_SRT;
}

//------------------------------------------------------------------------------

bool CMouseDriverSetup::LoadUserSetup(void)
{
    CXMLDocument xml_doc;
    CXMLParser   xml_parser;

    xml_parser.SetOutputXMLNode(&xml_doc);

    CFileName config_file = GlobalSetup->GetUserSetupPath() / "mouse.xml";
    if( ! CFileSystem::IsFile(config_file) ){
        // file does not exist
        return(true);
    }

    if( xml_parser.Parse(config_file) == false ) {
        ES_ERROR("unable to parse mouse configuration");
        return(false);
    }

    CXMLElement* p_mele = xml_doc.GetChildElementByPath("config");
    if( p_mele == NULL ){
        ES_ERROR("config element not fount");
        return(false);
    }

    p_mele->GetAttribute("rx",RSensitivity.x);
    p_mele->GetAttribute("ry",RSensitivity.y);
    p_mele->GetAttribute("rz",RSensitivity.z);

    p_mele->GetAttribute("mx",MSensitivity.x);
    p_mele->GetAttribute("my",MSensitivity.y);
    p_mele->GetAttribute("mz",MSensitivity.z);

    p_mele->GetAttribute("w",WheelSensitivity);

    p_mele->GetAttribute("tbs",ThreeButtonSimul);

    p_mele->GetAttribute("mode",MouseMode);

    return(true);
}

//------------------------------------------------------------------------------

bool CMouseDriverSetup::SaveUserSetup(void)
{
    CXMLDocument xml_doc;
    CXMLPrinter  xml_printer;

    CXMLElement* p_mele = xml_doc.GetChildElementByPath("config",true);

    p_mele->SetAttribute("rx",RSensitivity.x);
    p_mele->SetAttribute("ry",RSensitivity.y);
    p_mele->SetAttribute("rz",RSensitivity.z);

    p_mele->SetAttribute("mx",MSensitivity.x);
    p_mele->SetAttribute("my",MSensitivity.y);
    p_mele->SetAttribute("mz",MSensitivity.z);

    p_mele->SetAttribute("w",WheelSensitivity);

    p_mele->SetAttribute("tbs",ThreeButtonSimul);

    p_mele->SetAttribute("mode",MouseMode);

    xml_printer.SetPrintedXMLNode(&xml_doc);
    CFileName config_file = GlobalSetup->GetUserSetupPath() / "mouse.xml";
    if( xml_printer.Print(config_file) == false ) {
        ES_ERROR("unable to save mouse configuration");
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


