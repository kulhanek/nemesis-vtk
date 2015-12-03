// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2011 Jiri Klusak, anim4tor@mail.muni.cz
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
#include <Project.hpp>
#include <GLSelection.hpp>
#include <ElementColorsList.hpp>
#include <HistoryNode.hpp>

#include <Atom.hpp>
#include <AtomList.hpp>
#include <Structure.hpp>
#include <PrincipalAxes.hpp>

#include <Project.hpp>
#include <ProjectList.hpp>
#include <GraphicsView.hpp>

#include <NemesisCoreModule.hpp>
#include <VectorObject.hpp>
#include <VectorSetup.hpp>
#include <VectorObjectHistory.hpp>

#include "VectorObject.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* VectorObjectCB(void* p_data);

CExtUUID        VectorObjectID(
                    "{VECTOR_OBJECT:970f766f-c637-4107-8b25-2d76f558840f}",
                    "Vector");

CPluginObject   VectorObject(&NemesisCorePlugin,
                    VectorObjectID,
                    GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/objects/Vector.svg",
                    VectorObjectCB);

// -----------------------------------------------------------------------------

QObject* VectorObjectCB(void* p_data)
{
    return(new CVectorObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CVectorObject::CVectorObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&VectorObject,p_gl)
{ 
    PositionObject = NULL;
    Direction.z = 5.0;
    DirectionObject = NULL;
    Offset = 0;
    Length = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVectorObject::SetPositionWH(const CPoint& position)
{
    if( PositionObject != NULL ) return(false);
    if( Position == position ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("vector position"));
    if( p_history == NULL ) return (false);

    SetPosition(position,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CVectorObject::SetPositionWH(CProObject* p_position)
{
    if( PositionObject == p_position ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("vector position object"));
    if( p_history == NULL ) return (false);

    SetPosition(p_position,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CVectorObject::SetDirectionWH(const CPoint& direction)
{
    if( DirectionObject != NULL ) return(false);
    if( Direction == direction ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("vector direction"));
    if( p_history == NULL ) return (false);

    SetDirection(direction,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CVectorObject::SetDirectionWH(CProObject* p_direction)
{
    if( DirectionObject == p_direction ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("vector direction object"));
    if( p_history == NULL ) return (false);

    SetDirection(p_direction,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CVectorObject::SetOffsetWH(double offset)
{
    if( ! IsInFreeSizeMode() ) return(false);
    if( Offset == offset ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("vector offset"));
    if( p_history == NULL ) return (false);

    SetOffset(offset,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CVectorObject::SetLengthWH(double length)
{
    if( ! IsInFreeSizeMode() ) return(false);
    if( Length == length ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("vector length"));
    if( p_history == NULL ) return (false);

    SetLength(length,p_history);

    EndChangeWH();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CVectorObject::SetPosition(const CPoint& position,CHistoryNode* p_history)
{
    if( PositionObject != NULL ) return;
    if( Position == position ) return;

    if( p_history ){
        CVectorObjectChangePositionHI* p_item = new CVectorObjectChangePositionHI(this,position);
        p_history->Register(p_item);
    }

    Position = position;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CVectorObject::SetPosition(CProObject* p_position,CHistoryNode* p_history)
{
    if( PositionObject == p_position ) return;

    if( p_history ){
        CVectorObjectChangePositionObjHI* p_item = new CVectorObjectChangePositionObjHI(this,p_position);
        p_history->Register(p_item);
    }

    if( PositionObject ){
        PositionObject->UnregisterObject(this);
    }

    PositionObject = p_position;

    if( PositionObject ){
        PositionObject->RegisterObject(this);
    }

    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CVectorObject::SetDirection(const CPoint& direction,CHistoryNode* p_history)
{
    if( DirectionObject != NULL ) return;
    if( Direction == direction ) return;

    if( p_history ){
        CVectorObjectChangeDirectionHI* p_item = new CVectorObjectChangeDirectionHI(this,direction);
        p_history->Register(p_item);
    }

    Direction = direction;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CVectorObject::SetDirection(CProObject* p_direction,CHistoryNode* p_history)
{
    if( DirectionObject == p_direction ) return;

    if( p_history ){
        CVectorObjectChangeDirectionObjHI* p_item = new CVectorObjectChangeDirectionObjHI(this,p_direction);
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

void CVectorObject::SetOffset(double offset,CHistoryNode* p_history)
{
    if( ! IsInFreeSizeMode() ) return;
    if( Offset == offset ) return;

    if( p_history ){
        CVectorObjectChangeOffsetHI* p_item = new CVectorObjectChangeOffsetHI(this,offset);
        p_history->Register(p_item);
    }

    Offset = offset;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CVectorObject::SetLength(double length,CHistoryNode* p_history)
{
    if( ! IsInFreeSizeMode() ) return;
    if( Length == length ) return;

    if( p_history ){
        CVectorObjectChangeLengthHI* p_item = new CVectorObjectChangeLengthHI(this,length);
        p_history->Register(p_item);
    }

    Length = length;
    emit OnStatusChanged(ESC_OTHER);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPoint CVectorObject::GetPosition(void)
{
    if( PositionObject ){
        CGeoDescriptor descrip = PositionObject->GetGeoDescriptor();
        return(descrip.GetPoint1());
    } else {
        return(Position);
    }
}

//------------------------------------------------------------------------------

CProObject* CVectorObject::GetPositionObject(void)
{
    return(PositionObject);
}

//------------------------------------------------------------------------------

CPoint CVectorObject::GetDirection(void)
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

CProObject* CVectorObject::GetDirectionObject(void)
{
    return(DirectionObject);
}

//------------------------------------------------------------------------------

bool CVectorObject::IsInFreeSizeMode(void)
{
    return(IsFlagSet<EVectorObjectFlag>(EVOF_FREE_SIZE));
}

//------------------------------------------------------------------------------

double CVectorObject::GetOffset(void)
{
    if( IsInFreeSizeMode() ) return(Offset);
    return(0);
}

//------------------------------------------------------------------------------

double CVectorObject::GetLength(void)
{
    if( IsInFreeSizeMode() ) return(Length);
    return(Size(GetDirection()));
}

//------------------------------------------------------------------------------

CGeoDescriptor CVectorObject::GetGeoDescriptor(void)
{
    CGeoDescriptor descrip;

    CPoint pos1 = GetPosition();
    CPoint pos2 = GetDirection();

    descrip.SetType(EGDT_TWO_POINT_ORIENTED);
    descrip.SetPoint1(pos1);
    descrip.SetPoint2(pos1+pos2);

    return(descrip);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CVectorObject::Draw(void)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    Setup = GetSetup<CVectorSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    GLLoadObject(this);
    SetInitialColorScheme();
    DrawVector();
}

//---------------------------------------------------------------------------

void CVectorObject::SetInitialColorScheme(void)
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

//---------------------------------------------------------------------------

void CVectorObject::DrawVector(void)
{
    CSimplePoint<float>     od;
    CSimplePoint<float>     pd;
    float                   rotAngle;
    float                   cylH;

    if( IsInFreeSizeMode() ){
        od = GetPosition();
        pd = GetDirection();
        pd.Normalize();
        od -= pd*Offset;
        pd *= Length;
    } else {
        od = GetPosition();
        pd = GetDirection();
    }
    cylH = Size(pd);

    glPushMatrix();

        if (cylH == 0) {
            rotAngle = 0;
        } else {
            rotAngle = acos(pd.z/cylH)*180.0/3.14;
        }
        glTranslatef (od.x,od.y,od.z);
        if( (pd.y != 0) || (pd.x != 0) ) {
            glRotatef(rotAngle,-pd.y,pd.x, 0);
        } else {
            if( rotAngle > 90.0 ) glScalef(1,1,-1);
        }

        CColor*    sel_color = &ColorsList.SelectionMaterial.Color;

        if( Setup->AxisDifuseColor ){
            Cylinder.Draw(Setup->AxisRadius,cylH - Setup->ConeHeight,&Setup->AxisColor,&Setup->ConeColor);
        } else {
            Cylinder.Draw(Setup->AxisRadius,cylH - Setup->ConeHeight,&Setup->AxisColor,&Setup->AxisColor);
        }
        if( IsFlagSet(EPOF_SELECTED) == true ) {
            glEnable(GL_BLEND);
            Cylinder.Draw(2.0*Setup->AxisRadius,cylH - Setup->ConeHeight,sel_color,sel_color);
            glDisable(GL_BLEND);
        }

        glTranslatef(0,0,cylH - Setup->ConeHeight);

        glColor4fv(Setup->ConeColor);
        Cone.Draw(Setup->ConeRadius,0,Setup->ConeHeight);
        if( IsFlagSet(EPOF_SELECTED) == true ) {
            glEnable(GL_BLEND);
            glColor4fv(sel_color->ForGL());
            Cone.Draw(1.5*Setup->ConeRadius,0,1.5*Setup->ConeHeight);
            glDisable(GL_BLEND);
        }

    glPopMatrix();
}

//------------------------------------------------------------------------------

void CVectorObject::SetupChanged(void)
{
    Setup = GetSetup<CVectorSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    Cone.SetTessellationQuality(Setup->ConeTesselation);
    Cylinder.SetTessellationQuality(Setup->AxisTesselation);
}

//------------------------------------------------------------------------------

void CVectorObject::RemoveFromRegistered(CProObject* p_object,CHistoryNode* p_history)
{
    if( p_object == NULL ) return;

    if( p_object == PositionObject ) {
        CGeoDescriptor descr = PositionObject->GetGeoDescriptor();
        SetPosition(NULL,p_history);
        SetPosition(descr.GetPoint1(),p_history);
    }

    if( p_object == DirectionObject ) {
        CGeoDescriptor descr = DirectionObject->GetGeoDescriptor();
        SetDirection(NULL,p_history);
        SetDirection(descr.GetPoint1(),p_history);
    }
}

//------------------------------------------------------------------------------

void CVectorObject::GetObjectMetrics(CObjMetrics& metrics)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    metrics.CompareWith(GetPosition());
    metrics.CompareWith(GetPosition()+GetDirection());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CVectorObject::StartManipulation(CHistoryNode* p_history)
{
    if( p_history == NULL ) return;

    CHistoryItem* p_item = new CVectorObjectBackupGeoHI(this);
    p_history->Register(p_item);

    // use delayed event notification
    StartEvent();
}

//------------------------------------------------------------------------------

void CVectorObject::Move(const CPoint& dmov,CGraphicsView* p_view)
{
    if( PositionObject != NULL ) return;

    CPoint _dmov = dmov;

    CTransformation coord = p_view->GetTrans();
    coord.Invert();
    coord.Apply(_dmov);

    Position += _dmov;

    // use delayed event notification
    RaiseEvent();
}

//------------------------------------------------------------------------------

void CVectorObject::Rotate(const CPoint& drot,CGraphicsView* p_view)
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

void CVectorObject::EndManipulation(void)
{
    // end delayed event notification
    EndEvent();
}

//------------------------------------------------------------------------------

void CVectorObject::RespondToEvent(void)
{
    // respond to delayed notification
    emit OnStatusChanged(ESC_OTHER);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CVectorObject::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load object info --------------------------
    CGraphicsObject::LoadData(p_ele);

    // specific data -----------------------------
    p_ele->GetAttribute("px",Position.x);
    p_ele->GetAttribute("py",Position.y);
    p_ele->GetAttribute("pz",Position.z);

    // position object
    GetProject()->RequestToLinkObject(this,p_ele,"po");

    p_ele->GetAttribute("dx",Direction.x);
    p_ele->GetAttribute("dy",Direction.y);
    p_ele->GetAttribute("dz",Direction.z);

    // direction object
    GetProject()->RequestToLinkObject(this,p_ele,"do");

    p_ele->GetAttribute("of",Offset);
    p_ele->GetAttribute("le",Length);
}

//------------------------------------------------------------------------------

void CVectorObject::LinkObject(const QString& role,CProObject* p_object)
{
    if( role == "po" ){
        SetPosition(p_object);
    }
    if( role == "do" ){
        SetDirection(p_object);
    }
}

//------------------------------------------------------------------------------

void CVectorObject::SaveData(CXMLElement* p_ele)
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

    p_ele->SetAttribute("dx",Direction.x);
    p_ele->SetAttribute("dy",Direction.y);
    p_ele->SetAttribute("dz",Direction.z);

    if( DirectionObject ){
        p_ele->SetAttribute("do",DirectionObject->GetIndex());
    } else {
        int index = -1;
        p_ele->SetAttribute("do",index);
    }

    p_ele->SetAttribute("of",Offset);
    p_ele->SetAttribute("le",Length);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
