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

#include <PBCBoxObject.hpp>
#include <PBCBoxSetup.hpp>
#include <PBCBoxObjectHistory.hpp>

#include "PBCBoxObject.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PBCBoxObjectCB(void* p_data);

CExtUUID        PBCBoxObjectID(
                    "{PBC_BOX_OBJECT:2929a64d-921a-414b-8dcd-c81e53881ce1}",
                    "PBCBox");

CPluginObject   PBCBoxObject(&NemesisCorePlugin,
                    PBCBoxObjectID,
                    GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/objects/PBCBox.svg",
                    PBCBoxObjectCB
                    );

// -----------------------------------------------------------------------------

QObject* PBCBoxObjectCB(void* p_data)
{
    return(new CPBCBoxObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPBCBoxObject::CPBCBoxObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&PBCBoxObject,p_gl)
{
    Setup = NULL;
    KA = 0;
    KB = 0;
    KC = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CPBCBoxObject::SetKOffsetsWH(int ka,int kb,int kc)
{
    if( (KA == ka) && (KB == kb) && (KC == kc) ) return(true);

    // with history
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"box position");
    if( p_history == NULL ) return(false);

    SetKOffsets(ka,kb,kc,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

void CPBCBoxObject::SetKOffsets(int ka,int kb,int kc,CHistoryNode* p_history)
{
    if( (KA == ka) && (KB == kb) && (KC == kc) ) return;

    if( p_history ){
        CPBCBoxObjectChangePosHI* p_item = new CPBCBoxObjectChangePosHI(this,ka,kb,kc);
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

int CPBCBoxObject::GetKAOffset(void)
{
    return(KA);
}

//------------------------------------------------------------------------------

int CPBCBoxObject::GetKBOffset(void)
{
    return(KB);
}

//------------------------------------------------------------------------------

int CPBCBoxObject::GetKCOffset(void)
{
    return(KC);
}

//------------------------------------------------------------------------------

CGeoDescriptor CPBCBoxObject::GetGeoDescriptor(void)
{
    CGeoDescriptor geo;

    if( Objects.isEmpty() ) return(geo);
    
    CStructure* p_str = static_cast<CStructure*>(Objects.first());
    if( p_str == NULL ) return(geo);
    if( p_str->PBCInfo.Is3DPBCEnabled() == false ) return(geo);

    CPoint center = p_str->PBCInfo.GetBoxCenter();

    CPoint shift;

    if( IsFlagSet<EPBCBoxObjectFlag>(EPBCBOF_CENTER_TO_ORIGIN) ){
        shift -= p_str->PBCInfo.GetBoxCenter();
    }

    shift += p_str->PBCInfo.GetAVector()*KA;
    shift += p_str->PBCInfo.GetBVector()*KB;
    shift += p_str->PBCInfo.GetCVector()*KC;

    center += shift;

    geo.SetType(EGDT_ONE_POINT);
    geo.SetPoint1(center);

    return(geo);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CPBCBoxObject::CanBeObjectAdded(CProObject* p_object)
{
    if( p_object == NULL ) return(false);

    // is already assigned?
    if( Objects.count() > 0 ) return(false);

    // is it molecule?
    if( p_object->GetType() == StructureID ) return(true);

    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPBCBoxObject::Draw(void)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    Setup = GetSetup<CPBCBoxSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }
    
    if( Objects.isEmpty() ) return;
    
    CStructure* p_str = static_cast<CStructure*>(Objects.first());
    if( p_str == NULL ) return;

    // check if PBCInfo is valid
    if( p_str->PBCInfo.Is3DPBCEnabled() == false ) return;

    GLLoadObject(this);

    if( IsFlagSet<EPBCBoxObjectFlag>(EPBCBOF_SHOW_FAMILIAR) ){
        DrawFamiliarBox(p_str);
    } else {
        DrawRegularBox(p_str);
    }

    if( IsFlagSet<EPBCBoxObjectFlag>(EPBCBOF_SHOW_CENTER) ){
        DrawCenter(p_str);
    }
}

//------------------------------------------------------------------------------

void CPBCBoxObject::SetupChanged(void)
{
    Setup = GetSetup<CPBCBoxSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }
    Sphere.SetTessellationQuality(Setup->SphereTesselationQuality);
    Cylinder.SetTessellationQuality(Setup->TubeTesselationQuality);

    CGraphicsObject::SetupChanged();
}

//------------------------------------------------------------------------------

void CPBCBoxObject::DrawRegularBox(CStructure* p_str)
{
    CPoint l1;
    CPoint l2 = p_str->PBCInfo.GetAVector();
    CPoint l3 = p_str->PBCInfo.GetBVector();
    CPoint l4 = l2+l3;

    CPoint h1 = p_str->PBCInfo.GetCVector();
    CPoint h2 = l2+h1;
    CPoint h3 = l3+h1;
    CPoint h4 = l2+l3+h1;

    CPoint shift;

    if( IsFlagSet<EPBCBoxObjectFlag>(EPBCBOF_CENTER_TO_ORIGIN) ){
        shift -= p_str->PBCInfo.GetBoxCenter();
    }

    shift += p_str->PBCInfo.GetAVector()*KA;
    shift += p_str->PBCInfo.GetBVector()*KB;
    shift += p_str->PBCInfo.GetCVector()*KC;

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
        glDisable(GL_LIGHTING);
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

void CPBCBoxObject::DrawFamiliarBox(CStructure* p_str)
{


}

//------------------------------------------------------------------------------

void CPBCBoxObject::DrawTube(CPoint& p1,CPoint& p2)
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

void CPBCBoxObject::DrawSphere(CPoint& p1)
{
    glPushMatrix();
        glTranslatef (p1.x,p1.y,p1.z);
        Sphere.Draw(Setup->LineWidth);
    glPopMatrix();
}

//---------------------------------------------------------------------------

void CPBCBoxObject::SetInitialColorScheme(void)
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

void CPBCBoxObject::DrawCenter(CStructure* p_str)
{
    CPoint center = p_str->PBCInfo.GetBoxCenter();

    CPoint shift;

    if( IsFlagSet<EPBCBoxObjectFlag>(EPBCBOF_CENTER_TO_ORIGIN) ){
        shift -= p_str->PBCInfo.GetBoxCenter();
    }

    shift += p_str->PBCInfo.GetAVector()*KA;
    shift += p_str->PBCInfo.GetBVector()*KB;
    shift += p_str->PBCInfo.GetCVector()*KC;

    center += shift;

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

void CPBCBoxObject::GetObjectMetrics(CObjMetrics& metrics)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    if( Objects.isEmpty() ) return;

    CStructure* p_str = static_cast<CStructure*>(Objects.first());
    if( p_str == NULL ) return;

    // check if PBCInfo is valid
    if( p_str->PBCInfo.Is3DPBCEnabled() == false ) return;

    if( IsFlagSet<EPBCBoxObjectFlag>(EPBCBOF_SHOW_FAMILIAR) ){
        GetFamiliarBoxMetrics(p_str,metrics);
    } else {
        GetRegularBoxMetrics(p_str,metrics);
    }
}

//------------------------------------------------------------------------------

void CPBCBoxObject::GetRegularBoxMetrics(CStructure* p_str,CObjMetrics& metrics)
{
    CPoint l1;
    CPoint l2 = p_str->PBCInfo.GetAVector();
    CPoint l3 = p_str->PBCInfo.GetBVector();
    CPoint l4 = l2+l3;

    CPoint h1 = p_str->PBCInfo.GetCVector();
    CPoint h2 = l2+h1;
    CPoint h3 = l3+h1;
    CPoint h4 = l2+l3+h1;

    CPoint shift;

    if( IsFlagSet<EPBCBoxObjectFlag>(EPBCBOF_CENTER_TO_ORIGIN) ){
        shift -= p_str->PBCInfo.GetBoxCenter();
    }

    shift += p_str->PBCInfo.GetAVector()*KA;
    shift += p_str->PBCInfo.GetBVector()*KB;
    shift += p_str->PBCInfo.GetCVector()*KC;

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

void CPBCBoxObject::GetFamiliarBoxMetrics(CStructure* p_str,CObjMetrics& metrics)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPBCBoxObject::LoadData(CXMLElement* p_ele)
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
}

//------------------------------------------------------------------------------

void CPBCBoxObject::SaveData(CXMLElement* p_ele)
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
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


