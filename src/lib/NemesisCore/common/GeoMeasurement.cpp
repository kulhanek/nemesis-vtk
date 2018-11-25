// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <GeoMeasurement.hpp>
#include <SelectionList.hpp>
#include <ErrorSystem.hpp>
#include <PointSelection.hpp>
#include <DistanceSelection.hpp>
#include <AngleSelection.hpp>
#include <TorsionSelection.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPoint CGeoMeasurement::GetPosition(CSelectionList* p_selection)
{
    if( p_selection == NULL ) return(CPoint());

    // test number of selected objects
    if(p_selection->GetRequestType() != &SH_Point) return(CPoint());

    int numofobjects = p_selection->NumOfSelectedObjects();
    if( (numofobjects  == 0) || (numofobjects > 1) ) return(CPoint());

    CGeoDescriptor descrip1;

    // get object and its type
    CProObject* p_object1 = p_selection->GetSelectedObject(0);
    descrip1 = p_object1->GetGeoDescriptor();

    // one point
    if( numofobjects == 1 ) {
        switch(descrip1.GetType()) {
        case EGDT_NILL:
            return(CPoint());
        case EGDT_ONE_POINT:
            return(descrip1.GetPoint1());

        default:
            return(CPoint());
        }
    }
    return(CPoint());
}

//------------------------------------------------------------------------------

double CGeoMeasurement::GetDistance(CSelectionList* p_selection)
{
    if( p_selection == NULL ) return(0);

    // test number of selected objects
    if(p_selection->GetRequestType() != &SH_Distance) return(0);
    int numofobjects = p_selection->NumOfSelectedObjects();
    if( (numofobjects  == 0) || (numofobjects > 2) ) return(0);

    CGeoDescriptor descrip1;
    CGeoDescriptor descrip2;

    // get objects and their type
    CProObject* p_object1 = p_selection->GetSelectedObject(0);
    descrip1 = p_object1->GetGeoDescriptor();

    // two points
    if( numofobjects == 1 ) {
        if( (descrip1.GetType() == EGDT_TWO_POINTS)
                || (descrip1.GetType() == EGDT_TWO_POINTS_ORIENTED)  ) {
            return(GetDistance(descrip1.GetPoint1(),descrip1.GetPoint2()));
        }
        ES_ERROR("not implemented");
        return(0);
    }

    // point/point, point/plane
    CProObject* object2 = p_selection->GetSelectedObject(1);
    descrip2 = object2->GetGeoDescriptor();

    CPoint point1;

    if( descrip1.GetType() == EGDT_ONE_POINT ) {
        point1 = descrip1.GetPoint1();
    } else {
        if( descrip2.GetType() == EGDT_ONE_POINT ) {
            point1 = descrip2.GetPoint1();
            descrip2 = descrip1;
        } else {
            ES_ERROR("not implemented");
            return(0);
        }
    }

    switch(descrip2.GetType()) {
    case EGDT_ONE_POINT:
        return(GetDistance(point1,descrip2.GetPoint1()));

    case EGDT_PLANE:{
        // descrip2.Point1  plane position
        // descrip2.Point2  plane normal vector
        CPoint n = descrip2.GetPoint2();
        n.Normalize();
        double d = VectDot(n,descrip2.GetPoint1());
        double s = VectDot(n,point1);
        return(s-d);
        }

    default:
        ES_ERROR("not implemented");
        return(0);
    }
}

//..............................................................................

double CGeoMeasurement::GetAngle(CSelectionList* p_selection)
{
    if( p_selection == NULL ) return(0);

    // test number of selected objects
    if(p_selection->GetRequestType() != &SH_Angle) return(0);
    int numofobjects = p_selection->NumOfSelectedObjects();
    if( (numofobjects  < 1) || (numofobjects > 3) ) return(0);

    CProObject* object1;
    CProObject* object2;
    CGeoDescriptor descrip1;
    CGeoDescriptor descrip2;

    // get objects and their type
    object1 = p_selection->GetSelectedObject(0);
    descrip1 = object1->GetGeoDescriptor();

    if( descrip1.GetType() == EGDT_THREE_POINTS) {
        return(GetAngle(descrip1.GetPoint1(),descrip1.GetPoint2(),descrip1.GetPoint3()));
    }

    object2 = p_selection->GetSelectedObject(1);
    descrip2 = object2->GetGeoDescriptor();

    switch(descrip1.GetType()) {
    case EGDT_ONE_POINT: {
        if( descrip2.GetType() == EGDT_TWO_POINTS_ORIENTED) {
            return(GetAngle(descrip1.GetPoint1(),descrip2.GetPoint1(),descrip2.GetPoint1()));
        }
        if( descrip2.GetType() != EGDT_ONE_POINT ) return(0);
        CProObject* object3 = p_selection->GetSelectedObject(2);
        CGeoDescriptor descrip3 = object3->GetGeoDescriptor();
        if( descrip3.GetType() != EGDT_ONE_POINT ) return(0);
        return(GetAngle(descrip1.GetPoint1(),descrip2.GetPoint1(),descrip3.GetPoint1()));
    }
    case EGDT_PLANE:
        if( descrip2.GetType() == EGDT_PLANE ) {
            return(Angle(descrip1.GetPoint2(),descrip2.GetPoint2()));
        }
        ES_ERROR("not implemented");
        return(0);
    default:
        ES_ERROR("not implemented");
        return(0);
    }
}

//..............................................................................

double CGeoMeasurement::GetTorsion(CSelectionList* p_selection)
{
    if( p_selection == NULL ) return(0);

    // test number of selected objects
    if(p_selection->GetRequestType() != &SH_Torsion) return(0);
    int numofobjects = p_selection->NumOfSelectedObjects();
    if( (numofobjects  < 1) || (numofobjects > 4) ) return(0);

    CProObject* object1;
    CGeoDescriptor descrip1;

    object1 = p_selection->GetSelectedObject(0);
    descrip1 = object1->GetGeoDescriptor();

    switch(descrip1.GetType()) {
    case EGDT_FOUR_POINTS: {
        return(GetTorsion(descrip1.GetPoint1(),descrip1.GetPoint2(),
                          descrip1.GetPoint3(),descrip1.GetPoint4()));
    }
    break;

    case EGDT_ONE_POINT: {
        CProObject* object2;
        CProObject* object3;
        CProObject* object4;
        CGeoDescriptor descrip2;
        CGeoDescriptor descrip3;
        CGeoDescriptor descrip4;
        object2 = p_selection->GetSelectedObject(1);
        descrip2 = object2->GetGeoDescriptor();
        if(descrip2.GetType() != EGDT_ONE_POINT) return(0);
        object3 = p_selection->GetSelectedObject(2);
        descrip3 = object3->GetGeoDescriptor();
        if(descrip2.GetType() != EGDT_ONE_POINT) return(0);
        object4 = p_selection->GetSelectedObject(3);
        descrip4 = object4->GetGeoDescriptor();
        if(descrip2.GetType() != EGDT_ONE_POINT) return(0);

        return(GetTorsion(descrip1.GetPoint1(),descrip2.GetPoint1(),
                          descrip3.GetPoint1(),descrip4.GetPoint1()));
    }

    case EGDT_TWO_POINTS_ORIENTED: {
        CProObject* object2;
        CGeoDescriptor descrip2;
        object2 = p_selection->GetSelectedObject(2);
        descrip2 = object2->GetGeoDescriptor();
        if(descrip2.GetType() != EGDT_TWO_POINTS_ORIENTED) return(0);
        return(GetTorsion(descrip1.GetPoint1(),descrip1.GetPoint2(),
                          descrip2.GetPoint1(),descrip2.GetPoint2()));
    }

    case EGDT_PLANE: {
        ES_ERROR("not implemented");
    }
    return(0);

    default:
        return(0);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

double CGeoMeasurement::GetDistance(const CPoint& p1,const CPoint& p2)
{
    CPoint dif = p2-p1;
    return(Size(dif));
}

//------------------------------------------------------------------------------

double CGeoMeasurement::GetAngle(const CPoint& p1,
                                 const CPoint& p2,
                                 const CPoint& p3)
{
    CPoint v1,v2;
    v1 = p1-p2;
    v2 = p3-p2;

    double top = VectDot(v1,v2);
    double down = Size(v1)*Size(v2);

    if( down < 0.1e-7 ) return(0.0);
    double ac = top / down;
    if( ac >  1.0 ) ac = 1.0;
    if( ac < -1.0 ) ac = -1.0;

    return( acos( ac) );
}

//------------------------------------------------------------------------------

#define SIGN(a,b)  ((b >= 0.0) ? (a) : -(a))

double CGeoMeasurement::GetTorsion(const CPoint& p1,const CPoint& p2,
                                   const CPoint& p3,const CPoint& p4)
{
    float iax ,iay ,iaz ,ibx ,iby ,ibz ;
    float icx ,icy ,icz ,idx ,idy ,idz ;

    iax = p1.x;
    iay = p1.y;
    iaz = p1.z;

    ibx =p2.x;
    iby = p2.y;
    ibz = p2.z;

    icx = p3.x;
    icy = p3.y;
    icz = p3.z;

    idx = p4.x;
    idy = p4.y;
    idz = p4.z;

    float eabx,eaby,eabz;
    float ebcx,ebcy,ebcz;
    float ecdx,ecdy,ecdz;
    float abbcx,abbcy,abbcz;
    float dccbx,dccby,dccbz;
    float abcdx,abcdy,abcdz,signum;
    float cosdel,xrcd;
    float rab,rbc,rcd,xrbc,xrab,cosb,phib,xsinb,cosc,phic,xsinc;

    //       bond lengths and unit vectors

    eabx = ibx - iax;
    eaby = iby - iay;
    eabz = ibz - iaz;

    rab = sqrt (eabx * eabx + eaby * eaby + eabz * eabz);
    xrab = 1.0 / rab;
    eabx = eabx * xrab;
    eaby = eaby * xrab;
    eabz = eabz * xrab;
    ebcx = icx - ibx;
    ebcy = icy - iby;
    ebcz = icz - ibz;

    rbc = sqrt (ebcx * ebcx + ebcy * ebcy + ebcz * ebcz);
    xrbc = 1.0 / rbc;
    ebcx = ebcx * xrbc;
    ebcy = ebcy * xrbc;
    ebcz = ebcz * xrbc;
    ecdx = idx - icx;
    ecdy = idy - icy;
    ecdz = idz - icz;

    rcd = sqrt (ecdx * ecdx + ecdy * ecdy + ecdz * ecdz);
    xrcd = 1.0 / rcd;
    ecdx = ecdx * xrcd;
    ecdy = ecdy * xrcd;
    ecdz = ecdz * xrcd;

    //       cross and dot products between unit vectors, and bond (!)
    //       angles

    abbcx = eaby * ebcz - eabz * ebcy;
    abbcy = eabz * ebcx - eabx * ebcz;
    abbcz = eabx * ebcy - eaby * ebcx;
    cosb = - (eabx * ebcx + eaby * ebcy + eabz * ebcz);
    phib = acos(cosb);
    xsinb = 1.0 / sin(phib);
    dccbx = ecdy * ebcz - ecdz * ebcy;
    dccby = ecdz * ebcx - ecdx * ebcz;
    dccbz = ecdx * ebcy - ecdy * ebcx;
    cosc = - (ecdx * ebcx + ecdy * ebcy + ecdz * ebcz);
    phic = acos(cosc);
    xsinc = 1.0 / sin(phic);

    //        torsional angle

    abcdx = - ( abbcy * dccbz - abbcz * dccby );
    abcdy = - ( abbcz * dccbx - abbcx * dccbz );
    abcdz = - ( abbcx * dccby - abbcy * dccbx );
    signum = SIGN(1.0,
                  (abcdx * ebcx + abcdy * ebcy + abcdz * ebcz) );
    cosdel = - (abbcx * dccbx + abbcy * dccby + abbcz * dccbz)
             *  xsinb * xsinc;

    if(cosdel < -1.0) cosdel = -1.0;
    if(cosdel >  1.0) cosdel =  1.0;

    float  angle = signum * acos (cosdel);

    return(angle);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomGrad::CAtomGrad(void)
{
    Atom = NULL;
}

//------------------------------------------------------------------------------

