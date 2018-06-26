// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <EnergyProperty.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <EnergyPropertyHistory.hpp>
#include <HistoryNode.hpp>
#include <PropertyList.hpp>
#include <PhysicalQuantities.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* EnergyPropertyCB(void* p_data);

CExtUUID        EnergyPropertyID(
                    "{ENERGY_PROPERTY:ac7ee456-fe3e-4f3f-b2c3-c7205c89dbda}",
                    "Energy");

CPluginObject   EnergyPropertyObject(&NemesisCorePlugin,
                    EnergyPropertyID,PROPERTY_CAT,
                    ":/images/NemesisCore/properties/Energy.svg",
                    EnergyPropertyCB);

// -----------------------------------------------------------------------------

QObject* EnergyPropertyCB(void* p_data)
{
    return(new CEnergyProperty(static_cast<CPropertyList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CEnergyProperty::CEnergyProperty(CPropertyList *p_bl)
    : CProperty(&EnergyPropertyObject,p_bl)
{
    PropUnit = PQ_ENERGY;
    SET_FLAG(PropFlags,EPF_SCALAR_VALUE,true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CEnergyProperty::SetEnergyWH(double energy)
{
    if( GetEnergy() == energy ) return(true);

    // with history
    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,"energy value");
    if( p_history == NULL ) return(false);

    SetEnergy(energy,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CEnergyProperty::SetMethodWH(const QString& method)
{
    if( GetMethod() == method ) return(true);

    // with history
    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,"energy method");
    if( p_history == NULL ) return(false);

    SetMethod(method,p_history);

    EndChangeWH();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CEnergyProperty::SetEnergy(double energy,CHistoryNode* p_history)
{
    if( Energy == energy ) return;

    if( p_history ){
        CEnergyPropertyEnergyHI* p_hnode = new CEnergyPropertyEnergyHI(this,energy);
        p_history->Register(p_hnode);
    }

    Energy = energy;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CEnergyProperty::SetScalarValue(double value)
{
    if( Energy == value ) return;
    Energy = value;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CEnergyProperty::SetMethod(const QString& method,CHistoryNode* p_history)
{
    if( Method == method ) return;

    if( p_history ){
        CEnergyPropertyMethodHI* p_hnode = new CEnergyPropertyMethodHI(this,method);
        p_history->Register(p_hnode);
    }

    Method = method;
    emit OnStatusChanged(ESC_OTHER);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

double CEnergyProperty::GetScalarValue(void)
{
    return(GetEnergy());
}

//------------------------------------------------------------------------------

double CEnergyProperty::GetEnergy(void) const
{
    return(Energy);
}

//------------------------------------------------------------------------------

const QString& CEnergyProperty::GetMethod(void) const
{
    return(Method);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CEnergyProperty::LoadData(CXMLElement* p_ele)
{
    // FIXME
}

//------------------------------------------------------------------------------

void CEnergyProperty::SaveData(CXMLElement* p_ele)
{
    // FIXME
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


