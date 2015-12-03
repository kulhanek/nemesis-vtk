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

#include <StdPhysicalQuantity.hpp>
#include <ErrorSystem.hpp>
#include <XMLIterator.hpp>
#include <PhysicalQuantities.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>

#include "StdPhysicalQuantity.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        StdPhysicalQuantityID(
                    "{STD_PHYSICAL_QUANTITY:096b7684-9d35-4204-9c6e-74064a2c0610}",
                    "Standard physical quantity");

CPluginObject   StdPhysicalQuantityObject(&NemesisCorePlugin,
                   StdPhysicalQuantityID,GENERIC_CAT,NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CStdPhysicalQuantity::CStdPhysicalQuantity(CExtComObject* p_parent,
                                        const QString& quantity_name,
                                        const QString& main_unit,
                                        char main_format,
                                        int decimals)
    : CPhysicalQuantity(&StdPhysicalQuantityObject,p_parent)
{
    setObjectName(quantity_name);
    ConFlags |= EECOF_SUB_CONTAINERS;

    MainUnitName = main_unit;
    MainFormat = main_format;
    MainDecimals = decimals;
    MainOffset = 0.0;
    MainScale = 1.0;

    UnitName = MainUnitName;
    Format = MainFormat;
    Decimals = MainDecimals;
    Offset = MainOffset;
    Scale = MainScale;
}

//------------------------------------------------------------------------------

CStdPhysicalQuantity::CStdPhysicalQuantity(CExtComObject* p_parent,
                                        const QString& quantity_name,
                                        const QString& main_unit,
                                        char main_format,
                                        int decimals,
                                        double offset,
                                        double scale)
    : CPhysicalQuantity(&StdPhysicalQuantityObject,p_parent)
{
    setObjectName(quantity_name);

    MainUnitName = main_unit;
    MainFormat = main_format;
    MainDecimals = decimals;
    MainOffset = offset;
    MainScale = scale;

    UnitName = MainUnitName;
    Format = MainFormat;
    Decimals = MainDecimals;
    Offset = MainOffset;
    Scale = MainScale;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CStdPhysicalQuantity::GetQuantityName(void) const
{
    return(objectName());
}

//------------------------------------------------------------------------------

const QString CStdPhysicalQuantity::GetUnitName(void) const
{
    return(UnitName);
}

//------------------------------------------------------------------------------

char CStdPhysicalQuantity::GetUnitFormat(void) const
{
    return(Format);
}

//------------------------------------------------------------------------------

int CStdPhysicalQuantity::GetUnitDecimals(void) const
{
    return(Decimals);
}

//------------------------------------------------------------------------------

double CStdPhysicalQuantity::GetUnitOffset(void) const
{
    return(Offset);
}

//------------------------------------------------------------------------------

double CStdPhysicalQuantity::GetUnitScale(void) const
{
    return(Scale);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CStdPhysicalQuantity::ActivateUnit(int index)
{
    CXMLElement*   p_setup = GetQuantityXMLSetup();
    CXMLIterator   I(p_setup);

    if( (index < -1) ||
            (index > I.GetNumberOfChildElements("UNIT")) ) {
        RUNTIME_ERROR("index out of range");
    }

    // index == 0 for mainunit, or if there is no user unit
    if( I.GetNumberOfChildElements("UNIT") == 0 || index == 0) {
        UnitName = MainUnitName;
        Format = MainFormat;
        Decimals = MainDecimals;
        Offset = MainOffset;
        Scale = MainScale;

        if(index == 0) {
            // deactivate user units
            I.SetToFirstChildElement();
            CXMLElement* p_lu;
            while( (p_lu = I.GetNextChildElement("UNIT")) != NULL ) {
                p_lu->SetAttribute("active",false);
            }
        }

        emit OnUnitChanged();
        if( PhysicalQuantities ) PhysicalQuantities->EmitOnUnitChanged();

        return(true);
    }

    // find unit setup
    CXMLElement*   p_unit = NULL;

    if( index == -1 ) {
        // reactivate active unit
        while( (p_unit = I.GetNextChildElement("UNIT")) != NULL ) {
            bool active = false;
            p_unit->GetAttribute("active",active);
            if( active ) break;
        }
    } else {
        p_unit = I.SetToChildElement("UNIT",index - 1);
    }

    if( p_unit == NULL ) {
        // if no unit is active - activate the main unit
        UnitName = MainUnitName;
        Format = MainFormat;
        Decimals = MainDecimals;
        Offset = MainOffset;
        Scale = MainScale;

        I.SetToFirstChildElement();
        CXMLElement* p_lu;
        while( (p_lu = I.GetNextChildElement("UNIT")) != NULL ) {
            p_lu->SetAttribute("active",false);
        }

        emit OnUnitChanged();
        if( PhysicalQuantities ) PhysicalQuantities->EmitOnUnitChanged();

        return(true);
    }

    // update active flag -----------
    I.SetToFirstChildElement();
    CXMLElement* p_lu;
    while( (p_lu = I.GetNextChildElement("UNIT")) != NULL ) {
        p_lu->SetAttribute("active",false);
    }

    // set data ---------------------
    bool result = true;
    result &= p_unit->GetAttribute("name",UnitName);
    result &= p_unit->GetAttribute("format",Format);
    result &= p_unit->GetAttribute("decimals",Decimals);
    result &= p_unit->GetAttribute("offset",Offset);
    result &= p_unit->GetAttribute("scale",Scale);

    // and set item as active
    p_unit->SetAttribute("active",true);

    if( result == false ) {
        ES_ERROR("data corrupted");
    }

    if( result == true ) {
        emit OnUnitChanged();
        if( PhysicalQuantities ) PhysicalQuantities->EmitOnUnitChanged();
    }

    return(result);
}

//------------------------------------------------------------------------------

int CStdPhysicalQuantity::GetNumberOfUnits(void)
{
    CXMLElement*   p_setup = GetQuantityXMLSetup();
    CXMLIterator   I(p_setup);
    return((I.GetNumberOfChildElements("UNIT"))+1);
}

//------------------------------------------------------------------------------

bool CStdPhysicalQuantity::NewUnit(const QString& name,
                                char format, int decimals,
                                double offset, double scale, bool active)
{
    CXMLElement*   p_setup = GetQuantityXMLSetup();
    CXMLElement*   p_unit;

    if((p_unit = p_setup->CreateChildElement("UNIT")) == NULL) {
        ES_ERROR("index out of range");
        return(false);
    }

    bool result = true;
    p_unit->SetAttribute("name",name);
    p_unit->SetAttribute("format",format);
    p_unit->SetAttribute("decimals",decimals);
    p_unit->SetAttribute("offset",offset);
    p_unit->SetAttribute("scale",scale);
    p_unit->SetAttribute("active",active);

    if(active) {
        ActivateUnit(GetNumberOfUnits()-1);
    }

    emit OnUnitChanged();
    if( PhysicalQuantities ) PhysicalQuantities->EmitOnUnitChanged();

    return(result);
}

//------------------------------------------------------------------------------

void CStdPhysicalQuantity::RemoveUnit(int index)
{
    CXMLElement*   p_setup = GetQuantityXMLSetup();
    CXMLIterator   I(p_setup);
    CXMLElement*   p_unit;

    if((p_unit = I.SetToChildElement("UNIT",index-1)) == NULL) {
        LOGIC_ERROR("index out of range");
    }
    delete p_unit;
    ActivateUnit(-1);

    emit OnUnitChanged();
    if( PhysicalQuantities ) PhysicalQuantities->EmitOnUnitChanged();
}

//------------------------------------------------------------------------------

bool CStdPhysicalQuantity::GetUnit(int index,
                                QString& name, char& format, int& decimals,
                                double& offset, double& scale, bool& active)
{
    CXMLElement*   p_setup = GetQuantityXMLSetup();
    CXMLIterator   I(p_setup);
    CXMLElement*   p_unit;

    if(index==0) {
        name = MainUnitName;
        format = MainFormat;
        decimals = MainDecimals;
        offset = MainOffset;
        scale = MainScale;

        I.SetToFirstChildElement();
        bool oact;
        active = true;
        while( (p_unit = I.GetNextChildElement("UNIT")) != NULL ) {
            p_unit->GetAttribute("active", oact);
            active &= (!oact);
        }
        return (true);
    }

    if((p_unit = I.SetToChildElement("UNIT",index-1)) == NULL) {
        LOGIC_ERROR("index out of range");
    }

    bool result = true;
    result &= p_unit->GetAttribute("name",name);
    result &= p_unit->GetAttribute("format",format);
    result &= p_unit->GetAttribute("decimals",decimals);
    result &= p_unit->GetAttribute("offset",offset);
    result &= p_unit->GetAttribute("scale",scale);
    result &= p_unit->GetAttribute("active",active);

    return(result);
}

//------------------------------------------------------------------------------

void CStdPhysicalQuantity::SetUnit(int index,
                                const QString& name, char format,
                                int decimals, double offset, double scale, bool active)
{
    CXMLElement*   p_setup = GetQuantityXMLSetup();
    CXMLIterator   I(p_setup);
    CXMLElement*   p_unit;

    if(index == 0) {
        ActivateUnit(0);
        emit OnUnitChanged();
        if( PhysicalQuantities ) PhysicalQuantities->EmitOnUnitChanged();
        return;
    }

    if((p_unit = I.SetToChildElement("UNIT",index-1)) == NULL) {
        LOGIC_ERROR("index out of range");
    }

    p_unit->SetAttribute("name",name);
    p_unit->SetAttribute("format",format);
    p_unit->SetAttribute("decimals",decimals);
    p_unit->SetAttribute("offset",offset);
    p_unit->SetAttribute("scale",scale);

    if(active) {
        ActivateUnit(index);
    } else {
        ActivateUnit(0);
    }

    emit OnUnitChanged();
    if( PhysicalQuantities ) PhysicalQuantities->EmitOnUnitChanged();
}

//------------------------------------------------------------------------------

void CStdPhysicalQuantity::SetUnit(int index,
                                const QString& name, char format,
                                int decimals, double offset, double scale)
{
    CXMLElement*   p_setup = GetQuantityXMLSetup();
    CXMLIterator   I(p_setup);
    CXMLElement*   p_unit;

    if((p_unit = I.SetToChildElement("UNIT",index-1)) == NULL) {
        RUNTIME_ERROR("index out of range");
    }

    p_unit->SetAttribute("name",name);
    p_unit->SetAttribute("format",format);
    p_unit->SetAttribute("decimals",decimals);
    p_unit->SetAttribute("offset",offset);
    p_unit->SetAttribute("scale",scale);

    ActivateUnit(-1);
    emit OnUnitChanged();
    if( PhysicalQuantities ) PhysicalQuantities->EmitOnUnitChanged();
}

//------------------------------------------------------------------------------

CXMLElement* CStdPhysicalQuantity::GetQuantityXMLSetup(void)
{
    CPhysicalQuantities* p_parent = dynamic_cast<CPhysicalQuantities*>(parent());
    if( p_parent == NULL ){
        LOGIC_ERROR("parent is not available");
    }
    return(p_parent->GetQuantityXMLSetup(objectName()));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



