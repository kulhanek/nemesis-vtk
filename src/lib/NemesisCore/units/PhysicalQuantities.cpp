// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011        Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <PhysicalQuantities.hpp>
#include <XMLParser.hpp>
#include <XMLPrinter.hpp>
#include <XMLIterator.hpp>
#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <FileSystem.hpp>
#include <StdPhysicalQuantity.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <math.h>

// -----------------------------------------------------------------------------
// XMLSetup
/*
<QUANTITIES>
    <QUANTITY name="">
        <UNIT name="" format="" offset="" scale="" active="true"/>
        <UNIT ... />
    </QUANTITY>
    <QUANTITY name="">
        ..............
        ..............
</QUANTITIES>
*/

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPhysicalQuantities*  PhysicalQuantities = NULL;

CPhysicalQuantity* PQ_UNITY = NULL;
CPhysicalQuantity* PQ_TIME = NULL;
CPhysicalQuantity* PQ_DISTANCE = NULL;
CPhysicalQuantity* PQ_DIMENSION = NULL;
CPhysicalQuantity* PQ_ANGLE = NULL;
CPhysicalQuantity* PQ_PERCENTAGE = NULL;
CPhysicalQuantity* PQ_ABSOLUTE_ENERGY = NULL;
CPhysicalQuantity* PQ_ENERGY = NULL;
CPhysicalQuantity* PQ_TEMPERATURE = NULL;
CPhysicalQuantity* PQ_CHARGE = NULL;
CPhysicalQuantity* PQ_VOLUME = NULL;
CPhysicalQuantity* PQ_MASS = NULL;
CPhysicalQuantity* PQ_VELOCITY = NULL;
CPhysicalQuantity* PQ_FREQUENCY = NULL;
CPhysicalQuantity* PQ_IR_INTENSITY = NULL;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        PhysicalQuantitiesID(
                    "{PHYSICAL_QUANTITIES:a60f5b8a-b319-4954-a592-8da5675d3688}",
                    "Physical quantities");

CPluginObject   PhysicalQuantitiesObject(&NemesisCorePlugin,
                    PhysicalQuantitiesID,GENERIC_CAT,
                     ":/images/NemesisCore/units/PhysicalQuantities.svg",
                     NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPhysicalQuantities::CPhysicalQuantities(CExtComObject* p_parent)
    : CExtComObject(&PhysicalQuantitiesObject,p_parent)
{
    ConFlags |= EECOF_SUB_CONTAINERS;

    PQ_UNITY = new CStdPhysicalQuantity(this,"unity","",'f',3);
    PQ_TIME = new CStdPhysicalQuantity(this,"time","fs",'f',1);
    PQ_DISTANCE = new CStdPhysicalQuantity(this,"distance","Å",'f',3);
    PQ_DIMENSION = new CStdPhysicalQuantity(this,"dimension","Å",'f',1);
    PQ_ANGLE = new CStdPhysicalQuantity(this,"angle","°",'f',1,0,180.0/M_PI);
    PQ_PERCENTAGE = new CStdPhysicalQuantity(this,"percentage","%",'f',1,0,100);
    PQ_ABSOLUTE_ENERGY = new CStdPhysicalQuantity(this,"absolute energy","au",'f',9);
    PQ_ENERGY = new CStdPhysicalQuantity(this,"energy","kcal/mol",'f',2);
    PQ_TEMPERATURE = new CStdPhysicalQuantity(this,"temperature","K",'f',1);
    PQ_CHARGE = new CStdPhysicalQuantity(this,"charge","",'f',3);
    PQ_VOLUME = new CStdPhysicalQuantity(this,"volume","Å^3",'f',1);
    PQ_MASS = new CStdPhysicalQuantity(this,"mass","g/mol",'f',1);
    PQ_VELOCITY = new CStdPhysicalQuantity(this,"velocity","A/fs",'f',1);
    PQ_FREQUENCY = new CStdPhysicalQuantity(this,"frequency","1/cm",'f',1);
    PQ_IR_INTENSITY = new CStdPhysicalQuantity(this,"IR intensity","km/mol",'f',1);

    // Tady nelze volat LoadConfig, protoze poradi volani konstruktoru statickych
    // globalnich objektu nelze v C++ definovat. Muze se tedy stat, ze CProgramPaths
    // se bude inicializovat az po CPhysicalQuantities
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CPhysicalQuantities::LoadConfig(void)
{
    PhysicalQuantitiesData.RemoveAllChildNodes();

    // config name
    CFileName config_name;

    config_name = GlobalSetup->GetUserSetupPath() / "units.xml"; // first user setup
    if( CFileSystem::IsFile(config_name) == false ) {
        // if not present - try to load system config
        config_name = GlobalSetup->GetSystemSetupPath() / "units.xml"; // system setup
    }
    if( CFileSystem::IsFile(config_name) == false ) {
        // no setup is available
        // create config file header
        PhysicalQuantitiesData.CreateChildDeclaration();
        PhysicalQuantitiesData.CreateChildComment("NEMESIS physical quantity unit setup");
        return(true);
    }

    // open config file
    CXMLParser xml_parser;
    xml_parser.SetOutputXMLNode(&PhysicalQuantitiesData);

    if( xml_parser.Parse(config_name) == false ) {
        CSmallString error;
        error <<  "unable to parse configuration file (" << config_name << ")";
        ES_ERROR(error);
        return(false);
    }

    // activate unit setup for every quantity
    bool               result = true;
    foreach(CStdPhysicalQuantity* p_q, findChildren<CStdPhysicalQuantity*>()) {
        result &= p_q->ActivateUnit(-1);
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CPhysicalQuantities::LoadSystemConfig(void)
{
    PhysicalQuantitiesData.RemoveAllChildNodes();

    // config name
    CFileName config_name;
    config_name = GlobalSetup->GetSystemSetupPath() / "units.xml"; // system setup

    if( CFileSystem::IsFile(config_name) == false ) {
        // no setup is available
        // create config file header
        PhysicalQuantitiesData.CreateChildDeclaration();
        PhysicalQuantitiesData.CreateChildComment("NEMESIS physical quantity unit setup");
        return(true);
    }

    // open config file
    CXMLParser xml_parser;
    xml_parser.SetOutputXMLNode(&PhysicalQuantitiesData);

    if( xml_parser.Parse(config_name) == false ) {
        CSmallString error;
        error <<  "unable to parse configuration file (" << config_name << ")";
        ES_ERROR(error);
        return(false);
    }

    // activate unit setup for every quantity
    bool               result = true;
    foreach(CStdPhysicalQuantity* p_q, findChildren<CStdPhysicalQuantity*>()) {
        result &= p_q->ActivateUnit(-1);
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CPhysicalQuantities::SaveUserConfig(void)
{
    // config name
    CFileName config_name;

    config_name = GlobalSetup->GetUserSetupPath() / "units.xml";

    // open config file
    CXMLPrinter xml_printer;
    xml_printer.SetPrintedXMLNode(&PhysicalQuantitiesData);

    if( xml_printer.Print(config_name) == false ) {
        CSmallString error;
        error <<  "unable to save configuration file (" << config_name << ")";
        ES_ERROR(error);
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CPhysicalQuantities::RemoveUserConfig(void)
{
    // config name
    CFileName config_name;

    config_name = GlobalSetup->GetUserSetupPath() / "units.xml";
    if( CFileSystem::IsFile(config_name) == false ) {
        return(true); // file does not exist
    }

    // remove file
    return(CFileSystem::RemoveFile(config_name));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CPhysicalQuantities::NumOfQuantities(void)
{
    return(findChildren<CStdPhysicalQuantity*>().count());
}

//------------------------------------------------------------------------------

CStdPhysicalQuantity* CPhysicalQuantities::GetQuantity(int index)
{
    QList<CStdPhysicalQuantity*> list = findChildren<CStdPhysicalQuantity*>();

    if( (index < 0) || (index > list.count()) ) {
        ES_ERROR("quantity index is out of range");
        return(NULL);
    }
    return(list[index]);
}

//------------------------------------------------------------------------------

CStdPhysicalQuantity* CPhysicalQuantities::FindQuantity(
    const QString& name)
{
    QList<CStdPhysicalQuantity*> list = findChildren<CStdPhysicalQuantity*>(name);
    if( list.count() != 1 ) return(NULL);

    return(list[0]);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CXMLElement* CPhysicalQuantities::GetQuantityXMLSetup(
    const QString& name)
{
    CXMLIterator   I(PhysicalQuantitiesData.GetFirstChildElement("QUANTITIES"));
    CXMLElement*   p_ele;

    while( (p_ele = I.GetNextChildElement("QUANTITY")) != NULL ) {
        QString        lname;
        p_ele->GetAttribute("name",lname);
        if( lname == name ) return(p_ele);
    }

    // no setup -> create it
    if((p_ele = PhysicalQuantitiesData.GetFirstChildElement("QUANTITIES")) == NULL ) {
        if((p_ele = PhysicalQuantitiesData.CreateChildElement("QUANTITIES")) == NULL) {
            ES_ERROR("unable to create new QUANTITIES element");
            return(NULL);
        }
    }

    CXMLElement*   p_sele;
    p_sele = p_ele->CreateChildElement("QUANTITY");
    if( p_sele == NULL ) {
        ES_ERROR("unable to create new QUANTITY element");
        return(NULL);
    }

    p_sele->SetAttribute("name",name);
    return(p_sele);
}

//------------------------------------------------------------------------------

void CPhysicalQuantities::EmitOnUnitChanged(void)
{
    emit OnUnitChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


