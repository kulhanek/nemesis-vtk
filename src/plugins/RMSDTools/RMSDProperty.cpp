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

#include <QtGui>

#include "RMSDToolsModule.hpp"
#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <SelectionList.hpp>
#include <Atom.hpp>
#include <PeriodicTable.hpp>
#include <Transformation.hpp>
#include <AtomList.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>
#include <Residue.hpp>
#include <ResidueList.hpp>
#include <vector>
#include <algorithm>

#include "MapCouple.hpp"
#include "RMSDProperty.hpp"
#include "RMSDPropertySetup.hpp"
#include "MapCoupleHistory.hpp"
#include "RMSDPropertyHistory.hpp"
#include "RMSDProperty.moc"

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* RMSDPropertyCB(void* p_data);

CExtUUID        RMSDPropertyID(
                    "{RMSD_PROPERTY:f81e4c8b-d780-44f6-8ead-8cdff5497e0e}",
                    "RMSD");

CPluginObject   RMSDPropertyObject(&RMSDToolsPlugin,
                    RMSDPropertyID,PROPERTY_CAT,
                    ":/images/RMSDTools/RMSD.svg",
                    RMSDPropertyCB);

// -----------------------------------------------------------------------------

QObject* RMSDPropertyCB(void* p_data)
{
    return(new CRMSDProperty(static_cast<CPropertyList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRMSDProperty::CRMSDProperty(CPropertyList *p_bl)
    : CGraphicsProperty(&RMSDPropertyObject,p_bl)
{
    PropUnit = PQ_DISTANCE;

    Template = NULL;
    Target = NULL;

    SetFlag<ERMSDPropertyFlag>(ERMSDPF_ONLY_SAME_Z,true);
    SetFlag<ERMSDPropertyFlag>(ERMSDPF_MASS_WEIGHTED,true);
}

// -----------------------------------------------------------------------------

CRMSDProperty::~CRMSDProperty(void)
{

}

// -----------------------------------------------------------------------------

void CRMSDProperty::RemoveFromBaseList(CHistoryNode* p_history)
{
    if( Template != NULL ){
        Template->RemoveFromRegistered(this,p_history);
    }
    if( Target != NULL ){
        Target->RemoveFromRegistered(this,p_history);
    }

    CProperty::RemoveFromBaseList(p_history);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CRMSDProperty::SetTemplateStructureWH(CStructure* p_str)
{
    if( Template == p_str ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,"template structure");
    if( p_history == NULL ) return(false);

    SetTemplateStructure(p_str,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::SetTargetStructureWH(CStructure* p_str)
{
    if( Target == p_str ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,"target structure");
    if( p_history == NULL ) return(false);

    SetTargetStructure(p_str,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

CMapCouple* CRMSDProperty::AddCoupleWH(CAtom* p_at1,CAtom* p_at2)
{
    CMapCouple* p_couple;

    p_couple = AreCoupled(p_at1,p_at2);
    if( p_couple ) return(p_couple);
    if( IsInMap(p_at1) ) return(NULL);
    if( IsInMap(p_at2) ) return(NULL);

    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,"add map couple");
    if( p_history == NULL ) return(NULL);

    p_couple = AddCouple(p_at1,p_at2,p_history);

    EndChangeWH();
    return(p_couple);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::RemoveAllCouplesWH(void)
{
    if( GetNumberOfCouples() == 0 ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,"remove all map couples");
    if( p_history == NULL ) return(false);

    RemoveAllCouples(p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::RemoveHydrogenCouplesWH(void)
{
    if( GetNumberOfHydrogenCouples() == 0 ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,"remove hydrogen map couples");
    if( p_history == NULL ) return(false);

    RemoveHydrogenCouples(p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::AddMappingBySerIndxWH(void)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,"add mapping by serial index");
    if( p_history == NULL ) return(false);

    AddMappingBySerIndx(p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::AddMappingByDistanceWH(double treshold)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,"add mapping by distance");
    if( p_history == NULL ) return(false);

    AddMappingByDistance(treshold,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::AlignTargetWH(bool mass_weighted)
{
    if( IsReady() == false ) return(false);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GEOMETRY,"align target");
    if( p_history == NULL ) return(false);

    AlignTarget(mass_weighted,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::HideAllMappedAtomsWH(void)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"hide all mapped atoms");
    if( p_history == NULL ) return(false);

    HideAllMappedAtoms(p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::HideAllUnmappedAtomsWH(void)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"hide all unmapped atoms");
    if( p_history == NULL ) return(false);

    HideAllUnmappedAtoms(p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::ShowAllAtomsWH(void)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"show all atoms");
    if( p_history == NULL ) return(false);

    ShowAllAtoms(p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::MapTargetPosToTemplateWH(void)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"show all atoms");
    if( p_history == NULL ) return(false);

    MapTargetPosToTemplate(p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::MapTemplatePosToTargetWH(void)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"show all atoms");
    if( p_history == NULL ) return(false);

    MapTemplatePosToTarget(p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::ImprintTemplateTopologyToTargetWH(void)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_TOPOLOGY,"imprint template topology to target");
    if( p_history == NULL ) return(false);

    ImprintTemplateTopologyToTarget(p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::ImprintTargetTopologyToTemplateWH(void)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_TOPOLOGY,"imprint target topology to template");
    if( p_history == NULL ) return(false);

    ImprintTargetTopologyToTemplate(p_history);

    EndChangeWH();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRMSDProperty::SetTemplateStructure(CStructure* p_str,CHistoryNode* p_history)
{
    if( Template == p_str ) return;

    // remove all assigned couples if any
    RemoveAllCouples(p_history);

    // prepare history object
    if( p_history ){
        CHistoryItem* p_hi = new CRMSDPropertyTemplateHI(this,p_str);
        p_history->Register(p_hi);
    }

    // change template
    if( Template != NULL ){
        Template->RemoveFromRegistered(this,NULL);
    }

    Template = p_str;
    if( Template ){
        Template->RegisterObject(this);
    }

    OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CRMSDProperty::SetTargetStructure(CStructure* p_str,CHistoryNode* p_history)
{
    if( Target == p_str ) return;

    // remove all assigned couples if any
    RemoveAllCouples(p_history);

    // prepare history object
    if( p_history ){
        CHistoryItem* p_hi = new CRMSDPropertyTargetHI(this,p_str);
        p_history->Register(p_hi);
    }

    // change template
    if( Target != NULL ){
        Target->RemoveFromRegistered(this,NULL);
    }

    Target = p_str;
    if( Target ){
        Target->RegisterObject(this);
    }

    OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

CMapCouple* CRMSDProperty::AddCouple(CAtom* p_at1,CAtom* p_at2,CHistoryNode* p_history)
{
    if( p_at1 == NULL ){
        INVALID_ARGUMENT("p_at1 is NULL");
    }
    if( p_at2 == NULL ){
        INVALID_ARGUMENT("p_at1 is NULL");
    }
    if( (p_at1->GetStructure() != Template) &&
        (p_at1->GetStructure() != Target) ){
        INVALID_ARGUMENT("p_at1 is not member of template or target");
    }
    if( (p_at2->GetStructure() != Template) &&
        (p_at2->GetStructure() != Target) ){
        INVALID_ARGUMENT("p_at2 is not member of template or target");
    }

    if( IsFlagSet<ERMSDPropertyFlag>(ERMSDPF_ONLY_SAME_Z) ){
        if( p_at1->GetZ() != p_at2->GetZ() ){
            ES_ERROR("atoms do no have the same Z");
            return(NULL);
        }
    }

    if( AreCoupled(p_at1,p_at2) ){
        return(NULL);
    }

    if( IsInMap(p_at1) ) return(NULL);
    if( IsInMap(p_at2) ) return(NULL);

    CMapCouple* p_couple = new CMapCouple(this,p_at1,p_at2);

    if( p_history != NULL ) {
        CHistoryItem* p_hi = new CMapCoupleHI(p_couple,EHID_FORWARD);
        p_history->Register(p_hi);
    }

    EmitOnRMSDPropertyMapChanged();
    return(p_couple);
}

//------------------------------------------------------------------------------

CMapCouple* CRMSDProperty::AddCouple(CXMLElement* p_ele,CHistoryNode* p_history)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    CMapCouple* p_couple = new CMapCouple(this);
    p_couple->LoadData(p_ele);

    if( p_history != NULL ) {
        CHistoryItem* p_hi = new CMapCoupleHI(p_couple,EHID_FORWARD);
        p_history->Register(p_hi);
    }

    EmitOnRMSDPropertyMapChanged();
    return(p_couple);
}

//------------------------------------------------------------------------------

void CRMSDProperty::RemoveAllCouples(CHistoryNode* p_history)
{
    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        p_ac->RemoveFromBaseList(p_history);
    }
}

//------------------------------------------------------------------------------

void CRMSDProperty::RemoveHydrogenCouples(CHistoryNode* p_history)
{
    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        if( p_ac->IsHydrogenCouple() ){
            p_ac->RemoveFromBaseList(p_history);
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRMSDProperty::AddMappingBySerIndx(CHistoryNode* p_history)
{
    if( Template == NULL ){
        ES_ERROR("template is NULL");
        return;
    }

    if( Target == NULL ){
        ES_ERROR("target is NULL");
        return;
    }

    foreach(QObject* p_qobj,Template->GetAtoms()->children()) {
        CAtom* p_tea = static_cast<CAtom*>(p_qobj);
        int seridx = p_tea->GetSerIndex();
        CAtom* p_taa = Target->GetAtoms()->SearchBySerIndex(seridx);

        if( p_taa->GetZ() != p_tea->GetZ() ){
            if( IsFlagSet<ERMSDPropertyFlag>(ERMSDPF_ONLY_SAME_Z) ) continue;
        }
        AddCouple(p_tea,p_taa,p_history);
    }
}

//------------------------------------------------------------------------------

void CRMSDProperty::AddMappingByDistance(double treshold,CHistoryNode* p_history)
{
    if( Template == NULL ){
        ES_ERROR("template is NULL");
        return;
    }

    if( Target == NULL ){
        ES_ERROR("target is NULL");
        return;
    }

    // square
    treshold *= treshold;

    foreach(QObject* p_qobj,Template->GetAtoms()->children()) {
        CAtom* p_tea = static_cast<CAtom*>(p_qobj);

        foreach(QObject* p_qobj,Target->GetAtoms()->children()) {
            CAtom* p_taa = static_cast<CAtom*>(p_qobj);

            if( p_taa->GetZ() != p_tea->GetZ() ){
                if( IsFlagSet<ERMSDPropertyFlag>(ERMSDPF_ONLY_SAME_Z) ) continue;
            }

            CPoint diff = p_taa->GetPos() - p_tea->GetPos();
            if( Square(diff) <= treshold ){
                AddCouple(p_tea,p_taa,p_history);
            }
        }
    }
}

//------------------------------------------------------------------------------

void CRMSDProperty::HideAllUnmappedAtoms(CHistoryNode* p_history)
{
    if( Template != NULL ) {
        foreach(QObject* p_qobj,Template->GetAtoms()->children()) {
            CAtom* p_tea = static_cast<CAtom*>(p_qobj);
            if( ! IsInMap(p_tea) ){
                if( p_tea->IsFlagSet(EPOF_VISIBLE) ){
                    CProObjectFlags flags = p_tea->GetFlags();
                    SET_FLAG(flags,EPOF_VISIBLE,false);
                    p_tea->SetFlags(flags,p_history);
                }
            }
        }
    }

    if( Target != NULL ) {
        foreach(QObject* p_qobj,Target->GetAtoms()->children()) {
            CAtom* p_taa = static_cast<CAtom*>(p_qobj);
            if( ! IsInMap(p_taa) ){
                if( p_taa->IsFlagSet(EPOF_VISIBLE) ){
                    CProObjectFlags flags = p_taa->GetFlags();
                    SET_FLAG(flags,EPOF_VISIBLE,false);
                    p_taa->SetFlags(flags,p_history);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void CRMSDProperty::HideAllMappedAtoms(CHistoryNode* p_history)
{
    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        CAtom* p_at;
        p_at = p_ac->GetTargetAtom();
        if( p_at->IsFlagSet(EPOF_VISIBLE) ){
            CProObjectFlags flags = p_at->GetFlags();
            SET_FLAG(flags,EPOF_VISIBLE,false);
            p_at->SetFlags(flags,p_history);
        }
        p_at = p_ac->GetTemplateAtom();
        if( p_at->IsFlagSet(EPOF_VISIBLE) ){
            CProObjectFlags flags = p_at->GetFlags();
            SET_FLAG(flags,EPOF_VISIBLE,false);
            p_at->SetFlags(flags,p_history);
        }
    }
}

//------------------------------------------------------------------------------

void CRMSDProperty::ShowAllAtoms(CHistoryNode* p_history)
{
    if( Template != NULL ) {
        foreach(QObject* p_qobj,Template->GetAtoms()->children()) {
            CAtom* p_tea = static_cast<CAtom*>(p_qobj);
            if( ! p_tea->IsFlagSet(EPOF_VISIBLE) ){
                CProObjectFlags flags = p_tea->GetFlags();
                SET_FLAG(flags,EPOF_VISIBLE,true);
                p_tea->SetFlags(flags,p_history);
            }
        }
    }

    if( Target != NULL ) {
        foreach(QObject* p_qobj,Target->GetAtoms()->children()) {
            CAtom* p_taa = static_cast<CAtom*>(p_qobj);
            if( ! p_taa->IsFlagSet(EPOF_VISIBLE) ){
                CProObjectFlags flags = p_taa->GetFlags();
                SET_FLAG(flags,EPOF_VISIBLE,true);
                p_taa->SetFlags(flags,p_history);
            }
        }
    }
}

//------------------------------------------------------------------------------

void CRMSDProperty::MapTargetPosToTemplate(CHistoryNode* p_history)
{
    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        CAtom* p_tat = p_ac->GetTargetAtom();
        CAtom* p_tet = p_ac->GetTemplateAtom();
        p_tet->SetPos(p_tat->GetPos(),p_history);
    }
}

//------------------------------------------------------------------------------

void CRMSDProperty::MapTemplatePosToTarget(CHistoryNode* p_history)
{
    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        CAtom* p_tat = p_ac->GetTargetAtom();
        CAtom* p_tet = p_ac->GetTemplateAtom();
        p_tat->SetPos(p_tet->GetPos(),p_history);
    }
}

//------------------------------------------------------------------------------

bool ResidueSortBySeqIndex(const CResidue* d1, const CResidue* d2)
{
  return(d1->GetSeqIndex() < d2->GetSeqIndex());
}

//------------------------------------------------------------------------------

void CRMSDProperty::ImprintTemplateTopologyToTarget(CHistoryNode* p_history)
{
    if( Template == NULL ){
        ES_ERROR("template is NULL");
        return;
    }

    if( Target == NULL ){
        ES_ERROR("target is NULL");
        return;
    }

    // extract template residues that are related to the map
    vector<CResidue*>   ters;

    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        CAtom* p_tet = p_ac->GetTemplateAtom();
        // is residue in the list?
        bool found = false;
        for(size_t i=0; i < ters.size(); i++){
            if( ters[i] == p_tet->GetResidue() ){
                found = true;
                break;
            }
        }
        if( found == false ){
            ters.push_back(p_tet->GetResidue());
        }
    }

    // sort residues by sequence index
    sort(ters.begin(),ters.end(),ResidueSortBySeqIndex);

    Target->GetAtoms()->BeginUpdate();
    Target->GetResidues()->BeginUpdate();

    // imprint new residues
    for(size_t i=0; i < ters.size(); i++){
        int topseqindex = Target->GetResidues()->GetTopSeqIndex();
        int topserindex = Target->GetAtoms()->GetTopSerIndex();
        // create new residue
        CResidue* p_newres = Target->GetResidues()->CreateResidue(ters[i]->GetName(),
                                    ters[i]->GetChain(),ters[i]->GetSeqIndex()+topseqindex,p_history);
        p_newres->SetType(ters[i]->GetType(),p_history);
        p_newres->SetDescription(ters[i]->GetDescription(),p_history);

        // move atoms into the new residue
        foreach(QObject* p_qobj,children()) {
            CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
            CAtom* p_tet = p_ac->GetTemplateAtom();
            if( p_tet->GetResidue() == ters[i] ){
                CAtom* p_tat = p_ac->GetTargetAtom();
                p_newres->AddAtom(p_tat,p_history);
                p_tat->SetName(p_tet->GetName(),p_history);
                p_tat->SetDescription(p_tet->GetDescription(),p_history);
                p_tat->SetZ(p_tet->GetZ(),p_history);
                p_tat->SetType(p_tet->GetType(),p_history);
                p_tat->SetCharge(p_tet->GetCharge(),p_history);
                p_tat->SetLocIndex(p_tet->GetLocIndex(),p_history);
                p_tat->SetSerIndex(p_tet->GetSerIndex()+topserindex,p_history);
            }
        }
    }

    // clean target structure and finalize
    Target->GetResidues()->DeleteEmptyResidues(p_history);
    Target->GetResidues()->EndUpdate();
    Target->GetAtoms()->EndUpdate();
}

//------------------------------------------------------------------------------

void CRMSDProperty::ImprintTargetTopologyToTemplate(CHistoryNode* p_history)
{
    if( Template == NULL ){
        ES_ERROR("template is NULL");
        return;
    }

    if( Target == NULL ){
        ES_ERROR("target is NULL");
        return;
    }

    // extract target residues that are related to the map
    vector<CResidue*>   tars;

    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        CAtom* p_tat = p_ac->GetTargetAtom();
        // is residue in the list?
        bool found = false;
        for(size_t i=0; i < tars.size(); i++){
            if( tars[i] == p_tat->GetResidue() ){
                found = true;
                break;
            }
        }
        if( found == false ){
            tars.push_back(p_tat->GetResidue());
        }
    }

    // sort residues by sequence index
    sort(tars.begin(),tars.end(),ResidueSortBySeqIndex);

    Target->GetAtoms()->BeginUpdate();
    Target->GetResidues()->BeginUpdate();

    // imprint new residues
    for(size_t i=0; i < tars.size(); i++){
        int topseqindex = Template->GetResidues()->GetTopSeqIndex();
        int topserindex = Template->GetAtoms()->GetTopSerIndex();
        // create new residue
        CResidue* p_newres = Template->GetResidues()->CreateResidue(tars[i]->GetName(),
                                    tars[i]->GetChain(),tars[i]->GetSeqIndex()+topseqindex,p_history);
        p_newres->SetType(tars[i]->GetType(),p_history);
        p_newres->SetDescription(tars[i]->GetDescription(),p_history);

        // move atoms into the new residue
        foreach(QObject* p_qobj,children()) {
            CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
            CAtom* p_tat = p_ac->GetTargetAtom();
            if( p_tat->GetResidue() == tars[i] ){
                CAtom* p_tet = p_ac->GetTemplateAtom();
                p_newres->AddAtom(p_tet,p_history);
                p_tet->SetName(p_tat->GetName(),p_history);
                p_tet->SetDescription(p_tat->GetDescription(),p_history);
                p_tet->SetZ(p_tat->GetZ(),p_history);
                p_tet->SetType(p_tat->GetType(),p_history);
                p_tet->SetCharge(p_tat->GetCharge(),p_history);
                p_tet->SetLocIndex(p_tat->GetLocIndex(),p_history);
                p_tet->SetSerIndex(p_tat->GetSerIndex()+topserindex,p_history);
            }
        }
    }

    // clean target structure and finalize
    Target->GetResidues()->DeleteEmptyResidues(p_history);
    Target->GetResidues()->EndUpdate();
    Target->GetAtoms()->EndUpdate();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

ESelResult CRMSDProperty::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
{
    CProObject* p_obj = obj.GetObject();

    // special keys
    if( p_obj == NULL ){
        if( obj.GetSubID() == SEL_ESC_KEY ){
            // reset list
            p_sel->ResetSelection();
            return(ESR_SELECTED_OBJECTS_CHANGED);
        }
        return(ESR_NONE_SELECTED);
    }

    // none object is selected
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // is it already in selection list?
    if( p_sel->IsInList(obj) == true ) {
        p_sel->RemoveObject(obj);
        return(ESR_SELECTED_OBJECTS_CHANGED);
    }

    CAtom* p_atom = dynamic_cast<CAtom*>(p_obj);
    if( p_atom == NULL ){
        return(ESR_NONE_SELECTED);
    }

    // atom must be either from template or target structure
    if( (p_atom->GetStructure() != Template) &&
        (p_atom->GetStructure() != Target) ){
        return(ESR_NONE_SELECTED);
    }

    // first atom
    if( p_sel->NumOfSelectedObjects() == 0 ){
        p_sel->AddObject(obj);
        return(ESR_SELECTED_OBJECTS_CHANGED);
    }

    // second atom
    CAtom* p_atom1 = dynamic_cast<CAtom*>(p_sel->GetSelectedObject(0));

    if( IsFlagSet<ERMSDPropertyFlag>(ERMSDPF_ONLY_SAME_Z) ){
        if( p_atom1->GetZ() != p_atom->GetZ() ){
            return(ESR_NONE_SELECTED);
        }
    }

    if( p_atom1->GetStructure() != p_atom->GetStructure() ){
        p_sel->AddObject(obj);
        return(ESR_SELECTED_OBJECTS_END);
    }

    return(ESR_NONE_SELECTED);
}

//------------------------------------------------------------------------------

const QString CRMSDProperty::GetHandlerDescription(void) const
{
    return(tr("two atoms"));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CStructure* CRMSDProperty::GetTemplateStructure(void) const
{
    return(Template);
}

//------------------------------------------------------------------------------

CStructure* CRMSDProperty::GetTargetStructure(void) const
{
    return(Target);
}

//------------------------------------------------------------------------------

int CRMSDProperty::GetNumberOfCouples(void)
{
    return(children().count());
}

//------------------------------------------------------------------------------

int CRMSDProperty::GetNumberOfHydrogenCouples(void)
{
    int hcouples = 0;
    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        if( p_ac->IsHydrogenCouple() ) hcouples++;
    }
    return(hcouples);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::AreAllCouplesMassConsistent(void)
{
    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        if( p_ac->GetTemplateAtom()->GetZ() != p_ac->GetTargetAtom()->GetZ() ){
            return(false);
        }
    }
    return(true);
}

//------------------------------------------------------------------------------

CMapCouple* CRMSDProperty::AreCoupled(CAtom* p_at1,CAtom* p_at2)
{
    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        if( ((p_ac->GetTemplateAtom() == p_at1) && (p_ac->GetTargetAtom() == p_at2 )) ||
            ((p_ac->GetTemplateAtom() == p_at2) && (p_ac->GetTargetAtom() == p_at1 )) ){
            return(p_ac);
        }
    }
    return(NULL);
}

//------------------------------------------------------------------------------

bool CRMSDProperty::IsInMap(CAtom* p_at1)
{
    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        if( (p_ac->GetTemplateAtom() == p_at1) ) return(true);
        if( (p_ac->GetTemplateAtom() == p_at1) ) return(true);
    }
    return(NULL);
}

//------------------------------------------------------------------------------

CTransformation CRMSDProperty::GetAlignTransform(bool mass_weighted)
{
    CTransformation trans;

    if( Template == NULL ) return(trans);
    if( Target == NULL ) return(trans);

    if( AreAllCouplesMassConsistent() == false ){
        mass_weighted = false;
    }

    // get COMs
    CPoint com1;
    CPoint com2;
    double  totalmass = 0.0;
    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        double mass = 1.0;
        if( mass_weighted ){
            mass = PeriodicTable.GetMass(p_ac->GetTemplateAtom()->GetZ());
        }
        com1 += p_ac->GetTemplateAtom()->GetPos()*mass;
        com2 += p_ac->GetTargetAtom()->GetPos()*mass;
        totalmass += mass;
    }

    if( totalmass > 0 ){
        com1 = com1 / totalmass;
        com2 = com2 / totalmass;
    }

    // get the transformation matrix
    CSimpleSquareMatrix<double,4>   helper;
    double       rx,ry,rz;
    double       fx,fy,fz;
    double       xxyx, xxyy, xxyz;
    double       xyyx, xyyy, xyyz;
    double       xzyx, xzyy, xzyz;
    double       q[4];

// generate the upper triangle of the quadratic form matrix
    xxyx = 0.0;
    xxyy = 0.0;
    xxyz = 0.0;
    xyyx = 0.0;
    xyyy = 0.0;
    xyyz = 0.0;
    xzyx = 0.0;
    xzyy = 0.0;
    xzyz = 0.0;

    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        double mass = 1.0;
        if( mass_weighted ){
            mass = PeriodicTable.GetMass(p_ac->GetTemplateAtom()->GetZ());
        }
        rx = p_ac->GetTargetAtom()->GetPos().x - com2.x;
        ry = p_ac->GetTargetAtom()->GetPos().y - com2.y;
        rz = p_ac->GetTargetAtom()->GetPos().z - com2.z;
        fx = p_ac->GetTemplateAtom()->GetPos().x - com1.x;
        fy = p_ac->GetTemplateAtom()->GetPos().y - com1.y;
        fz = p_ac->GetTemplateAtom()->GetPos().z - com1.z;

        xxyx += fx*rx;
        xxyy += fx*ry;
        xxyz += fx*rz;
        xyyx += fy*rx;
        xyyy += fy*ry;
        xyyz += fy*rz;
        xzyx += fz*rx;
        xzyy += fz*ry;
        xzyz += fz*rz;
    }

    helper.Field[0][0] = xxyx + xyyy + xzyz;

    helper.Field[0][1] = xzyy - xyyz;
    helper.Field[1][1] = xxyx - xyyy - xzyz;

    helper.Field[0][2] = xxyz - xzyx;
    helper.Field[1][2] = xxyy + xyyx;
    helper.Field[2][2] = xyyy - xzyz - xxyx;

    helper.Field[0][3] = xyyx - xxyy;
    helper.Field[1][3] = xzyx + xxyz;
    helper.Field[2][3] = xyyz + xzyy;
    helper.Field[3][3] = xzyz - xxyx - xyyy;

// complete matrix
    helper.Field[1][0] = helper.Field[0][1];

    helper.Field[2][0] = helper.Field[0][2];
    helper.Field[2][1] = helper.Field[1][2];

    helper.Field[3][0] = helper.Field[0][3];
    helper.Field[3][1] = helper.Field[1][3];
    helper.Field[3][2] = helper.Field[2][3];

// diagonalize helper matrix
    helper.EigenProblem(q);

// extract the desired quaternion
    q[0] = helper.Field[0][3];
    q[1] = helper.Field[1][3];
    q[2] = helper.Field[2][3];
    q[3] = helper.Field[3][3];

// generate the rotation matrix
    helper.SetUnit();

    helper.Field[0][0] = q[0]*q[0] + q[1]*q[1] - q[2]*q[2] - q[3]*q[3];
    helper.Field[1][0] = 2.0 * (q[1] * q[2] - q[0] * q[3]);
    helper.Field[2][0] = 2.0 * (q[1] * q[3] + q[0] * q[2]);

    helper.Field[0][1] = 2.0 * (q[2] * q[1] + q[0] * q[3]);
    helper.Field[1][1] = q[0]*q[0] - q[1]*q[1] + q[2]*q[2] - q[3]*q[3];
    helper.Field[2][1] = 2.0 * (q[2] * q[3] - q[0] * q[1]);

    helper.Field[0][2] = 2.0 * (q[3] * q[1] - q[0] * q[2]);
    helper.Field[1][2] = 2.0 * (q[3] * q[2] + q[0] * q[1]);
    helper.Field[2][2] = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];

    trans.Translate(-com2);
    trans.MultFromRightWith(helper);
    trans.Translate(com1);

    return(trans);
}

//------------------------------------------------------------------------------

double CRMSDProperty::GetRMSD(bool mass_weighted)
{
    if( AreAllCouplesMassConsistent() == false ){
        mass_weighted = false;
    }

    double  rmsd = 0.0;
    double  totalmass = 0.0;

    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        CPoint diff = p_ac->GetTargetAtom()->GetPos() - p_ac->GetTemplateAtom()->GetPos();
        double mass = 1.0;
        if( mass_weighted ){
            mass = PeriodicTable.GetMass(p_ac->GetTemplateAtom()->GetZ());
        }
        rmsd += mass*Square(diff);
        totalmass += mass;
    }

    if( totalmass > 0 ){
        rmsd = sqrt( rmsd / totalmass );
    }

    return( rmsd );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CRMSDProperty::IsReady(void)
{
    return( GetNumberOfCouples() >= 3 );
}

//------------------------------------------------------------------------------

double CRMSDProperty::GetScalarValue(void)
{
    return( GetRMSD(IsFlagSet<ERMSDPropertyFlag>(ERMSDPF_MASS_WEIGHTED)) );
}

//------------------------------------------------------------------------------

void CRMSDProperty::AlignTarget(bool mass_weighted,CHistoryNode* p_history)
{
    if( Target == NULL ) return;

    CTransformation trans = GetAlignTransform(mass_weighted);

    foreach(QObject* p_qobj,Target->GetAtoms()->children()) {
        CAtom* p_at = static_cast<CAtom*>(p_qobj);
        p_at->SetPos(trans.GetTransform(p_at->GetPos()));
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRMSDProperty::RemoveFromRegistered(CProObject* p_object,CHistoryNode* p_history)
{
    if( p_object == Template ){
        SetTemplateStructure(NULL,p_history);
        return;
    }
    if( p_object == Target ){
        SetTargetStructure(NULL,p_history);
        return;
    }
}

//------------------------------------------------------------------------------

void CRMSDProperty::EmitOnRMSDPropertyMapChanged(void)
{
    emit OnStatusChanged(ESC_OTHER);
    emit OnRMSDPropertyMapChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRMSDProperty::LoadData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "property" ) {
        LOGIC_ERROR("p_ele is not property");
    }

    // load object info -----------------------------
    CGraphicsProperty::LoadData(p_ele);

    // load data ------------------------------------

    // local data
    int tei = -1;
    int tai = -1;

    p_ele->GetAttribute("tei",tei);
    CStructure* p_tei = static_cast<CStructure*>(GetProject()->FindObject(tei));
    SetTemplateStructure(p_tei);

    p_ele->GetAttribute("tai",tai);
    CStructure* p_tai = static_cast<CStructure*>(GetProject()->FindObject(tai));
    SetTargetStructure(p_tai);

    // couples
    CXMLElement* p_ael;
    p_ael = p_ele->GetFirstChildElement("couple");
    while( p_ael != NULL ) {
        AddCouple(p_ael);
        p_ael = p_ael->GetNextSiblingElement("couple");
    }
}

//------------------------------------------------------------------------------

void CRMSDProperty::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "property" ) {
        LOGIC_ERROR("p_ele is not property");
    }

    // save object info -----------------------------
    CGraphicsProperty::SaveData(p_ele);

    // save data ------------------------------------

    // local data
    if( Template ){
        p_ele->SetAttribute("tei",Template->GetIndex());
    }
    if( Target ){
        p_ele->SetAttribute("tai",Target->GetIndex());
    }

    // couples
    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_map = static_cast<CMapCouple*>(p_qobj);
        CXMLElement* p_bel = p_ele->CreateChildElement("couple");
        p_map->SaveData(p_bel);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRMSDProperty::Draw(void)
{
    if( IsReady() == false ) return;

    Setup = GetSetup<CRMSDPropertySetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);

    GLLoadObject(this);

    glColor4fv(Setup->LineColor.ForGL());
    glLineWidth(Setup->LineWidth);
    if( Setup->LineStippleFactor > 0 ){
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(Setup->LineStippleFactor,Setup->LineStipplePattern);
    } else {
        glDisable(GL_LINE_STIPPLE);
    }

    glBegin(GL_LINES);
    foreach(QObject* p_qobj,children()) {
        CMapCouple* p_ac = static_cast<CMapCouple*>(p_qobj);
        if( p_ac->GetTemplateAtom()->IsFlagSet(EPOF_VISIBLE) == false ) continue;
        if( p_ac->GetTargetAtom()->IsFlagSet(EPOF_VISIBLE) == false ) continue;
        CPoint p1 = p_ac->GetTemplateAtom()->GetPos();
        CPoint p2 = p_ac->GetTargetAtom()->GetPos();
        glVertex3dv(p1);
        glVertex3dv(p2);
    }
    glEnd();

    glDisable(GL_LINE_STIPPLE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



