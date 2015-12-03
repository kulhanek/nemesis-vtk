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
#include <GL/gl.h>
#include <GL/glu.h>
#include <Project.hpp>
#include <Atom.hpp>
#include <GraphicsProfileList.hpp>
#include <GeoMeasurement.hpp>
#include <PeriodicTable.hpp>
#include <ElementColorsList.hpp>
#include <HistoryNode.hpp>
#include <Structure.hpp>
#include <AtomList.hpp>
#include <GLSelection.hpp>
#include <GraphicsView.hpp>

#include <PointSetup.hpp>
#include <PointObject.hpp>
#include <PointObjectHistory.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PointObjectCB(void* p_data);

CExtUUID        PointObjectID(
                    "{POINT_OBJECT:089f4df8-cf38-48a2-84e1-f0bac7aa6ffe}",
                    "Point");

CPluginObject   PointObject(&NemesisCorePlugin,
                    PointObjectID,GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/objects/Point.svg",
                    PointObjectCB);

// -----------------------------------------------------------------------------

QObject* PointObjectCB(void* p_data)
{
    return(new CPointObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPointObject::CPointObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&PointObject,p_gl)
{
    RegisterAllowedObjectType(AtomID);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CPointObject::SetPositionWH(const CPoint& pos)
{
    if( Point == pos ) return(true);

    // with history
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"point position");
    if( p_history == NULL ) return(false);

    SetPosition(pos,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

void CPointObject::SetPosition(const CPoint& pos,CHistoryNode* p_history)
{
    if( Point == pos ) return;

    if( p_history ){
        CHistoryItem* p_hitem = new CPointObjectChangePosHI(this,pos);
        p_history->Register(p_hitem);
    }

    Point = pos;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

const CPoint& CPointObject::GetPosition(void)
{
    UpdatePosition();
    return(Point);
}

//------------------------------------------------------------------------------

bool CPointObject::IsAutonomousPoint(void)
{
    if( Objects.size() == 0 ) return(true);
    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPointObject::StartManipulation(CHistoryNode* p_history)
{
    if( p_history == NULL ) return;

    CHistoryItem* p_item = new CPointObjectBackupGeoHI(this);
    p_history->Register(p_item);

    // use delayed event notification
    StartEvent();
}

//------------------------------------------------------------------------------

void CPointObject::Move(const CPoint& dmov,CGraphicsView* p_view)
{
    CPoint _dmov = dmov;

    CTransformation coord = p_view->GetTrans();
    coord.Invert();
    coord.Apply(_dmov);

    Point += _dmov;

    // use delayed event notification
    RaiseEvent();
}

//------------------------------------------------------------------------------

void CPointObject::EndManipulation(void)
{
    // end delayed event notification
    EndEvent();
}

//------------------------------------------------------------------------------

void CPointObject::RespondToEvent(void)
{
    // respond to delayed notification
    emit OnStatusChanged(ESC_OTHER);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPointObject::Draw(void)
{
    Setup = GetSetup<CPointSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LINE_STIPPLE);

    GLLoadObject(this);

    UpdatePosition();
    DrawPosition();

    if( IsFlagSet(static_cast<EProObjectFlag>(ECPOF_SHOW_QUOTATION)) ) {
        DrawQuotation();
    }

    glDisable(GL_LINE_STIPPLE);
}

//------------------------------------------------------------------------------

void CPointObject::DrawPosition(void)
{
    // possition cross line points ---------------
    CSimplePoint<float>  xline1;
    CSimplePoint<float>  xline2;
    CSimplePoint<float>  yline1;
    CSimplePoint<float>  yline2;
    CSimplePoint<float>  zline1;
    CSimplePoint<float>  zline2;
    float                off = Setup->PointSize;

    // define line points ------------------------
    xline1.Set(Point.x - off, Point.y, Point.z);
    xline2.Set(Point.x + off, Point.y, Point.z);
    yline1.Set(Point.x, Point.y - off, Point.z);
    yline2.Set(Point.x, Point.y + off, Point.z);
    zline1.Set(Point.x, Point.y, Point.z - off);
    zline2.Set(Point.x, Point.y, Point.z + off);

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

void CPointObject::DrawQuotation(void)
{
    glEnable(GL_LINE_STIPPLE);

    glLineStipple(Setup->QuotationStippleFactor,Setup->QuotationStipplePattern);
    glLineWidth(Setup->QuotationWidth);
    glColor4fv(Setup->QuotationColor.ForGL());

    foreach(QObject* p_qobj,Objects) {
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        CPoint pos = p_atom->GetPos();

        glBegin(GL_LINES);
            glVertex3dv(Point);
            glVertex3dv(pos);
        glEnd();
    }
}

//------------------------------------------------------------------------------

void CPointObject::UpdatePosition(void)
{
    if( Objects.size() == 0 ) return;

    Point = CPoint();
    double totmass = 0.0;

    foreach(QObject* p_qobj,Objects) {
        CAtom* p_atom = dynamic_cast<CAtom*>(p_qobj);
        if( p_atom ){

            double mass = 1.0;

            if( IsFlagSet(static_cast<EProObjectFlag>(ECPOF_SHOW_COG)) == false ){
            mass = PeriodicTable.GetMass(p_atom->GetZ());
            }
            Point += p_atom->GetPos()*mass;
            totmass += mass;
        }
        CStructure* p_str = dynamic_cast<CStructure*>(p_qobj);
        if( p_str ){
            foreach(QObject* p_aobj,p_str->GetAtoms()->children()) {
                CAtom* p_atom = static_cast<CAtom*>(p_aobj);
                double mass = 1.0;

                if( IsFlagSet(static_cast<EProObjectFlag>(ECPOF_SHOW_COG)) == false ){
                    mass = PeriodicTable.GetMass(p_atom->GetZ());
                }
                Point += p_atom->GetPos()*mass;
                totmass += mass;
            }
        }
    }

    if( totmass > 0 ) {
        Point /= totmass;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGeoDescriptor CPointObject::GetGeoDescriptor(void)
{
    CGeoDescriptor descrip;

    descrip.SetType(EGDT_ONE_POINT);
    descrip.SetPoint1(GetPosition());

    return(descrip);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPointObject::AddObject(CProObject* p_object,CHistoryNode* p_history)
{
    if( CanBeObjectAdded(p_object) == false ) return;

    bool emit_sig = false;
    if( IsAutonomousPoint() ){
        if( p_history ){
            CPointObjectChangePosHI* p_hnode = new CPointObjectChangePosHI(this,Point);
            p_history->Register(p_hnode);
        }
        emit_sig = true;
    }

    CGraphicsObject::AddObject(p_object,p_history);

    if( emit_sig ) {
        emit OnStatusChanged(ESC_OTHER);
    }
}

//------------------------------------------------------------------------------

void CPointObject::RemoveObject(CProObject* p_object,CHistoryNode* p_history)
{
    CGraphicsObject::RemoveObject(p_object,p_history);

    if( IsAutonomousPoint() ){
        if( p_history ){
            CPointObjectChangePosHI* p_hnode = new CPointObjectChangePosHI(this,Point);
            p_history->Register(p_hnode);
        }
        emit OnStatusChanged(ESC_OTHER);
    }
}

//------------------------------------------------------------------------------

void CPointObject::GetObjectMetrics(CObjMetrics& metrics)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    metrics.CompareWith(GetPosition());

    if( IsFlagSet(static_cast<EProObjectFlag>(ECPOF_SHOW_QUOTATION)) ) {
        foreach(QObject* p_qobj,Objects) {
            CAtom* p_atom = dynamic_cast<CAtom*>(p_qobj);
            if( p_atom ){
                metrics.CompareWith(p_atom->GetPos());
            }
            CStructure* p_str = dynamic_cast<CStructure*>(p_qobj);
            if( p_str ){
                foreach(QObject* p_aobj,p_str->GetAtoms()->children()) {
                    CAtom* p_atom = static_cast<CAtom*>(p_aobj);
                    metrics.CompareWith(p_atom->GetPos());
                }
            }
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPointObject::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load object info --------------------------
    CGraphicsObject::LoadData(p_ele);

    // specific data -----------------------------
    // always read - we cannot determine now if it is AutonomousPoint
    p_ele->GetAttribute("px",Point.x);
    p_ele->GetAttribute("py",Point.y);
    p_ele->GetAttribute("pz",Point.z);
}

//------------------------------------------------------------------------------

void CPointObject::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // core data ---------------------------------
    CGraphicsObject::SaveData(p_ele);

    // specific data -----------------------------
    if( IsAutonomousPoint() ){
        p_ele->SetAttribute("px",Point.x);
        p_ele->SetAttribute("py",Point.y);
        p_ele->SetAttribute("pz",Point.z);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

