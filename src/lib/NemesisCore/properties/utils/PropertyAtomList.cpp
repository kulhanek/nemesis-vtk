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

#include <PropertyAtomList.hpp>
#include <CategoryUUID.hpp>
#include <NemesisCoreModule.hpp>
#include <Project.hpp>
#include <Property.hpp>
#include <ErrorSystem.hpp>
#include <HistoryList.hpp>
#include <PluginDatabase.hpp>
#include <PropertyAtomListHistory.hpp>
#include <XMLElement.hpp>
#include <SelectionList.hpp>
#include <Atom.hpp>
#include <PeriodicTable.hpp>
#include <Graphics.hpp>
#include <GraphicsObject.hpp>
#include <GraphicsProfile.hpp>
#include <GraphicsProfileList.hpp>
#include <GraphicsObjectList.hpp>

#include "PropertyAtomList.moc"

// remove AddAtom macro defined on Windows
#if defined AddAtom
#undef AddAtom
#endif

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        PropertyAtomListID(
                    "{PROPERTY_ATOM_LIST:5b9e75d4-a4ac-48df-bbc9-2a76b57d355f}",
                    "Property Atoms");

CPluginObject   PropertyAtomListObject(&NemesisCorePlugin,
                    PropertyAtomListID,GENERIC_CAT,NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPropertyAtomList::CPropertyAtomList(CProperty* p_property)
    : CProObject(&PropertyAtomListObject,p_property,p_property->GetProject())
{
    ConFlags |= EECOF_HIDDEN;
    SetName("");
}

//------------------------------------------------------------------------------

CPropertyAtomList::~CPropertyAtomList(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CPropertyAtomList::AddAtomWH(CAtom* p_atom)
{
    if( ContainsAtom(p_atom) ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,tr("add the atom to the property atom list"));
    if( p_history == NULL ) return (false);

    AddAtom(p_atom,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CPropertyAtomList::RemoveAtomWH(CAtom* p_atom)
{
    if( ! ContainsAtom(p_atom) ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,tr("remove the atom from the property atom list"));
    if( p_history == NULL ) return (false);

    RemoveAtom(p_atom,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CPropertyAtomList::AddSelectedAtomsWH(void)
{
    int nsel = GetProject()->GetSelection()->NumOfSelectedObjects();

    int count = 0;
    for(int i=0; i < nsel; i++){
        CProObject* p_sel = GetProject()->GetSelection()->GetSelectedObject(i);
        if( (p_sel != NULL) && p_sel->GetType() == AtomID ){
            count++;
        }
    }
    if( count == 0 ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,tr("add selected atoms to the property atom list"));
    if( p_history == NULL ) return (false);

    for(int i=0; i < nsel; i++){
        CProObject* p_sel = GetProject()->GetSelection()->GetSelectedObject(i);
        if( (p_sel != NULL) && p_sel->GetType() == AtomID ){
            AddAtom(dynamic_cast<CAtom*>(p_sel),p_history);
        }
    }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CPropertyAtomList::RemoveSelectedAtomsWH(void)
{
    int nsel = GetProject()->GetSelection()->NumOfSelectedObjects();

    int count = 0;
    for(int i=0; i < nsel; i++){
        CProObject* p_sel = GetProject()->GetSelection()->GetSelectedObject(i);
        if( (p_sel != NULL) && p_sel->GetType() == AtomID ){
            count++;
        }
    }
    if( count == 0 ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,tr("remove selected atoms from the property atom list"));
    if( p_history == NULL ) return (false);

    for(int i=0; i < nsel; i++){
        CProObject* p_sel = GetProject()->GetSelection()->GetSelectedObject(i);
        if( (p_sel != NULL) && p_sel->GetType() == AtomID ){
            RemoveAtom(dynamic_cast<CAtom*>(p_sel),p_history);
        }
    }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CPropertyAtomList::RemoveAllAtomsWH(void)
{
    if( GetNumberOfAtoms() == 0 ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,tr("remove all atoms from the property atom list"));
    if( p_history == NULL ) return (false);

    foreach(CAtom* p_atom, Atoms){
        RemoveAtom(p_atom,p_history);
    }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CPropertyAtomList::ShowAsPointWH(void)
{
    if( GetProject() == NULL ) return(false);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("create point"));
    if( p_history == NULL ) return (false);

    // create garphics object
    CGraphics* p_grp = GetProject()->GetGraphics();
    CExtUUID   uuid("{POINT_OBJECT:089f4df8-cf38-48a2-84e1-f0bac7aa6ffe}");
    CGraphicsObject* p_go = p_grp->GetObjects()->CreateObject(uuid);
    if( p_go ){
        foreach(CAtom* p_atom, Atoms){
            p_go->AddObject(p_atom,p_history);
        }
        CGraphicsProfile* p_pro = p_grp->GetProfiles()->GetActiveProfile();
        if( p_pro ){
            p_pro->AddObject(p_go,-1,p_history);
        }
    }

    EndChangeWH();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPropertyAtomList::AddAtom(CAtom* p_atom, CHistoryNode* p_history)
{
    if( p_atom == NULL ) {
        INVALID_ARGUMENT("p_atom is NULL");
    }

    // is the object already in the list?
    if( ContainsAtom(p_atom) ){
        // do not do nothing
        return;
    }

    // add objects
    Atoms.append(p_atom);

    // register object for original object
    p_atom->RegisterObject(this);

    if( p_history != NULL ) {
        CHistoryItem* p_item = new CPropertyAtomListAddAtomHI(this,p_atom,EHID_FORWARD);
        p_history->Register(p_item);
    }

    // notify about added object
    emit OnPropertyAtomListChanged();
}

//------------------------------------------------------------------------------

void CPropertyAtomList::RemoveAtom(CAtom* p_atom, CHistoryNode* p_history)
{
    if( p_atom == NULL ) {
        INVALID_ARGUMENT("p_atom is NULL");
    }

    if( ! Atoms.contains(p_atom) ){
        LOGIC_ERROR("list does not contain the object");
    }

    // remove object from the list
    Atoms.removeOne(p_atom);

    // unregister object
    p_atom->UnregisterObject(this);

    if( p_history != NULL ) {
        CHistoryItem* p_item = new CPropertyAtomListAddAtomHI(this,p_atom,EHID_BACKWARD);
        p_history->Register(p_item);
    }

    // notify about removal
    emit OnPropertyAtomListChanged();

}

//------------------------------------------------------------------------------

int CPropertyAtomList::GetNumberOfAtoms(void)
{
    return(Atoms.size());
}

//------------------------------------------------------------------------------

bool CPropertyAtomList::ContainsAtom(CAtom* p_atom)
{
    return(Atoms.contains(p_atom));
}

//------------------------------------------------------------------------------

bool CPropertyAtomList::ContainsAnyAtomFrom(CStructure* p_str)
{
    foreach(CAtom* p_atom, Atoms){
        if( p_atom->GetStructure() == p_str ) return(true);
    }
    return(false);
}

//------------------------------------------------------------------------------

CAtom* CPropertyAtomList::GetAtom(int index)
{
    return(static_cast<CAtom*>(Atoms[index]));
}

//------------------------------------------------------------------------------

CPoint CPropertyAtomList::GetCOM(void)
{
    double totmass;
    return(GetCOM(totmass));
}

//------------------------------------------------------------------------------

CPoint CPropertyAtomList::GetCOM(double& totmass)
{
    CPoint com;
    totmass = 0;

    foreach(CAtom* p_atom, Atoms){
        double mass = PeriodicTable.GetMass(p_atom->GetZ());
        totmass += mass;
        com += p_atom->GetPos()*mass;
    }
    if( totmass > 0 ){
        com /= totmass;
    }
    return(com);
}

//------------------------------------------------------------------------------

CProperty* CPropertyAtomList::GetProperty(void)
{
    return( dynamic_cast<CProperty*>(parent()) );
}

//------------------------------------------------------------------------------

const QList<CAtom*>& CPropertyAtomList::GetAtoms(void) const
{
    return(Atoms);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPropertyAtomList::LoadData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    // atoms ----------------------------------------
    CXMLElement* p_atele;
    p_atele = p_ele->GetFirstChildElement("atom");

    while( p_atele != NULL ) {
        int atid = 0;
        p_atele->GetAttribute("id",atid);
        CAtom* p_at = dynamic_cast<CAtom*>(GetProject()->FindObject(atid));
        if( p_at ){
            AddAtom(p_at);
        }
        p_atele = p_atele->GetNextSiblingElement("atom");
    }
}

//------------------------------------------------------------------------------

void CPropertyAtomList::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    // atoms ----------------------------------------
    foreach(CAtom* p_at,Atoms) {
        CXMLElement* p_atele = p_ele->CreateChildElement("atom");
        p_atele->SetAttribute("id",p_at->GetIndex());
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPropertyAtomList::RemoveFromRegistered(CProObject* p_object,CHistoryNode* p_history)
{
    // remove atom from the list
    CAtom* p_atom = dynamic_cast<CAtom*>(p_object);
    if( p_atom == NULL ) return;
    if( ContainsAtom(p_atom) ){
        RemoveAtom(p_atom,p_history);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPropertyAtomList::EmitOnPropertyAtomListChanged(void)
{
    emit OnPropertyAtomListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

