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

#include <PluginObject.hpp>
#include <XMLElement.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <GlobalSetup.hpp>
#include <NemesisCoreModule.hpp>
#include <GeoProperty.hpp>
#include <ErrorSystem.hpp>
#include <GeoPropertyHistory.hpp>
#include <HistoryNode.hpp>
#include <GeoPropertySetup.hpp>
#include <ElementColorsList.hpp>
#include <Atom.hpp>
#include <ElementColorsList.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGeoProperty::CGeoProperty(CPluginObject* p_po,CPropertyList *p_bl)
    : CGraphicsProperty(p_po,p_bl)
{

    SetFlag<EGeoPropertyObjectFlag>(EGPOF_SHOW_LABEL,true);
    SetFlag<EGeoPropertyObjectFlag>(EGPOF_RELATIVE_LABEL_POS,true);
    SetFlag<EGeoPropertyObjectFlag>(EGPOF_SHOW_COM_QUOTATION,true);
    SetFlag<EGeoPropertyObjectFlag>(EGPOF_SHOW_COM,true);
}

//------------------------------------------------------------------------------

CGeoProperty::~CGeoProperty(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CGeoProperty::SetLabelPositionWH(const CPoint& pos)
{
    if( LabelPos == pos ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("label position"));
    if( p_history == NULL ) return (false);

    SetLabelPosition(pos,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

void CGeoProperty::SetLabelPosition(const CPoint& pos,CHistoryNode* p_history)
{
    if( LabelPos == pos ) return;

    if( p_history ){
        CGeoPropertyChangePositionHI* p_item = new CGeoPropertyChangePositionHI(this,pos);
        p_history->Register(p_item);
    }

    LabelPos = pos;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

const CPoint& CGeoProperty::GetLabelPosition(void)
{
    return(LabelPos);
}

//------------------------------------------------------------------------------

void CGeoProperty::DrawText(const CSimplePoint<float>& pos,const QString& label)
{
    if( ! IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_SHOW_LABEL) ) return;

//    glPushMatrix();
//        if( IsFlagSet(EPOF_SELECTED) ){
//            glColor4fv(ColorsList.SelectionMaterial.Color);
//        } else {
//            glColor4fv(Setup->LabelColor);
//        }

//        glTranslatef(pos.x,pos.y,pos.z);

//        // the text should be faced to an user
//        ApplyFaceTransformation();

//        // rotate FTGL to our coordinate system
//        glRotatef(90,1,0,0);
//        glRotatef(90,0,1,0);
//        glScalef(0.01,0.01,0.01);

//        if( Setup->DepthTest ){
//            glEnable(GL_DEPTH_TEST);
//        } else {
//            glDisable(GL_DEPTH_TEST);
//        }

//        Setup->FTGLFont.Render(label.toStdWString().c_str());
//    glPopMatrix();

//    glEnable(GL_DEPTH_TEST);
}

//------------------------------------------------------------------------------

void CGeoProperty::DrawLabelQuotationLine(CSimplePoint<float>& p1,CSimplePoint<float>& p2)
{
    if( ! IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_SHOW_LABEL_QUOTATION) ) return;

//    if( Setup->LabelQuotationStippleFactor > 0 ){
//        glEnable(GL_LINE_STIPPLE);
//        glLineStipple(Setup->LabelQuotationStippleFactor,Setup->LabelQuotationStipplePattern);
//    } else {
//        glDisable(GL_LINE_STIPPLE);
//    }
//    glLineWidth(Setup->LabelQuotationLineWidth);
//    if( IsFlagSet(EPOF_SELECTED) ){
//        glColor4fv(ColorsList.SelectionMaterial.Color);
//    } else {
//        glColor4fv(Setup->LabelQuotationColor);
//    }

//    glBegin(GL_LINES);
//       glVertex3fv(p1);
//       glVertex3fv(p2);
//    glEnd();
}

//------------------------------------------------------------------------------

void CGeoProperty::DrawCOMPosition(CSimplePoint<float>& com)
{
    if( ! IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_SHOW_COM) ) return;

    // possition cross line points ---------------
    CSimplePoint<float>  xline1;
    CSimplePoint<float>  xline2;
    CSimplePoint<float>  yline1;
    CSimplePoint<float>  yline2;
    CSimplePoint<float>  zline1;
    CSimplePoint<float>  zline2;
    float                off = Setup->PointSize;

    // define line points ------------------------
    xline1.Set(com.x - off, com.y, com.z);
    xline2.Set(com.x + off, com.y, com.z);
    yline1.Set(com.x, com.y - off, com.z);
    yline2.Set(com.x, com.y + off, com.z);
    zline1.Set(com.x, com.y, com.z - off);
    zline2.Set(com.x, com.y, com.z + off);

    // draw position cross -----------------------
//    glLineWidth(Setup->PointWidth);
//    if( IsFlagSet(EPOF_SELECTED) ){
//        glColor4fv(ColorsList.SelectionMaterial.Color);
//    } else {
//        glColor4fv(Setup->PointColor);
//    }
//    glBegin(GL_LINES);
//        glVertex3fv(xline1);
//        glVertex3fv(xline2);
//        glVertex3fv(yline1);
//        glVertex3fv(yline2);
//        glVertex3fv(zline1);
//        glVertex3fv(zline2);
//    glEnd();
}

//------------------------------------------------------------------------------

void CGeoProperty::DrawCOMQuotation(CSimplePoint<float>& com,const QList<CAtom*>& atoms)
{
    if( ! IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_SHOW_COM_QUOTATION) ) return;

//    glEnable(GL_LINE_STIPPLE);

//    glLineStipple(Setup->PointQuotationStippleFactor,Setup->PointQuotationStipplePattern);
//    glLineWidth(Setup->PointQuotationWidth);

//    if( IsFlagSet(EPOF_SELECTED) ){
//        glColor4fv(ColorsList.SelectionMaterial.Color);
//    } else {
//        glColor4fv(Setup->PointQuotationColor);
//    }

//    foreach(CAtom* p_atom,atoms) {
//        CPoint pos = p_atom->GetPos();

//        glBegin(GL_LINES);
//            glVertex3fv(com);
//            glVertex3dv(pos);
//        glEnd();
//    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeoProperty::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }
    CProperty::LoadData(p_ele);
}

//---------------------------------------------------------------------------

void CGeoProperty::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }
    CProperty::SaveData(p_ele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



