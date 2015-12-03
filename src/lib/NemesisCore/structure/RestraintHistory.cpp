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

#include <RestraintHistory.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <Restraint.hpp>
#include <ProObjectHistory.hpp>
#include <ErrorSystem.hpp>
#include <RestraintList.hpp>
#include <XMLElement.hpp>
#include <NemesisCoreModule.hpp>
#include <Property.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,RestraintHI,
                        "{RST:9389429f-8e48-4331-ac9c-dc6acbd1732b}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,RestraintTargetHI,
                        "{RST_V:97068d6d-7b7d-499a-a49a-f55b2cf567ee}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,RestraintForceHI,
                        "{RST_F:a30a3c59-2988-46be-857e-a088bac3d5dd}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,RestraintPropertyHI,
                        "{RST_P:e8dd4dd4-b1cd-494a-96cf-b7f4708c4885}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRestraintHI::CRestraintHI(CRestraint* p_res,EHistoryItemDirection change)
    : CHistoryItem(&RestraintHIObject,p_res->GetProject(),change)
{
    MoleculeIndex = p_res->GetStructure()->GetIndex();
    CXMLElement* p_ele = RestraintData.CreateChildElement("restraint");
    p_res->SaveData(p_ele);
}

//------------------------------------------------------------------------------

void CRestraintHI::Forward(void)
{
    CStructure* p_str = dynamic_cast<CStructure*>(GetProject()->FindObject(MoleculeIndex));
    if(p_str == NULL) return;

    CXMLElement* p_ele = RestraintData.GetFirstChildElement("restraint");
    if( p_ele == NULL ) return;
    p_str->GetRestraints()->CreateRestraint(p_ele);
}

//------------------------------------------------------------------------------

void CRestraintHI::Backward(void)
{
    CXMLElement* p_ele = RestraintData.GetFirstChildElement("restraint");
    if( p_ele == NULL ) return;

    int rst_index = -1;
    if( p_ele->GetAttribute("id",rst_index) == false ){
        return;
    }

    CRestraint* p_rst = dynamic_cast<CRestraint*>(GetProject()->FindObject(rst_index));
    if(p_rst == NULL) return;

    // save designer setup
    CXMLElement* p_sdele = p_rst->GetDesignerData();
    if( p_sdele ){
        CXMLElement* p_tdele = RestraintData.GetChildElementByPath("restraint/designer",true);
        p_tdele->RemoveAllChildNodes();
        p_tdele->RemoveAllAttributes();
        p_tdele->CopyContentsFrom(p_sdele);
    }

    p_rst->RemoveFromBaseList();
}

//------------------------------------------------------------------------------

void CRestraintHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("mi",MoleculeIndex);
    CXMLElement* p_sele = p_ele->GetFirstChildElement("restraint");
    if( p_sele == NULL ) return;
    RestraintData.RemoveAllChildNodes();
    CXMLElement* p_dele = RestraintData.CreateChildElement("restraint");
    p_dele->CopyContentsFrom(p_sele);
}

//------------------------------------------------------------------------------

void CRestraintHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("mi",MoleculeIndex);
    CXMLElement* p_sele = RestraintData.GetFirstChildElement("restraint");
    if( p_sele == NULL ) return;
    CXMLElement* p_dele = p_ele->CreateChildElement("restraint");
    p_dele->CopyContentsFrom(p_sele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRestraintTargetHI::CRestraintTargetHI(CRestraint* p_rst,double target)
    : CHistoryItem(&RestraintTargetHIObject,p_rst->GetProject(),EHID_FORWARD)
{
    RstIndex = p_rst->GetIndex();
    NewTargetValue = target;
    OldTargetValue = p_rst->GetTargetValue();
}

//------------------------------------------------------------------------------

void CRestraintTargetHI::Forward(void)
{
    CRestraint *p_rst = dynamic_cast<CRestraint*>(GetProject()->FindObject(RstIndex));
    if(p_rst != NULL) p_rst->SetTargetValue(NewTargetValue);
}

//------------------------------------------------------------------------------

void CRestraintTargetHI::Backward(void)
{
    CRestraint *p_rst = dynamic_cast<CRestraint*>(GetProject()->FindObject(RstIndex));
    if(p_rst != NULL) p_rst->SetTargetValue(OldTargetValue);
}

//------------------------------------------------------------------------------

void CRestraintTargetHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("ai",RstIndex);
    p_ele->GetAttribute("nv",NewTargetValue);
    p_ele->GetAttribute("ov",OldTargetValue);
}

//------------------------------------------------------------------------------

void CRestraintTargetHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("ai",RstIndex);
    p_ele->SetAttribute("nv",NewTargetValue);
    p_ele->SetAttribute("ov",OldTargetValue);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRestraintForceHI::CRestraintForceHI(CRestraint* p_rst,double target)
    : CHistoryItem(&RestraintForceHIObject,p_rst->GetProject(),EHID_FORWARD)
{
    RstIndex = p_rst->GetIndex();
    NewForceValue = target;
    OldForceValue = p_rst->GetForceConstant();
}

//------------------------------------------------------------------------------

void CRestraintForceHI::Forward(void)
{
    CRestraint *p_rst = dynamic_cast<CRestraint*>(GetProject()->FindObject(RstIndex));
    if(p_rst != NULL) p_rst->SetForceConstant(NewForceValue);
}

//------------------------------------------------------------------------------

void CRestraintForceHI::Backward(void)
{
    CRestraint *p_rst = dynamic_cast<CRestraint*>(GetProject()->FindObject(RstIndex));
    if(p_rst != NULL) p_rst->SetForceConstant(OldForceValue);
}

//------------------------------------------------------------------------------

void CRestraintForceHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("ai",RstIndex);
    p_ele->GetAttribute("nf",NewForceValue);
    p_ele->GetAttribute("of",OldForceValue);
}

//------------------------------------------------------------------------------

void CRestraintForceHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("ai",RstIndex);
    p_ele->SetAttribute("nf",NewForceValue);
    p_ele->SetAttribute("of",OldForceValue);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRestraintPropertyHI::CRestraintPropertyHI(CRestraint* p_rst,CProperty* p_prop)
    : CHistoryItem(&RestraintPropertyHIObject,p_rst->GetProject(),EHID_FORWARD)
{
    RstIndex = p_rst->GetIndex();
    if( p_prop ){
        NewPropertyIndex = p_prop->GetIndex();
    } else {
        NewPropertyIndex = -1;
    }
    if( p_rst->GetProperty() ){
        OldPropertyIndex = p_rst->GetProperty()->GetIndex();
    } else {
        OldPropertyIndex = -1;
    }
}

//------------------------------------------------------------------------------

void CRestraintPropertyHI::Forward(void)
{
    CRestraint *p_rst = dynamic_cast<CRestraint*>(GetProject()->FindObject(RstIndex));
    CProperty  *p_prop = dynamic_cast<CProperty*>(GetProject()->FindObject(NewPropertyIndex));
    if(p_rst != NULL) p_rst->SetProperty(p_prop);
}

//------------------------------------------------------------------------------

void CRestraintPropertyHI::Backward(void)
{
    CRestraint *p_rst = dynamic_cast<CRestraint*>(GetProject()->FindObject(RstIndex));
    CProperty  *p_prop = dynamic_cast<CProperty*>(GetProject()->FindObject(OldPropertyIndex));
    if(p_rst != NULL) p_rst->SetProperty(p_prop);
}

//------------------------------------------------------------------------------

void CRestraintPropertyHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("ai",RstIndex);
    p_ele->GetAttribute("ni",NewPropertyIndex);
    p_ele->GetAttribute("oi",OldPropertyIndex);
}

//------------------------------------------------------------------------------

void CRestraintPropertyHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("ai",RstIndex);
    p_ele->SetAttribute("ni",NewPropertyIndex);
    p_ele->SetAttribute("oi",OldPropertyIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
