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

#include <BondList.hpp>
#include <Structure.hpp>
#include <TemplIterator.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <BondHistory.hpp>
#include <Atom.hpp>
#include <OpenBabelUtils.hpp>
#include <AtomList.hpp>
#include <HistoryNode.hpp>
#include <BondListHistory.hpp>

#include "BondList.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        BondListID(
                    "{BOND_LIST:f2ba967c-fb2c-4727-84f3-eb48cab26cb6}",
                    "Bonds");

CPluginObject   BondListObject(&NemesisCorePlugin,
                    BondListID,STRUCTURE_CAT,
                    ":/images/NemesisCore/structure/BondList.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBondList::CBondList(CStructure* p_str)
    : CProObject(&BondListObject,p_str,p_str->GetProject())
{
    Changed = false;
    UpdateLevel = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBond* CBondList::CreateBondWH(CAtom *p_a1,CAtom *p_a2,EBondOrder order)
{
    CHistoryNode*   p_history;
    p_history = BeginChangeWH(EHCL_TOPOLOGY,"add bond");
    if( p_history == NULL ) return (NULL);

    CBond* p_bond = CreateBond(p_a1,p_a2,order,p_history);

    EndChangeWH();
    return(p_bond);
}

//------------------------------------------------------------------------------

bool CBondList::AddBondsWH(void)
{
    CHistoryNode*    p_history;
    p_history = BeginChangeWH(EHCL_TOPOLOGY,"add single bonds");
    if( p_history == NULL ) return (false);

    // protonate structure by openbabel
    OpenBabel::OBMol obmol;
    COpenBabelUtils::Nemesis2OpenBabel(GetStructure(),obmol,true);

    unsigned int top_bond = obmol.NumBonds();

    // newly added atoms are added to the end of molecule
    obmol.ConnectTheDots();

    // add added hydrogens new hydrogens
    for(unsigned int i= top_bond ; i < obmol.NumBonds(); i++) {
        OpenBabel::OBBond* p_obbond = obmol.GetBond(i);

        // atom 1
        OpenBabel::OBAtom* p_obatom1 = p_obbond->GetBeginAtom();
        CAtom* p_atom1 = static_cast<CAtom*>(GetStructure()->GetAtoms()->children()[p_obatom1->GetIdx()-1]);

        // atom 2
        OpenBabel::OBAtom* p_obatom2 = p_obbond->GetEndAtom();
        CAtom* p_atom2 = static_cast<CAtom*>(GetStructure()->GetAtoms()->children()[p_obatom2->GetIdx()-1]);

        // bond
        CreateBond(p_atom1,p_atom2,BO_SINGLE,p_history);
    }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CBondList::RemoveBondsWH(void)
{
    CHistoryNode*    p_history;
    p_history = BeginChangeWH(EHCL_TOPOLOGY,"delete all bonds");
    if( p_history == NULL ) return (false);

    foreach(QObject* p_qobj,children()) {
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        p_bond->RemoveFromBaseList(p_history);
    }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CBondList::RemoveSelectedBondsWH(void)
{
    CHistoryNode*    p_history;
    p_history = BeginChangeWH(EHCL_TOPOLOGY,"delete selected bonds");
    if( p_history == NULL ) return (false);

    foreach(QObject* p_qobj,children()) {
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        if( p_bond->IsFlagSet(EPOF_SELECTED) ){
            p_bond->RemoveFromBaseList(p_history);
        }
    }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CBondList::RemoveInvalidBondsWH(void)
{
    CHistoryNode*    p_history;
    p_history = BeginChangeWH(EHCL_TOPOLOGY,"delete invalid bonds");
    if( p_history == NULL ) return (false);

    foreach(QObject* p_qobj,children()) {
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        if( p_bond->IsInvalidBond() ){
            p_bond->RemoveFromBaseList(p_history);
        }
    }

    EndChangeWH();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBond* CBondList::CreateBond(CAtom *p_a1,CAtom *p_a2,EBondOrder order,
                              CHistoryNode* p_history)
{
    CBond* p_bond = new CBond(this);

    p_bond->SetBondOrder(order);
    p_bond->SetFirstAtom(p_a1);
    p_bond->SetSecondAtom(p_a2);

    if( p_history != NULL ) {
        CBondHI* p_data = new CBondHI(p_bond,EHID_FORWARD);
        p_history->Register(p_data);
    }

    ListSizeChanged();

    return(p_bond);
}

//------------------------------------------------------------------------------

CBond* CBondList::CreateBond(CXMLElement* p_ele, CHistoryNode* p_history)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CBond* p_bond = new CBond(this,true);
    p_bond->LoadData(p_ele);

    // history stuff --------------------------------
    if( p_history != NULL ) {
        CBondHI* p_data = new CBondHI(p_bond,EHID_FORWARD);
        p_history->Register(p_data);
    }

    ListSizeChanged();

    return(p_bond);
}

//------------------------------------------------------------------------------

CBond* CBondList::CreateBond(const CBondData& bond_data, CHistoryNode* p_history)
{
    CBond*p_bond = new CBond(this);

    int baseindex = 0;
    // get base index
    if( GetProject() != NULL ){
        baseindex = GetProject()->GetBaseObjectIndex();
    }

    CAtom* p_atom1;
    CAtom* p_atom2;

    p_bond->SetName(bond_data.Name);
    p_bond->SetDescription(bond_data.Description);

    p_bond->SetIndex(bond_data.Index+baseindex);

    p_atom1 = dynamic_cast<CAtom*>(GetProject()->FindObject(bond_data.A1+baseindex));
    p_bond->SetFirstAtom(p_atom1);

    p_atom2 = dynamic_cast<CAtom*>(GetProject()->FindObject(bond_data.A2+baseindex));
    p_bond->SetSecondAtom(p_atom2);

    p_bond->SetBondOrder(bond_data.Order);

    if( p_history != NULL ) {
        CBondHI* p_data = new CBondHI(p_bond,EHID_FORWARD);
        p_history->Register(p_data);
    }

    ListSizeChanged();

    return(p_bond);
}

//------------------------------------------------------------------------------

void CBondList::UnregisterAllRegisteredBonds(CHistoryNode* p_history)
{
    foreach(QObject* p_qobj,children()) {
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        p_bond->UnregisterAllRegisteredObjects(p_history);
    }
}

//------------------------------------------------------------------------------

int CBondList::AssignPBCBondIndexes(void)
{
    double minlen = GetStructure()->PBCInfo.GetLargestSphereRadius();
    int id = 0;
    foreach(QObject* p_qobj,children()) {
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        if( p_bond->GetLength() >= minlen ){
            id++;
            p_bond->SetPBCIndex(id);
        } else {
            p_bond->SetPBCIndex(0);
        }
    }
    return(id);
}

//------------------------------------------------------------------------------

void CBondList::MoveAllBondsFrom(CBondList* p_source,CHistoryNode* p_history)
{
    if( this == p_source ) return;  // source and target must be different

    BeginUpdate();
    p_source->BeginUpdate();

    QVector<int>    indexes;
    indexes.reserve(p_source->GetNumberOfBonds());

    foreach(QObject* p_qobj, p_source->children()){
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        int index = p_bond->GetIndex();
        indexes.push_back(index);
        p_bond->ChangeParent(this);
    }

    // record the change to history list
    if( p_history ){
        CHistoryItem* p_hi = new CBondListChangeParentHI(GetStructure(),
                                                         p_source->GetStructure(),indexes);
        p_history->Register(p_hi);
    }

    p_source->EndUpdate();
    EndUpdate();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CStructure* CBondList::GetStructure(void) const
{
    return(dynamic_cast<CStructure*>(parent()));
}

//------------------------------------------------------------------------------

int CBondList::GetNumberOfBonds(void)
{
    return(children().count());
}

//------------------------------------------------------------------------------

int CBondList::GetNumberOfCompletedBonds(void)
{
    int count = 0;
    foreach(QObject* p_qobj, children()){
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        if( ! p_bond->IsInvalidBond() ) count++;
    }
    return(count);
}

//------------------------------------------------------------------------------

bool CBondList::HasInvalidBonds(void)
{
    bool invalid = false;
    foreach(QObject* p_qobj, children()){
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        invalid |= p_bond->IsInvalidBond();
    }
    return(invalid);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBondList::LoadData(CXMLElement* p_ele,CHistoryNode* p_history)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "bonds" ) {
        LOGIC_ERROR("p_ele is not bonds");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    // load data ------------------------------------
    CXMLElement* p_ael;
    p_ael = p_ele->GetFirstChildElement("bond");
    while( p_ael != NULL ) {
        CreateBond(p_ael,p_history);
        p_ael = p_ael->GetNextSiblingElement("bond");
    }
}

//------------------------------------------------------------------------------

void CBondList::LoadData(CXMLElement* p_ele)
{
    LoadData(p_ele,NULL);
}

//------------------------------------------------------------------------------

void CBondList::SaveData(CXMLElement* p_ele,bool selected)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "bonds" ) {
        LOGIC_ERROR("p_ele is not bonds");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    // save data ------------------------------------
    foreach(QObject* p_qobj,children()) {
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        if( selected ) {
            // for EPOF_MANIP_FLAG see to bool CStructure::Copy(CXMLElement* p_ele,bool selected_only);
            if( p_bond->IsFlagSet(EPOF_MANIP_FLAG) ) {
                CXMLElement* p_bel = p_ele->CreateChildElement("bond");
                p_bond->SaveData(p_bel);
            }
        } else {
            CXMLElement* p_bel = p_ele->CreateChildElement("bond");
            p_bond->SaveData(p_bel);
        }
    }
}

//------------------------------------------------------------------------------

void CBondList::SaveData(CXMLElement* p_ele)
{
    SaveData(p_ele,false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBondList::BeginUpdate(void)
{
    UpdateLevel++;
    blockSignals(true);
}

//------------------------------------------------------------------------------

void CBondList::EndUpdate(void)
{
    if( UpdateLevel == 0 ) return;
    UpdateLevel--;
    if( UpdateLevel == 0 ){
        blockSignals(false);
        if( Changed ){
            emit OnBondListChanged();
            Changed = false;
        }
    }
}

//------------------------------------------------------------------------------

void CBondList::EmitOnBondListChanged(void)
{
    if( UpdateLevel > 0 ) Changed = true;
    emit OnBondListChanged();
}

//------------------------------------------------------------------------------

void CBondList::ListSizeChanged(void)
{
    if( UpdateLevel > 0 ){
        Changed = true;
        return;
    }
    EmitOnBondListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
