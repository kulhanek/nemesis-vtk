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

#include <TorsionProperty.hpp>
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
#include <GeoPropertySetup.hpp>
#include <GraphicsUtil.hpp>
#include <ElementColorsList.hpp>

#include "TorsionProperty.moc"

#if defined _WIN32 || defined __CYGWIN__
#undef DrawText
#endif

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* TorsionPropertyCB(void* p_data);

CExtUUID        TorsionPropertyID(
                    "{TORSION_PROPERTY:49fb346b-b34b-4845-90e5-35d72f60fbae}",
                    "Torsion");

CPluginObject   TorsionPropertyObject(&NemesisCorePlugin,
                    TorsionPropertyID,PROPERTY_CAT,
                    ":/images/NemesisCore/properties/Torsion.svg",
                    TorsionPropertyCB);

// -----------------------------------------------------------------------------

QObject* TorsionPropertyCB(void* p_data)
{
    return(new CTorsionProperty(static_cast<CPropertyList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTorsionProperty::CTorsionProperty(CPropertyList *p_bl)
    : CGeoProperty(&TorsionPropertyObject,p_bl)
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

    PointD = new CPropertyAtomList(this);
    connect(PointD,SIGNAL(OnPropertyAtomListChanged(void)),
            this,SLOT(PropertyAtomListChanged(void)));

    SET_FLAG(PropFlags,EPF_SCALAR_VALUE,true);
    SET_FLAG(PropFlags,EPF_CARTESIAN_GRADIENT,true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTorsionProperty::IsReady(void)
{
    bool cready = true;

    cready &= PointA->GetNumberOfAtoms() > 0;
    cready &= PointB->GetNumberOfAtoms() > 0;
    cready &= PointC->GetNumberOfAtoms() > 0;
    cready &= PointD->GetNumberOfAtoms() > 0;

    return( cready );
}

//------------------------------------------------------------------------------

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

//------------------------------------------------------------------------------

double CTorsionProperty::GetScalarValue(void)
{
    if( IsReady() == false ) return(0.0);

    // point A
    double atotmass;
    CPoint acom = PointA->GetCOM(atotmass);
    if( atotmass == 0.0 ) return(0.0);

    // point B
    double btotmass;
    CPoint bcom = PointB->GetCOM(btotmass);
    if( btotmass == 0.0 ) return(0.0);

    // point C
    double ctotmass;
    CPoint ccom = PointC->GetCOM(ctotmass);
    if( ctotmass == 0.0 ) return(0.0);

    // point D
    double dtotmass;
    CPoint dcom = PointD->GetCOM(dtotmass);
    if( dtotmass == 0.0 ) return(0.0);

    double rijx = acom.x - bcom.x;
    double rijy = acom.y - bcom.y;
    double rijz = acom.z - bcom.z;

    double rkjx = ccom.x - bcom.x;
    double rkjy = ccom.y - bcom.y;
    double rkjz = ccom.z - bcom.z;

    double rklx = ccom.x - dcom.x;
    double rkly = ccom.y - dcom.y;
    double rklz = ccom.z - dcom.z;

//    if( fenable_pbc ) then
//        call pmf_pbc_image_vector3(rijx,rijy,rijz)
//        call pmf_pbc_image_vector3(rkjx,rkjy,rkjz)
//        call pmf_pbc_image_vector3(rklx,rkly,rklz)
//    end if

//    ! coordinate definition
//    !
//    ! d = rij x rkj
//    ! g = rkj x rkl
//    !
//    ! s1 = (rkjy*rklz - rkjz*rkly)*rijx + (rkjz*rklx - rkjx*rklz)*rijy + (rkjx*rkly - rkjy*rklx)*rijz
//    !
//    ! ksi = sign(s1)arccos(d.g/(|d|.|g|))
//    !

//    ! d = rij x rkj
    double dx = rijy*rkjz - rijz*rkjy;
    double dy = rijz*rkjx - rijx*rkjz;
    double dz = rijx*rkjy - rijy*rkjx;

    double d2 = dx*dx + dy*dy + dz*dz;
    if( d2 == 0 ) return(0.0);

//    ! g = rkj x rkl
    double gx = rkjy*rklz - rkjz*rkly;
    double gy = rkjz*rklx - rkjx*rklz;
    double gz = rkjx*rkly - rkjy*rklx;

    double g2 = gx*gx + gy*gy + gz*gz;
    if( g2 == 0 ) return(0.0);

//    ! value of coordinate

    double s1 = (rkjy*rklz - rkjz*rkly)*rijx + (rkjz*rklx - rkjx*rklz)*rijy + (rkjx*rkly - rkjy*rklx)*rijz;

    double value = sgn<double>(s1)*acos( (dx*gx + dy*gy + dz*gz)/sqrt(d2*g2) );

    return(value);
}

//------------------------------------------------------------------------------

double CTorsionProperty::GetGradient(QVector<CAtomGrad>& grads)
{
    // point A
    double atotmass;
    CPoint acom = PointA->GetCOM(atotmass);
    if( atotmass == 0.0 ) return(0.0);

    // point B
    double btotmass;
    CPoint bcom = PointB->GetCOM(btotmass);
    if( btotmass == 0.0 ) return(0.0);

    // point C
    double ctotmass;
    CPoint ccom = PointC->GetCOM(ctotmass);
    if( ctotmass == 0.0 ) return(0.0);

    // point D
    double dtotmass;
    CPoint dcom = PointD->GetCOM(dtotmass);
    if( dtotmass == 0.0 ) return(0.0);

    double rijx = acom.x - bcom.x;
    double rijy = acom.y - bcom.y;
    double rijz = acom.z - bcom.z;

    double rkjx = ccom.x - bcom.x;
    double rkjy = ccom.y - bcom.y;
    double rkjz = ccom.z - bcom.z;

    double rklx = ccom.x - dcom.x;
    double rkly = ccom.y - dcom.y;
    double rklz = ccom.z - dcom.z;

//    if( fenable_pbc ) then
//        call pmf_pbc_image_vector3(rijx,rijy,rijz)
//        call pmf_pbc_image_vector3(rkjx,rkjy,rkjz)
//        call pmf_pbc_image_vector3(rklx,rkly,rklz)
//    end if

//    ! coordinate definition
//    !
//    ! d = rij x rkj
//    ! g = rkj x rkl
//    !
//    ! s1 = (rkjy*rklz - rkjz*rkly)*rijx + (rkjz*rklx - rkjx*rklz)*rijy + (rkjx*rkly - rkjy*rklx)*rijz
//    !
//    ! ksi = sign(s1)arccos(d.g/(|d|.|g|))
//    !

//    ! d = rij x rkj
    double dx = rijy*rkjz - rijz*rkjy;
    double dy = rijz*rkjx - rijx*rkjz;
    double dz = rijx*rkjy - rijy*rkjx;

    double d2 = dx*dx + dy*dy + dz*dz;
    if( d2 == 0 ) return(0.0);

//    ! g = rkj x rkl
    double gx = rkjy*rklz - rkjz*rkly;
    double gy = rkjz*rklx - rkjx*rklz;
    double gz = rkjx*rkly - rkjy*rklx;

    double g2 = gx*gx + gy*gy + gz*gz;
    if( g2 == 0 ) return(0.0);

//    ! value of coordinate

    double s1 = (rkjy*rklz - rkjz*rkly)*rijx + (rkjz*rklx - rkjx*rklz)*rijy + (rkjx*rkly - rkjy*rklx)*rijz;

    double value = sgn<double>(s1)*acos( (dx*gx + dy*gy + dz*gz)/sqrt(d2*g2) );

//    ! and it's first derivatives --------------------------

    double rkj2 = rkjx*rkjx + rkjy*rkjy + rkjz*rkjz;
    if( rkj2 == 0 ) return(0.0);

    double rkj = sqrt(rkj2);

    double rkj_d2 = rkj / d2;
    double mrkj_g2 = -rkj / g2;

    double a_xix = rkj_d2 * dx;
    double a_xiy = rkj_d2 * dy;
    double a_xiz = rkj_d2 * dz;

    double a_xlx = mrkj_g2 * gx;
    double a_xly = mrkj_g2 * gy;
    double a_xlz = mrkj_g2 * gz;

    double rijorkj = rijx * rkjx + rijy * rkjy + rijz * rkjz;
    double rkjorkl = rkjx * rklx + rkjy * rkly + rkjz * rklz;

    double WjA = rijorkj / rkj2 - 1;
    double WjB = rkjorkl / rkj2;

    double WkA = rkjorkl / rkj2 - 1;
    double WkB = rijorkj / rkj2;

    double a_xjx = WjA * a_xix - WjB * a_xlx;
    double a_xjy = WjA * a_xiy - WjB * a_xly;
    double a_xjz = WjA * a_xiz - WjB * a_xlz;

    double a_xkx = WkA * a_xlx - WkB * a_xix;
    double a_xky = WkA * a_xly - WkB * a_xiy;
    double a_xkz = WkA * a_xlz - WkB * a_xiz;

    // allocate space
    int numofatms = PointA->GetNumberOfAtoms() + PointB->GetNumberOfAtoms()
                  + PointC->GetNumberOfAtoms() + PointD->GetNumberOfAtoms();
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

    foreach(CAtom* p_atom, PointD->GetAtoms()){
        double    tmp = p_atom->GetMass() / dtotmass;
        CAtomGrad grd;
        grd.Atom = p_atom;
        grd.Grad.x = a_xlx * tmp;
        grd.Grad.y = a_xly * tmp;
        grd.Grad.z = a_xlz * tmp;
        grads[index++] = grd;
    }

    return(value);
}

//------------------------------------------------------------------------------

CPropertyAtomList* CTorsionProperty::GetPointA(void)
{
    return(PointA);
}

//------------------------------------------------------------------------------

CPropertyAtomList* CTorsionProperty::GetPointB(void)
{
    return(PointB);
}

//------------------------------------------------------------------------------

CPropertyAtomList* CTorsionProperty::GetPointC(void)
{
    return(PointC);
}

//------------------------------------------------------------------------------

CPropertyAtomList* CTorsionProperty::GetPointD(void)
{
    return(PointD);
}

//------------------------------------------------------------------------------

bool CTorsionProperty::HasGradient(CStructure* p_structure)
{
    if( PointA->ContainsAnyAtomFrom(p_structure) ) return(true);
    if( PointB->ContainsAnyAtomFrom(p_structure) ) return(true);
    if( PointC->ContainsAnyAtomFrom(p_structure) ) return(true);
    if( PointD->ContainsAnyAtomFrom(p_structure) ) return(true);
    return(false);
}

//------------------------------------------------------------------------------

void CTorsionProperty::PropertyAtomListChanged(void)
{
    emit OnStatusChanged(ESC_OTHER);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTorsionProperty::LoadData(CXMLElement* p_ele)
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
    if( p_pele ) {
        PointC->LoadData(p_pele);
    }

    p_pele = p_ele->GetFirstChildElement("point_d");
    if( p_pele ){
        PointD->LoadData(p_pele);
    }
}

//------------------------------------------------------------------------------

void CTorsionProperty::SaveData(CXMLElement* p_ele)
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
    PointC->SaveData(p_pele);

    p_pele = p_ele->CreateChildElement("point_d");
    PointD->SaveData(p_pele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTorsionProperty::Draw(void)
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

    LabelTorsion();

    glDisable(GL_LINE_STIPPLE);
}

//------------------------------------------------------------------------------

void CTorsionProperty::LabelTorsion(void)
{    
    CSimplePoint<float> pos1 = PointA->GetCOM();
    CSimplePoint<float> pos2 = PointB->GetCOM();
    CSimplePoint<float> pos3 = PointC->GetCOM();
    CSimplePoint<float> pos4 = PointD->GetCOM();

    // calculate dihed ---------------------------
    double              dihed;
    dihed = CGeoMeasurement::GetTorsion(pos1,pos2,pos3,pos4);

    // draw dihed --------------------------------
    glLineWidth(Setup->LineWidth);
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

    if( IsFlagSet(EPOF_SELECTED) ){
        glColor4fv(ColorsList.SelectionMaterial.Color);
    } else {
        glColor4fv(Setup->LineColor);
    }

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
    CSimplePoint<float>  textpos;

    if( IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_RELATIVE_LABEL_POS) ){
        textpos = GetLabelPosition() + pm;
    } else {
        textpos = GetLabelPosition();
    }

    if( IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_SHOW_LABEL) ){
        QString text = PQ_ANGLE->GetRealValueText(dihed);

        if( Setup->ShowUnit == true ){
           text += " " + PQ_ANGLE->GetUnitName();
           }

        DrawText(textpos,text);
    }

    DrawLabelQuotationLine(pm,textpos);

    DrawCOMPosition(pos1);
    DrawCOMQuotation(pos1,PointA->GetAtoms());
    DrawCOMPosition(pos2);
    DrawCOMQuotation(pos2,PointB->GetAtoms());
    DrawCOMPosition(pos3);
    DrawCOMQuotation(pos3,PointC->GetAtoms());
    DrawCOMPosition(pos4);
    DrawCOMQuotation(pos4,PointD->GetAtoms());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


