// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <NemesisCoreModule.hpp>
#include <PluginObject.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <GraphicsObjectList.hpp>
#include <GLSelection.hpp>
#include <HistoryNode.hpp>

#include <LightObject.hpp>
#include <LightSetup.hpp>
#include <LightObjectHistory.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* LightObjectCB(void* p_data);

CExtUUID        LightObjectID(
                    "{LIGHT_OBJECT:03f65a4c-2f8a-4629-8cdc-3409df428db8}",
                    "Light");

CPluginObject   LightObject(&NemesisCorePlugin,
                    LightObjectID,GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/objects/Light.svg",
                    LightObjectCB);

// -----------------------------------------------------------------------------

QObject* LightObjectCB(void* p_data)
{
    return(new CLightObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CLightObject::CLightObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&LightObject,p_gl)
{
    LightIndex = 0;

    Position.Set(10,3,3);

    CPoint dir = -Position;
    dir.Normalize();
    dir.ConvertToSpherical();

    Orientation.x = dir.y;
    Orientation.y = dir.z;
    Orientation.z = 0;

    SetFlag<ELightObjectFlag>(ELOF_GLOBAL_POSITION,true);
    SetFlag<ELightObjectFlag>(ELOF_SPOT_ENABLED,true);
    SetFlag<ELightObjectFlag>(ELOF_SHOW_COVER,true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CLightObject::SetPositionWH(const CPoint& pos)
{
    if( Position == pos ) return(true);

    // with history
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"light position");
    if( p_history == NULL ) return(false);

    SetPosition(pos,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CLightObject::SetOrientationWH(const CPoint& rot)
{
    if( Orientation == rot ) return(true);

    // with history
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,"light orientation");
    if( p_history == NULL ) return(false);

    SetOrientation(rot,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

void CLightObject::SetPosition(const CPoint& pos,CHistoryNode* p_history)
{
    if( Position == pos ) return;

    if( p_history ){
        CLightObjectChangePositionHI* p_hnode = new CLightObjectChangePositionHI(this,pos);
        p_history->Register(p_hnode);
    }

    Position = pos;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CLightObject::SetOrientation(const CPoint& rot,CHistoryNode* p_history)
{
    if( Orientation == rot ) return;

    if( p_history ){
        CLightObjectChangeOrientationHI* p_hnode = new CLightObjectChangeOrientationHI(this,rot);
        p_history->Register(p_hnode);
    }

    Orientation = rot;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

const CPoint& CLightObject::GetPosition(void) const
{
    return(Position);
}

//------------------------------------------------------------------------------

const CPoint& CLightObject::GetOrientation(void) const
{
    return(Orientation);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CLightObject::Draw(void)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    Setup = GetSetup<CLightSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    GLLoadObject(this);

//    glPushMatrix();
//        if( IsFlagSet<ELightObjectFlag>(ELOF_GLOBAL_POSITION) ){
//            ApplyGlobalViewTransformation();
//        }

//        InitLight();
//        MakeLight();
//        DrawCover();
//    glPopMatrix();
}

//------------------------------------------------------------------------------

void CLightObject::GetObjectMetrics(CObjMetrics& metrics)
{
    if( IsFlagSet(EPOF_VISIBLE) == false ) return;
    if( ! IsFlagSet<ELightObjectFlag>(ELOF_GLOBAL_POSITION) ){
        // it does not have sense for global position
        metrics.CompareWith(Position);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

//------------------------------------------------------------------------------

void CLightObject::InitLight(void)
{
//    GLenum light = GetLightEnumeration(LightIndex);
//    glEnable(light);

//    glLightfv(light,GL_AMBIENT,Setup->Ambient);
//    glLightfv(light,GL_DIFFUSE,Setup->Diffuse);
//    glLightfv(light,GL_SPECULAR,Setup->Specular);

//    glLightf(light,GL_CONSTANT_ATTENUATION,Setup->Attenuation[0]);
//    glLightf(light,GL_LINEAR_ATTENUATION,Setup->Attenuation[1]);
//    glLightf(light,GL_QUADRATIC_ATTENUATION,Setup->Attenuation[2]);

//    glLightf(light, GL_SPOT_EXPONENT, Setup->SpotExponent);

//    if( IsFlagSet<ELightObjectFlag>(ELOF_SPOT_ENABLED) ){
//        glLightf(light, GL_SPOT_CUTOFF, Setup->SpotCutoff);
//    } else {
//        glLightf(light, GL_SPOT_CUTOFF, 180.0f);
//    }
}

//------------------------------------------------------------------------------

void CLightObject::MakeLight(void)
{
    //GLenum light = GetLightEnumeration(LightIndex);
    float pos[4];

    pos[0] = Position.x;
    pos[1] = Position.y;
    pos[2] = Position.z;
    pos[3] = IsFlagSet<ELightObjectFlag>(ELOF_ATTENUATION_ENABLED);   // pokud je nenulov. je povolena atenuace

    //glLightfv(light, GL_POSITION, pos);

    CPoint direction;
    direction.x = 1.0;
    direction.y = Orientation.x;
    direction.z = Orientation.y;
    direction.ConvertToCartesian();

    float dir[4];

    dir[0] = direction.x;
    dir[1] = direction.y;
    dir[2] = direction.z;
    dir[3] = 0;

//    if( IsFlagSet<ELightObjectFlag>(ELOF_SPOT_ENABLED) ){
//        glLightfv(light, GL_SPOT_DIRECTION,dir );
//    }
}

//------------------------------------------------------------------------------

void CLightObject::DrawCover(void)
{
    CPoint direction;
    direction.x = 1.0;
    direction.y = Orientation.x;
    direction.z = Orientation.y;
    direction.ConvertToCartesian();

    float cylH = Size(direction);
    float rotAngle;

    if (cylH == 0) {
        rotAngle = 0;
    } else {
        rotAngle = acos(direction.z/cylH)*180.0/3.14; // konstanta je 180/PI
    }

//    glPushMatrix();
//        glTranslatef (Position.x,Position.y,Position.z);
//        if( (direction.y != 0) || (direction.x != 0) ) {
//            glRotatef(rotAngle,-direction.y,direction.x, 0);
//        } else {
//            if( rotAngle > 90.0 ) glScalef(1,1,-1);
//        }

//        glEnable(GL_COLOR_MATERIAL);
//        glColor4fv(Setup->CoverColor);
//        Cylinder.Draw(0,Setup->CoverRadius,Setup->CoverHeight);
//    glPopMatrix();
}

//------------------------------------------------------------------------------

void CLightObject::SetupChanged(void)
{
    Setup = GetSetup<CLightSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    Cylinder.SetTessellationQuality(Setup->CoverTesselation);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CLightObject::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load object info -----------------------------
    CGraphicsObject::LoadData(p_ele);

    // specific data -----------------------------
    p_ele->GetAttribute("px",Position.x);
    p_ele->GetAttribute("py",Position.y);
    p_ele->GetAttribute("pz",Position.z);

    p_ele->GetAttribute("ox",Orientation.x);
    p_ele->GetAttribute("oy",Orientation.y);
    p_ele->GetAttribute("oz",Orientation.z);
}

//------------------------------------------------------------------------------

void CLightObject::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // core data ---------------------------------
    CGraphicsObject::SaveData(p_ele);

    // specific data -----------------------------
    p_ele->SetAttribute("px",Position.x);
    p_ele->SetAttribute("py",Position.y);
    p_ele->SetAttribute("pz",Position.z);

    p_ele->SetAttribute("ox",Orientation.x);
    p_ele->SetAttribute("oy",Orientation.y);
    p_ele->SetAttribute("oz",Orientation.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================











