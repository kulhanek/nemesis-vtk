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

#include <PBCInfo.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <float.h>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPBCInfo::CPBCInfo(void)
{
    // PBC
    PeriodicA = false;
    PeriodicB = false;
    PeriodicC = false;
    ValidData = false;

    A = 0.0;
    B = 0.0;
    C = 0.0;
    Alpha = 0.0;
    Beta = 0.0;
    Gamma = 0.0;

    memset(UCELL,0,sizeof(UCELL));
    memset(RECIP,0,sizeof(RECIP));

    Volume = 0.0;
    Radius = 0.0;
}

//------------------------------------------------------------------------------

CPBCInfo::CPBCInfo(const CPBCInfo& src)
{
    SetDimmensions(src.GetSizes(),src.GetAngles());
    SetPeriodicity(src.IsPeriodicAlongA(),src.IsPeriodicAlongB(),src.IsPeriodicAlongC());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CPBCInfo::IsPBCEnabled(void) const
{
    return( (PeriodicA || PeriodicB || PeriodicC) && ValidData );
}

//------------------------------------------------------------------------------

bool CPBCInfo::Is3DPBCEnabled(void) const
{
    return( PeriodicA && PeriodicB && PeriodicC && ValidData );
}

//------------------------------------------------------------------------------

bool CPBCInfo::IsValid(void) const
{
    return( ValidData );
}

//------------------------------------------------------------------------------

bool CPBCInfo::IsPeriodicAlongA(void) const
{
    return(PeriodicA);
}

//------------------------------------------------------------------------------

bool CPBCInfo::IsPeriodicAlongB(void) const
{
    return(PeriodicB);
}

//------------------------------------------------------------------------------

bool CPBCInfo::IsPeriodicAlongC(void) const
{
    return(PeriodicC);
}

//------------------------------------------------------------------------------

bool CPBCInfo::IsRectangularBox(void) const
{
    if( IsPBCEnabled() ) return(false);
    bool ok = true;

    if( abs(Alpha - M_PI/2.0) > 0.0001 ) ok = false;
    if( abs(Beta - M_PI/2.0) > 0.0001 ) ok = false;
    if( abs(Gamma - M_PI/2.0) > 0.0001 ) ok = false;

    return(ok);
}

//------------------------------------------------------------------------------

CPoint CPBCInfo::GetSizes(void) const
{
    CPoint sizes(A,B,C);
    return(sizes);
}

//------------------------------------------------------------------------------

double CPBCInfo::GetAVectorSize(void) const
{
    return(A);
}

//------------------------------------------------------------------------------

double CPBCInfo::GetBVectorSize(void) const
{
    return(B);
}

//------------------------------------------------------------------------------

double CPBCInfo::GetCVectorSize(void) const
{
    return(C);
}

//------------------------------------------------------------------------------

CPoint CPBCInfo::GetAngles(void) const
{
    CPoint angles(Alpha,Beta,Gamma);
    return(angles);
}

//------------------------------------------------------------------------------

double CPBCInfo::GetAlpha(void) const
{
    return(Alpha);
}

//------------------------------------------------------------------------------

double CPBCInfo::GetBeta(void) const
{
    return(Beta);
}

//------------------------------------------------------------------------------

double CPBCInfo::GetGamma(void) const
{
    return(Gamma);
}

//------------------------------------------------------------------------------

double  CPBCInfo::GetVolume(void) const
{
    if( IsPBCEnabled() ) return(Volume);
    return(0.0);
}

//------------------------------------------------------------------------------

double  CPBCInfo::GetLargestSphereRadius(void) const
{
    if( IsPBCEnabled() ) return(Radius);
    return(0.0);
}

//------------------------------------------------------------------------------

const CPoint CPBCInfo::GetBoxCenter(void) const
{
    CPoint box_center;
    if( ! IsPBCEnabled() ) return(box_center);

    box_center.x = 0.5*UCELL[0][0] + 0.5*UCELL[0][1] + 0.5*UCELL[0][2];
    box_center.y = 0.5*UCELL[1][0] + 0.5*UCELL[1][1] + 0.5*UCELL[1][2];
    box_center.z = 0.5*UCELL[2][0] + 0.5*UCELL[2][1] + 0.5*UCELL[2][2];
    return(box_center);
}

//------------------------------------------------------------------------------

const CPoint CPBCInfo::GetAVector(void) const
{
    CPoint vec;
    if( ! IsPBCEnabled() ) return(vec);
    vec.x = UCELL[0][0];
    vec.y = UCELL[1][0];
    vec.z = UCELL[2][0];
    return(vec);
}

//------------------------------------------------------------------------------

const CPoint CPBCInfo::GetBVector(void) const
{
    CPoint vec;
    if( ! IsPBCEnabled() ) return(vec);
    vec.x = UCELL[0][1];
    vec.y = UCELL[1][1];
    vec.z = UCELL[2][1];
    return(vec);
}

//------------------------------------------------------------------------------

const CPoint CPBCInfo::GetCVector(void) const
{
    CPoint vec;
    if( ! IsPBCEnabled() ) return(vec);
    vec.x = UCELL[0][2];
    vec.y = UCELL[1][2];
    vec.z = UCELL[2][2];
    return(vec);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPBCInfo::SetDimmensions(double a, double b, double c, double alpha, double beta, double gamma)
{
    A = a;
    B = b;
    C = c;
    Alpha = alpha;
    Beta = beta;
    Gamma = gamma;

    ValidData = true;

    memset(UCELL,0,sizeof(UCELL));
    memset(RECIP,0,sizeof(RECIP));

    Volume = 0.0;
    Radius = 0.0;

    if( (A <= 0) || (B <= 0) || (C <= 0) ) {
        ValidData = false;
        return;
    }

    if( (Alpha <= 0) || (Beta <= 0) || (Gamma <= 0) ) {
        ValidData = false;
        return;
    }

    if( (Alpha > M_PI) || (Beta > M_PI) || (Gamma > M_PI) ) {
        ValidData = false;
        return;
    }

    // UCELL - orthogonalization matrix
    UCELL[0][0] = A;
    UCELL[1][0] = 0.0;
    UCELL[2][0] = 0.0;

    UCELL[0][1] = B*cos(Gamma);
    UCELL[1][1] = B*sin(Gamma);
    UCELL[2][1] = 0.0;

    UCELL[0][2] = C*cos(Beta);
    if( UCELL[1][1] > 0 ){
        UCELL[1][2] = (B*C*cos(Alpha) - UCELL[0][2]*UCELL[0][1])/UCELL[1][1];
    } else {
        memset(UCELL,0,sizeof(UCELL));
        memset(RECIP,0,sizeof(RECIP));
        Volume = 0.0;
        Radius = 0.0;
        ValidData = false;
        return;
    }
    if( C*C - UCELL[0][2]*UCELL[0][2] - UCELL[1][2]*UCELL[1][2] > 0) {
        UCELL[2][2] = sqrt(C*C - UCELL[0][2]*UCELL[0][2] - UCELL[1][2]*UCELL[1][2]);
    } else {
        memset(UCELL,0,sizeof(UCELL));
        memset(RECIP,0,sizeof(RECIP));
        Volume = 0.0;
        Radius = 0.0;
        ValidData = false;
        return;
    }

    //         for(int i=0; i < 3; i++){
    //             for(int j=0; j < 3; j++){
    //                 printf("%f ",UCELL[i][j]);
    //                 }
    //             printf("\n");
    //             }

    double u12[3];
    double u20[3];
    double u01[3];

    // RECIP - deorthogonalization matrix
    u12[0] = UCELL[1][1]*UCELL[2][2] - UCELL[2][1]*UCELL[1][2];
    u12[1] = UCELL[2][1]*UCELL[0][2] - UCELL[0][1]*UCELL[2][2];
    u12[2] = UCELL[0][1]*UCELL[1][2] - UCELL[1][1]*UCELL[0][2];

    u20[0] = UCELL[1][2]*UCELL[2][0] - UCELL[2][2]*UCELL[1][0];
    u20[1] = UCELL[2][2]*UCELL[0][0] - UCELL[0][2]*UCELL[2][0];
    u20[2] = UCELL[0][2]*UCELL[1][0] - UCELL[1][2]*UCELL[0][0];

    u01[0] = UCELL[1][0]*UCELL[2][1] - UCELL[2][0]*UCELL[1][1];
    u01[1] = UCELL[2][0]*UCELL[0][1] - UCELL[0][0]*UCELL[2][1];
    u01[2] = UCELL[0][0]*UCELL[1][1] - UCELL[1][0]*UCELL[0][1];

    Volume = UCELL[0][0]*u12[0] + UCELL[1][0]*u12[1]+ UCELL[2][1]*u12[2];

    if( Volume > 0 ){
        for(int i=0; i < 3; i++) RECIP[0][i] = u12[i] / Volume;
        for(int i=0; i < 3; i++) RECIP[1][i] = u20[i] / Volume;
        for(int i=0; i < 3; i++) RECIP[2][i] = u01[i] / Volume;
    } else {
        memset(UCELL,0,sizeof(UCELL));
        memset(RECIP,0,sizeof(RECIP));
        Volume = 0.0;
        Radius = 0.0;
        ValidData = false;
        return;
    }

    //         for(int i=0; i < 3; i++){
    //             for(int j=0; j < 3; j++){
    //                 printf("%f ",RECIP[i][j]);
    //                 }
    //             printf("\n");
    //             }

    Radius = a * b * c;

    for(int i=0; i < 3; i++) {
        double dist = 0;
        double tmp = RECIP[i][0]*RECIP[i][0] + RECIP[i][1]*RECIP[i][1] + RECIP[i][2]*RECIP[i][2];
        if( tmp > 0 ) {
            dist = (RECIP[i][0]*UCELL[0][i] + RECIP[i][1]*UCELL[1][i] + RECIP[i][2]*UCELL[2][i]) / sqrt(tmp);
        } else {
            memset(UCELL,0,sizeof(UCELL));
            memset(RECIP,0,sizeof(RECIP));
            Volume = 0.0;
            Radius = 0.0;
            ValidData = false;
            return;
        }
        if( dist < Radius ) Radius = dist;
    }
    Radius = 0.5 * Radius;
}

//------------------------------------------------------------------------------

void CPBCInfo::SetDimmensions(const CPoint& sizes, const CPoint& angles)
{
    SetDimmensions(sizes.x,sizes.y,sizes.z,angles.x,angles.y,angles.z);
}

//------------------------------------------------------------------------------

void CPBCInfo::SetPeriodicity(bool pa, bool pb, bool pc)
{
    PeriodicA = pa;
    PeriodicB = pb;
    PeriodicC = pc;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const CPoint CPBCInfo::ImagePoint(const CPoint& pos,
                                  int kx,int ky,int kz,
                                  bool origin,bool familiar)
{
    CPoint ipos;

    if( ! IsPBCEnabled() ){
        return(pos);
    }

    if( IsRectangularBox() ){
        double fx,fy,fz;
        // base cell offset
        fx = kx;
        fy = ky;
        fz = kz;

        // origin offset
        if( origin ) {
            fx -= 0.5;
            fy -= 0.5;
            fz -= 0.5;
        }
        ipos.x = pos.x - UCELL[0][0]*floor(pos.x*RECIP[0][0]) + fx*UCELL[0][0];
        ipos.y = pos.y - UCELL[1][1]*floor(pos.y*RECIP[1][1]) + fy*UCELL[1][1];
        ipos.z = pos.z - UCELL[2][2]*floor(pos.z*RECIP[2][2]) + fz*UCELL[2][2];

        return(ipos);
    }

    // calculate fractional coordinates
    double fx,fy,fz;
    fx = pos.x*RECIP[0][0] + pos.y*RECIP[0][1] + pos.z*RECIP[0][2];
    fy = pos.x*RECIP[1][0] + pos.y*RECIP[1][1] + pos.z*RECIP[1][2];
    fz = pos.x*RECIP[2][0] + pos.y*RECIP[2][1] + pos.z*RECIP[2][2];

    // in which are we?
    double ffx,ffy,ffz;
    ffx = floor(fx);
    ffy = floor(fy);
    ffz = floor(fz);

    // do familiar imaging
    if( familiar ) {
        double rfx,rfy,rfz;
        rfx = fx - floor(fx);
        rfy = fy - floor(fy);
        rfz = fz - floor(fz);

        // coordinates of cell centre
        double ox,oy,oz;
        ox = 0.5*UCELL[0][0] + 0.5*UCELL[0][1] + 0.5*UCELL[0][2];
        oy = 0.5*UCELL[1][0] + 0.5*UCELL[1][1] + 0.5*UCELL[1][2];
        oz = 0.5*UCELL[2][0] + 0.5*UCELL[2][1] + 0.5*UCELL[2][2];

        int lx,ly,lz;
        int mx=0,my=0,mz=0;

        // this is just upper guess of possible distance
        double min_dis = DBL_MAX;

        for(lx=-1; lx <= 1; lx++) {
            for(ly=-1; ly <= 1; ly++) {
                for(lz=-1; lz <= 1; lz++) {
                    double ofx,ofy,ofz;
                    ofx = rfx + lx;
                    ofy = rfy + ly;
                    ofz = rfz + lz;

                    double px,py,pz;
                    px = ofx*UCELL[0][0] + ofy*UCELL[0][1] + ofz*UCELL[0][2];
                    py = ofx*UCELL[1][0] + ofy*UCELL[1][1] + ofz*UCELL[1][2];
                    pz = ofx*UCELL[2][0] + ofy*UCELL[2][1] + ofz*UCELL[2][2];

                    double ds = (px-ox)*(px-ox) + (py-oy)*(py-oy) + (pz-oz)*(pz-oz);

                    if( ds < min_dis ) {
                        mx = lx;
                        my = ly;
                        mz = lz;
                        min_dis = ds;
                    }
                }
            }
        }
        ffx -= mx;
        ffy -= my;
        ffz -= mz;
    }

    // base cell offset
    ffx -= kx;
    ffy -= ky;
    ffz -= kz;

    // origin offset
    if( origin ) {
        ffx += 0.5;
        ffy += 0.5;
        ffz += 0.5;
    }

    double dx,dy,dz;

    dx = ffx*UCELL[0][0] + ffy*UCELL[0][1] + ffz*UCELL[0][2];
    dy = ffx*UCELL[1][0] + ffy*UCELL[1][1] + ffz*UCELL[1][2];
    dz = ffx*UCELL[2][0] + ffy*UCELL[2][1] + ffz*UCELL[2][2];
    // image point
    ipos.x = pos.x - dx;
    ipos.y = pos.y - dy;
    ipos.z = pos.z - dz;
    return(ipos);
}

//------------------------------------------------------------------------------

const CPoint CPBCInfo::ImageVector(const CPoint& vec)
{
    CPoint ipos = GetBoxCenter() + vec;
    ipos = ImagePoint(ipos,0,0,0,false,true);
    ipos = ipos - GetBoxCenter();
    return(ipos);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPBCInfo::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }

    if( p_ele->GetName() != "pbc" ) {
        LOGIC_ERROR("p_ele is not pbc");
    }

    bool    pa = false;
    bool    pb = false;
    bool    pc = false;
    double  a = 0;
    double  b = 0;
    double  c = 0;
    double  aa = 0;
    double  ab = 0;
    double  ac = 0;

    p_ele->GetAttribute("pa",pa);
    p_ele->GetAttribute("pb",pb);
    p_ele->GetAttribute("pc",pc);

    p_ele->GetAttribute("a",a);
    p_ele->GetAttribute("b",b);
    p_ele->GetAttribute("c",c);

    p_ele->GetAttribute("aa",aa);
    p_ele->GetAttribute("ab",ab);
    p_ele->GetAttribute("ac",ac);

    SetPeriodicity(pa,pb,pc);
    SetDimmensions(a,b,c,aa,ab,ac);
}

//------------------------------------------------------------------------------

void CPBCInfo::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }

    p_ele->SetAttribute("pa",PeriodicA);
    p_ele->SetAttribute("pb",PeriodicB);
    p_ele->SetAttribute("pc",PeriodicC);

    p_ele->SetAttribute("a",A);
    p_ele->SetAttribute("b",B);
    p_ele->SetAttribute("c",C);

    p_ele->SetAttribute("aa",Alpha);
    p_ele->SetAttribute("ab",Beta);
    p_ele->SetAttribute("ac",Gamma);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



