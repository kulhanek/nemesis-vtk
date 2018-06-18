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

#include <AngleProperty.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <HistoryNode.hpp>
#include <PropertyList.hpp>
#include <PropertyAtomList.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>
#include <GeoMeasurement.hpp>
#include <GLSelection.hpp>
#include <Atom.hpp>
#include <GraphicsUtil.hpp>
#include <GeoPropertySetup.hpp>
#include <ElementColorsList.hpp>

#if defined _WIN32 || defined __CYGWIN__
#undef DrawText
#endif

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* AnglePropertyCB(void* p_data);

CExtUUID        AnglePropertyID(
                    "{ANGLE_PROPERTY:b0f6eb5d-36fc-4f80-9d9f-11173c6fdc1e}",
                    "Angle");

CPluginObject   AnglePropertyObject(&NemesisCorePlugin,
                    AnglePropertyID,PROPERTY_CAT,
                    ":/images/NemesisCore/properties/Angle.svg",
                    AnglePropertyCB);

// -----------------------------------------------------------------------------

QObject* AnglePropertyCB(void* p_data)
{
    return(new CAngleProperty(static_cast<CPropertyList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAngleProperty::CAngleProperty(CPropertyList *p_bl)
    : CGeoProperty(&AnglePropertyObject,p_bl)
{  
    PropUnit = PQ_ANGLE;

    PointA = new CPropertyAtomList(this);
    connect(PointA,SIGNAL(OnPropertyAtomListChanged(void)),
            this,SLOT(PropertyAtomListChanged(void)));

    PointB = new CPropertyAtomList(this);
    connect(PointB,SIGNAL(OnPropertyAtomListChanged(void)),
            this,SLOT(PropertyAtomListChanged(void)));

    PointC = new CPropertyAtomList(this);
    connect(PointC,SIGNAL(OnPropertyAtomListChanged(void)),
            this,SLOT(PropertyAtomListChanged(void)));

    SET_FLAG(PropFlags,EPF_SCALAR_VALUE,true);
    SET_FLAG(PropFlags,EPF_CARTESIAN_GRADIENT,true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CAngleProperty::IsReady(void)
{
    bool cready = true;

    cready &= PointA->GetNumberOfAtoms() > 0;
    cready &= PointB->GetNumberOfAtoms() > 0;
    cready &= PointC->GetNumberOfAtoms() > 0;

    return( cready );
}

//------------------------------------------------------------------------------

double CAngleProperty::GetScalarValue(void)
{
    if( IsReady() == false ) return(0.0);
    CPoint coma = PointA->GetCOM();
    CPoint comb = PointB->GetCOM();
    CPoint comc = PointC->GetCOM();
    if( Square(coma-comb) == 0 ) return(0.0);
    if( Square(comc-comb) == 0 ) return(0.0);
    return(Angle(coma-comb,comc-comb));
}

//------------------------------------------------------------------------------

double CAngleProperty::GetGradient(QVector<CAtomGrad>& grads)
{
    // point A
    double atotmass;
    CPoint acom = PointA->GetCOM(atotmass);
    if( atotmass == 0.0 ) return(0.0);

    // point B
    double btotmass;
    CPoint bcom = PointB->GetCOM(btotmass);
    if( btotmass == 0.0 ) return(0.0);

    // point c
    double ctotmass;
    CPoint ccom = PointC->GetCOM(ctotmass);
    if( ctotmass == 0.0 ) return(0.0);

    double rijx = acom.x - bcom.x;
    double rijy = acom.y - bcom.y;
    double rijz = acom.z - bcom.z;

    double rkjx = ccom.x - bcom.x;
    double rkjy = ccom.y - bcom.y;
    double rkjz = ccom.z - bcom.z;

    // TODO PBC
//    if( fenable_pbc ) then
//        call pmf_pbc_image_vector3(rijx,rijy,rijz)
//        call pmf_pbc_image_vector3(rkjx,rkjy,rkjz)
//    end if

    double rij2 = rijx*rijx + rijy*rijy + rijz*rijz;
    double rkj2 = rkjx*rkjx + rkjy*rkjy + rkjz*rkjz;

    double rij = sqrt(rij2);
    if( rij == 0 ) return(0);

    double rkj = sqrt(rkj2);
    if( rkj == 0 ) return(0);

    double one_rij2 = 1.0 / rij2;
    double one_rij  = 1.0 / rij;
    double one_rkj2 = 1.0 / rkj2;
    double one_rkj  = 1.0 / rkj;

    double one_rijrkj  = one_rij*one_rkj;

    // value
    double arg = (rijx*rkjx + rijy*rkjy + rijz*rkjz)*one_rijrkj;

    if ( arg >  1.0 ){
        arg =  1.0;
    } else if ( arg < -1.0 ) {
        arg = -1.0;
    }

    double value = acos(arg);

    // derivatives
    double argone_rij2 = arg*one_rij2;
    double argone_rkj2 = arg*one_rkj2;

    double f1 = sin(value);
    if( fabs(f1) < 1.0e-12 ){
        // avoid division by zero
        f1 = -1.0e12;
    } else {
        f1 = -1.0 / f1;
    }

    double a_xix = f1*(rkjx*one_rijrkj - argone_rij2*rijx);
    double a_xiy = f1*(rkjy*one_rijrkj - argone_rij2*rijy);
    double a_xiz = f1*(rkjz*one_rijrkj - argone_rij2*rijz);

    double a_xkx = f1*(rijx*one_rijrkj - argone_rkj2*rkjx);
    double a_xky = f1*(rijy*one_rijrkj - argone_rkj2*rkjy);
    double a_xkz = f1*(rijz*one_rijrkj - argone_rkj2*rkjz);

    double a_xjx = -(a_xix + a_xkx);
    double a_xjy = -(a_xiy + a_xky);
    double a_xjz = -(a_xiz + a_xkz);

    // allocate space
    int numofatms = PointA->GetNumberOfAtoms() + PointB->GetNumberOfAtoms() + PointC->GetNumberOfAtoms();
    grads.resize(numofatms);

    int index = 0;
    foreach(CAtom* p_atom, PointA->GetAtoms()){
        double    tmp = p_atom->GetMass() / atotmass;
        CAtomGrad grd;
        grd.Atom = p_atom;

        grd.Grad.x = a_xix * tmp;
        grd.Grad.y = a_xiy * tmp;
        grd.Grad.z = a_xiz * tmp;

        grads[index++] = grd;
    }

    foreach(CAtom* p_atom, PointB->GetAtoms()){
        double    tmp = p_atom->GetMass() / btotmass;
        CAtomGrad grd;
        grd.Atom = p_atom;

        grd.Grad.x = a_xjx * tmp;
        grd.Grad.y = a_xjy * tmp;
        grd.Grad.z = a_xjz * tmp;

        grads[index++] = grd;
    }

    foreach(CAtom* p_atom, PointC->GetAtoms()){
        double    tmp = p_atom->GetMass() / ctotmass;
        CAtomGrad grd;
        grd.Atom = p_atom;

        grd.Grad.x = a_xkx * tmp;
        grd.Grad.y = a_xky * tmp;
        grd.Grad.z = a_xkz * tmp;

        grads[index++] = grd;
    }

    return(value);
}

//------------------------------------------------------------------------------

CPropertyAtomList* CAngleProperty::GetPointA(void)
{
    return(PointA);
}

//------------------------------------------------------------------------------

CPropertyAtomList* CAngleProperty::GetPointB(void)
{
    return(PointB);
}

//------------------------------------------------------------------------------

CPropertyAtomList* CAngleProperty::GetPointC(void)
{
    return(PointC);
}

//------------------------------------------------------------------------------

bool CAngleProperty::HasGradient(CStructure* p_structure)
{
    if( PointA->ContainsAnyAtomFrom(p_structure) ) return(true);
    if( PointB->ContainsAnyAtomFrom(p_structure) ) return(true);
    if( PointC->ContainsAnyAtomFrom(p_structure) ) return(true);
    return(false);
}

//------------------------------------------------------------------------------

void CAngleProperty::PropertyAtomListChanged(void)
{
    emit OnStatusChanged(ESC_OTHER);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CAngleProperty::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // core ----------------------------
    CGeoProperty::LoadData(p_ele);

    // datapoints ----------------------
    CXMLElement* p_pele;
    p_pele = p_ele->GetFirstChildElement("point_a");
    if( p_pele ) {
        PointA->LoadData(p_pele);
    }

    p_pele = p_ele->GetFirstChildElement("point_b");
    if( p_pele ){
        PointB->LoadData(p_pele);
    }

    p_pele = p_ele->GetFirstChildElement("point_c");
    if( p_pele ){
        PointC->LoadData(p_pele);
    }
}

//------------------------------------------------------------------------------

void CAngleProperty::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // core ----------------------------
    CGeoProperty::SaveData(p_ele);

    // datapoints ----------------------
    CXMLElement* p_pele;

    p_pele = p_ele->CreateChildElement("point_a");
    PointA->SaveData(p_pele);

    p_pele = p_ele->CreateChildElement("point_b");
    PointB->SaveData(p_pele);

    p_pele = p_ele->CreateChildElement("point_c");
    PointB->SaveData(p_pele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CAngleProperty::Draw(void)
{
    if( IsReady() == false ) return;

    Setup = GetSetup<CGeoPropertySetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);

    GLLoadObject(this);

    LabelAngle();

    glDisable(GL_LINE_STIPPLE);
}

//------------------------------------------------------------------------------

void CAngleProperty::LabelAngle(void)
{
    CSimplePoint<float> pos1 = PointA->GetCOM();
    CSimplePoint<float> pos2 = PointB->GetCOM();
    CSimplePoint<float> pos3 = PointC->GetCOM();

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
    if( IsFlagSet(EPOF_SELECTED) ){
        glColor4fv(ColorsList.SelectionMaterial.Color);
    } else {
        glColor4fv(Setup->LineColor);
    }
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
        arc.Draw(v1,v2,v3,angle,str);
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

    if( IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_RELATIVE_LABEL_POS) ){
        textpos = GetLabelPosition() + str;
    } else {
        textpos = GetLabelPosition();
    }

    if( IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_SHOW_LABEL) ){
        QString text = PQ_ANGLE->GetRealValueText(angle);

        if( Setup->ShowUnit == true ){
           text += " " + PQ_ANGLE->GetUnitName();
           }

        DrawText(textpos,text);
    }

    DrawLabelQuotationLine(str,textpos);

    DrawCOMPosition(pos1);
    DrawCOMQuotation(pos1,PointA->GetAtoms());
    DrawCOMPosition(pos2);
    DrawCOMQuotation(pos2,PointB->GetAtoms());
    DrawCOMPosition(pos3);
    DrawCOMQuotation(pos3,PointC->GetAtoms());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


