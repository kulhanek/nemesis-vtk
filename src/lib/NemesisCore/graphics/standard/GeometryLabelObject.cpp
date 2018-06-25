// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2011 Jiri Klusak, anim4tor@mail.muni.cz
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

#include <PluginObject.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <GraphicsObjectList.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <Project.hpp>
#include <Structure.hpp>
#include <Atom.hpp>
#include <AtomList.hpp>
#include <GraphicsProfileList.hpp>
#include <GraphicsUtil.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>
#include <GeoMeasurement.hpp>
#include <NemesisCoreModule.hpp>
#include <GeometryLabelObject.hpp>
#include <GeometryLabelSetup.hpp>
#include <GeometryLabelObjectHistory.hpp>
#include <GLSelection.hpp>
#include <GraphicsView.hpp>
#include <ElementColorsList.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GeometryLabelObjectCB(void* p_data);

CExtUUID        GeometryLabelObjectID(
                    "{GEOMETRY_LABEL_OBJECT:e8f67b10-14ec-4f72-9099-9a74827485b6}",
                    "Geometry Label");

CPluginObject   GeometryLabelObject(&NemesisCorePlugin,
                    GeometryLabelObjectID,GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/objects/GeometryLabel.svg",
                    GeometryLabelObjectCB);

// -----------------------------------------------------------------------------

QObject* GeometryLabelObjectCB(void* p_data)
{
    return(new CGeometryLabelObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGeometryLabelObject::CGeometryLabelObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&GeometryLabelObject,p_gl)
{
    Setup = NULL;
    Type = ESGLT_NONE;

    RegisterAllowedObjectType(PrivateObjectID);

    SetFlag<EStandardModelObjectFlag>(ESMOF_SHOW_TEXT,true);
    SetFlag<EStandardModelObjectFlag>(ESMOF_RELATIVE_TEXT_POS,true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CGeometryLabelObject::SetTypeWH(EGeometryLabelType type)
{
    if( Type == type ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("geometry label type"));
    if( p_history == NULL ) return (false);

    SetType(type,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CGeometryLabelObject::SetOffsetWH(const CPoint& offset)
{
    if( Offset == offset ) return(true);

    // with history
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"geometry label offset");
    if( p_history == NULL ) return(false);

    SetOffset(offset,p_history);

    EndChangeWH();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeometryLabelObject::SetType(EGeometryLabelType type,CHistoryNode* p_history)
{
    if( Type == type ) return;

    if( p_history ){
        CHistoryItem* p_item = new CGeometryLabelObjectChangeTypeHI(this,type);
        p_history->Register(p_item);
    }

    Type = type;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CGeometryLabelObject::SetOffset(const CPoint& offset,CHistoryNode* p_history)
{
    if( Offset == offset ) return;

    if( p_history ){
        CGeometryLabelObjectChangeOffsetHI* p_hnode = new CGeometryLabelObjectChangeOffsetHI(this,offset);
        p_history->Register(p_hnode);
    }

    Offset = offset;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

EGeometryLabelType CGeometryLabelObject::GetType(void)
{
    return(Type);
}

//------------------------------------------------------------------------------

const CPoint& CGeometryLabelObject::GetOffset(void) const
{
    return(Offset);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeometryLabelObject::StartManipulation(CHistoryNode* p_history)
{
    if( p_history == NULL ) return;

    CHistoryItem* p_item = new CGeometryLabelObjectBackupGeoHI(this);
    p_history->Register(p_item);

    // use delayed event notification
    StartEvent();
}

//------------------------------------------------------------------------------

void CGeometryLabelObject::Move(const CPoint& dmov,CGraphicsView* p_view)
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

void CGeometryLabelObject::EndManipulation(void)
{
    // end delayed event notification
    EndEvent();
}

//------------------------------------------------------------------------------

void CGeometryLabelObject::RespondToEvent(void)
{
    // respond to delayed notification
    emit OnStatusChanged(ESC_OTHER);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeometryLabelObject::Draw(void)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    Setup = GetSetup<CGeometryLabelSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);

    GLLoadObject(this);

    switch(Type){
        case ESGLT_NONE:
            // nothing to do
            break;
        case ESGLT_POINT:
            LabelPosition();
            break;
        case ESGLT_DISTANCE:
            LabelDistance();
            break;
        case ESGLT_ANGLE:
            LabelAngle();
            break;
        case ESGLT_DIHEDRAL:
            LabelDihedral();
            break;
    }

    glDisable(GL_LINE_STIPPLE);
}

//------------------------------------------------------------------------------

void CGeometryLabelObject::GetObjectMetrics(CObjMetrics& metrics)
{
    switch(Type){
        case ESGLT_NONE:
            // nothing to do
            break;
        case ESGLT_POINT:
            GetObjectMetricsPosition(metrics);
            break;
        case ESGLT_DISTANCE:
            GetObjectMetricsDistance(metrics);
            break;
        case ESGLT_ANGLE:
            GetObjectMetricsAngle(metrics);
            break;
        case ESGLT_DIHEDRAL:
            GetObjectMetricsDihedral(metrics);
            break;
    }
}

//------------------------------------------------------------------------------

void CGeometryLabelObject::LabelPosition(void)
{
    CSimplePoint<float> pos1;
    CGeoDescriptor      geo1;
    bool                completed = false;

    if( Objects.count() < 1 ) return;

    CProObject* p_obj1 = static_cast<CProObject*>(Objects[0]);
    geo1 = p_obj1->GetGeoDescriptor();

    switch(geo1.GetType()){
       case EGDT_NILL:
           return;
       case EGDT_ONE_POINT:
           pos1 = geo1.GetPoint1();
           completed = true;
           break;
       default:
           return;
    }

    if( completed == false ) return;

    // possition cross line points ---------------
    CSimplePoint<float>  xline1;
    CSimplePoint<float>  xline2;
    CSimplePoint<float>  yline1;
    CSimplePoint<float>  yline2;
    CSimplePoint<float>  zline1;
    CSimplePoint<float>  zline2;
    float                off = 0.5;

    // define line points ------------------------
    xline1.Set(pos1.x - off, pos1.y, pos1.z);
    xline2.Set(pos1.x + off, pos1.y, pos1.z);
    yline1.Set(pos1.x, pos1.y - off, pos1.z);
    yline2.Set(pos1.x, pos1.y + off, pos1.z);
    zline1.Set(pos1.x, pos1.y, pos1.z - off);
    zline2.Set(pos1.x, pos1.y, pos1.z + off);

    // draw position cross -----------------------
    glLineWidth(Setup->LineWidth);
    if( Setup->LineStippleFactor > 0 ){
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(Setup->LineStippleFactor,Setup->LineStipplePattern);
    } else {
        glDisable(GL_LINE_STIPPLE);
    }
    glColor4fv(Setup->LineColor);
    glBegin(GL_LINES);
        glVertex3fv(xline1);
        glVertex3fv(xline2);
        glVertex3fv(yline1);
        glVertex3fv(yline2);
        glVertex3fv(zline1);
        glVertex3fv(zline2);
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    // draw text and quotation -------------------
    CSimplePoint<float>  textpos;

    textpos.Set(xline2.x, yline2.y, zline2.z);

    if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_TEXT) ){

        QString text;
        QString unit = PQ_DISTANCE->GetUnitName();
        QString px = PQ_DISTANCE->GetRealValueText(pos1.x);
        QString py = PQ_DISTANCE->GetRealValueText(pos1.y);
        QString pz = PQ_DISTANCE->GetRealValueText(pos1.z);

        if( Setup->ShowUnit == true ){
            text = QString("(%1, %2, %3) %4").arg(px).arg(py).arg(pz).arg(unit);
        } else {
            text = QString("(%1, %2, %3)").arg(px).arg(py).arg(pz);
        }

        DrawText(textpos,text);
    }

}

//------------------------------------------------------------------------------

void CGeometryLabelObject::LabelDistance(void)
{
    CSimplePoint<float> pos1;
    CSimplePoint<float> pos2;
    CGeoDescriptor      geo1;
    CGeoDescriptor      geo2;
    bool                completed = false;

    if( Objects.count() < 1 ) return;

    CProObject* p_obj1 = static_cast<CProObject*>(Objects[0]);
    geo1 = p_obj1->GetGeoDescriptor();

    switch(geo1.GetType()){
       case EGDT_NILL:
           return;
       case EGDT_ONE_POINT:
           pos1 = geo1.GetPoint1();
           break;
       case EGDT_TWO_POINT:
       case EGDT_TWO_POINT_ORIENTED:
           pos1 = geo1.GetPoint1();
           pos2 = geo2.GetPoint2();
           completed = true;
           break;
       default:
           return;
    }

    if( completed == false ){
        if( Objects.count() < 2 ) return;
        CProObject* p_obj2 = static_cast<CProObject*>(Objects[1]);
        geo2 = p_obj2->GetGeoDescriptor();
        switch(geo2.GetType()){
           case EGDT_ONE_POINT:
               pos2 = geo2.GetPoint1();
               completed = true;
               break;
           default:
               break;
         }
    }

    if( completed == false ) return;

    // calculate distance ------------------------
    double              dis;
    dis = CGeoMeasurement::GetDistance(pos1,pos2);

    // draw distance -----------------------------
    glLineWidth(Setup->LineWidth);
    if( Setup->LineStippleFactor > 0 ){
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(Setup->LineStippleFactor,Setup->LineStipplePattern);
    } else {
        glDisable(GL_LINE_STIPPLE);
    }
    glColor4fv(Setup->LineColor.ForGL());
    glBegin(GL_LINES);
        glVertex3fv(pos1);
        glVertex3fv(pos2);
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    // draw text and quotation -------------------
    CSimplePoint<float>  textpos;
    CSimplePoint<float>  pd;
    CSimplePoint<float>  pm;

    pd = pos2 - pos1;
    pm = pos1 + pd*0.5;

    if( dis == 0.0 ) return;

    if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_RELATIVE_TEXT_POS) ){
        textpos = Offset + pm;
    } else {
        textpos = Offset;
    }

    if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_TEXT) ){
        QString text = PQ_DISTANCE->GetRealValueText(dis);

        if( Setup->ShowUnit == true ){
           text += " " + PQ_DISTANCE->GetUnitName();
           }

        DrawText(textpos,text);
    }

    DrawQuotationLine(pm,textpos);
}

//------------------------------------------------------------------------------

void CGeometryLabelObject::LabelAngle(void)
{
    CSimplePoint<float> pos1;
    CSimplePoint<float> pos2;
    CSimplePoint<float> pos3;
    CGeoDescriptor      geo1;
    CGeoDescriptor      geo2;
    CGeoDescriptor      geo3;
    bool                completed = false;

    if( Objects.count() < 1 ) return;

    CProObject* p_obj1 = static_cast<CProObject*>(Objects[0]);
    geo1 = p_obj1->GetGeoDescriptor();

    switch(geo1.GetType()){
       case EGDT_NILL:
           return;
       case EGDT_ONE_POINT:
           pos1 = geo1.GetPoint1();
           break;
       case EGDT_TWO_POINT:
       case EGDT_TWO_POINT_ORIENTED:
           pos1 = geo1.GetPoint1();
           pos2 = geo2.GetPoint2();
           break;
       default:
           return;
    }

    if( completed == false ){
        if( Objects.count() < 2 ) return;
        CProObject* p_obj2 = static_cast<CProObject*>(Objects[1]);
        geo2 = p_obj2->GetGeoDescriptor();
        switch(geo2.GetType()){
           case EGDT_ONE_POINT:
               pos2 = geo2.GetPoint1();
               break;
           default:
               break;
         }
    }

    if( completed == false ){
        if( Objects.count() < 3 ) return;
        CProObject* p_obj3 = static_cast<CProObject*>(Objects[2]);
        geo3 = p_obj3->GetGeoDescriptor();
        switch(geo3.GetType()){
           case EGDT_ONE_POINT:
               pos3 = geo3.GetPoint1();
               completed = true;
               break;
           default:
               break;
         }
    }

    if( completed == false ) return;

    // calculate angle ---------------------------
    double              angle;
    angle = CGeoMeasurement::GetAngle(pos1,pos2,pos3);

    // arc positions -----------------------------
    CArc                    arc;
    CSimplePoint<GLfloat>   v1;
    CSimplePoint<GLfloat>   v2;
    CSimplePoint<GLfloat>   v3;
    CSimplePoint<GLfloat>   str;

    // draw angle --------------------------------
    v2 = pos2;
    glLineWidth(Setup->LineWidth);
    if( Setup->LineStippleFactor > 0 ){
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(Setup->LineStippleFactor,Setup->LineStipplePattern);
    } else {
        glDisable(GL_LINE_STIPPLE);
    }
    glColor4fv(Setup->LineColor.ForGL());
    if(Size(pos1-pos2) > Size(pos3-pos2)) {
        v1 = pos2 + Norm(pos1-pos2)*Size(pos3-pos2)*Setup->AngleArcScope;
        v3 = pos2 + Norm(pos3-pos2)*Size(pos3-pos2)*Setup->AngleArcScope;
        arc.Draw(v3,v2,v1,angle,str);
        glBegin(GL_LINE_STRIP);
        if(Size(pos3-pos2) > Size(v3-v2)) {
            glVertex3fv(pos3);
        } else {
            glVertex3fv(v3);
        }
            glVertex3fv(v2);
        if(Size(pos1-pos2) > Size(v1-v2)) {
            glVertex3fv(pos1);
        } else {
            glVertex3fv(v1);
        }
        glEnd();
    } else {
        v1 = pos2 + Norm(pos1-pos2)*Size(pos1-pos2)*Setup->AngleArcScope;
        v3 = pos2 + Norm(pos3-pos2)*Size(pos1-pos2)*Setup->AngleArcScope;
        arc.Draw(v3,v2,v1,angle,str);
        glBegin(GL_LINE_STRIP);
        if(Size(pos1 - pos2) > Size(v1 - v2)) {
            glVertex3fv(pos1);
        } else {
            glVertex3fv(v1);
        }
            glVertex3fv(v2);
        if(Size(pos3 - pos2) > Size(v3 - v2)) {
            glVertex3fv(pos3);
        } else {
            glVertex3fv(v3);
        }
        glEnd();
    }
    glDisable(GL_LINE_STIPPLE);

    // draw text and quotation -------------------
    CSimplePoint<float>  textpos;

    if( angle == 0.0 ) return;

    if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_RELATIVE_TEXT_POS) ){
        textpos = Offset + str;
    } else {
        textpos = Offset;
    }

    if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_TEXT) ){
        QString text = PQ_ANGLE->GetRealValueText(angle);

        if( Setup->ShowUnit == true ){
           text += " " + PQ_ANGLE->GetUnitName();
           }

        DrawText(textpos,text);
    }

    DrawQuotationLine(str,textpos);
}

//------------------------------------------------------------------------------

void CGeometryLabelObject::LabelDihedral(void)
{
    CSimplePoint<float> pos1;
    CSimplePoint<float> pos2;
    CSimplePoint<float> pos3;
    CSimplePoint<float> pos4;
    CGeoDescriptor      geo1;
    CGeoDescriptor      geo2;
    CGeoDescriptor      geo3;
    CGeoDescriptor      geo4;
    bool                completed = false;

    if( Objects.count() < 1 ) return;

    CProObject* p_obj1 = static_cast<CProObject*>(Objects[0]);
    geo1 = p_obj1->GetGeoDescriptor();

    switch(geo1.GetType()){
       case EGDT_NILL:
           return;
       case EGDT_ONE_POINT:
           pos1 = geo1.GetPoint1();
           break;
       case EGDT_TWO_POINT:
       case EGDT_TWO_POINT_ORIENTED:
           pos1 = geo1.GetPoint1();
           pos2 = geo2.GetPoint2();
           break;
       default:
           return;
    }

    if( completed == false ){
        if( Objects.count() < 2 ) return;
        CProObject* p_obj2 = static_cast<CProObject*>(Objects[1]);
        geo2 = p_obj2->GetGeoDescriptor();
        switch(geo2.GetType()){
           case EGDT_ONE_POINT:
               pos2 = geo2.GetPoint1();
               break;
           default:
               break;
         }
    }

    if( completed == false ){
        if( Objects.count() < 3 ) return;
        CProObject* p_obj3 = static_cast<CProObject*>(Objects[2]);
        geo3 = p_obj3->GetGeoDescriptor();
        switch(geo3.GetType()){
           case EGDT_ONE_POINT:
               pos3 = geo3.GetPoint1();
               break;
           default:
               break;
         }
    }

    if( completed == false ){
        if( Objects.count() < 4 ) return;
        CProObject* p_obj4 = static_cast<CProObject*>(Objects[3]);
        geo4 = p_obj4->GetGeoDescriptor();
        switch(geo4.GetType()){
           case EGDT_ONE_POINT:
               pos4 = geo4.GetPoint1();
               completed = true;
               break;
           default:
               break;
         }
    }

    if( completed == false ) return;

    // calculate dihed ---------------------------
    double              dihed;
    dihed = CGeoMeasurement::GetTorsion(pos1,pos2,pos3,pos4);

    // draw dihed --------------------------------
    glLineWidth(Setup->LineWidth);
    glColor4fv(Setup->LineColor.ForGL());
    if( Setup->LineStippleFactor > 0 ){
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(Setup->LineStippleFactor,Setup->LineStipplePattern);
    } else {
        glDisable(GL_LINE_STIPPLE);
    }

    CSimplePoint<float> mp = (pos2+pos3)*0.5;
    CSimplePoint<float> v1,v2,c1,c2;
    v1 = pos1-pos2;
    v2 = pos3-pos2;
    c1 = CrossDot(v1,v2);
    c2 = CrossDot(v2,c1);

    CSimplePoint<float> s1 = Norm(c2)+mp;

    v1 = pos4-pos3;
    v2 = pos2-pos3;
    c1 = CrossDot(v1,v2);
    c2 = CrossDot(v2,c1);

    CSimplePoint<float> s2 = Norm(c2)+mp;

    glBegin(GL_LINES);
        glVertex3fv(pos1);
        glVertex3fv(pos2);
        glVertex3fv(pos2);
        glVertex3fv(pos3);
        glVertex3fv(pos3);
        glVertex3fv(pos4);
        glVertex3fv(mp);
        glVertex3fv(s1);
        glVertex3fv(pos1);
        glVertex3fv(s1);
        glVertex3fv(mp);
        glVertex3fv(s2);
        glVertex3fv(pos4);
        glVertex3fv(s2);
    glEnd();

    float angle = CGeoMeasurement::GetAngle(s1,mp,s2);

    CArc arc;
    CSimplePoint<float>  pm;
    arc.Draw(s1,mp,s2,angle,pm);

    glDisable(GL_LINE_STIPPLE);

    // draw text and quotation -------------------
    CSimplePoint<float> textpos;
    if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_RELATIVE_TEXT_POS) ){
        textpos = Offset + pm;
    } else {
        textpos = Offset;
    }

    if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_TEXT) ){
        QString text = PQ_ANGLE->GetRealValueText(dihed);

        if( Setup->ShowUnit == true ){
           text += " " + PQ_ANGLE->GetUnitName();
           }

        DrawText(textpos,text);
    }

    DrawQuotationLine(pm,textpos);
}

//------------------------------------------------------------------------------

void CGeometryLabelObject::DrawText(const CSimplePoint<float>& pos,const QString& label)
{
    glPushMatrix();
        if( IsFlagSet(EPOF_SELECTED) ) {
            glColor4fv(ColorsList.SelectionMaterial.Color);
        } else {
            glColor4fv(Setup->LabelColor);
        }
        glTranslatef(pos.x,pos.y,pos.z);

        // the text should be faced to an user
        ApplyFaceTransformation();

        // rotate FTGL to our coordinate system
        glRotatef(90,1,0,0);
        glRotatef(90,0,1,0);
        glScalef(0.01,0.01,0.01);

        if( Setup->DepthTest ){
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        Setup->FTGLFont.Render(label.toStdWString().c_str());
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
}

//------------------------------------------------------------------------------

void CGeometryLabelObject::DrawQuotationLine(CSimplePoint<float>& p1,CSimplePoint<float>& p2)
{
    if( IsFlagSet<EStandardModelObjectFlag>(ESMOF_SHOW_QUOTATION) ){
        if( Setup->QuotationStippleFactor > 0 ){
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(Setup->QuotationStippleFactor,Setup->QuotationStipplePattern);
        } else {
            glDisable(GL_LINE_STIPPLE);
        }
        glLineWidth(Setup->QuotationLineWidth);
        glColor4fv(Setup->QuotationColor.ForGL());
        glBegin(GL_LINES);
           glVertex3fv(p1);
           glVertex3fv(p2);
        glEnd();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeometryLabelObject::GetObjectMetricsPosition(CObjMetrics& metrics)
{
    CSimplePoint<float> pos1;
    CGeoDescriptor      geo1;
    bool                completed = false;

    if( Objects.count() < 1 ) return;

    CProObject* p_obj1 = static_cast<CProObject*>(Objects[0]);
    geo1 = p_obj1->GetGeoDescriptor();

    switch(geo1.GetType()){
       case EGDT_NILL:
           return;
       case EGDT_ONE_POINT:
           pos1 = geo1.GetPoint1();
           completed = true;
           break;
       default:
           return;
    }

    if( completed == false ) return;

    metrics.CompareWith(pos1);
}

//------------------------------------------------------------------------------

void CGeometryLabelObject::GetObjectMetricsDistance(CObjMetrics& metrics)
{
    CSimplePoint<float> pos1;
    CSimplePoint<float> pos2;
    CGeoDescriptor      geo1;
    CGeoDescriptor      geo2;
    bool                completed = false;

    if( Objects.count() < 1 ) return;

    CProObject* p_obj1 = static_cast<CProObject*>(Objects[0]);
    geo1 = p_obj1->GetGeoDescriptor();

    switch(geo1.GetType()){
       case EGDT_NILL:
           return;
       case EGDT_ONE_POINT:
           pos1 = geo1.GetPoint1();
           break;
       case EGDT_TWO_POINT:
       case EGDT_TWO_POINT_ORIENTED:
           pos1 = geo1.GetPoint1();
           pos2 = geo2.GetPoint2();
           completed = true;
           break;
       default:
           return;
    }

    if( completed == false ){
        if( Objects.count() < 2 ) return;
        CProObject* p_obj2 = static_cast<CProObject*>(Objects[1]);
        geo2 = p_obj2->GetGeoDescriptor();
        switch(geo2.GetType()){
           case EGDT_ONE_POINT:
               pos2 = geo2.GetPoint1();
               completed = true;
               break;
           default:
               break;
         }
    }

    if( completed == false ) return;

    metrics.CompareWith(pos1);
    metrics.CompareWith(pos2);
}

//------------------------------------------------------------------------------

void CGeometryLabelObject::GetObjectMetricsAngle(CObjMetrics& metrics)
{
    CSimplePoint<float> pos1;
    CSimplePoint<float> pos2;
    CSimplePoint<float> pos3;
    CGeoDescriptor      geo1;
    CGeoDescriptor      geo2;
    CGeoDescriptor      geo3;
    bool                completed = false;

    if( Objects.count() < 1 ) return;

    CProObject* p_obj1 = static_cast<CProObject*>(Objects[0]);
    geo1 = p_obj1->GetGeoDescriptor();

    switch(geo1.GetType()){
       case EGDT_NILL:
           return;
       case EGDT_ONE_POINT:
           pos1 = geo1.GetPoint1();
           break;
       case EGDT_TWO_POINT:
       case EGDT_TWO_POINT_ORIENTED:
           pos1 = geo1.GetPoint1();
           pos2 = geo2.GetPoint2();
           break;
       default:
           return;
    }

    if( completed == false ){
        if( Objects.count() < 2 ) return;
        CProObject* p_obj2 = static_cast<CProObject*>(Objects[1]);
        geo2 = p_obj2->GetGeoDescriptor();
        switch(geo2.GetType()){
           case EGDT_ONE_POINT:
               pos2 = geo2.GetPoint1();
               break;
           default:
               break;
         }
    }

    if( completed == false ){
        if( Objects.count() < 3 ) return;
        CProObject* p_obj3 = static_cast<CProObject*>(Objects[2]);
        geo3 = p_obj3->GetGeoDescriptor();
        switch(geo3.GetType()){
           case EGDT_ONE_POINT:
               pos3 = geo3.GetPoint1();
               completed = true;
               break;
           default:
               break;
         }
    }

    if( completed == false ) return;

    metrics.CompareWith(pos1);
    metrics.CompareWith(pos2);
    metrics.CompareWith(pos3);
}

//------------------------------------------------------------------------------

void CGeometryLabelObject::GetObjectMetricsDihedral(CObjMetrics& metrics)
{
    CSimplePoint<float> pos1;
    CSimplePoint<float> pos2;
    CSimplePoint<float> pos3;
    CSimplePoint<float> pos4;
    CGeoDescriptor      geo1;
    CGeoDescriptor      geo2;
    CGeoDescriptor      geo3;
    CGeoDescriptor      geo4;
    bool                completed = false;

    if( Objects.count() < 1 ) return;

    CProObject* p_obj1 = static_cast<CProObject*>(Objects[0]);
    geo1 = p_obj1->GetGeoDescriptor();

    switch(geo1.GetType()){
       case EGDT_NILL:
           return;
       case EGDT_ONE_POINT:
           pos1 = geo1.GetPoint1();
           break;
       case EGDT_TWO_POINT:
       case EGDT_TWO_POINT_ORIENTED:
           pos1 = geo1.GetPoint1();
           pos2 = geo2.GetPoint2();
           break;
       default:
           return;
    }

    if( completed == false ){
        if( Objects.count() < 2 ) return;
        CProObject* p_obj2 = static_cast<CProObject*>(Objects[1]);
        geo2 = p_obj2->GetGeoDescriptor();
        switch(geo2.GetType()){
           case EGDT_ONE_POINT:
               pos2 = geo2.GetPoint1();
               break;
           default:
               break;
         }
    }

    if( completed == false ){
        if( Objects.count() < 3 ) return;
        CProObject* p_obj3 = static_cast<CProObject*>(Objects[2]);
        geo3 = p_obj3->GetGeoDescriptor();
        switch(geo3.GetType()){
           case EGDT_ONE_POINT:
               pos3 = geo3.GetPoint1();
               break;
           default:
               break;
         }
    }

    if( completed == false ){
        if( Objects.count() < 4 ) return;
        CProObject* p_obj4 = static_cast<CProObject*>(Objects[3]);
        geo4 = p_obj4->GetGeoDescriptor();
        switch(geo4.GetType()){
           case EGDT_ONE_POINT:
               pos4 = geo4.GetPoint1();
               completed = true;
               break;
           default:
               break;
         }
    }

    if( completed == false ) return;

    metrics.CompareWith(pos1);
    metrics.CompareWith(pos2);
    metrics.CompareWith(pos3);
    metrics.CompareWith(pos4);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeometryLabelObject::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }
    // load object info -----------------------------
    CGraphicsObject::LoadData(p_ele);

    // specific data -----------------------------
    p_ele->GetAttribute<EGeometryLabelType>("type",Type);
    p_ele->GetAttribute("tx",Offset.x);
    p_ele->GetAttribute("ty",Offset.y);
    p_ele->GetAttribute("tz",Offset.z);
}

//------------------------------------------------------------------------------

void CGeometryLabelObject::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // core data ---------------------------------
    CGraphicsObject::SaveData(p_ele);

    // specific data -----------------------------
    p_ele->SetAttribute("type",Type);
    p_ele->SetAttribute("tx",Offset.x);
    p_ele->SetAttribute("ty",Offset.y);
    p_ele->SetAttribute("tz",Offset.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

