// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <BondHistory.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <Bond.hpp>
#include <ProObjectHistory.hpp>
#include <ErrorSystem.hpp>
#include <BondList.hpp>
#include <NemesisCoreModule.hpp>
#include <Atom.hpp>
#include <XMLElement.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,BondHI,
                        "{BOND:f151cd55-781f-4773-9cd3-213308f14095}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,BondOrderHI,
                        "{BOND_ORD:065fe68e-56c5-4b9e-b0ce-ab1ebfa1b949}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,BondTypeHI,
                        "{BOND_TY:f72ff2a7-9a83-4e0c-b88b-521ea46b225c}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,BondSetFirstAtomHI,
                        "{BOND_FA:388f7a37-4c83-4666-bd43-9a3c569ef1b6}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,BondSetSecondAtomHI,
                        "{BOND_SA:37ae9fc0-f167-467c-83cb-731f6ad082b2}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBondHI::CBondHI(CBond* p_bond,EHistoryItemDirection change)
    : CHistoryItem(&BondHIObject,p_bond->GetProject(),change)
{
    MoleculeIndex = p_bond->GetStructure()->GetIndex();
    CXMLElement* p_ele = BondData.CreateChildElement("bond");
    p_bond->SaveData(p_ele);
}

//------------------------------------------------------------------------------

void CBondHI::Forward(void)
{
    CStructure* p_str = dynamic_cast<CStructure*>(GetProject()->FindObject(MoleculeIndex));
    if(p_str == NULL) return;

    CXMLElement* p_ele = BondData.GetFirstChildElement("bond");
    p_str->GetBonds()->CreateBond(p_ele);
}

//------------------------------------------------------------------------------

void CBondHI::Backward(void)
{
    CXMLElement* p_ele = BondData.GetFirstChildElement("bond");
    if( p_ele == NULL ) return;
    int bond_index = -1;
    p_ele->GetAttribute("id",bond_index);

    CBond* p_bond = dynamic_cast<CBond*>(GetProject()->FindObject(bond_index));
    if(p_bond == NULL) return;

    // save designer setup
    CXMLElement* p_sdele = p_bond->GetDesignerData();
    if( p_sdele ){
        CXMLElement* p_tdele = BondData.GetChildElementByPath("bond/designer",true);
        p_tdele->RemoveAllChildNodes();
        p_tdele->RemoveAllAttributes();
        p_tdele->CopyContentsFrom(p_sdele);
    }

    p_bond->RemoveFromBaseList();
}

//------------------------------------------------------------------------------

void CBondHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("mi",MoleculeIndex);
    CXMLElement* p_sele = p_ele->GetFirstChildElement("bond");
    if( p_sele == NULL ) return;
    BondData.RemoveAllChildNodes();
    CXMLElement* p_dele = BondData.CreateChildElement("bond");
    p_dele->CopyContentsFrom(p_sele);
}

//------------------------------------------------------------------------------

void CBondHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("mi",MoleculeIndex);
    CXMLElement* p_sele = BondData.GetFirstChildElement("bond");
    if( p_sele == NULL ) return;
    CXMLElement* p_dele = p_ele->CreateChildElement("bond");
    p_dele->CopyContentsFrom(p_sele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBondOrderHI::CBondOrderHI(CBond* p_bond,EBondOrder order)
    : CHistoryItem(&BondOrderHIObject,p_bond->GetProject(),EHID_FORWARD)
{
    BondIndex = p_bond->GetIndex();
    NewOrder = order;
    OldOrder = p_bond->GetBondOrder();
}

//------------------------------------------------------------------------------

void CBondOrderHI::Forward(void)
{
    CBond *p_bond = dynamic_cast<CBond*>(GetProject()->FindObject(BondIndex));
    if(p_bond != NULL) p_bond->SetBondOrder(NewOrder);
}

//------------------------------------------------------------------------------

void CBondOrderHI::Backward(void)
{
    CBond *p_bond = dynamic_cast<CBond*>(GetProject()->FindObject(BondIndex));
    if(p_bond != NULL) p_bond->SetBondOrder(OldOrder);
}

//------------------------------------------------------------------------------

void CBondOrderHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("bi",BondIndex);
    p_ele->GetAttribute("no",NewOrder);
    p_ele->GetAttribute("oo",OldOrder);
}

//------------------------------------------------------------------------------

void CBondOrderHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("bi",BondIndex);
    p_ele->SetAttribute("no",NewOrder);
    p_ele->SetAttribute("oo",OldOrder);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBondTypeHI::CBondTypeHI(CBond* p_bond,const QString& type)
    : CHistoryItem(&BondTypeHIObject,p_bond->GetProject(),EHID_FORWARD)
{
    BondIndex = p_bond->GetIndex();
    NewType = type;
    OldType = p_bond->GetBondType();
}

//------------------------------------------------------------------------------

void CBondTypeHI::Forward(void)
{
    CBond *p_bond = dynamic_cast<CBond*>(GetProject()->FindObject(BondIndex));
    if(p_bond != NULL) p_bond->SetBondType(NewType);
}

//------------------------------------------------------------------------------

void CBondTypeHI::Backward(void)
{
    CBond *p_bond = dynamic_cast<CBond*>(GetProject()->FindObject(BondIndex));
    if(p_bond != NULL) p_bond->SetBondType(OldType);
}

//------------------------------------------------------------------------------

void CBondTypeHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("bi",BondIndex);
    p_ele->GetAttribute("nt",NewType);
    p_ele->GetAttribute("ot",OldType);
}

//------------------------------------------------------------------------------

void CBondTypeHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("bi",BondIndex);
    p_ele->SetAttribute("nt",NewType);
    p_ele->SetAttribute("ot",OldType);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBondSetFirstAtomHI::CBondSetFirstAtomHI(CBond* p_bond,CAtom* p_fa)
    : CHistoryItem(&BondSetFirstAtomHIObject,p_bond->GetProject(),EHID_FORWARD)
{
    BondIndex = p_bond->GetIndex();
    if( p_bond->GetFirstAtom() ){
        OldFirstAtomIndex =  p_bond->GetFirstAtom()->GetIndex();
    } else {
        OldFirstAtomIndex = -1;
    }
    if( p_fa ){
        NewFirstAtomIndex =  p_fa->GetIndex();
    } else {
        NewFirstAtomIndex = -1;
    }
}

//------------------------------------------------------------------------------

void CBondSetFirstAtomHI::Forward(void)
{
    CBond *p_bond = dynamic_cast<CBond*>(GetProject()->FindObject(BondIndex));
    if(p_bond == NULL) return;
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(NewFirstAtomIndex));
    p_bond->SetFirstAtom(p_atom);
}

//------------------------------------------------------------------------------

void CBondSetFirstAtomHI::Backward(void)
{
    CBond *p_bond = dynamic_cast<CBond*>(GetProject()->FindObject(BondIndex));
    if(p_bond == NULL) return;
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(OldFirstAtomIndex));
    p_bond->SetFirstAtom(p_atom);
}

//------------------------------------------------------------------------------

void CBondSetFirstAtomHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("bi",BondIndex);
    p_ele->GetAttribute("nf",OldFirstAtomIndex);
    p_ele->GetAttribute("of",NewFirstAtomIndex);
}

//------------------------------------------------------------------------------

void CBondSetFirstAtomHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("bi",BondIndex);
    p_ele->SetAttribute("nf",OldFirstAtomIndex);
    p_ele->SetAttribute("of",NewFirstAtomIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBondSetSecondAtomHI::CBondSetSecondAtomHI(CBond* p_bond,CAtom* p_fa)
    : CHistoryItem(&BondSetSecondAtomHIObject,p_bond->GetProject(),EHID_FORWARD)
{
    BondIndex = p_bond->GetIndex();
    if( p_bond->GetSecondAtom() ){
        OldSecondAtomIndex =  p_bond->GetSecondAtom()->GetIndex();
    } else {
        OldSecondAtomIndex = -1;
    }
    if( p_fa ){
        NewSecondAtomIndex =  p_fa->GetIndex();
    } else {
        NewSecondAtomIndex = -1;
    }
}

//------------------------------------------------------------------------------

void CBondSetSecondAtomHI::Forward(void)
{
    CBond *p_bond = dynamic_cast<CBond*>(GetProject()->FindObject(BondIndex));
    if(p_bond == NULL) return;
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(NewSecondAtomIndex));
    p_bond->SetSecondAtom(p_atom);
}

//------------------------------------------------------------------------------

void CBondSetSecondAtomHI::Backward(void)
{
    CBond *p_bond = dynamic_cast<CBond*>(GetProject()->FindObject(BondIndex));
    if(p_bond == NULL) return;
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(OldSecondAtomIndex));
    p_bond->SetSecondAtom(p_atom);
}

//------------------------------------------------------------------------------

void CBondSetSecondAtomHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("bi",BondIndex);
    p_ele->GetAttribute("ns",OldSecondAtomIndex);
    p_ele->GetAttribute("os",NewSecondAtomIndex);
}

//------------------------------------------------------------------------------

void CBondSetSecondAtomHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("bi",BondIndex);
    p_ele->SetAttribute("ns",OldSecondAtomIndex);
    p_ele->SetAttribute("os",NewSecondAtomIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
