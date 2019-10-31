// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <GraphicsUtil.hpp>
#include <Transformation.hpp>
#include <IndexCounter.hpp>
#include <ProObject.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSphere::CSphere(void)
{
    Vertices = NULL;
    NumOfVertices = 0;
    Tessellation = 0;
    SetTessellationQuality(3);
}

//---------------------------------------------------------------------------

CSphere::~CSphere(void)
{
    if( Vertices != NULL ) delete[] Vertices;
}

//---------------------------------------------------------------------------

void CSphere::Draw(const float radius)
{
    CSimplePoint<float>  v;
    CSimplePoint<float>* data = Vertices;
    unsigned int           i=0;

//    glBegin(GL_TRIANGLES);
//    while(i < NumOfVertices) {
//        v = *data;
//        glNormal3fv(v);
//        v *= radius;
//        glVertex3fv(v);
//        data++;
//        i++;
//    }
//    glEnd();
//
}

//---------------------------------------------------------------------------

bool CSphere::SetTessellationQuality(const unsigned int quality)
{
    if( quality == Tessellation ) return(true);
    if( (quality < 1 ) || (quality>8) )return(false);

    Tessellation = quality;

    if( Vertices != NULL ) {
        delete[] Vertices;
    }

    NumOfVertices = 3*20*pow(4,Tessellation-1);

    Vertices = new CSimplePoint<float> [NumOfVertices];

    ComputeVertices();

    return(true);
}

//---------------------------------------------------------------------------

void CSphere::ComputeVertices(void)
{
    float x = 0.525731112119133606f, z = 0.850650808352039932f;

    static float vdata[12][3] = {
        {-x, 0, z}, {x, 0, z}, {-x, 0, -z}, {x, 0, -z},
        {0, z, x}, {0, z, -x}, {0, -z, x}, {0, -z, -x},
        {z, x, 0}, {-z, x, 0}, {z, -x, 0}, {-z, -x, 0}
    };

    static int tindices[20][3] = {
        {0, 4, 1}, {0, 9, 4}, {9, 5, 4}, {4, 5, 8}, {4, 8, 1},
        {8, 10, 1}, {8, 3, 10}, {5, 3, 8}, {5, 2, 3}, {2, 7, 3},
        {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1, 6},
        {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5}, {7, 2, 11}
    };

    DummyPos = 0;
    for (unsigned int i = 0; i < 20; i++)
        ComputePartition(((CSimplePoint<float>*)vdata[tindices[i][0]])[0], ((CSimplePoint<float>*)vdata[tindices[i][1]])[0], ((CSimplePoint<float>*)vdata[tindices[i][2]])[0],Vertices,Tessellation - 1);

}

//---------------------------------------------------------------------------

void CSphere::ComputePartition(const CSimplePoint<float>& v1,
                               const CSimplePoint<float>& v2,
                               const CSimplePoint<float>& v3,
                               CSimplePoint<float> data[],
                               const unsigned int complexity)
{
    if (complexity == 0) {
        data[DummyPos++] = v1;
        data[DummyPos++] = v2;
        data[DummyPos++] = v3;
    } else {
        CSimplePoint<float> v12,v23,v31;
        v12 = v1 + v2;
        v23 = v2 + v3;
        v31 = v3 + v1;
        v12.Normalize();
        v23.Normalize();
        v31.Normalize();
        ComputePartition(v1,v12,v31,data,complexity - 1);
        ComputePartition(v2,v23,v12,data,complexity - 1);
        ComputePartition(v3,v31,v23,data,complexity - 1);
        ComputePartition(v12,v23,v31,data,complexity - 1);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CHalfSphere::CHalfSphere(void)
{
    Vertices = NULL;
    Tessellation = 0;
    NumOfVertices = 0;
    SetTessellationQuality(3);
}

//---------------------------------------------------------------------------

CHalfSphere::~CHalfSphere(void)
{
    if( Vertices != NULL ) delete[] Vertices;
}

//---------------------------------------------------------------------------

void CHalfSphere::Draw(const float radius)
{
    CSimplePoint<float>  v;
    CSimplePoint<float>* data = Vertices;
    unsigned int           i=0;

//    glBegin(GL_TRIANGLES);
//    while(i < NumOfVertices) {
//        v = *data;
//        glNormal3fv(v);
//        v *= radius;
//        glVertex3fv(v);
//        data++;
//        i++;
//    }
//    glEnd();
}

//---------------------------------------------------------------------------

bool CHalfSphere::SetTessellationQuality(const unsigned int quality)
{
    if( quality == Tessellation ) return(true);
    if( (quality < 1 ) || (quality>8) )return(false);

    Tessellation = quality;

    if( Vertices != NULL ) {
        delete[] Vertices;
    }

    NumOfVertices = 3*20*pow(4,Tessellation-1);

    Vertices = new CSimplePoint<float> [NumOfVertices];

    ComputeVertices();

    return(true);
}

//---------------------------------------------------------------------------

void CHalfSphere::ComputeVertices(void)
{
    float x = 0.525731112119133606f, z = 0.850650808352039932f;

    static float vdata[12][3] = {
        {-x, 0, z}, {x, 0, z}, {-x, 0, -z}, {x, 0, -z},
        {0, z, x}, {0, z, -x}, {0, -z, x}, {0, -z, -x},
        {z, x, 0}, {-z, x, 0}, {z, -x, 0}, {-z, -x, 0}
    };

    static int tindices[20][3] = {
        {0, 4, 1}, {0, 9, 4}, {9, 5, 4}, {4, 5, 8}, {4, 8, 1},
        {8, 10, 1}, {8, 3, 10}, {5, 3, 8}, {5, 2, 3}, {2, 7, 3},
        {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1, 6},
        {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5}, {7, 2, 11}
    };

    DummyPos = 0;
    for (unsigned int i = 0; i < 20; i++)
        ComputePartition(((CSimplePoint<float>*)vdata[tindices[i][0]])[0], ((CSimplePoint<float>*)vdata[tindices[i][1]])[0], ((CSimplePoint<float>*)vdata[tindices[i][2]])[0],Vertices,Tessellation - 1);

}

//---------------------------------------------------------------------------

void CHalfSphere::ComputePartition(const CSimplePoint<float>& v1,const CSimplePoint<float>& v2,const CSimplePoint<float>& v3,CSimplePoint<float> data[],const unsigned int complexity)
{

    if (complexity == 0) {
        data[DummyPos++] = v1;
        data[DummyPos++] = v2;
        data[DummyPos++] = v3;
    } else {
        CSimplePoint<float> v12,v23,v31;
        v12 = v1 + v2;
        v23 = v2 + v3;
        v31 = v3 + v1;
        v12.Normalize();
        v23.Normalize();
        v31.Normalize();
        ComputePartition(v1,v12,v31,data,complexity - 1);
        ComputePartition(v2,v23,v12,data,complexity - 1);
        ComputePartition(v3,v31,v23,data,complexity - 1);
        ComputePartition(v12,v23,v31,data,complexity - 1);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCylinder::CCylinder(void)
{
    Vertices = NULL;
    NumOfFaces = 0;
    Tessellation = 0;
    SetTessellationQuality(3);
}

//---------------------------------------------------------------------------

CCylinder::~CCylinder(void)
{
    if( Vertices != NULL ) delete[] Vertices;
}

//---------------------------------------------------------------------------

void CCylinder::Draw(const float radius,const float height)
{
    CSimplePoint<float>  v;
    CSimplePoint<float>  n;

    CSimplePoint<float>* data = Vertices;
    unsigned int           i=0;

//    glBegin(GL_QUAD_STRIP);
//    while(i < NumOfFaces) {
//        n = *data;
//        v = n;
//        glNormal3fv(n);
//        v.x *= radius;
//        v.y *= radius;
//        glVertex3fv(v);
//        glNormal3fv(n);
//        v.z += height;
//        glVertex3fv(v);
//        data++;
//        i++;
//    }
//    n = *Vertices;
//    v = n;
//    glNormal3fv(n);
//    v.x *= radius;
//    v.y *= radius;
//    glVertex3fv(v);
//    glNormal3fv(n);
//    v.z += height;
//    glVertex3fv(v);

//    glEnd();
}

//---------------------------------------------------------------------------

void CCylinder::Draw(const float radius1,const float radius2,const float height)
{
    CSimplePoint<float>  v;
    CSimplePoint<float>  n;

    CSimplePoint<float>* data = Vertices;
    unsigned int           i=0;

//    glBegin(GL_QUAD_STRIP);
//    while(i < NumOfFaces) {
//        n = *data;
//        v = n;
//        glNormal3fv(n);
//        v.x *= radius1;
//        v.y *= radius1;
//        glVertex3fv(v);
//        glNormal3fv(n);
//        v = n;
//        v.x *= radius2;
//        v.y *= radius2;
//        v.z += height;
//        glVertex3fv(v);
//        data++;
//        i++;
//    }
//    n = *Vertices;
//    v = n;
//    glNormal3fv(n);
//    v.x *= radius1;
//    v.y *= radius1;
//    glVertex3fv(v);
//    glNormal3fv(n);
//    v = n;
//    v.x *= radius2;
//    v.y *= radius2;
//    v.z += height;
//    glVertex3fv(v);

//    glEnd();
}

//---------------------------------------------------------------------------

void CCylinder::Draw(const float radius,const float height,const CColor* color1,const CColor* color2)
{
    CSimplePoint<float>  v,n;
    CSimplePoint<float>* data = Vertices;
    unsigned int           i=0;

//    glBegin(GL_QUAD_STRIP);
//    while(i < NumOfFaces) {
//        n = *data;
//        v = n;
//        glNormal3fv(n);
//        v.x *= radius;
//        v.y *= radius;
//        glColor4fv(*color1);
//        glVertex3fv(v);
//        glNormal3fv(n);
//        v.z += height;
//        glColor4fv(*color2);
//        glVertex3fv(v);
//        data++;
//        i++;
//    }
//    n = *Vertices;
//    v = n;
//    glNormal3fv(n);
//    v.x *= radius;
//    v.y *= radius;
//    glColor4fv(*color1);
//    glVertex3fv(v);
//    glNormal3fv(n);
//    v.z += height;
//    glColor4fv(*color2);
//    glVertex3fv(v);
//    glEnd();
}

//---------------------------------------------------------------------------

void CCylinder::DrawWithMaterialColors(const float radius,const float height,
                                       const CElementColors* color1,
                                       const CElementColors* color2)
{
    CSimplePoint<float>  v,n;
    CSimplePoint<float>* data = Vertices;
    unsigned int           i=0;

//    glBegin(GL_QUAD_STRIP);
//    while(i < NumOfFaces) {
//        n = *data;
//        v = n;
//        glNormal3fv(n);
//        v.x *= radius;
//        v.y *= radius;
//        color1->ApplyMaterialColor();
//        glVertex3fv(v);
//        glNormal3fv(n);
//        v.z += height;
//        color2->ApplyMaterialColor();
//        glVertex3fv(v);
//        data++;
//        i++;
//    }
//    n = *Vertices;
//    v = n;
//    glNormal3fv(n);
//    v.x *= radius;
//    v.y *= radius;
//    color1->ApplyMaterialColor();
//    glVertex3fv(v);
//    glNormal3fv(n);
//    v.z += height;
//    color2->ApplyMaterialColor();
//    glVertex3fv(v);
//    glEnd();
}

//---------------------------------------------------------------------------

void CCylinder::DrawWithUniformColors(const float radius,const float height,
                                      const CElementColors* color1,
                                      const CElementColors* color2)
{
    CSimplePoint<float>  v,n;
    CSimplePoint<float>* data = Vertices;
    unsigned int           i=0;

//    glBegin(GL_QUAD_STRIP);
//    while(i < NumOfFaces) {
//        n = *data;
//        v = n;
//        glNormal3fv(n);
//        v.x *= radius;
//        v.y *= radius;
//        color1->ApplyUniformColor();
//        glVertex3fv(v);
//        glNormal3fv(n);
//        v.z += height;
//        color2->ApplyUniformColor();
//        glVertex3fv(v);
//        data++;
//        i++;
//    }
//    n = *Vertices;
//    v = n;
//    glNormal3fv(n);
//    v.x *= radius;
//    v.y *= radius;
//    color1->ApplyUniformColor();
//    glVertex3fv(v);
//    glNormal3fv(n);
//    v.z += height;
//    color2->ApplyUniformColor();
//    glVertex3fv(v);
//    glEnd();
}

//---------------------------------------------------------------------------

bool CCylinder::SetTessellationQuality(const unsigned int quality)
{
    if( quality == Tessellation ) return(true);
    if( (quality < 1 ) || (quality>8) )return(false);

    Tessellation = quality;

    if( Vertices != NULL ) {
        delete[] Vertices;
    }

    NumOfFaces = 8 * pow(2, quality - 1);

    Vertices = new CSimplePoint<float> [NumOfFaces];

    ComputeVertices();

    return(true);
}

//---------------------------------------------------------------------------

void CCylinder::ComputeVertices(void)
{
    for (unsigned int i = 0; i < NumOfFaces; i++) {
        Vertices[i].x = (float)cos((6.28318530717959 * i) / (double)NumOfFaces);
        Vertices[i].y = (float)sin((6.28318530717959 * i) / (double)NumOfFaces);
        Vertices[i].z = 0;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CArc::CArc(void)
{
    Vertices = NULL;
    NumOfFaces = 0;
    Tessellation = 0;
    SetTessellationQuality(3);
}

//---------------------------------------------------------------------------

CArc::~CArc(void)
{
    if( Vertices != NULL ) delete[] Vertices;
}

//---------------------------------------------------------------------------

bool CArc::SetTessellationQuality(const unsigned int quality)
{
    if( quality == Tessellation ) return(true);
    if( (quality < 1 ) || (quality>8) )return(false);

    Tessellation = quality;

    if( Vertices != NULL ) {
        delete[] Vertices;
    }

    NumOfFaces = 8 * pow(2, quality - 1);

    Vertices = new CSimplePoint<float> [NumOfFaces];

    ComputeVertices();
    return(true);
}

//---------------------------------------------------------------------------

void CArc::Draw(const CSimplePoint<float>& v1,
                const CSimplePoint<float>& v2,
                const CSimplePoint<float>& v3,
                float angle,CSimplePoint<float>& str)
{
    CTransformation trans;

    CPoint d1 = v1-v2;
    CPoint d2 = v3-v2;
    CPoint nv3 = CrossDot(d1,d2);

    float size = Size(d1);
    trans.Scale(size,size,1);

    CPoint z(0,0,1);
    trans.ChangeDirection(z,nv3);
    CPoint x(1,0,0);
    CPoint tx = trans.GetTransform(x);
    double rot = Angle(d1,tx);
    CPoint nv4 = CrossDot(d1,tx);
    trans.Rotate(-rot,nv4);

    trans.Translate(v2);

    CPoint end;

    end = d2;
    end.Normalize();
    end = end*Size(d1)+v2;

    int numoffac = angle*NumOfFaces/3.14;

//    glBegin(GL_LINE_STRIP);
//    for(int i=0; i<numoffac; i++) {
//        CPoint newpos = trans.GetTransform(Vertices[i]);
//        glVertex3dv(newpos);
//    }
//    glVertex3dv(end);
//    glEnd();

    str = trans.GetTransform(Vertices[numoffac/2]);
}

//---------------------------------------------------------------------------

void CArc::ComputeVertices(void)
{
    for (unsigned int i = 0; i < NumOfFaces; i++) {
        Vertices[i].x = (float)cos((3.14 * i) / (double)NumOfFaces);
        Vertices[i].y = (float)sin((3.14 * i) / (double)NumOfFaces);
        Vertices[i].z = 0;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

COpenGL2DFont::COpenGL2DFont(void)
{
    FirstGlyph = 0;
    NumOfGlyph = 0;
    ListBase = 0;
}

//---------------------------------------------------------------------------

COpenGL2DFont::~COpenGL2DFont(void)
{
 //   glDeleteLists(ListBase,NumOfGlyph);
}

//---------------------------------------------------------------------------

//http://ftgl.sourceforge.net/docs/html/ftgl-tutorial.html
// bool COpenGL2DFont::CreateFont(TFont* p_font,TWinControl* p_win)
// {
//  HFONT         HFont,OldHFont;
//  LOGFONT       LogFont;
//
//  FirstGlyph = 32;
//  NumOfGlyph = 224;
//
//  if( (p_font==NULL) ) return(false);
//  if( (ListBase=glGenLists(NumOfGlyph))==0 ) return(false);
//
//  bool result;
//
//  GetObject(p_font->Handle,sizeof(LOGFONT),&LogFont);
//  HFont = CreateFontIndirect(&LogFont);
//  HDC  DisplayDeviceContext;
//
//  if( p_win == NULL ){
//     DisplayDeviceContext = GetDC(NULL);
//     }
//   else{
//     DisplayDeviceContext = GetDC(p_win->Handle);
//     }
//  OldHFont = SelectObject(DisplayDeviceContext,HFont);
//
//  result = wglUseFontBitmaps(DisplayDeviceContext, FirstGlyph, NumOfGlyph, ListBase);
//
//  if( result == false ){
//     wglUseFontBitmaps(DisplayDeviceContext, FirstGlyph, NumOfGlyph, ListBase);
//     }
//
//  SelectObject(DisplayDeviceContext,OldHFont);
//  DeleteObject(HFont);
//  ReleaseDC(p_win->Handle,DisplayDeviceContext);
//
//  return(true);
// }

//---------------------------------------------------------------------------

bool COpenGL2DFont::IsInitialized(void)
{
//    if( ListBase > 0 ) {
//        if( glIsList(ListBase) == false ) {
//            ListBase = 0;
//            return(false);
//        }
//        return(true);
//    }
    return(false);
}

//---------------------------------------------------------------------------

void COpenGL2DFont::DestroyFont(void)
{
    //if( NumOfGlyph > 0 ) glDeleteLists(ListBase,NumOfGlyph);
    FirstGlyph = 0;
    NumOfGlyph = 0;
    ListBase = 0;
}

//---------------------------------------------------------------------------

void COpenGL2DFont::DrawText(const QString& text)
{   
    if( (ListBase == 0) || (text.size()==0) ) return;

    QByteArray array = text.toLatin1();

    for(int i=0; i < array.size(); i++){
        int index = array[i] - FirstGlyph;
        if( (index < 0) || (index >= NumOfGlyph) ){
            array[i] = ' ';
        }
    }

//    glPushAttrib(GL_LIST_BIT);
//        glListBase(ListBase-FirstGlyph);
//        glCallLists(array.size(), GL_UNSIGNED_BYTE, array.constData());
//    glPopAttrib();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

