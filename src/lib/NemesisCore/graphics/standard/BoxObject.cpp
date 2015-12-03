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

#include <NemesisCoreModule.hpp>
#include <PluginObject.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <GraphicsObjectList.hpp>
#include <HistoryNode.hpp>

#include <Project.hpp>
#include <ProjectList.hpp>
#include <Structure.hpp>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GLSelection.hpp>
#include <ElementColorsList.hpp>
#include <Transformation.hpp>
#include <GraphicsView.hpp>

#include <BoxObject.hpp>
#include <BoxSetup.hpp>
#include <BoxObjectHistory.hpp>

#include "BoxObject.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* BoxObjectCB(void* p_data);

CExtUUID        BoxObjectID(
                    "{BOX_OBJECT:132cddc8-556c-46d7-8159-337ff4476366}",
                    "Box");

CPluginObject   BoxObject(&NemesisCorePlugin,
                    BoxObjectID,
                    GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/objects/Box.svg",
                    BoxObjectCB
                    );

// -----------------------------------------------------------------------------

QObject* BoxObjectCB(void* p_data)
{
    return(new CBoxObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBoxObject::CBoxObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&BoxObject,p_gl)
{
    Setup = NULL;
    PositionObject = NULL;
    Dimensions.x = 5.0;
    Dimensions.y = 5.0;
    Dimensions.z = 5.0;
    // do not use (0,0,1) it is not visible to user for default scene orientation
    Direction.x = -1.0;
    Direction.y = 0.0;
    Direction.z = 0.0;
    DirectionObject = NULL;
    Rotation = 0.0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBoxObject::CopyBoxObject(CBoxObject* p_box)
{
    // We can copy on box to another
    p_box->Setup = Setup;
    p_box->PositionObject = PositionObject;
    p_box->Position.x = Position.x;
    p_box->Position.y = Position.y;
    p_box->Position.z = Position.z;

    p_box->Dimensions.x = Dimensions.x;
    p_box->Dimensions.y = Dimensions.y;
    p_box->Dimensions.z = Dimensions.z;

    // do not use (0,0,1) it is not visible to user for default scene orientation
    p_box->DirectionObject = DirectionObject;
    p_box->Direction.x = Direction.x;
    p_box->Direction.x = Direction.x;
    p_box->Direction.x = Direction.x;
    p_box->Sphere = Sphere;
    p_box->Cylinder = Cylinder;
    p_box->Rotation = Rotation;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
bool CBoxObject::SetPositionWH(const CPoint& pos)
{
    if( PositionObject != NULL ) return(false);
    if( Position == pos ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("change position"));
    if( p_history == NULL ) return (false);

    SetPosition(pos,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CBoxObject::SetPositionWH(CProObject* p_posobj)
{
    if( PositionObject == p_posobj ) return(false);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("change position"));
    if( p_history == NULL ) return (false);

    SetPosition(p_posobj,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CBoxObject::SetDirectionWH(const CPoint& direction)
{
    if( DirectionObject != NULL ) return(false);
    if( Direction == direction ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("box direction"));
    if( p_history == NULL ) return (false);

    SetDirection(direction,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CBoxObject::SetDirectionWH(CProObject* p_direction)
{
    if( DirectionObject == p_direction ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("box direction object"));
    if( p_history == NULL ) return (false);

    SetDirection(p_direction,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CBoxObject::SetDimensionsWH(const CPoint& dim)
{
    if( Dimensions == dim ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("change dimensions"));
    if( p_history == NULL ) return (false);

    SetDimensions(dim,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CBoxObject::SetRotationWH(double rotation)
{
    if( Rotation == rotation ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("box rotation"));
    if( p_history == NULL ) return (false);

    SetRotation(rotation,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

void CBoxObject::SetPosition(const CPoint& pos,CHistoryNode* p_history)
{
    if( PositionObject != NULL ) return;
    if( Position == pos ) return;

    if( p_history ){
        CBoxObjectChangePositionHI* p_item = new CBoxObjectChangePositionHI(this,pos);
        p_history->Register(p_item);
    }

    Position = pos;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CBoxObject::SetPosition(CProObject* p_posobj,CHistoryNode* p_history)
{
    if( PositionObject == p_posobj ) return;

    if( p_history ){
        CBoxObjectChangePositionObjHI* p_item = new CBoxObjectChangePositionObjHI(this,p_posobj);
        p_history->Register(p_item);
    }

    if( PositionObject ){
        PositionObject->UnregisterObject(this);
    }

    PositionObject = p_posobj;

    if( PositionObject ){
        PositionObject->RegisterObject(this);
    }

    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CBoxObject::SetDimensions(const CPoint& dim,CHistoryNode* p_history)
{
    if( Dimensions == dim ) return;

    if( p_history ){
        CBoxObjectChangeDimensionsHI* p_item = new CBoxObjectChangeDimensionsHI(this,dim);
        p_history->Register(p_item);
    }

    Dimensions = dim;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CBoxObject::SetDirection(const CPoint& direction,CHistoryNode* p_history)
{
    if( DirectionObject != NULL ) return;
    if( Direction == direction ) return;

    if( p_history ){
        CHistoryItem* p_item = new CBoxObjectChangeDirectionHI(this,direction);
        p_history->Register(p_item);
    }

    Direction = direction;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CBoxObject::SetDirection(CProObject* p_direction,CHistoryNode* p_history)
{
    if( DirectionObject == p_direction ) return;

    if( p_history ){
        CHistoryItem* p_item = new CBoxObjectChangeDirectionObjHI(this,p_direction);
        p_history->Register(p_item);
    }

    if( DirectionObject ){
        DirectionObject->UnregisterObject(this);
    }

    DirectionObject = p_direction;

    if( DirectionObject ){
        DirectionObject->RegisterObject(this);
    }

    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CBoxObject::SetRotation(double rotation,CHistoryNode* p_history)
{
    if( Rotation == rotation ) return;

    if( p_history ){
        CHistoryItem* p_item = new CBoxObjectChangeRotationHI(this,rotation);
        p_history->Register(p_item);
    }

    Rotation = rotation;
    emit OnStatusChanged(ESC_OTHER);
}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGeoDescriptor CBoxObject::GetGeoDescriptor(void)
{
    CGeoDescriptor geo;

    if( IsFlagSet<EBoxObjectFlag>(EBCBOF_ORIGIN_IN_CENTER) ){
        geo.SetType(EGDT_ONE_POINT);
        geo.SetPoint1(GetPosition());
        return(geo);
    }

    geo.SetType(EGDT_ONE_POINT);
    geo.SetPoint1(GetPosition()+GetDimensions()/2.0);
    return(geo);
}

//------------------------------------------------------------------------------

CPoint CBoxObject::GetPosition(void)
{
    if( PositionObject ){
        CGeoDescriptor geo = PositionObject->GetGeoDescriptor();
        if( geo.GetType() == EGDT_ONE_POINT ){
            return(geo.GetPoint1());
        } else {
            return( CPoint() );
        }

    }
    return(Position);
}

//------------------------------------------------------------------------------

CProObject* CBoxObject::GetPositionObject(void)
{
    return(PositionObject);
}

//------------------------------------------------------------------------------

CPoint CBoxObject::GetDirection(void)
{
    if( DirectionObject ){
        CPoint pos = GetPosition();
        CGeoDescriptor descrip = DirectionObject->GetGeoDescriptor();
        return(descrip.GetPoint1()-pos);
    } else {
        return(Direction);
    }
}

//------------------------------------------------------------------------------

CProObject* CBoxObject::GetDirectionObject(void)
{
    return(DirectionObject);
}

//------------------------------------------------------------------------------

const CPoint& CBoxObject::GetDimensions(void)
{
    return(Dimensions);
}

//------------------------------------------------------------------------------

double CBoxObject::GetRotation(void)
{
    return(Rotation);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBoxObject::StartManipulation(CHistoryNode* p_history)
{
    if( p_history == NULL ) return;

    CHistoryItem* p_item = new CBoxObjectBackupGeoHI(this);
    p_history->Register(p_item);

    // use delayed event notification
    StartEvent();
}

//------------------------------------------------------------------------------

void CBoxObject::Move(const CPoint& dmov,CGraphicsView* p_view)
{
    CPoint _dmov = dmov;

    CTransformation coord = p_view->GetTrans();
    coord.Invert();
    coord.Apply(_dmov);

    Position += _dmov;

    // use delayed event notification
    RaiseEvent();
}

//------------------------------------------------------------------------------

void CBoxObject::Rotate(const CPoint& drot,CGraphicsView* p_view)
{
        if( DirectionObject != NULL ) return;

        CTransformation trans;
        CTransformation coord = p_view->GetTrans();
        trans *= coord;
        trans.Rotate(drot);
        coord.Invert();
        trans *= coord;

        trans.Apply(Direction);

        // use delayed event notification
        RaiseEvent();
}

//------------------------------------------------------------------------------

void CBoxObject::EndManipulation(void)
{
    // end delayed event notification
    EndEvent();
}

//------------------------------------------------------------------------------

void CBoxObject::RespondToEvent(void)
{
    // respond to delayed notification
    emit OnStatusChanged(ESC_OTHER);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBoxObject::Draw(void)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    Setup = GetSetup<CBoxSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    GLLoadObject(this);
    DrawBox();

    if( IsFlagSet<EBoxObjectFlag>(EBCBOF_SHOW_CENTER) ){
        DrawCenter();
    }
}

//------------------------------------------------------------------------------

void CBoxObject::SetupChanged(void)
{
    Setup = GetSetup<CBoxSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }
    Sphere.SetTessellationQuality(Setup->SphereTesselationQuality);
    Cylinder.SetTessellationQuality(Setup->TubeTesselationQuality);

    CGraphicsObject::SetupChanged();
}

//------------------------------------------------------------------------------

void CBoxObject::DrawBox(void)
{
    CPoint dim = GetDimensions();

    CPoint l1;
    CPoint l2 = CPoint(dim.x,0,0);
    CPoint l3 = CPoint(0,dim.y,0);
    CPoint l4 = l2+l3;

    CPoint h1 = CPoint(0,0,dim.z);
    CPoint h2 = l2+h1;
    CPoint h3 = l3+h1;
    CPoint h4 = l2+l3+h1;

    CPoint shift;

    if( IsFlagSet<EBoxObjectFlag>(EBCBOF_ORIGIN_IN_CENTER) ){
        shift = GetPosition()-dim/2.0;
    } else {
        shift = GetPosition();
    }
    // Jirka
    CPoint dir = GetDirection();

    float cylH = ::Size(dir);
    float rotAngle;

    if (cylH == 0) {
        rotAngle = 0;
    } else {
        rotAngle = acos(dir.z/cylH)*180.0/3.14; // konstanta je 180/PI
    }

    glTranslated(shift.x,shift.y,shift.z);
    if( (dir.y != 0) || (dir.x != 0) ) {
        glRotatef(rotAngle,-dir.y,dir.x, 0);
    } else {
        if( rotAngle > 90.0 ) glScalef(1,1,-1);
    }

    glScaled(1,1,1);
    glRotatef(Rotation,dim.x,dim.y,dim.z);

    // Jirka

    l1 += shift;
    l2 += shift;
    l3 += shift;
    l4 += shift;

    h1 += shift;
    h2 += shift;
    h3 += shift;
    h4 += shift;

    if( Setup->UseTubes ){
        SetInitialColorScheme();
    }

    if( IsFlagSet(EPOF_SELECTED) ){
        glColor4fv(ColorsList.SelectionMaterial.Color);
    } else {
        glColor4fv(Setup->LineColor);
    }

    if( Setup->UseTubes == false ){
        // draw box
        glEnable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);

        glLineWidth(Setup->LineWidth*100);

        if( Setup->LineStippleFactor > 0 ){
            glLineStipple(Setup->LineStippleFactor,Setup->LineStipplePattern);
            glEnable(GL_LINE_STIPPLE);
        }
        glBegin(GL_LINES);
            glVertex3d(l1.x,l1.y,l1.z);
            glVertex3d(l2.x,l2.y,l2.z);
            glVertex3d(l1.x,l1.y,l1.z);
            glVertex3d(l3.x,l3.y,l3.z);
            glVertex3d(l2.x,l2.y,l2.z);
            glVertex3d(l4.x,l4.y,l4.z);
            glVertex3d(l3.x,l3.y,l3.z);
            glVertex3d(l4.x,l4.y,l4.z);

            glVertex3d(h1.x,h1.y,h1.z);
            glVertex3d(h2.x,h2.y,h2.z);
            glVertex3d(h1.x,h1.y,h1.z);
            glVertex3d(h3.x,h3.y,h3.z);
            glVertex3d(h2.x,h2.y,h2.z);
            glVertex3d(h4.x,h4.y,h4.z);
            glVertex3d(h3.x,h3.y,h3.z);
            glVertex3d(h4.x,h4.y,h4.z);

            glVertex3d(l1.x,l1.y,l1.z);
            glVertex3d(h1.x,h1.y,h1.z);
            glVertex3d(l2.x,l2.y,l2.z);
            glVertex3d(h2.x,h2.y,h2.z);
            glVertex3d(l3.x,l3.y,l3.z);
            glVertex3d(h3.x,h3.y,h3.z);
            glVertex3d(l4.x,l4.y,l4.z);
            glVertex3d(h4.x,h4.y,h4.z);
        glEnd();

        glDisable(GL_LINE_STIPPLE);
    } else {
        DrawTube(l1,l2);
        DrawTube(l1,l3);
        DrawTube(l2,l4);
        DrawTube(l3,l4);
        DrawTube(h1,h2);
        DrawTube(h1,h3);
        DrawTube(h2,h4);
        DrawTube(h3,h4);
        DrawTube(l1,h1);
        DrawTube(l2,h2);
        DrawTube(l3,h3);
        DrawTube(l4,h4);

        DrawSphere(l1);
        DrawSphere(l2);
        DrawSphere(l3);
        DrawSphere(l4);
        DrawSphere(h1);
        DrawSphere(h2);
        DrawSphere(h3);
        DrawSphere(h4);
    }

}

//------------------------------------------------------------------------------

void CBoxObject::DrawTube(CPoint& p1,CPoint& p2)
{
    CSimplePoint<float>     pd;
    float                   rotAngle;
    float                   cylH;

    pd = p2 - p1;
    cylH = Size(pd);

    glPushMatrix();
        if (cylH == 0) {
            rotAngle = 0;
        } else {
            rotAngle = acos(pd.z/cylH)*180.0/3.14;
        }
        glTranslatef (p1.x,p1.y,p1.z);
        if( (pd.y != 0) || (pd.x != 0) ) {
            glRotatef(rotAngle,-pd.y,pd.x, 0);
        } else {
            if( rotAngle > 90.0 ) glScalef(1,1,-1);
        }
        Cylinder.Draw(Setup->LineWidth,cylH);
    glPopMatrix();
}

//------------------------------------------------------------------------------

void CBoxObject::DrawSphere(CPoint& p1)
{
    glPushMatrix();
        glTranslatef (p1.x,p1.y,p1.z);
        Sphere.Draw(Setup->LineWidth);
    glPopMatrix();
}

//---------------------------------------------------------------------------

void CBoxObject::SetInitialColorScheme(void)
{
    // draw individual objects
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);

    // set global color enviroment
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,ColorsList.GlobalMaterial.Ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,ColorsList.GlobalMaterial.Diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,ColorsList.GlobalMaterial.Emission);

    float shininess[1];
    shininess[0] = ColorsList.GlobalMaterial.Shininess;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS ,shininess);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,ColorsList.GlobalMaterial.Specular);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK,ColorsList.MaterialMode);
}

//------------------------------------------------------------------------------

void CBoxObject::DrawCenter(void)
{
    CPoint center;

    if( IsFlagSet<EBoxObjectFlag>(EBCBOF_ORIGIN_IN_CENTER) ){
        center = GetPosition();
    } else {
        center = GetPosition()  + GetDimensions()/2.0;
    }

    glLineWidth(Setup->PointWidth);

    if( IsFlagSet(EPOF_SELECTED) ){
        glColor4fv(ColorsList.SelectionMaterial.Color);
    } else {
        glColor4fv(Setup->PointColor);
    }

    // possition cross line points ---------------
    CSimplePoint<float>  xline1;
    CSimplePoint<float>  xline2;
    CSimplePoint<float>  yline1;
    CSimplePoint<float>  yline2;
    CSimplePoint<float>  zline1;
    CSimplePoint<float>  zline2;
    float                off = Setup->PointSize;

    // define line points ------------------------
    xline1.Set(center.x - off, center.y, center.z);
    xline2.Set(center.x + off, center.y, center.z);
    yline1.Set(center.x, center.y - off, center.z);
    yline2.Set(center.x, center.y + off, center.z);
    zline1.Set(center.x, center.y, center.z - off);
    zline2.Set(center.x, center.y, center.z + off);

    // draw position cross -----------------------
    glLineWidth(Setup->PointWidth);
    if( IsFlagSet(EPOF_SELECTED) ){
        glColor4fv(ColorsList.SelectionMaterial.Color);
    } else {
        glColor4fv(Setup->PointColor);
    }
    glBegin(GL_LINES);
        glVertex3fv(xline1);
        glVertex3fv(xline2);
        glVertex3fv(yline1);
        glVertex3fv(yline2);
        glVertex3fv(zline1);
        glVertex3fv(zline2);
    glEnd();
}

//------------------------------------------------------------------------------

void CBoxObject::GetObjectMetrics(CObjMetrics& metrics)
{
    // Jirka
    CTransformation trans;
    CPoint pos = GetPosition();
    CPoint dir = GetDirection();

    float cylH = ::Size(dir);
    float rotAngle;

    if (cylH == 0) {
        rotAngle = 0;
    } else {
        rotAngle = acos(dir.z/cylH)*180.0/3.14; // konstanta je 180/PI
    }

    trans.Translate(pos.x,pos.y,pos.z);
    if( (dir.y != 0) || (dir.x != 0) ) {
        trans.Rotate(rotAngle,-dir.y,dir.x, 0);
    } else {
        if( rotAngle > 90.0 ) trans.Scale(1,1,-1);
    }

//    trans.Scale(Size,Size,1);
    trans.Rotate(Rotation,0,0,1);
    // Jirka

    CPoint dim = GetDimensions();

    CPoint l1;
    CPoint l2 = CPoint(dim.x,0,0);
    CPoint l3 = CPoint(0,dim.y,0);
    CPoint l4 = l2+l3;

    CPoint h1 = CPoint(0,0,dim.z);
    CPoint h2 = l2+h1;
    CPoint h3 = l3+h1;
    CPoint h4 = l2+l3+h1;

    CPoint shift;

    if( IsFlagSet<EBoxObjectFlag>(EBCBOF_ORIGIN_IN_CENTER) ){
        shift = GetPosition()-dim/2.0;
    } else {
        shift = GetPosition();
    }

    l1 += shift;
    l2 += shift;
    l3 += shift;
    l4 += shift;

    h1 += shift;
    h2 += shift;
    h3 += shift;
    h4 += shift;

    metrics.CompareWith(l1);
    metrics.CompareWith(l2);
    metrics.CompareWith(l3);
    metrics.CompareWith(l4);

    metrics.CompareWith(h1);
    metrics.CompareWith(h2);
    metrics.CompareWith(h3);
    metrics.CompareWith(h4);
}

//------------------------------------------------------------------------------

void CBoxObject::RemoveFromRegistered(CProObject* p_object,CHistoryNode* p_history)
{
    if( p_object == NULL ) return;

    if( p_object == PositionObject ) {
        CGeoDescriptor descr = PositionObject->GetGeoDescriptor();
        SetPosition(NULL,p_history);
        SetPosition(descr.GetPoint1(),p_history);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBoxObject::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load object info -----------------------------
    CGraphicsObject::LoadData(p_ele);

    // specific data -----------------------------
    p_ele->GetAttribute("px",Position.x);
    p_ele->GetAttribute("py",Position.y);
    p_ele->GetAttribute("pz",Position.z);

    // position object
    GetProject()->RequestToLinkObject(this,p_ele,"po");

    p_ele->GetAttribute("dx",Dimensions.x);
    p_ele->GetAttribute("dy",Dimensions.y);
    p_ele->GetAttribute("dz",Dimensions.z);
}

//------------------------------------------------------------------------------

void CBoxObject::LinkObject(const QString& role,CProObject* p_object)
{
    if( role == "po" ){
        SetPosition(p_object);
    }
}

//------------------------------------------------------------------------------

void CBoxObject::SaveData(CXMLElement* p_ele)
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

    if( PositionObject ){
        p_ele->SetAttribute("po",PositionObject->GetIndex());
    } else {
        int index = -1;
        p_ele->SetAttribute("po",index);
    }

    p_ele->SetAttribute("dx",Dimensions.x);
    p_ele->SetAttribute("dy",Dimensions.y);
    p_ele->SetAttribute("dz",Dimensions.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


