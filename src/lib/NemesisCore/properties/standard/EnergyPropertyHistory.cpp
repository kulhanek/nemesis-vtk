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

#include <EnergyPropertyHistory.hpp>
#include <EnergyProperty.hpp>
#include <ProObjectHistory.hpp>
#include <Project.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,EnergyPropertyEnergyHI,
                        "{ENEP_ENE:2b8788a4-3022-416c-8a21-73f50c816c80}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,EnergyPropertyMethodHI,
                        "{ENEP_MET:bcd16709-15c6-4d3a-8aa0-227874ecbfea}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CEnergyPropertyEnergyHI::CEnergyPropertyEnergyHI(
                        CEnergyProperty* p_prop,double newenergy)
    : CHistoryItem(&EnergyPropertyEnergyHIObject,p_prop->GetProject(),EHID_FORWARD)
{
    ObjectID = p_prop->GetIndex();
    OldEnergy = p_prop->GetEnergy();
    NewEnergy = newenergy;
}

//------------------------------------------------------------------------------

void CEnergyPropertyEnergyHI::Forward(void)
{
    CEnergyProperty* p_go = dynamic_cast<CEnergyProperty*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetEnergy(NewEnergy);
}

//------------------------------------------------------------------------------

void CEnergyPropertyEnergyHI::Backward(void)
{
    CEnergyProperty* p_go = dynamic_cast<CEnergyProperty*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetEnergy(OldEnergy);
}

//------------------------------------------------------------------------------

void CEnergyPropertyEnergyHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("pi",ObjectID);
    p_ele->GetAttribute("ne",NewEnergy);
    p_ele->GetAttribute("oe",OldEnergy);
}

//------------------------------------------------------------------------------

void CEnergyPropertyEnergyHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("pi",ObjectID);
    p_ele->SetAttribute("ne",NewEnergy);
    p_ele->SetAttribute("oe",OldEnergy);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CEnergyPropertyMethodHI::CEnergyPropertyMethodHI(
                        CEnergyProperty* p_prop,const QString& newmethod)
    : CHistoryItem(&EnergyPropertyMethodHIObject,p_prop->GetProject(),EHID_FORWARD)
{
    ObjectID = p_prop->GetIndex();
    OldMethod = p_prop->GetMethod();
    NewMethod = newmethod;
}

//------------------------------------------------------------------------------

void CEnergyPropertyMethodHI::Forward(void)
{
    CEnergyProperty* p_go = dynamic_cast<CEnergyProperty*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetMethod(NewMethod);
}

//------------------------------------------------------------------------------

void CEnergyPropertyMethodHI::Backward(void)
{
    CEnergyProperty* p_go = dynamic_cast<CEnergyProperty*>(GetProject()->FindObject(ObjectID));
    if(p_go == NULL) return;
    p_go->SetMethod(OldMethod);
}

//------------------------------------------------------------------------------

void CEnergyPropertyMethodHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("pi",ObjectID);
    p_ele->GetAttribute("nm",NewMethod);
    p_ele->GetAttribute("om",OldMethod);
}

//------------------------------------------------------------------------------

void CEnergyPropertyMethodHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("pi",ObjectID);
    p_ele->SetAttribute("nm",NewMethod);
    p_ele->SetAttribute("om",OldMethod);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


