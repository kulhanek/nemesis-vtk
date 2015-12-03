// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008-2009 Petr Kulhanek, kulhanek@enzim.hu,
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

#include <PluginObject.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <GraphicsObjectList.hpp>
#include <Project.hpp>
#include <GLSelection.hpp>
#include <ElementColorsList.hpp>

#include <NemesisCoreModule.hpp>
#include <XYZAxesObject.hpp>
#include <XYZAxesSetup.hpp>

#include "XYZAxesObject.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* XYZAxesObjectCB(void* p_data);

CExtUUID        XYZAxesObjectID(
                    "{XYZ_AXES_OBJECT:47c4e99c-2335-49c9-a5cc-7d8d06d9214e}",
                    "XYZ Axes");

CPluginObject   XYZAxesObject(&NemesisCorePlugin,
                    XYZAxesObjectID,
                    GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/objects/XYZAxes.svg",
                    XYZAxesObjectCB);

// -----------------------------------------------------------------------------

QObject* XYZAxesObjectCB(void* p_data)
{
    return(new CXYZAxesObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CXYZAxesObject::CXYZAxesObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&XYZAxesObject,p_gl)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CXYZAxesObject::SetupChanged(void)
{
    Setup = GetSetup<CXYZAxesSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    Cone.SetTessellationQuality(Setup->ConeTesselation);
    Cylinder.SetTessellationQuality(Setup->CylinderTesselation);

    // call core update
    CGraphicsObject::SetupChanged();
}

//------------------------------------------------------------------------------

void CXYZAxesObject::Draw(void)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    Setup = GetSetup<CXYZAxesSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    GLLoadObject(this);

    SetInitialColorScheme();

    if( Setup->ShowX == true ) {
        CPoint vect(1,0,0);
        DrawAxis(vect,Setup->ShowMX,Setup->MagnificationX,Setup->MagnificationMX,
                 &Setup->PXColor,&Setup->NXColor,&Setup->ConeXColor);
    }
    if( Setup->ShowY == true ) {
        CPoint vect(0,1,0);
        DrawAxis(vect,Setup->ShowMY,Setup->MagnificationY,Setup->MagnificationMY,
                 &Setup->PYColor,&Setup->NYColor,&Setup->ConeYColor);
    }
    if( Setup->ShowZ == true ) {
        CPoint vect(0,0,1);
        DrawAxis(vect,Setup->ShowMZ,Setup->MagnificationZ,Setup->MagnificationMZ,
                 &Setup->PZColor,&Setup->NZColor,&Setup->ConeZColor);
    }
}

//---------------------------------------------------------------------------

void CXYZAxesObject::SetInitialColorScheme(void)
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

//---------------------------------------------------------------------------

void CXYZAxesObject::DrawAxis(const CPoint& vect,bool showm,
                            float pmag,float nmag,
                            CColor* ppart,CColor* npart,CColor* conec
                            )
{
    CSimplePoint<float>  pos1;
    CSimplePoint<float>  pos2;

    if( showm == true ) pos1 = -vect*nmag;
    pos2 = vect*pmag;

    glPushMatrix();
    if( vect.x > 0 ) {
        glRotatef(90.0,0,1,0);
    }
    if( vect.y > 0 ) {
        glRotatef(-90.0,1,0,0);
    }

    // negative part
    float height1 = Size(pos1);
    if( showm == true ) {
        glColor4fv(*npart);
        glTranslatef(0,0,-height1);
        Cylinder.Draw(Setup->AxisRadius,height1);
    }

    // positive part
    glColor4fv(*ppart);
    float height2 = Size(pos2);
    glTranslatef(0,0,height1);
    Cylinder.Draw(Setup->AxisRadius,height2);

    glTranslatef(0,0,height2);

    glColor4fv(*conec);
    Cone.Draw(Setup->ConeRadius,0,Setup->ConeHeight);

    glPopMatrix();
}

//------------------------------------------------------------------------------

void CXYZAxesObject::GetObjectMetrics(CObjMetrics& metrics)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    CPoint vect;

    if( Setup->ShowX ){
        vect = CPoint(1,0,0);
        vect *= Setup->MagnificationX;
        metrics.CompareWith(vect);

        if( Setup->ShowMX ){
            vect = CPoint(1,0,0);
            vect *= -Setup->MagnificationMX;
            metrics.CompareWith(vect);
        }
    }

    if( Setup->ShowY ){
        vect = CPoint(0,1,0);
        vect *= Setup->MagnificationY;
        metrics.CompareWith(vect);

        if( Setup->ShowMY ){
            vect = CPoint(0,1,0);
            vect *= -Setup->MagnificationMY;
            metrics.CompareWith(vect);
        }
    }

    if( Setup->ShowZ ){
        vect = CPoint(0,0,1);
        vect *= Setup->MagnificationZ;
        metrics.CompareWith(vect);

        if( Setup->ShowMZ ){
            vect = CPoint(0,0,1);
            vect *= -Setup->MagnificationMZ;
            metrics.CompareWith(vect);
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


