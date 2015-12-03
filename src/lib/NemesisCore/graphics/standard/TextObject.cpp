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
#include <Atom.hpp>
#include <Residue.hpp>
#include <AtomList.hpp>
#include <GraphicsViewList.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>
#include <HistoryNode.hpp>
#include <Property.hpp>
#include <ElementColorsList.hpp>
#include <GLSelection.hpp>

#include <TextObjectHistory.hpp>
#include <TextObject.hpp>
#include <TextSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* TextObjectCB(void* p_data);

CExtUUID        TextObjectID(
                    "{TEXT_OBJECT:47bc1a07-c933-4841-8a6e-776a0d85e82e}",
                    "Text");

CPluginObject   TextObject(&NemesisCorePlugin,
                     TextObjectID,GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/objects/Text.svg",
                     TextObjectCB);

// -----------------------------------------------------------------------------

QObject* TextObjectCB(void* p_data)
{
    return(new CTextObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTextObject::CTextObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&TextObject,p_gl)
{
    RegisterAllowedObjectType(PROPERTY_CAT);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTextObject::SetTextWH(const QString& text)
{
    if( Text == text ) return(true);

    // with history
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"text");
    if( p_history == NULL ) return(false);

    SetText(text,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CTextObject::SetPositionWH(const CPoint& pos)
{
    if( Position == pos ) return(true);

    // with history
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"text position");
    if( p_history == NULL ) return(false);

    SetPosition(pos,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CTextObject::SetOrientationWH(const CPoint& rot)
{
    if( Orientation == rot ) return(true);

    // with history
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"text orientation");
    if( p_history == NULL ) return(false);

    SetOrientation(rot,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

void CTextObject::SetText(const QString& text,CHistoryNode* p_history)
{
    if( Text == text ) return;

    if( p_history ){
        CTextObjectChangeTextHI* p_hnode = new CTextObjectChangeTextHI(this,text);
        p_history->Register(p_hnode);
    }

    Text = text;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CTextObject::SetPosition(const CPoint& pos,CHistoryNode* p_history)
{
    if( Position == pos ) return;

    if( p_history ){
        CTextObjectChangePositionHI* p_hnode = new CTextObjectChangePositionHI(this,pos);
        p_history->Register(p_hnode);
    }

    Position = pos;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CTextObject::SetOrientation(const CPoint& rot,CHistoryNode* p_history)
{
    if( Orientation == rot ) return;

    if( p_history ){
        CTextObjectChangeOrientationHI* p_hnode = new CTextObjectChangeOrientationHI(this,rot);
        p_history->Register(p_hnode);
    }

    Orientation = rot;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

const CPoint& CTextObject::GetPosition(void) const
{
    return(Position);
}

//------------------------------------------------------------------------------

const CPoint& CTextObject::GetOrientation(void) const
{
    return(Orientation);
}

//------------------------------------------------------------------------------

const QString& CTextObject::GetText(void) const
{
    return(Text);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTextObject::Draw(void)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    CTextSetup* p_setup = GetSetup<CTextSetup>();
    // does setup exist?
    if( p_setup == NULL ) return;

    GLLoadObject(this);

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);

    QString label = Text;

    // inject property values
    int i=0;
    foreach(CProObject* p_obj, Objects){
        i++;
        CProperty* p_property = dynamic_cast<CProperty*>(p_obj);
        if( p_obj == NULL ) continue;
        QString sub;
        QString val;
        sub = "%" + QString("%1").arg(i);
        if( p_property->GetPropertyUnit() ){
            val = p_property->GetPropertyUnit()->GetRealValueText(p_property->GetScalarValue());
        }
        label = label.replace(sub,val);

        sub = "#" + QString("%1").arg(i);
        if( p_property->GetPropertyUnit() ){
            val = p_property->GetPropertyUnit()->GetUnitName();
        }
        label = label.replace(sub,val);
    }

    // draw label
    glPushMatrix();
        if( IsFlagSet(EPOF_SELECTED) ){
            glColor4fv(ColorsList.SelectionMaterial.Color);
        } else {
            glColor4fv(p_setup->LabelColor);
        }

        // move to global frame
        if( IsFlagSet<ETextObjectFlag>(ETOF_GLOBAL_POSITION) ){
            ApplyGlobalViewTransformation();
        }
        glTranslatef(Position.x,Position.y,Position.z);

        // the text should be faced to an user
        if( IsFlagSet<ETextObjectFlag>(ETOF_FACE_ORIENTATION) ){
            ApplyFaceTransformation();
        }
        glRotatef(Orientation.x*180.0/M_PI,0,0,1);
        glRotatef(Orientation.y*180.0/M_PI,0,1,0);
        glRotatef(Orientation.z*180.0/M_PI,1,0,0);

        // rotate FTGL to our coordinate system
        glRotatef(90,1,0,0);
        glRotatef(90,0,1,0);
        glScalef(0.01,0.01,0.01);

        if( p_setup->DepthTest ){
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        p_setup->FTGLFont.Render(label.toLatin1());
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
}

//------------------------------------------------------------------------------

void CTextObject::GetObjectMetrics(CObjMetrics& metrics)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    if( ! IsFlagSet<ETextObjectFlag>(ETOF_GLOBAL_POSITION) ){
        // it has sence only for local frame
        metrics.CompareWith(Position);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTextObject::StartManipulation(CHistoryNode* p_history)
{
    if( p_history == NULL ) return;

    CHistoryItem* p_item = new CTextObjectBackupGeoHI(this);
    p_history->Register(p_item);

    // use delayed event notification
    StartEvent();
}

//------------------------------------------------------------------------------

void CTextObject::Move(const CPoint& dmov,CGraphicsView* p_view)
{
    CPoint _dmov = dmov;

    if( ! IsFlagSet(ETOF_GLOBAL_POSITION) ){
        CTransformation coord = p_view->GetTrans();
        coord.Invert();
        coord.Apply(_dmov);
    }

    Position += _dmov;

    // use delayed event notification
    RaiseEvent();
}

//------------------------------------------------------------------------------

void CTextObject::Rotate(const CPoint& drot,CGraphicsView* p_view)
{
    CTransformation trans;
    CTransformation coord = p_view->GetTrans();
    trans *= coord;
    trans.Rotate(drot);
    coord.Invert();
    trans *= coord;

    trans.Apply(Orientation);

    // use delayed event notification
    RaiseEvent();
}

//------------------------------------------------------------------------------

void CTextObject::EndManipulation(void)
{
    // end delayed event notification
    EndEvent();
}

//------------------------------------------------------------------------------

void CTextObject::RespondToEvent(void)
{
    // respond to delayed notification
    emit OnStatusChanged(ESC_OTHER);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTextObject::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load object info -----------------------------
    CGraphicsObject::LoadData(p_ele);

    p_ele->GetAttribute("px",Position.x);
    p_ele->GetAttribute("py",Position.y);
    p_ele->GetAttribute("pz",Position.z);

    p_ele->GetAttribute("ox",Orientation.x);
    p_ele->GetAttribute("oy",Orientation.y);
    p_ele->GetAttribute("oz",Orientation.z);

    p_ele->GetAttribute("text",Text);
}

//------------------------------------------------------------------------------

void CTextObject::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // core data ---------------------------------
    CGraphicsObject::SaveData(p_ele);

    // specific data -----------------------------
    p_ele->SetAttribute("px",Position.x);
    p_ele->SetAttribute("py",Position.y);
    p_ele->SetAttribute("pz",Position.z);

    p_ele->SetAttribute("ox",Orientation.x);
    p_ele->SetAttribute("oy",Orientation.y);
    p_ele->SetAttribute("oz",Orientation.z);

    p_ele->SetAttribute("text",Text);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

