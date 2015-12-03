// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <PluginObject.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <PeriodicTable.hpp>
#include <GraphicsObjectList.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <Project.hpp>
#include <Structure.hpp>
#include <BondList.hpp>
#include <Bond.hpp>
#include <Atom.hpp>
#include <Residue.hpp>
#include <ElementColorsList.hpp>
#include <GraphicsUtil.hpp>
#include <GLSelection.hpp>
#include <GOColorMode.hpp>
#include <StructureList.hpp>
#include <FastModelObjectHistory.hpp>
#include <AtomList.hpp>

#include <NemesisCoreModule.hpp>
#include <FastModelObject.hpp>
#include <FastModelSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* FastModelObjectCB(void* p_data);

CExtUUID        FastModelObjectID(
                    "{FAST_MODEL_OBJECT:172377a6-4faf-4803-8245-eb07eb6a459f}",
                    "Fast Model");

CPluginObject   FastModelObject(&NemesisCorePlugin,
                    FastModelObjectID,
                    GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/objects/FastModel.svg",
                    FastModelObjectCB);

// -----------------------------------------------------------------------------

QObject* FastModelObjectCB(void* p_data)
{
    return(new CFastModelObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFastModelObject::CFastModelObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&FastModelObject,p_gl)
{
    SetFlag<EFastModelObjectFlag>(EFMOF_SHOW_HYDROGENS,true);

    RegisterAllowedObjectType(StructureListID);
    RegisterAllowedObjectType(StructureID);
    RegisterAllowedObjectType(ResidueID);
    RegisterAllowedObjectType(BondID);
    RegisterAllowedObjectType(AtomID);

    ColorMode = new CGOColorMode(this,"Model color",true);
    // color mode is child object so we need
    // to prevent ObjectInspector to expand this node
    ColorMode->ConFlags |= EECOF_HIDDEN;

    KA = 0;
    KB = 0;
    KC = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CFastModelObject::SetKOffsetsWH(int ka,int kb,int kc)
{
    if( (ka == KA) && (kb == KB) && (kc == KC) ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("change k-position"));
    if( p_history == NULL ) return (false);

    SetKOffsets(ka,kb,kc,p_history);

    EndChangeWH();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFastModelObject::Draw(void)
{
    CFastModelSetup* p_setup = GetSetup<CFastModelSetup>();
    if( p_setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LINE_STIPPLE);

    glLineWidth(p_setup->LineWidth);

    foreach(QObject* p_qobj,Objects) {
        CProObject* p_object = static_cast<CProObject*>(p_qobj);
        CStructureList* p_strlist;
        CStructure*     p_str;
        CBond*          p_bond;
        CResidue*       p_res;
        CAtom*          p_atom;
        koffset.SetZero();
        if( (p_strlist = dynamic_cast<CStructureList*>(p_object)) != NULL ){
            FastDrawStructureList(p_strlist);
        }
        if( (p_str = dynamic_cast<CStructure*>(p_object)) != NULL ){
            SetPBCOffset(p_str);
            FastDrawStructure(p_str);
        }
        if( (p_res = dynamic_cast<CResidue*>(p_object)) != NULL ){
            SetPBCOffset(p_res->GetStructure());
            FastDrawResidue(p_res);
        }
        if( (p_bond = dynamic_cast<CBond*>(p_object)) != NULL ){
            SetPBCOffset(p_bond->GetStructure());
            FastDrawBond(p_bond);
        }
        if( (p_atom = dynamic_cast<CAtom*>(p_object)) != NULL ){
            SetPBCOffset(p_atom->GetStructure());
            FastDrawAtom(p_atom);
        }
    }
}

//------------------------------------------------------------------------------

void CFastModelObject::SetKOffsets(int ka,int kb,int kc,CHistoryNode* p_history)
{
    if( (KA == ka) && (KB == kb) && (KC == kc) ) return;

    if( p_history ){
        CHistoryItem* p_item = new CFastModelChangeKPosHI(this,ka,kb,kc);
        p_history->Register(p_item);
    }

    KA = ka;
    KB = kb;
    KC = kc;

    emit OnStatusChanged(ESC_OTHER);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFastModelObject::GetObjectMetrics(CObjMetrics& metrics)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    foreach(QObject* p_qobj,Objects) {
        CProObject* p_object = static_cast<CProObject*>(p_qobj);
        CStructureList* p_strlist;
        CStructure*  p_str;
        CResidue*   p_res;
        CBond*      p_bond;
        CAtom*      p_atom;

        koffset.SetZero();
        if( (p_strlist = dynamic_cast<CStructureList*>(p_object)) != NULL ){
            GetStructureListMetrics(p_strlist,metrics);
        }
        if( (p_str = dynamic_cast<CStructure*>(p_object)) != NULL ){
            SetPBCOffset(p_str);
            GetStructureMetrics(p_str,metrics);
        }
        if( (p_res = dynamic_cast<CResidue*>(p_object)) != NULL ){
            SetPBCOffset(p_res->GetStructure());
            GetResidueMetrics(p_res,metrics);
        }
        if( (p_bond = dynamic_cast<CBond*>(p_object)) != NULL ){
            SetPBCOffset(p_bond->GetStructure());
            GetBondMetrics(p_bond,metrics);
        }
        if( (p_atom = dynamic_cast<CAtom*>(p_object)) != NULL ){
            SetPBCOffset(p_atom->GetStructure());
            GetAtomMetrics(p_atom,metrics);
        }
    }
}

//------------------------------------------------------------------------------

void CFastModelObject::GetStructureListMetrics(CStructureList* p_sl,CObjMetrics& metrics)
{
    if( (IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HIDDEN) == false)
        && ( p_sl->IsFlagSet(EPOF_VISIBLE) == false) )  return;

    foreach(QObject* p_qobj,p_sl->children()) {
        CStructure* p_str = static_cast<CStructure*>(p_qobj);
        koffset.SetZero();
        SetPBCOffset(p_str);
        GetStructureMetrics(p_str,metrics);
    }
}

//------------------------------------------------------------------------------

void CFastModelObject::GetStructureMetrics(CStructure* p_str,CObjMetrics& metrics)
{
    if( IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HIDDEN) == false ){
        if( p_str->IsFlagSet(EPOF_VISIBLE) == false ) return;
    }

    if( IsFlagSet<EFastModelObjectFlag>(EFMOF_ONLY_ATOMS) == false ){
        foreach(QObject* p_qobj,p_str->GetBonds()->children()) {
            CBond* p_bond = static_cast<CBond*>(p_qobj);
            GetBondMetrics(p_bond,metrics);
        }
    } else {
        foreach(QObject* p_qobj,p_str->GetAtoms()->children()) {
            CAtom* p_atom = static_cast<CAtom*>(p_qobj);
            GetAtomMetrics(p_atom,metrics);
        }
    }
}

//------------------------------------------------------------------------------

void CFastModelObject::GetResidueMetrics(CResidue* p_res,CObjMetrics& metrics)
{
    if( IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HIDDEN) == false ){
        if( p_res->IsFlagSet(EPOF_VISIBLE) == false ) return;
        if( p_res->GetStructure() && (p_res->GetStructure()->IsFlagSet(EPOF_VISIBLE) == false) ) return;
    }

    if( IsFlagSet<EFastModelObjectFlag>(EFMOF_ONLY_ATOMS) == false ){
        foreach(CBond* p_bond,p_res->GetBonds(true)) {
            GetBondMetrics(p_bond,metrics);
        }
    } else {
        foreach(CAtom* p_atom,p_res->GetAtoms()) {
            GetAtomMetrics(p_atom,metrics);
        }
    }
}

//------------------------------------------------------------------------------

void CFastModelObject::GetAtomMetrics(CAtom* p_atom,CObjMetrics& metrics)
{
    if( p_atom == NULL ) return;

    if( IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HIDDEN) == false ){
        if( p_atom->IsFlagSet(EPOF_VISIBLE) == false ) return;
        if( p_atom->GetResidue() && (p_atom->GetResidue()->IsFlagSet(EPOF_VISIBLE) == false) ) return;
        if( p_atom->GetStructure() && (p_atom->GetStructure()->IsFlagSet(EPOF_VISIBLE) == false) ) return;
    }

    int Z = p_atom->GetZ();

    if( PeriodicTable.IsVirtual(Z) &&
        (IsFlagSet((EProObjectFlag)EFMOF_SHOW_HYDROGENS) == false) ) return;

    metrics.CompareWith(p_atom->GetPos()+koffset);
}

//------------------------------------------------------------------------------

void CFastModelObject::GetBondMetrics(CBond* p_bond,CObjMetrics& metrics)
{
    if( p_bond->IsInvalidBond() ) return;

    CAtom* p_atom1 = p_bond->GetFirstAtom();
    CAtom* p_atom2 = p_bond->GetSecondAtom();

    if( IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HIDDEN) == false ){
        if( p_bond->IsFlagSet(EPOF_VISIBLE) == false ) return;
        if( p_bond->GetStructure() && (p_bond->GetStructure()->IsFlagSet(EPOF_VISIBLE) == false) ) return;
        if( p_atom1->GetResidue() && (p_atom1->GetResidue()->IsFlagSet(EPOF_VISIBLE) == false) ) return;
        if( p_atom2->GetResidue() && (p_atom2->GetResidue()->IsFlagSet(EPOF_VISIBLE) == false) ) return;
    }

    int Z1 = p_atom1->GetZ();
    int Z2 = p_atom2->GetZ();

    if( (PeriodicTable.IsVirtual(Z1) || PeriodicTable.IsVirtual(Z2)) &&
        (IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HYDROGENS) == false) ) return;

    metrics.CompareWith(p_atom1->GetPos()+koffset);
    metrics.CompareWith(p_atom2->GetPos()+koffset);
}

//------------------------------------------------------------------------------

void CFastModelObject::SetPBCOffset(CStructure* p_str)
{
    if( p_str == NULL ) return;

    if( p_str->PBCInfo.IsPeriodicAlongA() ) {
        koffset += p_str->PBCInfo.GetAVector()*KA;
    }
    if( p_str->PBCInfo.IsPeriodicAlongB() ) {
        koffset += p_str->PBCInfo.GetBVector()*KB;
    }
    if( p_str->PBCInfo.IsPeriodicAlongC() ) {
        koffset += p_str->PBCInfo.GetCVector()*KC;
    }
}

//------------------------------------------------------------------------------

CGOColorMode* CFastModelObject::GetColorMode(void)
{
    return(ColorMode);
}

//------------------------------------------------------------------------------

int CFastModelObject::GetKAOffset(void)
{
    return(KA);
}

//------------------------------------------------------------------------------

int CFastModelObject::GetKBOffset(void)
{
    return(KB);
}

//------------------------------------------------------------------------------

int CFastModelObject::GetKCOffset(void)
{
    return(KC);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFastModelObject::FastDrawStructureList(CStructureList* p_strlist)
{
    if( (IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HIDDEN) == false)
        && ( p_strlist->IsFlagSet(EPOF_VISIBLE) == false) )  return;

    foreach(QObject* p_qobj,p_strlist->children()) {
        CStructure* p_str = static_cast<CStructure*>(p_qobj);
        koffset.SetZero();
        SetPBCOffset(p_str);
        FastDrawStructure(p_str);
    }
}

//------------------------------------------------------------------------------

void CFastModelObject::FastDrawStructure(CStructure* p_str)
{
    if( IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HIDDEN) == false ){
        if( p_str->IsFlagSet(EPOF_VISIBLE) == false ) return;
    }

    if( IsFlagSet<EFastModelObjectFlag>(EFMOF_ONLY_ATOMS) == false ){
        foreach(QObject* p_qobj,p_str->GetBonds()->children()) {
            CBond* p_bond = static_cast<CBond*>(p_qobj);
            FastDrawBond(p_bond);
        }
    } else {
        foreach(QObject* p_qobj,p_str->GetAtoms()->children()) {
            CAtom* p_atom = static_cast<CAtom*>(p_qobj);
            FastDrawAtom(p_atom);
        }
    }
}

//------------------------------------------------------------------------------

void CFastModelObject::FastDrawResidue(CResidue* p_res)
{
    if( IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HIDDEN) == false ){
        if( p_res->IsFlagSet(EPOF_VISIBLE) == false ) return;
        if( p_res->GetStructure() && (p_res->GetStructure()->IsFlagSet(EPOF_VISIBLE) == false) ) return;
    }

    if( IsFlagSet<EFastModelObjectFlag>(EFMOF_ONLY_ATOMS) == false ){
        foreach(CBond* p_bond,p_res->GetBonds(true)) {
            FastDrawBond(p_bond);
        }
    } else {
        foreach(CAtom* p_atom,p_res->GetAtoms()) {
            FastDrawAtom(p_atom);
        }
    }
}

//------------------------------------------------------------------------------

void CFastModelObject::FastDrawAtom(CAtom* p_atom)
{
    if( p_atom == NULL ) return;

    if( IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HIDDEN) == false ){
        if( p_atom->IsFlagSet(EPOF_VISIBLE) == false ) return;
        if( p_atom->GetResidue() && (p_atom->GetResidue()->IsFlagSet(EPOF_VISIBLE) == false) ) return;
        if( p_atom->GetStructure() && (p_atom->GetStructure()->IsFlagSet(EPOF_VISIBLE) == false) ) return;
    }

    int Z = p_atom->GetZ();

    if( PeriodicTable.IsVirtual(Z) &&
        (IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HYDROGENS) == false) ) return;

    CElementColors* p_color;

    bool selected = p_atom->IsFlagSet(EPOF_SELECTED);
    if( p_atom->GetStructure() != NULL ) {
        selected |= p_atom->GetStructure()->IsFlagSet(EPOF_SELECTED);
    }
    if( p_atom->GetResidue() != NULL ) {
        selected |= p_atom->GetResidue()->IsFlagSet(EPOF_SELECTED);
    }

    if( selected ){
        p_color = &ColorsList.SelectionMaterial;
    } else {
        p_color = ColorMode->GetElementColor(p_atom);
    }

    CSimplePoint<float> pos1 = p_atom->GetPos() + koffset;

    GLLoadObject(p_atom);
    glBegin(GL_POINTS);
        glColor4fv(p_color->Diffuse);
        glVertex3fv(pos1);
    glEnd();
}

//------------------------------------------------------------------------------

void CFastModelObject::FastDrawBond(CBond* p_bond)
{   
    if( p_bond->IsInvalidBond() ) return;

    CAtom* p_atom1 = p_bond->GetFirstAtom();
    CAtom* p_atom2 = p_bond->GetSecondAtom();

    if( IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HIDDEN) == false ){
        if( p_bond->IsFlagSet(EPOF_VISIBLE) == false ) return;
        if( p_bond->GetStructure() && (p_bond->GetStructure()->IsFlagSet(EPOF_VISIBLE) == false) ) return;
        if( p_atom1->GetResidue() && (p_atom1->GetResidue()->IsFlagSet(EPOF_VISIBLE) == false) ) return;
        if( p_atom2->GetResidue() && (p_atom2->GetResidue()->IsFlagSet(EPOF_VISIBLE) == false) ) return;
    }

    int Z1 = p_atom1->GetZ();
    int Z2 = p_atom2->GetZ();

    if( (PeriodicTable.IsVirtual(Z1) || PeriodicTable.IsVirtual(Z2)) &&
        (IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HYDROGENS) == false) ){
        return;
    }

    CElementColors* p_color1;
    CElementColors* p_color2;

    bool selected = p_bond->IsFlagSet(EPOF_SELECTED);
    if( p_bond->GetStructure() != NULL ) {
        selected |= p_bond->GetStructure()->IsFlagSet(EPOF_SELECTED);
    }
    selected |= p_bond->IsSelectedByResidues();

    if( p_atom1->IsFlagSet(EPOF_SELECTED) || selected ){
        p_color1 = &ColorsList.SelectionMaterial;
    } else {
        p_color1 = ColorMode->GetElementColor(p_atom1);
    }

    if( p_atom2->IsFlagSet(EPOF_SELECTED) || selected ){
        p_color2 = &ColorsList.SelectionMaterial;
    } else {
        p_color2 = ColorMode->GetElementColor(p_atom2);
    }

    if( IsFlagSet<EFastModelObjectFlag>(EFMOF_PBC_BONDS) ){
        CSimplePoint<float> pos1 = p_atom1->GetPos() + koffset;
        CSimplePoint<float> pos2 = p_atom2->GetPos() + koffset;

        double length = Size(pos1-pos2);
        if( length > p_bond->GetStructure()->PBCInfo.GetLargestSphereRadius() ){
            CSimplePoint<float> diff = pos1-pos2;
            CSimplePoint<float> pos1_backup = pos1;
            CSimplePoint<float> posM;
            diff = p_bond->GetStructure()->PBCInfo.ImageVector(diff);

            pos1 = pos2 + diff;
            posM = (pos1 + pos2)*0.5;
            DrawCoreBond(p_atom1,p_atom2,p_color1,p_color2,pos1,posM,pos2);

            pos1 = pos1_backup;
            pos2 = pos1 - diff;
            posM = (pos1 + pos2)*0.5;
            DrawCoreBond(p_atom1,p_atom2,p_color1,p_color2,pos1,posM,pos2);

        } else {
            CSimplePoint<float> posM = (pos1 + pos2)*0.5;
            DrawCoreBond(p_atom1,p_atom2,p_color1,p_color2,pos1,posM,pos2);
        }

    } else {
        CSimplePoint<float> pos1 = p_atom1->GetPos() + koffset;
        CSimplePoint<float> pos2 = p_atom2->GetPos() + koffset;
        CSimplePoint<float> posM = (pos1 + pos2)*0.5;
        DrawCoreBond(p_atom1,p_atom2,p_color1,p_color2,pos1,posM,pos2);
    }
}

//------------------------------------------------------------------------------

void CFastModelObject::DrawCoreBond(CAtom* p_atom1,CAtom* p_atom2,
                                    CElementColors* p_color1,CElementColors* p_color2,
                                    CSimplePoint<float>& pos1,
                                    CSimplePoint<float>& posM,
                                    CSimplePoint<float>& pos2)
{
    GLLoadObject(p_atom1);
    glBegin(GL_LINES);
        glColor4fv(p_color1->Diffuse);
        glVertex3fv(pos1);
        glColor4fv(p_color1->Diffuse);
        glVertex3fv(posM);
    glEnd();

    GLLoadObject(p_atom2);
    glBegin(GL_LINES);
        glColor4fv(p_color2->Diffuse);
        glVertex3fv(posM);
        glColor4fv(p_color2->Diffuse);
        glVertex3fv(pos2);
    glEnd();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFastModelObject::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load object info -----------------------------
    CGraphicsObject::LoadData(p_ele);

    // specific data -----------------------------
    p_ele->GetAttribute("ka",KA);
    p_ele->GetAttribute("kb",KB);
    p_ele->GetAttribute("kc",KC);

    CXMLElement* p_sele = p_ele->GetFirstChildElement("cmode");
    if( p_sele ) {
        ColorMode->LoadData(p_sele);
    }
}

//------------------------------------------------------------------------------

void CFastModelObject::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // core data ---------------------------------
    CGraphicsObject::SaveData(p_ele);

    // specific data -----------------------------
    p_ele->SetAttribute("ka",KA);
    p_ele->SetAttribute("kb",KB);
    p_ele->SetAttribute("kc",KC);

    CXMLElement* p_sele = p_ele->CreateChildElement("cmode");
    ColorMode->SaveData(p_sele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


