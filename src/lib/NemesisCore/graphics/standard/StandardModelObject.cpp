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

#include <NemesisCoreModule.hpp>
#include <PluginObject.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <GraphicsObjectList.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <Atom.hpp>
#include <Bond.hpp>
#include <ElementColorsList.hpp>
#include <GLSelection.hpp>
#include <AtomList.hpp>
#include <BondList.hpp>
#include <Residue.hpp>
#include <StructureList.hpp>
#include <GOColorMode.hpp>

#include <StandardModelObject.hpp>
#include <StandardModelObjectHistory.hpp>
#include <StandardModelSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* StandardModelObjectCB(void* p_data);

CExtUUID        StandardModelObjectID(
                    "{STANDARD_MODEL_OBJECT:b8ec4377-b886-44c3-a206-4d40eaf7d5f2}",
                    "Standard Model");

CPluginObject   StandardModelObjectObject(&NemesisCorePlugin,
                    StandardModelObjectID,GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/objects/StandardModel.svg",
                    StandardModelObjectCB);

// -----------------------------------------------------------------------------

QObject* StandardModelObjectCB(void* p_data)
{
    return(new CStandardModelObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CStandardModelObject::CStandardModelObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&StandardModelObjectObject,p_gl)
{
    RegisterAllowedObjectType(StructureListID);
    RegisterAllowedObjectType(StructureID);
    RegisterAllowedObjectType(ResidueID);
    RegisterAllowedObjectType(BondID);
    RegisterAllowedObjectType(AtomID);

    //ExtQuad = gluNewQuadric();
    Setup = NULL;
    ColorMode = new CGOColorMode(this,"Model color",true);
    // color mode is child object so we need
    // to prevent ObjectInspector to expand this node
    ColorMode->ConFlags |= EECOF_HIDDEN;

    Model = MODEL_NO_MODEL;

    KA = 0;
    KB = 0;
    KC = 0;

    AtomsSet = NULL;
    BondsSet = NULL;
    ModelSet = NULL;

    SetModel(MODEL_TUBES_AND_BALLS);

    SetFlag<EStandardModelObjectFlag>(ESMOF_SHOW_HYDROGENS,true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CStandardModelObject::SetModelWH(EModel model)
{   
    if( Model == model ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("change model"));
    if( p_history == NULL ) return (false);

    SetModel(model,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CStandardModelObject::SetKOffsetsWH(int ka,int kb,int kc)
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

void CStandardModelObject::Draw(void)
{
    Setup = GetSetup<CStandardModelSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    // draw individual objects
//    glShadeModel(GL_SMOOTH);

//    glEnable(GL_LIGHTING);
    SetInitialColorScheme();

    foreach(QObject* p_qobj,Objects) {
        CProObject* p_object = static_cast<CProObject*>(p_qobj);
        CStructureList* p_strlist;
        CStructure*     p_str;
        CResidue*       p_res;
        CAtom*          p_atom;
        CBond*          p_bond;

        koffset.SetZero();
        if( (p_strlist = dynamic_cast<CStructureList*>(p_object)) != NULL ){
            DrawStructureList(p_strlist);
        }
        if( (p_str = dynamic_cast<CStructure*>(p_object)) != NULL ){
            SetPBCOffset(p_str);
            DrawStructure(p_str);
        }
        if( (p_res = dynamic_cast<CResidue*>(p_object)) != NULL ){
            SetPBCOffset(p_res->GetStructure());
            DrawResidue(p_res);
        }
        if( (p_atom = dynamic_cast<CAtom*>(p_object)) != NULL ){
            SetPBCOffset(p_atom->GetStructure());
            DrawAtom(p_atom);
        }
        if( (p_bond = dynamic_cast<CBond*>(p_object)) != NULL ){
            SetPBCOffset(p_bond->GetStructure());
            DrawBond(p_bond);
        }
    }
}

//------------------------------------------------------------------------------

void CStandardModelObject::SetModel(EModel model,CHistoryNode* p_history)
{
    if( Model == model ) return;

    if( p_history != NULL ){
        CHistoryItem* p_item = new CStandardModelChangeModelHI(this,model);
        p_history->Register(p_item);
    }

    Model = model;

    emit OnStatusChanged(ESC_OTHER);

    UpdateSetup();
}

//------------------------------------------------------------------------------

void CStandardModelObject::SetKOffsets(int ka,int kb,int kc,CHistoryNode* p_history)
{
    if( (KA == ka) && (KB == kb) && (KC == kc) ) return;

    if( p_history ){
        CHistoryItem* p_item = new CStandardModelChangeKPosHI(this,ka,kb,kc);
        p_history->Register(p_item);
    }

    KA = ka;
    KB = kb;
    KC = kc;

    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CStandardModelObject::SetupChanged(void)
{
    UpdateSetup();
    CGraphicsObject::SetupChanged();
}

//------------------------------------------------------------------------------

void CStandardModelObject::UpdateSetup(void)
{
    Setup = GetSetup<CStandardModelSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    AtomsSet = &Setup->Models[Model].Atoms;
    BondsSet = &Setup->Models[Model].Bonds;

    if( AtomsSet->Type == 2 ) {
        Sphere.SetTessellationQuality(AtomsSet->TessellationQuality);
    }
    if( BondsSet->Type == 2 ) {
        Cylinder.SetTessellationQuality(BondsSet->TessellationQuality);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStandardModelObject::GetObjectMetrics(CObjMetrics& metrics)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    foreach(QObject* p_qobj,Objects) {
        CProObject* p_object = static_cast<CProObject*>(p_qobj);

        CStructureList* p_strlist;
        CStructure*     p_str;
        CResidue*       p_res;
        CAtom*          p_atom;
        CBond*          p_bond;

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
        if( (p_atom = dynamic_cast<CAtom*>(p_object)) != NULL ){
            SetPBCOffset(p_atom->GetStructure());
            GetAtomMetrics(p_atom,metrics);
        }
        if( (p_bond = dynamic_cast<CBond*>(p_object)) != NULL ){
            SetPBCOffset(p_bond->GetStructure());
            GetBondMetrics(p_bond,metrics);
        }
    }
}

//------------------------------------------------------------------------------

void CStandardModelObject::GetStructureListMetrics(CStructureList* p_sl,CObjMetrics& metrics)
{
    if( (IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HIDDEN) == false)
        && ( p_sl->IsFlagSet(EPOF_VISIBLE) == false) )  return;

    foreach(QObject* p_qobj,p_sl->children()) {
        CStructure* p_str = static_cast<CStructure*>(p_qobj);
        koffset.SetZero();
        SetPBCOffset(p_str);
        GetStructureMetrics(p_str,metrics);
    }
}

//------------------------------------------------------------------------------

void CStandardModelObject::GetStructureMetrics(CStructure* p_str,CObjMetrics& metrics)
{
    if( (IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HIDDEN) == false)
        && ( p_str->IsFlagSet(EPOF_VISIBLE) == false) )  return;

    foreach(QObject* p_qobj,p_str->GetAtoms()->children()) {
        CAtom* p_a = static_cast<CAtom*>(p_qobj);
        GetAtomMetrics(p_a,metrics);
    }

    foreach(QObject* p_qobj,p_str->GetBonds()->children()) {
        CBond* p_b = static_cast<CBond*>(p_qobj);
        GetBondMetrics(p_b,metrics);
    }
}

//------------------------------------------------------------------------------

void CStandardModelObject::GetResidueMetrics(CResidue* p_res,CObjMetrics& metrics)
{
    if( (IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HIDDEN) == false)
        && ( p_res->IsFlagSet(EPOF_VISIBLE) == false) )  return;

    foreach(CAtom* p_atm,p_res->GetAtoms()) {
        GetAtomMetrics(p_atm,metrics);
    }
}

//------------------------------------------------------------------------------

void CStandardModelObject::GetAtomMetrics(CAtom* p_atom,CObjMetrics& metrics)
{
    if( (IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HIDDEN) == false) &&
        (p_atom->IsFlagSet(EPOF_VISIBLE) == false) )  return;

    if( p_atom->GetResidue() ){
        if( (IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HIDDEN) == false) &&
            (p_atom->GetResidue()->IsFlagSet(EPOF_VISIBLE) == false) )  return;
    }

    int Z = p_atom->GetZ();

    if( PeriodicTable.IsVirtual(Z) ){
        if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HYDROGENS) == false ) return;
    }

    metrics.CompareWith(p_atom->GetPos()+koffset);
}

//------------------------------------------------------------------------------

void CStandardModelObject::GetBondMetrics(CBond* p_bond,CObjMetrics& metrics)
{
    if( BondsSet->Radius == 0 ) return;

    if( p_bond->IsInvalidBond() ) return;

    CAtom* p_atom1 = p_bond->GetFirstAtom();
    CAtom* p_atom2 = p_bond->GetSecondAtom();

    if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HIDDEN) == false ){
        if( p_bond->IsFlagSet(EPOF_VISIBLE) == false ) return;
        if( p_atom1->GetResidue() && (p_atom1->GetResidue()->IsFlagSet(EPOF_VISIBLE) == false)
            && p_atom2->GetResidue() && (p_atom2->GetResidue()->IsFlagSet(EPOF_VISIBLE) == false) ) return;
    }

    if( (p_atom1->IsFlagSet(EPOF_VISIBLE) == false) && (p_atom2->IsFlagSet(EPOF_VISIBLE) == false) ) return;

    int Z1,Z2;
    Z1 = p_atom1->GetZ();
    Z2 = p_atom2->GetZ();

    if( PeriodicTable.IsVirtual(Z1) || PeriodicTable.IsVirtual(Z2) ){
        if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HYDROGENS) == false ) return;
    }

    metrics.CompareWith(p_atom1->GetPos()+koffset);
    metrics.CompareWith(p_atom2->GetPos()+koffset);
}

//------------------------------------------------------------------------------

EModel CStandardModelObject::GetModel(void)
{
    return(Model);
}

//------------------------------------------------------------------------------

int CStandardModelObject::GetKAOffset(void)
{
    return(KA);
}

//------------------------------------------------------------------------------

int CStandardModelObject::GetKBOffset(void)
{
    return(KB);
}

//------------------------------------------------------------------------------

int CStandardModelObject::GetKCOffset(void)
{
    return(KC);
}

//------------------------------------------------------------------------------

CGOColorMode* CStandardModelObject::GetColorMode(void)
{
    return(ColorMode);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStandardModelObject::SetInitialColorScheme(void)
{
//    // set global color enviroment
//    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,ColorsList.GlobalMaterial.Ambient);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,ColorsList.GlobalMaterial.Diffuse);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,ColorsList.GlobalMaterial.Emission);

//    float shininess[1];
//    shininess[0] = ColorsList.GlobalMaterial.Shininess;
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS ,shininess);

//    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,ColorsList.GlobalMaterial.Specular);

//    glEnable(GL_COLOR_MATERIAL);
//    glColorMaterial(GL_FRONT_AND_BACK,ColorsList.MaterialMode);
}

//------------------------------------------------------------------------------

void CStandardModelObject::SetPBCOffset(CStructure* p_str)
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

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStandardModelObject::DrawStructureList(CStructureList* p_sl)
{
    if( (IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HIDDEN) == false)
        && ( p_sl->IsFlagSet(EPOF_VISIBLE) == false) )  return;

    foreach(QObject* p_qobj,p_sl->children()) {
        CStructure* p_str = static_cast<CStructure*>(p_qobj);
        koffset.SetZero();
        SetPBCOffset(p_str);
        DrawStructure(p_str);
    }
}

//------------------------------------------------------------------------------

void CStandardModelObject::DrawStructure(CStructure* p_str)
{
    if( (IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HIDDEN) == false)
        && ( p_str->IsFlagSet(EPOF_VISIBLE) == false) )  return;

    foreach(QObject* p_qobj,p_str->GetAtoms()->children()) {
        CAtom* p_a = static_cast<CAtom*>(p_qobj);
        DrawAtom(p_a);
    }

    foreach(QObject* p_qobj,p_str->GetBonds()->children()) {
        CBond* p_b = static_cast<CBond*>(p_qobj);
        DrawBond(p_b);
    }
}

//------------------------------------------------------------------------------

void CStandardModelObject::DrawResidue(CResidue* p_res)
{
    if( (IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HIDDEN) == false)
        && ( p_res->IsFlagSet(EPOF_VISIBLE) == false) )  return;

    // draw atoms
    foreach(CAtom* p_atom,p_res->GetAtoms()) {
        DrawAtom(p_atom);
    }

    // draw bonds
    foreach(CBond* p_bond, p_res->GetBonds(true)){
        DrawBond(p_bond);
    }
}

//------------------------------------------------------------------------------

void CStandardModelObject::DrawAtom(CAtom* p_atom)
{
    if( (IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HIDDEN) == false) &&
        (p_atom->IsFlagSet(EPOF_VISIBLE) == false) )  return;

    if( p_atom->GetResidue() ){
        if( (IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HIDDEN) == false) &&
            (p_atom->GetResidue()->IsFlagSet(EPOF_VISIBLE) == false) )  return;
    }

    CSimplePoint<float>    pos;
    float                  radius;
    CElementColors*        color;
    bool                   selected;
    int                    Z;

//    if(AtomsSet->Type > 0 ) {
//        glEnable(GL_LIGHTING);
//    } else {
//        glDisable(GL_LIGHTING);
//    }

    pos = p_atom->GetPos()+koffset;
    Z = p_atom->GetZ();

    if( PeriodicTable.IsVirtual(Z) ){
        if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HYDROGENS) == false ) return;
    }

    selected = p_atom->IsFlagSet(EPOF_SELECTED);
    if( p_atom->GetStructure() != NULL ) {
        selected |= p_atom->GetStructure()->IsFlagSet(EPOF_SELECTED);
    }
    if( p_atom->GetResidue() != NULL ) {
        selected |= p_atom->GetResidue()->IsFlagSet(EPOF_SELECTED);
    }
    color = ColorMode->GetElementColor(p_atom);

    GLLoadObject(p_atom);

    if( AtomsSet->Radius != 0 ) {
        radius = AtomsSet->Radius;
    } else {
        radius = PeriodicTable.GetVdWRadius(Z);
        radius *= AtomsSet->Ratio;
    }

//    glPushMatrix();
//    glTranslatef(pos.x, pos.y, pos.z);
    color->ApplyMaterialColor();

    switch(AtomsSet->Type){
    case 0:
        break;
    case 1:
        DrawSphere(radius,AtomsSet->TessellationQuality,AtomsSet->TessellationQuality);
        break;
    case 2:
        Sphere.Draw(radius);
        break;
    }

    if( selected == true ) {
//        glEnable(GL_BLEND);
        ColorsList.SelectionMaterial.ApplyMaterialColor();
        switch(AtomsSet->Type){
        case 0:
            DrawSphere(radius+0.1,AtomsSet->TessellationQuality,AtomsSet->TessellationQuality);
            break;
        case 1:
            DrawSphere(radius+0.1,AtomsSet->TessellationQuality,AtomsSet->TessellationQuality);
            break;
        case 2:
            Sphere.Draw(radius+0.1);
            break;
        }
//        glDisable(GL_BLEND);
    }
//    glPopMatrix();


}

//------------------------------------------------------------------------------

void CStandardModelObject::DrawBond(CBond* p_bond)
{
    if( BondsSet->Radius == 0 ) return;

    if( p_bond->IsInvalidBond() ) return;

    CAtom* p_atom1 = p_bond->GetFirstAtom();
    CAtom* p_atom2 = p_bond->GetSecondAtom();

    if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HIDDEN) == false ){
        if( p_bond->IsFlagSet(EPOF_VISIBLE) == false ) return;
        if( p_atom1->GetResidue() && (p_atom1->GetResidue()->IsFlagSet(EPOF_VISIBLE) == false)
            && p_atom2->GetResidue() && (p_atom2->GetResidue()->IsFlagSet(EPOF_VISIBLE) == false) ) return;
    }
    if( (p_atom1->IsFlagSet(EPOF_VISIBLE) == false) && (p_atom2->IsFlagSet(EPOF_VISIBLE) == false) ) return;

//    if(BondsSet->Type > 0 ) {
//        glEnable(GL_LIGHTING);
//    } else {
//        glDisable(GL_LIGHTING);
//    }

    int Z1,Z2;
    Z1 = p_atom1->GetZ();
    Z2 = p_atom2->GetZ();

    if( PeriodicTable.IsVirtual(Z1) || PeriodicTable.IsVirtual(Z2) ){
        if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_HYDROGENS) == false ) return;
    }

    CElementColors*    color1;
    CElementColors*    color2;
    bool               selected;
    float              VdW1;
    float              VdW2;

    selected = p_bond->IsFlagSet(EPOF_SELECTED);
    if( p_bond->GetStructure() != NULL ) {
        selected |= p_bond->GetStructure()->IsFlagSet(EPOF_SELECTED);
    }
    selected |= p_bond->IsSelectedByResidues();

    color1 = ColorMode->GetElementColor(p_atom1);
    color2 = ColorMode->GetElementColor(p_atom2);

    VdW1   = PeriodicTable.GetVdWRadius(Z1);               // van der Waalsovy polomery
    VdW2   = PeriodicTable.GetVdWRadius(Z2);

    GLLoadObject(p_bond);

    CSimplePoint<float> pos1;
    CSimplePoint<float> posm;
    CSimplePoint<float> pos2;
    CSimplePoint<float> ordervect;
    EBondOrder          order;

    order = p_bond->GetBondOrder();
    if( order > BO_SINGLE ){
        ordervect = p_bond->GetMainVector();
        ordervect *= BondsSet->Pitch;
    }

    if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_PBC_BONDS) ){
        pos1 = p_atom1->GetPos()+koffset;
        pos2 = p_atom2->GetPos()+koffset;

        double length = Size(pos1-pos2);
        if( length > p_bond->GetStructure()->PBCInfo.GetLargestSphereRadius() ){
            CPoint diff = pos1-pos2;
            CPoint pos1_backup = pos1;
            diff = p_bond->GetStructure()->PBCInfo.ImageVector(diff);

            pos1 = pos2 + diff;
            posm = pos1 + (pos2 - pos1)*(VdW1 / (VdW1 + VdW2));
            DrawRawBond(pos1,pos2,posm,color1,color2,order,ordervect,selected);

            pos1 = pos1_backup;
            pos2 = pos1 - diff;
            posm = pos1 + (pos2 - pos1)*(VdW1 / (VdW1 + VdW2));
            DrawRawBond(pos1,pos2,posm,color1,color2,order,ordervect,selected);

        } else {
            posm = pos1 + (pos2 - pos1)*(VdW1 / (VdW1 + VdW2));
            DrawRawBond(pos1,pos2,posm,color1,color2,order,ordervect,selected);
        }

    } else {
        pos1 = p_atom1->GetPos()+koffset;
        pos2 = p_atom2->GetPos()+koffset;

        posm = pos1 + (pos2 - pos1)*(VdW1 / (VdW1 + VdW2));

        DrawRawBond(pos1,pos2,posm,color1,color2,order,ordervect,selected);
    }
}

//------------------------------------------------------------------------------

void CStandardModelObject::DrawRawBond(CSimplePoint<float>& pos1,
                                       CSimplePoint<float>& pos2,
                                       CSimplePoint<float>& posm,
                                       CElementColors* color1,CElementColors* color2,
                                       EBondOrder order,
                                       const CSimplePoint<float>& ordervect,
                                       bool selected)
{
    float radius = BondsSet->Radius;

//    glPushMatrix();

    switch( BondsSet->Type ){
        // ----------------
        case 0:
//            glLineWidth(radius*100);
            if( BondsSet->ShowOrder == true ){
                if( BondsSet->Diffuse == true ) {
                    DrawStick(pos1,pos2,color1,color2,order,ordervect);
                } else {
                    DrawStick(pos1,posm,pos2,color1,color2,order,ordervect);
                }
            } else {
                if( BondsSet->Diffuse == true ) {
                    DrawStick(pos1,pos2,color1,color2);
                } else {
                    DrawStick(pos1,posm,pos2,color1,color2);
                }
            }
        break;
        // ----------------
        case 1:
            if( BondsSet->ShowOrder == true ){
                if( BondsSet->Diffuse == true ) {
                    DrawTube(pos1,pos2,radius,color1,color2,order,ordervect);
                } else {
                    DrawTube(pos1,posm,pos2,radius,color1,color2,order,ordervect);
                }
            } else {
                if( BondsSet->Diffuse == true ) {
                    DrawTube(pos1,pos2,radius,color1,color2);
                } else {
                    DrawTube(pos1,posm,pos2,radius,color1,color2);
                }
            }
        break;
    }

    if( selected == true ) {
//        glEnable(GL_BLEND);
        CElementColors*    color1 = &ColorsList.SelectionMaterial;
        DrawTube(pos1,pos2,radius+0.05,color1,color1);
//        glDisable(GL_BLEND);
    }

//    glPopMatrix();

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStandardModelObject::DrawTube(
                                    CSimplePoint<float>& p1,
                                    CSimplePoint<float>& p2,
                                    float  radius,
                                    const CElementColors* color1,
                                    const CElementColors* color2
                                    )
{
    CSimplePoint<float>     pd;
    float                   rotAngle;
    float                   cylH;

    pd = p2 - p1;
    cylH = Size(pd);

//    glPushMatrix();

    if (cylH == 0) {
        rotAngle = 0;
    } else {
        rotAngle = acos(pd.z/cylH)*180.0/3.14;
    }
//    glTranslatef (p1.x,p1.y,p1.z);
//    if( (pd.y != 0) || (pd.x != 0) ) {
//        glRotatef(rotAngle,-pd.y,pd.x, 0);
//    } else {
//        if( rotAngle > 90.0 ) glScalef(1,1,-1);
//    }

    Cylinder.DrawWithMaterialColors(radius,cylH,color1,color2);

//    glPopMatrix();
}

//---------------------------------------------------------------------------

void CStandardModelObject::DrawTube(
                                    CSimplePoint<float>& p1,
                                    CSimplePoint<float>& m,
                                    CSimplePoint<float>& p2,
                                    float  radius,
                                    const CElementColors* color1,
                                    const CElementColors* color2
                                    )
{
    CSimplePoint<float>     pd;
    CSimplePoint<float>     pm;
    float                   rotAngle;
    float                   cylH;
    float                   cylM;

    pd = p2 - p1;
    pm = m - p1;
    cylH = Size(pd);
    cylM = Size(pm);

//    glPushMatrix();

    if (cylH == 0) {
        rotAngle = 0;
    } else {
        rotAngle = acos(pd.z/cylH)*180.0/3.14;
    }
//    glTranslatef (p1.x,p1.y,p1.z);
//    if( (pd.y != 0) || (pd.x != 0) ) {
//        glRotatef(rotAngle,-pd.y,pd.x, 0);
//    } else {
//        if( rotAngle > 90.0 ) glScalef(1,1,-1);
//    }

//    color1->ApplyMaterialColor();
//    Cylinder.Draw(radius,cylM);
//    glTranslatef(0,0,cylM);
//    color2->ApplyMaterialColor();
//    Cylinder.Draw(radius,cylH-cylM);

//    glPopMatrix();
}

//---------------------------------------------------------------------------

void CStandardModelObject::DrawTube(
                                    CSimplePoint<float>& p1,
                                    CSimplePoint<float>& p2,
                                    float  radius,
                                    const CElementColors* color1,
                                    const CElementColors* color2,
                                    EBondOrder order,
                                    const CSimplePoint<float> &ordervect
                                    )
{
    CSimplePoint<float> pl1;
    CSimplePoint<float> pl2;

    switch(order) {
        case BO_SINGLE:
            DrawTube(p1,p2,radius,color1,color2);
            return;

        case BO_SINGLE_H:
            pl1 = p1 + ordervect*0.5;
            pl2 = p2 + ordervect*0.5;
            DrawTube(pl1,pl2,radius,color1,color2);
            pl1 = p1 - ordervect*0.5;
            pl2 = p2 - ordervect*0.5;
            DrawTube(pl1,pl2,radius,color1,color2);
            return;

        case BO_DOUBLE:
            pl1 = p1 + ordervect*0.5;
            pl2 = p2 + ordervect*0.5;
            DrawTube(pl1,pl2,radius,color1,color2);
            pl1 = p1 - ordervect*0.5;
            pl2 = p2 - ordervect*0.5;
            DrawTube(pl1,pl2,radius,color1,color2);
            return;

        case BO_DOUBLE_H:
            DrawTube(p1,p2,radius,color1,color2);
            pl1 = p1 + ordervect;
            pl2 = p2 + ordervect;
            DrawTube(pl1,pl2,radius,color1,color2);
            pl1 = p1 - ordervect;
            pl2 = p2 - ordervect;
            DrawTube(pl1,pl2,radius,color1,color2);
            return;

        case BO_TRIPLE:
            DrawTube(p1,p2,radius,color1,color2);
            pl1 = p1 + ordervect;
            pl2 = p2 + ordervect;
            DrawTube(pl1,pl2,radius,color1,color2);
            pl1 = p1 - ordervect;
            pl2 = p2 - ordervect;
            DrawTube(pl1,pl2,radius,color1,color2);
            return;

        case BO_NONE:
        case BO_WEAK:
        default:
            return;
    }
}

//---------------------------------------------------------------------------

void CStandardModelObject::DrawTube(
                                    CSimplePoint<float>& p1,
                                    CSimplePoint<float>& pm,
                                    CSimplePoint<float>& p2,
                                    float  radius,
                                    const CElementColors* color1,
                                    const CElementColors* color2,
                                    EBondOrder order,
                                    const CSimplePoint<float>& ordervect
                                    )
{
    CSimplePoint<float> pl1;
    CSimplePoint<float> plm;
    CSimplePoint<float> pl2;

    switch(order) {
        case BO_SINGLE:
            DrawTube(p1,pm,p2,radius,color1,color2);
            return;

        case BO_SINGLE_H:
            pl1 = p1 + ordervect*0.5;
            plm = pm + ordervect*0.5;
            pl2 = p2 + ordervect*0.5;
            DrawTube(pl1,plm,pl2,radius,color1,color2);
            pl1 = p1 - ordervect*0.5;
            plm = pm - ordervect*0.5;
            pl2 = p2 - ordervect*0.5;
            DrawTube(pl1,plm,pl2,radius,color1,color2);
            return;

        case BO_DOUBLE:
            pl1 = p1 + ordervect*0.5;
            plm = pm + ordervect*0.5;
            pl2 = p2 + ordervect*0.5;
            DrawTube(pl1,plm,pl2,radius,color1,color2);
            pl1 = p1 - ordervect*0.5;
            plm = pm - ordervect*0.5;
            pl2 = p2 - ordervect*0.5;
            DrawTube(pl1,plm,pl2,radius,color1,color2);
            return;

        case BO_DOUBLE_H:
            DrawTube(p1,pm,p2,radius,color1,color2);
            pl1 = p1 + ordervect;
            plm = pm + ordervect;
            pl2 = p2 + ordervect;
            DrawTube(pl1,plm,pl2,radius,color1,color2);
            pl1 = p1 - ordervect;
            plm = pm - ordervect;
            pl2 = p2 - ordervect;
            DrawTube(pl1,plm,pl2,radius,color1,color2);
            return;

        case BO_TRIPLE:
            DrawTube(p1,pm,p2,radius,color1,color2);
            pl1 = p1 + ordervect;
            plm = pm + ordervect;
            pl2 = p2 + ordervect;
            DrawTube(pl1,plm,pl2,radius,color1,color2);
            pl1 = p1 - ordervect;
            plm = pm - ordervect;
            pl2 = p2 - ordervect;
            DrawTube(pl1,plm,pl2,radius,color1,color2);
            return;

        case BO_NONE:
        case BO_WEAK:
        default:
            return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStandardModelObject::DrawStick(
                                    CSimplePoint<float>& p1,
                                    CSimplePoint<float>& p2,
                                    const CElementColors* color1,
                                    const CElementColors* color2
                                    )
{
//    glBegin(GL_LINES);
//        color1->ApplyMaterialColor();
//        glVertex3fv(p1);
//        if( color2 != color1 ) color2->ApplyMaterialColor();
//        glVertex3fv(p2);
//    glEnd();
}

//---------------------------------------------------------------------------

void CStandardModelObject::DrawStick(
                                    CSimplePoint<float>& p1,
                                    CSimplePoint<float>& m,
                                    CSimplePoint<float>& p2,
                                    const CElementColors* color1,
                                    const CElementColors* color2
                                    )
{
//    glBegin(GL_LINES);
//        color1->ApplyMaterialColor();
//        glVertex3fv(p1);
//        glVertex3fv(m);
//        if( color2 != color1 ) color2->ApplyMaterialColor();
//        glVertex3fv(m);
//        glVertex3fv(p2);
//    glEnd();
}

//---------------------------------------------------------------------------

void CStandardModelObject::DrawStick(
                                    CSimplePoint<float>& p1,
                                    CSimplePoint<float>& p2,
                                    const CElementColors* color1,
                                    const CElementColors* color2,
                                    EBondOrder order,
                                    const CSimplePoint<float> &ordervect
                                    )
{
    CSimplePoint<float> pl1;
    CSimplePoint<float> pl2;

    switch(order) {
        case BO_SINGLE:
            DrawStick(p1,p2,color1,color2);
            return;

        case BO_SINGLE_H:
            pl1 = p1 + ordervect*0.5;
            pl2 = p2 + ordervect*0.5;
            DrawStick(pl1,pl2,color1,color2);
            pl1 = p1 - ordervect*0.5;
            pl2 = p2 - ordervect*0.5;
            DrawStick(pl1,pl2,color1,color2);
            return;

        case BO_DOUBLE:
            pl1 = p1 + ordervect*0.5;
            pl2 = p2 + ordervect*0.5;
            DrawStick(pl1,pl2,color1,color2);
            pl1 = p1 - ordervect*0.5;
            pl2 = p2 - ordervect*0.5;
            DrawStick(pl1,pl2,color1,color2);
            return;

        case BO_DOUBLE_H:
            DrawStick(p1,p2,color1,color2);
            pl1 = p1 + ordervect;
            pl2 = p2 + ordervect;
            DrawStick(pl1,pl2,color1,color2);
            pl1 = p1 - ordervect;
            pl2 = p2 - ordervect;
            DrawStick(pl1,pl2,color1,color2);
            return;

        case BO_TRIPLE:
            DrawStick(p1,p2,color1,color2);
            pl1 = p1 + ordervect;
            pl2 = p2 + ordervect;
            DrawStick(pl1,pl2,color1,color2);
            pl1 = p1 - ordervect;
            pl2 = p2 - ordervect;
            DrawStick(pl1,pl2,color1,color2);
            return;

        case BO_NONE:
        case BO_WEAK:
        default:
            return;
    }
}

//---------------------------------------------------------------------------

void CStandardModelObject::DrawStick(
                                CSimplePoint<float>& p1,
                                CSimplePoint<float>& pm,
                                CSimplePoint<float>& p2,
                                const CElementColors* color1,
                                const CElementColors* color2,
                                EBondOrder order,
                                const CSimplePoint<float>& ordervect
                                )
{
    CSimplePoint<float> pl1;
    CSimplePoint<float> plm;
    CSimplePoint<float> pl2;

    switch(order) {
        case BO_SINGLE:
            DrawStick(p1,pm,p2,color1,color2);
            return;

        case BO_SINGLE_H:
            pl1 = p1 + ordervect*0.5;
            plm = pm + ordervect*0.5;
            pl2 = p2 + ordervect*0.5;
            DrawStick(pl1,plm,pl2,color1,color2);
            pl1 = p1 - ordervect*0.5;
            plm = pm - ordervect*0.5;
            pl2 = p2 - ordervect*0.5;
            DrawStick(pl1,plm,pl2,color1,color2);
            return;

        case BO_DOUBLE:
            pl1 = p1 + ordervect*0.5;
            plm = pm + ordervect*0.5;
            pl2 = p2 + ordervect*0.5;
            DrawStick(pl1,plm,pl2,color1,color2);
            pl1 = p1 - ordervect*0.5;
            plm = pm - ordervect*0.5;
            pl2 = p2 - ordervect*0.5;
            DrawStick(pl1,plm,pl2,color1,color2);
            return;

        case BO_DOUBLE_H:
            DrawStick(p1,pm,p2,color1,color2);
            pl1 = p1 + ordervect;
            plm = pm + ordervect;
            pl2 = p2 + ordervect;
            DrawStick(pl1,plm,pl2,color1,color2);
            pl1 = p1 - ordervect;
            plm = pm - ordervect;
            pl2 = p2 - ordervect;
            DrawStick(pl1,plm,pl2,color1,color2);
            return;

        case BO_TRIPLE:
            DrawStick(p1,pm,p2,color1,color2);
            pl1 = p1 + ordervect;
            plm = pm + ordervect;
            pl2 = p2 + ordervect;
            DrawStick(pl1,plm,pl2,color1,color2);
            pl1 = p1 - ordervect;
            plm = pm - ordervect;
            pl2 = p2 - ordervect;
            DrawStick(pl1,plm,pl2,color1,color2);
            return;

        case BO_NONE:
        case BO_WEAK:
        default:
            return;
    }
}

//------------------------------------------------------------------------------

void CStandardModelObject::DrawSphere(float radius,int slices,int stacks)
{
//    gluSphere(ExtQuad,radius,slices,stacks);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStandardModelObject::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load object info -----------------------------
    CGraphicsObject::LoadData(p_ele);

    // specific data -----------------------------
    EModel model;
    p_ele->GetAttribute<EModel>("model",model);
    SetModel(model);

    p_ele->GetAttribute("ka",KA);
    p_ele->GetAttribute("kb",KB);
    p_ele->GetAttribute("kc",KC);

    CXMLElement* p_sele = p_ele->GetFirstChildElement("cmode");
    if( p_sele ) {
        ColorMode->LoadData(p_sele);
    } 
}

//---------------------------------------------------------------------------

void CStandardModelObject::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // core data ---------------------------------
    CGraphicsObject::SaveData(p_ele);

    // specific data -----------------------------
    p_ele->SetAttribute("model",Model);
    p_ele->SetAttribute("ka",KA);
    p_ele->SetAttribute("kb",KB);
    p_ele->SetAttribute("kc",KC);

    CXMLElement* p_sele = p_ele->CreateChildElement("cmode");
    ColorMode->SaveData(p_sele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

