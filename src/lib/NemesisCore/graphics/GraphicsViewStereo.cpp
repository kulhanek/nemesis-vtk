// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <GraphicsViewStereo.hpp>
#include <ErrorSystem.hpp>
#include <QGLWidget>

//------------------------------------------------------------------------------

std::map<EStereoMode,GLSmode>   CGraphicsViewStereo::GLSModes;
std::map<EStereoMode,QString>   CGraphicsViewStereo::GLSModeDescrs;
std::map<EStereoMode,QString>   CGraphicsViewStereo::GLSModeSDescrs;
std::map<EStereoMode,QString>   CGraphicsViewStereo::GLSModeCDescrs;
bool                            CGraphicsViewStereo::HasQuadStereo = false;

// ---------------------------------------

void CGraphicsViewStereo::InitGLSModes(void)
{
    // was it initialized?
    if( GLSModes.size() > 0 ) return;

    // set translation
    GLSModes[ESM_OFF] = 0;

    GLSModeDescrs[ESM_OFF] = "monoscopic";
    GLSModeSDescrs[ESM_OFF] = "monoscopic";
    GLSModeCDescrs[ESM_OFF] = "";

    // detect if open GL can create quad buffer opengl widgets
    QGLWidget* p_widget = new QGLWidget(QGLFormat(QGL::StereoBuffers));
    HasQuadStereo = p_widget->format().stereo();
    delete p_widget;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CGraphicsViewStereo::GetNumberOfStereoModes(void)
{
    return(GLSModeDescrs.size());
}

//------------------------------------------------------------------------------

const QString CGraphicsViewStereo::GetStereoModeDescription(EStereoMode mode)
{
    return(GLSModeDescrs[mode]);
}

//------------------------------------------------------------------------------

void CGraphicsViewStereo::GetStereoModeDescription(EStereoMode mode,QString& category,QString& descr)
{
    category = GLSModeCDescrs[mode];
    descr = GLSModeSDescrs[mode];
}

//------------------------------------------------------------------------------

GLSmode CGraphicsViewStereo::GetGLSMode(EStereoMode mode)
{
    return(GLSModes[mode]);
}

//------------------------------------------------------------------------------

bool CGraphicsViewStereo::IsStereoModeAvailable(EStereoMode stereo)
{
    switch(stereo){
        case ESM_OFF:
            return(true);

        default:
            return(false);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
