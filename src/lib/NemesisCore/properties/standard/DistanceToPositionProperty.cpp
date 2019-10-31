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

#include <DistanceToPositionProperty.hpp>
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
#include <DistanceToPositionPropertyHistory.hpp>
#include <ElementColorsList.hpp>

#if defined _WIN32 || defined __CYGWIN__
#undef DrawText
#endif

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DistanceToPositionPropertyCB(void* p_data);

CExtUUID        DistanceToPositionPropertyID(
                    "{DISTANCE_TO_POSITION_PROPERTY:aa3ddded-6025-445f-9ff5-ba498425892c}",
                    "Distance to position");

CPluginObject   DistanceToPositionPropertyObject(&NemesisCorePlugin,
                    DistanceToPositionPropertyID,PROPERTY_CAT,
                    ":/images/NemesisCore/properties/DistanceToPosition.svg",
                    DistanceToPositionPropertyCB);

// -----------------------------------------------------------------------------

QObject* DistanceToPositionPropertyCB(void* p_data)
{
    return(new CDistanceToPositionProperty(static_cast<CPropertyList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDistanceToPositionProperty::CDistanceToPositionProperty(CPropertyList *p_bl)
    : CGeoProperty(&DistanceToPositionPropertyObject,p_bl)
{  
    PropUnit = PQ_DISTANCE;

    PointA = new CPropertyAtomList(this);
    connect(PointA,SIGNAL(OnPropertyAtomListChanged(void)),
            this,SLOT(PropertyAtomListChanged(void)));

    SET_FLAG(PropFlags,EPF_SCALAR_VALUE,true);
    SET_FLAG(PropFlags,EPF_CARTESIAN_GRADIENT,true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CDistanceToPositionProperty::SetPointBWH(const CPoint& pb)
{
    if( PointB == pb ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_PROPERTY,tr("set position of point B"));
    if( p_history == NULL ) return (false);

    SetPointB(pb,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

void CDistanceToPositionProperty::SetPointB(const CPoint& pb,CHistoryNode* p_history)
{
    if( PointB == pb ) return;

    if( p_history ){
        CHistoryItem* p_item = new CDistanceToPositionPropertyChangePointBHI(this,pb);
        p_history->Register(p_item);
    }

    PointB = pb;
    emit OnStatusChanged(ESC_OTHER);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CDistanceToPositionProperty::IsReady(void)
{
    bool cready = true;

    cready &= PointA->GetNumberOfAtoms() > 0;

    return( cready );
}

//------------------------------------------------------------------------------

double CDistanceToPositionProperty::GetScalarValue(void)
{
    if( IsReady() == false ) return(0.0);
    CPoint coma = PointA->GetCOM();
    CPoint comb = PointB;
    return(Size(coma-comb));
}

//------------------------------------------------------------------------------

double CDistanceToPositionProperty::GetGradient(QVector<CAtomGrad>& grads)
{
    // gradient of point A
    double atotmass;
    CPoint acom = PointA->GetCOM(atotmass);
    if( atotmass == 0.0 ) return(0.0);

    // gradient of point B
    CPoint bcom = PointB;

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
    int numofatms = PointA->GetNumberOfAtoms();
    grads.resize(numofatms);

    int index = 0;
    foreach(CAtom* p_atom, PointA->GetAtoms()){
        double    mass = p_atom->GetMass();
        CAtomGrad grd;
        grd.Atom = p_atom;
        grd.Grad = dx*sc*mass/atotmass;
        grads[index++] = grd;
    }

    return(value);
}

//------------------------------------------------------------------------------

CPropertyAtomList* CDistanceToPositionProperty::GetPointA(void)
{
    return(PointA);
}

//------------------------------------------------------------------------------

const CPoint& CDistanceToPositionProperty::GetPointB(void)
{
    return(PointB);
}

//------------------------------------------------------------------------------

bool CDistanceToPositionProperty::HasGradient(CStructure* p_structure)
{
    if( PointA->ContainsAnyAtomFrom(p_structure) ) return(true);
    return(false);
}

//------------------------------------------------------------------------------

void CDistanceToPositionProperty::PropertyAtomListChanged(void)
{
    emit OnStatusChanged(ESC_OTHER);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDistanceToPositionProperty::LoadData(CXMLElement* p_ele)
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

    p_ele->GetAttribute("pbx",PointB.x);
    p_ele->GetAttribute("pby",PointB.y);
    p_ele->GetAttribute("pbz",PointB.z);
}

//------------------------------------------------------------------------------

void CDistanceToPositionProperty::SaveData(CXMLElement* p_ele)
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

    p_ele->SetAttribute("pbx",PointB.x);
    p_ele->SetAttribute("pby",PointB.y);
    p_ele->SetAttribute("pbz",PointB.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDistanceToPositionProperty::Draw(void)
{
    if( IsReady() == false ) return;

    Setup = GetSetup<CGeoPropertySetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

//    glDisable(GL_LIGHTING);
//    glDisable(GL_COLOR_MATERIAL);

//    GLLoadObject(this);

//    LabelDistance();

//    glDisable(GL_LINE_STIPPLE);
}

//------------------------------------------------------------------------------

void CDistanceToPositionProperty::LabelDistance(void)
{
    CSimplePoint<float> pos1;
    CSimplePoint<float> pos2;

    pos1 = PointA->GetCOM();
    pos2 = PointB;

    // calculate distance ------------------------
    double              dis;
    dis = CGeoMeasurement::GetDistance(pos1,pos2);

    // draw distance -----------------------------
//    glLineWidth(Setup->LineWidth);
//    if( Setup->LineStippleFactor > 0 ){
//        glEnable(GL_LINE_STIPPLE);
//        glLineStipple(Setup->LineStippleFactor,Setup->LineStipplePattern);
//    } else {
//        glDisable(GL_LINE_STIPPLE);
//    }
//    if( IsFlagSet(EPOF_SELECTED) ){
//        glColor4fv(ColorsList.SelectionMaterial.Color);
//    } else {
//        glColor4fv(Setup->LineColor);
//    }
//    glBegin(GL_LINES);
//        glVertex3fv(pos1);
//        glVertex3fv(pos2);
//    glEnd();
//    glDisable(GL_LINE_STIPPLE);

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
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


