// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <GL/gl.h>
#include <GL/glu.h>
#include <Project.hpp>
#include <Structure.hpp>
#include <StructureList.hpp>
#include <Atom.hpp>
#include <Residue.hpp>
#include <AtomList.hpp>
#include <GraphicsProfileList.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>
#include <HistoryNode.hpp>
#include <GLSelection.hpp>
#include <GraphicsView.hpp>
#include <ElementColorsList.hpp>

#include <AtomLabelObjectHistory.hpp>
#include <AtomLabelObject.hpp>
#include <AtomLabelSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* AtomLabelObjectCB(void* p_data);

CExtUUID        AtomLabelObjectID(
                    "{ATOM_LABEL_OBJECT:9949e9ed-1c44-4bc6-b510-9f055893e2b1}",
                    "Atom Label");

CPluginObject   AtomLabelObject(&NemesisCorePlugin,
                     AtomLabelObjectID,GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/objects/AtomLabel.svg",
                     AtomLabelObjectCB);

// -----------------------------------------------------------------------------

QObject* AtomLabelObjectCB(void* p_data)
{
    return(new CAtomLabelObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomLabelObject::CAtomLabelObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&AtomLabelObject,p_gl)
{
    RegisterAllowedObjectType(StructureListID);
    RegisterAllowedObjectType(StructureID);
    RegisterAllowedObjectType(ResidueID);
    RegisterAllowedObjectType(AtomID);
    SetFlag<EAtomLabelObjectFlag>(EALOF_SHOW_NAME,true);
    SetFlag<EAtomLabelObjectFlag>(EALOF_SHOW_HYDROGENS,true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CAtomLabelObject::SetCustomFormatWH(const QString& format)
{
    if( GetCustomFormat() == format ) return(true);

    // with history
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"custom format");
    if( p_history == NULL ) return(false);

    SetCustomFormat(format,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CAtomLabelObject::SetOffsetWH(const CPoint& offset)
{
    if( Offset == offset ) return(true);

    // with history
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"offset");
    if( p_history == NULL ) return(false);

    SetOffset(offset,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

void CAtomLabelObject::SetCustomFormat(const QString& format,CHistoryNode* p_history)
{
    if( Format == format ) return;

    if( p_history ){
        CAtomLabelObjectChangeFormatHI* p_hnode = new CAtomLabelObjectChangeFormatHI(this,format);
        p_history->Register(p_hnode);
    }

    Format = format;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CAtomLabelObject::SetOffset(const CPoint& offset,CHistoryNode* p_history)
{
    if( Offset == offset ) return;

    if( p_history ){
        CAtomLabelObjectChangeOffsetHI* p_hnode = new CAtomLabelObjectChangeOffsetHI(this,offset);
        p_history->Register(p_hnode);
    }

    Offset = offset;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

const QString& CAtomLabelObject::GetCustomFormat(void) const
{
    return(Format);
}

//------------------------------------------------------------------------------

const CPoint& CAtomLabelObject::GetOffset(void) const
{
    return(Offset);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CAtomLabelObject::StartManipulation(CHistoryNode* p_history)
{
    if( p_history == NULL ) return;

    CHistoryItem* p_item = new CAtomLabelObjectBackupGeoHI(this);
    p_history->Register(p_item);

    // use delayed event notification
    StartEvent();
}

//------------------------------------------------------------------------------

void CAtomLabelObject::Move(const CPoint& dmov,CGraphicsView* p_view)
{
    CPoint _dmov = dmov;

    CTransformation coord = p_view->GetTrans();
    coord.Invert();
    coord.Apply(_dmov);

    Offset += _dmov;

    // use delayed event notification
    RaiseEvent();
}

//------------------------------------------------------------------------------

void CAtomLabelObject::EndManipulation(void)
{
    // end delayed event notification
    EndEvent();
}

//------------------------------------------------------------------------------

void CAtomLabelObject::RespondToEvent(void)
{
    // respond to delayed notification
    emit OnStatusChanged(ESC_OTHER);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CAtomLabelObject::Draw(void)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    GLLoadObject(this);

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);

    foreach(QObject* p_qobj,Objects) {
        CProObject*     p_object = static_cast<CProObject*>(p_qobj);
        CStructureList* p_strlist;
        CStructure*     p_str;
        CResidue*       p_res;
        CAtom*          p_atom;
        if( (p_strlist = dynamic_cast<CStructureList*>(p_object)) != NULL ){
            LabelStructureList(p_strlist);
        }
        if( (p_str = dynamic_cast<CStructure*>(p_object)) != NULL ){
            LabelStructure(p_str);
        }
        if( (p_res = dynamic_cast<CResidue*>(p_object)) != NULL ){
            LabelResidue(p_res);
        }
        if( (p_atom = dynamic_cast<CAtom*>(p_object)) != NULL ){
            LabelAtom(p_atom);
        }
    }

}

//------------------------------------------------------------------------------

void CAtomLabelObject::LabelStructureList(CStructureList* p_strlist)
{
    if( (p_strlist->IsFlagSet(EPOF_VISIBLE) == false) && (IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_HIDDEN) == false) ) return;

    foreach(QObject* p_qobj,p_strlist->children()) {
        CStructure* p_str = static_cast<CStructure*>(p_qobj);
        LabelStructure(p_str);
    }
}

//------------------------------------------------------------------------------

void CAtomLabelObject::LabelStructure(CStructure* p_str)
{
    if( (p_str->IsFlagSet(EPOF_VISIBLE) == false) && (IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_HIDDEN) == false) ) return;

    foreach(QObject* p_qobj,p_str->GetAtoms()->children()) {
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        LabelAtom(p_atom);
    }
}

//------------------------------------------------------------------------------

void CAtomLabelObject::LabelResidue(CResidue* p_res)
{
    if( (p_res->IsFlagSet(EPOF_VISIBLE) == false) && (IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_HIDDEN) == false) ) return;

    foreach(CAtom* p_atom,p_res->GetAtoms()) {
        LabelAtom(p_atom);
    }
}

//------------------------------------------------------------------------------

void CAtomLabelObject::LabelAtom(CAtom* p_atom)
{
    if( (p_atom->IsFlagSet(EPOF_VISIBLE) == false) &&
        (IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_HIDDEN) == false) ) return;
    if( p_atom->GetResidue() ){
        if( (p_atom->GetResidue()->IsFlagSet(EPOF_VISIBLE) == false) &&
            (IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_HIDDEN) == false) ) return;
    }
    if( (p_atom->GetStructure()->IsFlagSet(EPOF_VISIBLE) == false) &&
        (IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_HIDDEN) == false) ) return;

    if( p_atom->IsVirtual() &&
       (IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_HYDROGENS) == false) ) return;

    CAtomLabelSetup* p_setup = GetSetup<CAtomLabelSetup>();
    // does setup exist?
    if( p_setup == NULL ) return;

    QString label;

    // complete label
    if( IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_CUSTOM) ) {
        label = Format;
        // do not use arg() as it does not allow substitution when some field is not requested by an user
        label = label.replace("%1",p_atom->GetName());
        label = label.replace("%2",p_atom->GetType());
        label = label.replace("%3",PQ_CHARGE->GetRealValueText(p_atom->GetCharge()));
        label = label.replace("%4",p_atom->GetResidue()->GetName());
    } else {
        if( IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_NAME) ) {
            label += p_atom->GetName() + " ";
        }
        if( IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_TYPE) ) {
            label += p_atom->GetType() + " ";
        }
        if( IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_CHARGE) ) {
            label += PQ_CHARGE->GetRealValueText(p_atom->GetCharge()) + " ";
        }
        if( IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_RESNAME) ) {
            if( p_atom->GetResidue() ){
                label += p_atom->GetResidue()->GetName() + " ";
            }
        }
    }

    // set position
    CPoint  pos1,pos2;
    pos1 = p_atom->GetPos();
    pos2 = pos1 + Offset;

    // draw label
    glPushMatrix();
        if( IsFlagSet(EPOF_SELECTED) ) {
            glColor4fv(ColorsList.SelectionMaterial.Color);
        } else {
            glColor4fv(p_setup->LabelColor);
        }
        glTranslatef(pos2.x,pos2.y,pos2.z);

        // the text should be faced to an user
        ApplyFaceTransformation();

        // rotate FTGL to our coordinate system
        glRotatef(90,1,0,0);
        glRotatef(90,0,1,0);
        glScalef(0.01,0.01,0.01);

        if( p_setup->DepthTest ){
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        p_setup->FTGLFont.Render(label.toStdWString().c_str());
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);

    // draw line
    if( IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_LINE) ){
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(p_setup->LineStippleFactor,p_setup->LineStipplePattern);
        glLineWidth(p_setup->LineWidth);
        glColor4fv(p_setup->LineColor);
        glBegin(GL_LINES);
            glVertex3dv(pos1);
            glVertex3dv(pos2);
        glEnd();
        glDisable(GL_LINE_STIPPLE);
    }
}

//------------------------------------------------------------------------------

void CAtomLabelObject::GetObjectMetrics(CObjMetrics& metrics)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    foreach(QObject* p_qobj,Objects) {
        CProObject*     p_object = static_cast<CProObject*>(p_qobj);
        CStructureList* p_strlist;
        CStructure*     p_str;
        CResidue*       p_res;
        CAtom*          p_atom;
        if( (p_strlist = dynamic_cast<CStructureList*>(p_object)) != NULL ){
            GetStructureListMetrics(p_strlist,metrics);
        }
        if( (p_str = dynamic_cast<CStructure*>(p_object)) != NULL ){
            GetStructureMetrics(p_str,metrics);
        }
        if( (p_res = dynamic_cast<CResidue*>(p_object)) != NULL ){
            GetResidueMetrics(p_res,metrics);
        }
        if( (p_atom = dynamic_cast<CAtom*>(p_object)) != NULL ){
            GetAtomMetrics(p_atom,metrics);
        }
    }
}

//------------------------------------------------------------------------------

void CAtomLabelObject::GetStructureListMetrics(CStructureList* p_strlist,CObjMetrics& metrics)
{
    if( (p_strlist->IsFlagSet(EPOF_VISIBLE) == false) && (IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_HIDDEN) == false) ) return;

    foreach(QObject* p_qobj,p_strlist->children()) {
        CStructure* p_str = static_cast<CStructure*>(p_qobj);
        GetStructureMetrics(p_str,metrics);
    }
}

//------------------------------------------------------------------------------

void CAtomLabelObject::GetStructureMetrics(CStructure* p_str,CObjMetrics& metrics)
{
    if( (p_str->IsFlagSet(EPOF_VISIBLE) == false) && (IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_HIDDEN) == false) ) return;

    foreach(QObject* p_qobj,p_str->GetAtoms()->children()) {
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        GetAtomMetrics(p_atom,metrics);
    }
}

//------------------------------------------------------------------------------

void CAtomLabelObject::GetResidueMetrics(CResidue* p_res,CObjMetrics& metrics)
{
    if( (p_res->IsFlagSet(EPOF_VISIBLE) == false) && (IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_HIDDEN) == false) ) return;

    foreach(CAtom* p_atom,p_res->GetAtoms()) {
        GetAtomMetrics(p_atom,metrics);
    }
}

//------------------------------------------------------------------------------

void CAtomLabelObject::GetAtomMetrics(CAtom* p_atom,CObjMetrics& metrics)
{
    if( (p_atom->IsFlagSet(EPOF_VISIBLE) == false) &&
        (IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_HIDDEN) == false) ) return;
    if( p_atom->GetResidue() ){
        if( (p_atom->GetResidue()->IsFlagSet(EPOF_VISIBLE) == false) &&
            (IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_HIDDEN) == false) ) return;
    }
    if( (p_atom->GetStructure()->IsFlagSet(EPOF_VISIBLE) == false) &&
        (IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_HIDDEN) == false) ) return;

    if( p_atom->IsVirtual() &&
       (IsFlagSet<EAtomLabelObjectFlag>(EALOF_SHOW_HYDROGENS) == false) ) return;

    metrics.CompareWith(p_atom->GetPos()+Offset);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CAtomLabelObject::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load object info -----------------------------
    CGraphicsObject::LoadData(p_ele);

    // specific data -----------------------------
    p_ele->GetAttribute("ox",Offset.x);
    p_ele->GetAttribute("oy",Offset.y);
    p_ele->GetAttribute("oz",Offset.z);
    p_ele->GetAttribute("format",Format);
}

//------------------------------------------------------------------------------

void CAtomLabelObject::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // core data ---------------------------------
    CGraphicsObject::SaveData(p_ele);

    // specific data -----------------------------
    p_ele->SetAttribute("ox",Offset.x);
    p_ele->SetAttribute("oy",Offset.y);
    p_ele->SetAttribute("oz",Offset.z);
    p_ele->SetAttribute("format",Format);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

