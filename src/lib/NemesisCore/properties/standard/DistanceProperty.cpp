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

#include <DistanceProperty.hpp>
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
#include <ElementColorsList.hpp>

#if defined _WIN32 || defined __CYGWIN__
#undef DrawText
#endif

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DistancePropertyCB(void* p_data);

CExtUUID        DistancePropertyID(
                    "{DISTANCE_PROPERTY:053a018d-7c50-48e6-940a-a967b89aa976}",
                    "Distance");

CPluginObject   DistancePropertyObject(&NemesisCorePlugin,
                    DistancePropertyID,PROPERTY_CAT,
                    ":/images/NemesisCore/properties/Distance.svg",
                    DistancePropertyCB);

// -----------------------------------------------------------------------------

QObject* DistancePropertyCB(void* p_data)
{
    return(new CDistanceProperty(static_cast<CPropertyList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDistanceProperty::CDistanceProperty(CPropertyList *p_bl)
    : CGeoProperty(&DistancePropertyObject,p_bl)
{  
    PropUnit = PQ_DISTANCE;

    PointA = new CPropertyAtomList(this);
    connect(PointA,SIGNAL(OnPropertyAtomListChanged(void)),
            this,SLOT(PropertyAtomListChanged(void)));

    PointB = new CPropertyAtomList(this);
    connect(PointB,SIGNAL(OnPropertyAtomListChanged(void)),
            this,SLOT(PropertyAtomListChanged(void)));

    SET_FLAG(PropFlags,EPF_SCALAR_VALUE,true);
    SET_FLAG(PropFlags,EPF_CARTESIAN_GRADIENT,true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CDistanceProperty::IsReady(void)
{
    bool cready = true;

    cready &= PointA->GetNumberOfAtoms() > 0;
    cready &= PointB->GetNumberOfAtoms() > 0;

    return( cready );
}

//------------------------------------------------------------------------------

double CDistanceProperty::GetScalarValue(void)
{
    if( IsReady() == false ) return(0.0);
    CPoint coma = PointA->GetCOM();
    CPoint comb = PointB->GetCOM();
    return(Size(coma-comb));
}

//------------------------------------------------------------------------------

double CDistanceProperty::GetGradient(QVector<CAtomGrad>& grads)
{
    // point A
    double atotmass;
    CPoint acom = PointA->GetCOM(atotmass);
    if( atotmass == 0.0 ) return(0.0);

    // point B
    double btotmass;
    CPoint bcom = PointB->GetCOM(btotmass);
    if( btotmass == 0.0 ) return(0.0);

    // value
    CPoint dx = acom - bcom;

    // TODO PBC
//    if( fenable_pbc ) then
//        call pmf_pbc_image_vector(dx)
//    end if

    double value = Size(dx);

    // derivatives
    double sc = 0.0;
    if( value > 1e-7 ){
        sc = 1.0 / value;
    }

    // allocate space
    int numofatms = PointA->GetNumberOfAtoms() + PointB->GetNumberOfAtoms();
    grads.resize(numofatms);

    int index = 0;
    foreach(CAtom* p_atom, PointA->GetAtoms()){
        double    mass = p_atom->GetMass();
        CAtomGrad grd;
        grd.Atom = p_atom;
        grd.Grad = dx*sc*mass/atotmass;
        grads[index++] = grd;
    }

    foreach(CAtom* p_atom, PointB->GetAtoms()){
        double    mass = p_atom->GetMass();
        CAtomGrad grd;
        grd.Atom = p_atom;
        grd.Grad = -dx*sc*mass/btotmass;
        grads[index++] = grd;
    }

    return(value);
}

//------------------------------------------------------------------------------

CPropertyAtomList* CDistanceProperty::GetPointA(void)
{
    return(PointA);
}

//------------------------------------------------------------------------------

CPropertyAtomList* CDistanceProperty::GetPointB(void)
{
    return(PointB);
}

//------------------------------------------------------------------------------

bool CDistanceProperty::HasGradient(CStructure* p_structure)
{
    if( PointA->ContainsAnyAtomFrom(p_structure) ) return(true);
    if( PointB->ContainsAnyAtomFrom(p_structure) ) return(true);
    return(false);
}

//------------------------------------------------------------------------------

void CDistanceProperty::PropertyAtomListChanged(void)
{
    emit OnStatusChanged(ESC_OTHER);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDistanceProperty::LoadData(CXMLElement* p_ele)
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
}

//------------------------------------------------------------------------------

void CDistanceProperty::SaveData(CXMLElement* p_ele)
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
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDistanceProperty::Draw(void)
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

    LabelDistance();

    glDisable(GL_LINE_STIPPLE);
}

//------------------------------------------------------------------------------

void CDistanceProperty::LabelDistance(void)
{
    CSimplePoint<float> pos1;
    CSimplePoint<float> pos2;

    pos1 = PointA->GetCOM();
    pos2 = PointB->GetCOM();

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
    if( IsFlagSet(EPOF_SELECTED) ){
        glColor4fv(ColorsList.SelectionMaterial.Color);
    } else {
        glColor4fv(Setup->LineColor);
    }
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

    if( IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_RELATIVE_LABEL_POS) ){
        textpos = GetLabelPosition() + pm;
    } else {
        textpos = GetLabelPosition();
    }

    if( IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_SHOW_LABEL) ){
        QString text = PQ_DISTANCE->GetRealValueText(dis);

        if( Setup->ShowUnit == true ){
           text += " " + PQ_DISTANCE->GetUnitName();
           }

        DrawText(textpos,text);
    }

    DrawLabelQuotationLine(pm,textpos);

    DrawCOMPosition(pos1);
    DrawCOMQuotation(pos1,PointA->GetAtoms());
    DrawCOMPosition(pos2);
    DrawCOMQuotation(pos2,PointB->GetAtoms());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


