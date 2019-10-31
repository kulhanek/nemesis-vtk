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

#include <GraphicsViewManipulator.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsViewManipulator::CGraphicsViewManipulator(void)
{
    for(int i=0; i < 16; i++) {
        //GlobalModelMatrix[i] = 0;
    }
    ScaleFac = 1;
    Changed = false;
    UpdateLevel = 0;
}

//------------------------------------------------------------------------------

CGraphicsViewManipulator::~CGraphicsViewManipulator(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsViewManipulator::Move(const CPoint &dmov,CGraphicsView* p_view)
{
    Pos += dmov;
    DataUpdated();
}

//------------------------------------------------------------------------------

void CGraphicsViewManipulator::MoveCentrum(const CPoint& dmov,CGraphicsView* p_view)
{
    Centrum += dmov;
    DataUpdated();
}

//------------------------------------------------------------------------------

void CGraphicsViewManipulator::ChangeScale(double increment,CGraphicsView* p_view)
{
    ScaleFac += increment;
    if( ScaleFac < 0.0 ) ScaleFac = 0.01;
    DataUpdated();
}

//------------------------------------------------------------------------------

void CGraphicsViewManipulator::Rotate(const CPoint &drot,CGraphicsView* p_view)
{
    Trans.Rotate(drot);
    DataUpdated();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsViewManipulator::SetRotation(const CPoint &rot)
{
    Trans.Rotate(rot);
    DataUpdated();
}

//------------------------------------------------------------------------------

void CGraphicsViewManipulator::SetPos(const CPoint &pos)
{
    Pos = pos;
    DataUpdated();
}

//------------------------------------------------------------------------------

void CGraphicsViewManipulator::SetTrans(const CTransformation& tran)
{
    Trans = tran;
    DataUpdated();
}

//------------------------------------------------------------------------------

void CGraphicsViewManipulator::SetCentrum(const CPoint& centrum)
{
    Centrum = centrum;
    DataUpdated();
}

//------------------------------------------------------------------------------

void CGraphicsViewManipulator::SetScale(double scale)
{
    ScaleFac = scale;
    DataUpdated();
}

//------------------------------------------------------------------------------

void CGraphicsViewManipulator::ApplyGlobalViewTransformation(void)
{
    //glLoadMatrixd(GlobalModelMatrix);
}

//------------------------------------------------------------------------------

void CGraphicsViewManipulator::ApplyFaceTransformation(void)
{
    CTransformation trans = GetTrans();
    trans.Invert();
  //  glMultMatrixd(trans.GetRawDataField());
}

//------------------------------------------------------------------------------

void CGraphicsViewManipulator::ResetManip(void)
{
    Pos = CPoint();
    Centrum = CPoint();
    Trans.SetUnit();
    ScaleFac = 1.0;
    DataUpdated();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsViewManipulator::ManipDraw(void)
{
//    glPushMatrix();
//    glGetDoublev(GL_MODELVIEW_MATRIX,GlobalModelMatrix);
//    glScaled(ScaleFac,ScaleFac,ScaleFac);
//    glTranslated(Pos.x,Pos.y,Pos.z);
//    glPushMatrix();
//    glMultMatrixd(Trans.GetRawDataField());
//    glTranslated(-Centrum.x,-Centrum.y,-Centrum.z);
//    RawDraw();
//    glPopMatrix();
//    glPopMatrix();
}

//------------------------------------------------------------------------------

void  CGraphicsViewManipulator::RawDraw(void)
{
    // nothing here
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const CPoint& CGraphicsViewManipulator::GetPos(void) const
{
    return(Pos);
}

//------------------------------------------------------------------------------

const CTransformation& CGraphicsViewManipulator::GetTrans(void) const
{
    return(Trans);
}

//------------------------------------------------------------------------------

const CPoint& CGraphicsViewManipulator::GetCentrum(void) const
{
    return(Centrum);
}

//------------------------------------------------------------------------------

double CGraphicsViewManipulator::GetScale(void) const
{
    return(ScaleFac);
}

//------------------------------------------------------------------------------

const CPoint CGraphicsViewManipulator::GetEulerAngles(void) const
{
    CPoint angles;

    if( Trans.Field[2][0] > 0.998 ){
        angles.x = atan2(Trans.Field[0][1],Trans.Field[0][2]);
        angles.y = M_PI/2.0;
        angles.z = 0.0;
    } else if ( Trans.Field[2][0] < -0.998 ) {
        angles.x = atan2(-Trans.Field[0][1],-Trans.Field[0][2]);
        angles.y = -M_PI/2.0;
        angles.z = 0.0;
    } else {
        angles.y = asin(Trans.Field[2][0]);
        double cf = cos(angles.y);
        angles.x = atan2(-Trans.Field[2][1]/cf,Trans.Field[2][2]/cf);
        angles.z = atan2(-Trans.Field[1][0]/cf,Trans.Field[0][0]/cf);
    }
    return(angles);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsViewManipulator::LoadManip(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    p_ele->GetAttribute("px",Pos.x);
    p_ele->GetAttribute("py",Pos.y);
    p_ele->GetAttribute("pz",Pos.z);

    p_ele->GetAttribute("cx",Centrum.x);
    p_ele->GetAttribute("cy",Centrum.y);
    p_ele->GetAttribute("cz",Centrum.z);

    p_ele->GetAttribute("sc",ScaleFac);

    Trans.Load(p_ele);

    DataUpdated();
}

//------------------------------------------------------------------------------

void CGraphicsViewManipulator::SaveManip(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    p_ele->SetAttribute("px",Pos.x);
    p_ele->SetAttribute("py",Pos.y);
    p_ele->SetAttribute("pz",Pos.z);

    p_ele->SetAttribute("cx",Centrum.x);
    p_ele->SetAttribute("cy",Centrum.y);
    p_ele->SetAttribute("cz",Centrum.z);

    p_ele->SetAttribute("sc",ScaleFac);

    Trans.Save(p_ele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsViewManipulator::BeginUpdate(void)
{
    UpdateLevel++;
}

//------------------------------------------------------------------------------

void CGraphicsViewManipulator::EndUpdate(void)
{
    if( UpdateLevel == 0 ) return;
    UpdateLevel--;
    if( Changed ){
        EmitManipulatorChanged();
        Changed = false;
    }
}

//------------------------------------------------------------------------------

void CGraphicsViewManipulator::DataUpdated(void)
{
    if( UpdateLevel == 0 ){
        EmitManipulatorChanged();
        return;
    }
    Changed = true;
}

//------------------------------------------------------------------------------

void CGraphicsViewManipulator::EmitManipulatorChanged(void)
{
    // nothing to be here
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


