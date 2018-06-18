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

#include <RestraintList.hpp>
#include <Restraint.hpp>
#include <RestraintHistory.hpp>
#include <Atom.hpp>
#include <SelectionList.hpp>

#include <Structure.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <PluginDatabase.hpp>
#include <Property.hpp>

#include <RestraintListHistory.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        RestraintListID(
                    "{RESTRAINT_LIST:6e1dc6e5-cf47-4155-87a6-07821fad1629}",
                    "Restraints");

CPluginObject   RestraintListObject(&NemesisCorePlugin,
                    RestraintListID,STRUCTURE_CAT,
                    ":/images/NemesisCore/structure/RestraintList.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRestraintList::CRestraintList(CStructure* p_str)
    : CProObject(&RestraintListObject,p_str,p_str->GetProject())
{
    Changed = false;
    UpdateLevel = 0;

    SetFlag(EPOF_ENABLED,true);
}

//------------------------------------------------------------------------------

CRestraintList::~CRestraintList(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CRestraintList::IsEnabled(void)
{
    return(IsFlagSet(EPOF_ENABLED));
}

//------------------------------------------------------------------------------

bool CRestraintList::IsRestrained(CProperty* p_prop)
{
    foreach(QObject* p_qobj,children()) {
        CRestraint* p_res = static_cast<CRestraint*>(p_qobj);
        if( p_res->GetProperty() == p_prop ) return(true);
    }

    return(false);
}

//------------------------------------------------------------------------------

CStructure* CRestraintList::GetStructure(void) const
{
    return(dynamic_cast<CStructure*>(parent()));
}

//------------------------------------------------------------------------------

int CRestraintList::GetNumberOfRestraints(void)
{
    return(children().count());
}

//------------------------------------------------------------------------------

double CRestraintList::GetEnergy(CVector& gradients,const QHash<CAtom*,int>& map)
{
    gradients.SetZero();

    double energy = 0.0;

    if( IsFlagSet(EPOF_ENABLED) == false ){
        return(energy);
    }

    // for each restraint
    foreach(QObject* p_qobj,children()) {
        CRestraint* p_res = static_cast<CRestraint*>(p_qobj);
        if( ! p_res->IsEnabled() ) continue;

        // get restraint energy and gradients
        QVector<CAtomGrad>  grads;
        energy += p_res->GetEnergy(grads);

        // map restraint gradients to linear molecule gradient
        for(int i=0; i < grads.size(); i++){
            CAtom* p_at = grads.at(i).Atom;
            if( p_at == NULL ){
                RUNTIME_ERROR("grads.at(i).Atom is NULL");
            }
            if( p_at->GetStructure() != GetStructure() ) continue;
            int idx = map[p_at];
            CPoint grd = grads.at(i).Grad;
            gradients[idx*3+0] += grd.x;
            gradients[idx*3+1] += grd.y;
            gradients[idx*3+2] += grd.z;
        }
    }

    return(energy);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRestraint* CRestraintList::CreateRestraintWH(const QString& name,
                                              const QString& descr)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GEOMETRY,tr("create a new restraint"));
    if( p_history == NULL ) return (NULL);

    CRestraint* p_rst = NULL;
    p_rst = CreateRestraint(name,descr,p_history);

    EndChangeWH();
    return(p_rst);
}

//------------------------------------------------------------------------------

bool CRestraintList::DeleteAllRestraintsWH(void)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GEOMETRY,tr("delete all restraints"));
    if( p_history == NULL ) return(false);

    foreach(QObject* p_qobj,children()) {
        CRestraint* p_res = static_cast<CRestraint*>(p_qobj);
        p_res->RemoveFromBaseList(p_history);
    }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRestraintList::DeleteInvalidRestraintsWH(void)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GEOMETRY,tr("delete invalid restraints"));
    if( p_history == NULL ) return(false);

    foreach(QObject* p_qobj,children()) {
        CRestraint* p_res = static_cast<CRestraint*>(p_qobj);
        if( p_res->GetProperty() == NULL ){
            p_res->RemoveFromBaseList(p_history);
        } else {
            if( p_res->GetProperty()->IsReady() == false ){
                // property removal removes also the restraint
                p_res->GetProperty()->RemoveFromBaseList(p_history);
            }
        }
    }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRestraintList::DeleteSelectedRestraintsWH(void)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GEOMETRY,tr("delete selected restraints"));
    if( p_history == NULL ) return(false);

    foreach(QObject* p_qobj,children()) {
        CRestraint* p_res = static_cast<CRestraint*>(p_qobj);
        if( p_res->IsFlagSet(EPOF_SELECTED) ) {
            p_res->RemoveFromBaseList(p_history);
        }
    }

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRestraintList::EnableRestraintsWH(bool set)
{
    if( IsFlagSet(EPOF_ENABLED) == set ) return(true);

    QString msg;
    if( set ){
        msg = tr("enable all restraints");
    } else {
        msg = tr("disable all restraints");
    }

    CHistoryNode* p_history = BeginChangeWH(EHCL_GEOMETRY,msg);
    if( p_history == NULL ) return(false);

    EnableRestraints(set,p_history);

    EndChangeWH();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRestraint* CRestraintList::CreateRestraint(const QString& name, const QString& descr,
                                            CHistoryNode* p_history)
{
    // create restraint
    CRestraint* p_rst = new CRestraint(this);

    QString obj_name;
    if( name.isEmpty() ){
        obj_name = GenerateName("Rst%1");
    } else {
        obj_name = name;
    }

    // insert object to the list
    p_rst->SetName(obj_name);
    p_rst->SetDescription(descr);

    // register change to history
    if( p_history != NULL ){
        CRestraintHI* p_rstdata = new CRestraintHI(p_rst,EHID_FORWARD);
        p_history->Register(p_rstdata);
    }

    // notify list models
    ListSizeChanged();

    return(p_rst);
}

//------------------------------------------------------------------------------

CRestraint* CRestraintList::CreateRestraint(CXMLElement* p_data,
                                            CHistoryNode* p_history)
{
    if( p_data == NULL ){
        INVALID_ARGUMENT("p_data == NULL");
    }

    // create restraint
    CRestraint* p_rst = new CRestraint(this);

    // load data
    GetProject()->BeginLinkProcedure(0);
    p_rst->LoadData(p_data);
    GetProject()->EndLinkProcedure();

    // register change to history
    if( p_history != NULL ){
        CRestraintHI* p_rstdata = new CRestraintHI(p_rst,EHID_FORWARD);
        p_history->Register(p_rstdata);
    }

    // notify list models
    ListSizeChanged();

    return(p_rst);
}

//------------------------------------------------------------------------------

void CRestraintList::EnableRestraints(bool set, CHistoryNode* p_history)
{
    if( IsFlagSet(EPOF_ENABLED) == set ) return;

    CProObjectFlags flags = GetFlags();
    SET_FLAG(flags,EPOF_ENABLED,set);
    SetFlags(flags,p_history);

    EmitOnRestraintListChanged();
}

//------------------------------------------------------------------------------

void CRestraintList::UnregisterAllRegisteredRestraints(CHistoryNode* p_history)
{
    foreach(QObject* p_qobj,children()) {
        CRestraint* p_res = static_cast<CRestraint*>(p_qobj);
        p_res->UnregisterAllRegisteredObjects(p_history);
    }
}

//------------------------------------------------------------------------------

void CRestraintList::MoveAllRestraintsFrom(CRestraintList* p_source,CHistoryNode* p_history)
{
    if( this == p_source ) return;  // source and target must be different

    BeginUpdate();
    p_source->BeginUpdate();

    QVector<int>    indexes;
    indexes.reserve(p_source->GetNumberOfRestraints());

    foreach(QObject* p_qobj, p_source->children()){
        CRestraint* p_rst = static_cast<CRestraint*>(p_qobj);
        int index = p_rst->GetIndex();
        indexes.push_back(index);
        p_rst->ChangeParent(this);
    }

    // record the change to history list
    if( p_history ){
        CHistoryItem* p_hi = new CRestraintListChangeParentHI(GetStructure(),
                                                         p_source->GetStructure(),indexes);
        p_history->Register(p_hi);
    }

    p_source->EndUpdate();
    EndUpdate();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRestraintList::LoadData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "restraints" ) {
        LOGIC_ERROR("p_ele is not restraints");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    CXMLElement* p_rstele;
    p_rstele = p_ele->GetFirstChildElement("restraint");

    while( p_rstele != NULL ) {
        CRestraint* p_rstbj = new CRestraint(this);
        p_rstbj->LoadData(p_rstele);
        p_rstele = p_rstele->GetNextSiblingElement("restraint");
    }
}

//------------------------------------------------------------------------------

void CRestraintList::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "restraints" ) {
        LOGIC_ERROR("p_ele is not restraints");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    foreach(QObject* p_qobj,children()) {
        CRestraint* p_rst = static_cast<CRestraint*>(p_qobj);
        CXMLElement* p_rstele = p_ele->CreateChildElement("restraint");
        p_rst->SaveData(p_rstele);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRestraintList::BeginUpdate(void)
{
    UpdateLevel++;
    blockSignals(true);
}

//------------------------------------------------------------------------------

void CRestraintList::EndUpdate(void)
{
    if( UpdateLevel == 0 ) return;
    UpdateLevel--;
    if( UpdateLevel == 0 ){
        blockSignals(false);
        if( Changed ){
            emit OnRestraintListChanged();
            Changed = false;
        }
    }
}

//------------------------------------------------------------------------------

void CRestraintList::EmitOnRestraintListChanged(void)
{
    if( UpdateLevel > 0 ){
        Changed = true;
    } else {
        emit OnRestraintListChanged();
    }
}

//------------------------------------------------------------------------------

void CRestraintList::ListSizeChanged(void)
{
    if( UpdateLevel > 0 ){
        Changed = true;
        return;
    }
    EmitOnRestraintListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


