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
    GLSModes[ESM_QUAD_BUFFER_STEREO] = GLS_MODE_QUAD_BUFFER_STEREO;
    GLSModes[ESM_MONO_LEFT] =  GLS_MODE_MONO_LEFT;
    GLSModes[ESM_MONO_RIGHT] = GLS_MODE_MONO_RIGHT;
    GLSModes[ESM_LEFT_RIGHT] = GLS_MODE_LEFT_RIGHT;
    GLSModes[ESM_TOP_BOTTOM] = GLS_MODE_TOP_BOTTOM;
    GLSModes[ESM_HDMI_FRAME_PACK] =  GLS_MODE_HDMI_FRAME_PACK;
    GLSModes[ESM_EVEN_ODD_ROWS] =  GLS_MODE_EVEN_ODD_ROWS;
    GLSModes[ESM_EVEN_ODD_COLUMNS] = GLS_MODE_EVEN_ODD_COLUMNS;
    GLSModes[ESM_CHECKERBOARD] = GLS_MODE_CHECKERBOARD;
    GLSModes[ESM_RED_CYAN_MONOCHROME] =  GLS_MODE_RED_CYAN_MONOCHROME;
    GLSModes[ESM_RED_CYAN_HALF_COLOR] = GLS_MODE_RED_CYAN_HALF_COLOR;
    GLSModes[ESM_RED_CYAN_FULL_COLOR] = GLS_MODE_RED_CYAN_FULL_COLOR;
    GLSModes[ESM_RED_CYAN_DUBOIS] = GLS_MODE_RED_CYAN_DUBOIS;
    GLSModes[ESM_GREEN_MAGENTA_MONOCHROME] = GLS_MODE_GREEN_MAGENTA_MONOCHROME;
    GLSModes[ESM_GREEN_MAGENTA_HALF_COLOR] = GLS_MODE_GREEN_MAGENTA_HALF_COLOR;
    GLSModes[ESM_GREEN_MAGENTA_FULL_COLOR] = GLS_MODE_GREEN_MAGENTA_FULL_COLOR;
    GLSModes[ESM_GREEN_MAGENTA_DUBOIS] = GLS_MODE_GREEN_MAGENTA_DUBOIS;
    GLSModes[ESM_AMBER_BLUE_MONOCHROME] = GLS_MODE_AMBER_BLUE_MONOCHROME;
    GLSModes[ESM_AMBER_BLUE_HALF_COLOR] = GLS_MODE_AMBER_BLUE_HALF_COLOR;
    GLSModes[ESM_AMBER_BLUE_FULL_COLOR] = GLS_MODE_AMBER_BLUE_FULL_COLOR;
    GLSModes[ESM_AMBER_BLUE_DUBOIS] = GLS_MODE_AMBER_BLUE_DUBOIS;
    GLSModes[ESM_RED_GREEN_MONOCHROME] = GLS_MODE_RED_GREEN_MONOCHROME;
    GLSModes[ESM_RED_BLUE_MONOCHROME] = GLS_MODE_RED_BLUE_MONOCHROME;

    GLSModeDescrs[ESM_OFF] = "monoscopic";
    GLSModeDescrs[ESM_QUAD_BUFFER_STEREO] = "quad buffer stereo";
    GLSModeDescrs[ESM_MONO_LEFT] =  "mono left";
    GLSModeDescrs[ESM_MONO_RIGHT] = "mono right";
    GLSModeDescrs[ESM_LEFT_RIGHT] = "left/right";
    GLSModeDescrs[ESM_TOP_BOTTOM] = "top/bottom";
    GLSModeDescrs[ESM_HDMI_FRAME_PACK] =  "hdmi frame pack";
    GLSModeDescrs[ESM_EVEN_ODD_ROWS] =  "even/odd rows";
    GLSModeDescrs[ESM_EVEN_ODD_COLUMNS] = "even/odd columns";
    GLSModeDescrs[ESM_CHECKERBOARD] = "checkerboard";
    GLSModeDescrs[ESM_RED_CYAN_MONOCHROME] =  "red/cyan monochrome";
    GLSModeDescrs[ESM_RED_CYAN_HALF_COLOR] = "red/cyan half color";
    GLSModeDescrs[ESM_RED_CYAN_FULL_COLOR] = "red/cyan full color";
    GLSModeDescrs[ESM_RED_CYAN_DUBOIS] = "red/cyan dubois";
    GLSModeDescrs[ESM_GREEN_MAGENTA_MONOCHROME] = "green/magenta monochrome";
    GLSModeDescrs[ESM_GREEN_MAGENTA_HALF_COLOR] = "green/magenta half color";
    GLSModeDescrs[ESM_GREEN_MAGENTA_FULL_COLOR] = "green/magenta full color";
    GLSModeDescrs[ESM_GREEN_MAGENTA_DUBOIS] = "green/magenta dubois";
    GLSModeDescrs[ESM_AMBER_BLUE_MONOCHROME] = "amber/blue monochrome";
    GLSModeDescrs[ESM_AMBER_BLUE_HALF_COLOR] = "amber/blue half color";
    GLSModeDescrs[ESM_AMBER_BLUE_FULL_COLOR] = "amber/blue full color";
    GLSModeDescrs[ESM_AMBER_BLUE_DUBOIS] = "amber/blue dubois";
    GLSModeDescrs[ESM_RED_GREEN_MONOCHROME] = "green/green monochrome";
    GLSModeDescrs[ESM_RED_BLUE_MONOCHROME] = "red/blue monochrome";

    GLSModeSDescrs[ESM_OFF] = "monoscopic";
    GLSModeSDescrs[ESM_QUAD_BUFFER_STEREO] = "quad buffer stereo";
    GLSModeSDescrs[ESM_MONO_LEFT] =  "left";
    GLSModeSDescrs[ESM_MONO_RIGHT] = "right";
    GLSModeSDescrs[ESM_LEFT_RIGHT] = "left/right";
    GLSModeSDescrs[ESM_TOP_BOTTOM] = "top/bottom";
    GLSModeSDescrs[ESM_HDMI_FRAME_PACK] =  "hdmi frame pack";
    GLSModeSDescrs[ESM_EVEN_ODD_ROWS] =  "rows";
    GLSModeSDescrs[ESM_EVEN_ODD_COLUMNS] = "columns";
    GLSModeSDescrs[ESM_CHECKERBOARD] = "checkerboard";
    GLSModeSDescrs[ESM_RED_CYAN_MONOCHROME] = "monochrome";
    GLSModeSDescrs[ESM_RED_CYAN_HALF_COLOR] = "half color";
    GLSModeSDescrs[ESM_RED_CYAN_FULL_COLOR] = "full color";
    GLSModeSDescrs[ESM_RED_CYAN_DUBOIS] = "dubois";
    GLSModeSDescrs[ESM_GREEN_MAGENTA_MONOCHROME] = "monochrome";
    GLSModeSDescrs[ESM_GREEN_MAGENTA_HALF_COLOR] = "half color";
    GLSModeSDescrs[ESM_GREEN_MAGENTA_FULL_COLOR] = "full color";
    GLSModeSDescrs[ESM_GREEN_MAGENTA_DUBOIS] = "dubois";
    GLSModeSDescrs[ESM_AMBER_BLUE_MONOCHROME] = "monochrome";
    GLSModeSDescrs[ESM_AMBER_BLUE_HALF_COLOR] = "half color";
    GLSModeSDescrs[ESM_AMBER_BLUE_FULL_COLOR] = "full color";
    GLSModeSDescrs[ESM_AMBER_BLUE_DUBOIS] = "dubois";
    GLSModeSDescrs[ESM_RED_GREEN_MONOCHROME] = "green/green monochrome";
    GLSModeSDescrs[ESM_RED_BLUE_MONOCHROME] = "red/blue monochrome";

    GLSModeCDescrs[ESM_OFF] = "";
    GLSModeCDescrs[ESM_QUAD_BUFFER_STEREO] = "";
    GLSModeCDescrs[ESM_MONO_LEFT] =  "mono";
    GLSModeCDescrs[ESM_MONO_RIGHT] = "mono";
    GLSModeCDescrs[ESM_LEFT_RIGHT] = "";
    GLSModeCDescrs[ESM_TOP_BOTTOM] = "";
    GLSModeCDescrs[ESM_HDMI_FRAME_PACK] =  "";
    GLSModeCDescrs[ESM_EVEN_ODD_ROWS] =  "even/odd";
    GLSModeCDescrs[ESM_EVEN_ODD_COLUMNS] = "even/odd";
    GLSModeCDescrs[ESM_CHECKERBOARD] = "";
    GLSModeCDescrs[ESM_RED_CYAN_MONOCHROME] =  "red/cyan";
    GLSModeCDescrs[ESM_RED_CYAN_HALF_COLOR] = "red/cyan";
    GLSModeCDescrs[ESM_RED_CYAN_FULL_COLOR] = "red/cyan";
    GLSModeCDescrs[ESM_RED_CYAN_DUBOIS] = "red/cyan";
    GLSModeCDescrs[ESM_GREEN_MAGENTA_MONOCHROME] = "green/magenta";
    GLSModeCDescrs[ESM_GREEN_MAGENTA_HALF_COLOR] = "green/magenta";
    GLSModeCDescrs[ESM_GREEN_MAGENTA_FULL_COLOR] = "green/magenta";
    GLSModeCDescrs[ESM_GREEN_MAGENTA_DUBOIS] = "green/magenta";
    GLSModeCDescrs[ESM_AMBER_BLUE_MONOCHROME] = "amber/blue";
    GLSModeCDescrs[ESM_AMBER_BLUE_HALF_COLOR] = "amber/blue";
    GLSModeCDescrs[ESM_AMBER_BLUE_FULL_COLOR] = "amber/blue";
    GLSModeCDescrs[ESM_AMBER_BLUE_DUBOIS] = "amber/blue";
    GLSModeCDescrs[ESM_RED_GREEN_MONOCHROME] = "";
    GLSModeCDescrs[ESM_RED_BLUE_MONOCHROME] = "";

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

        case ESM_QUAD_BUFFER_STEREO:{
            return(HasQuadStereo);
        }

        case ESM_MONO_LEFT:
        case ESM_MONO_RIGHT:
        case ESM_LEFT_RIGHT:
        case ESM_TOP_BOTTOM:
        case ESM_EVEN_ODD_ROWS:
        case ESM_EVEN_ODD_COLUMNS:
        case ESM_RED_CYAN_MONOCHROME:
        case ESM_RED_CYAN_HALF_COLOR:
        case ESM_RED_CYAN_FULL_COLOR:
        case ESM_RED_CYAN_DUBOIS:
        case ESM_GREEN_MAGENTA_MONOCHROME:
        case ESM_GREEN_MAGENTA_HALF_COLOR:
        case ESM_GREEN_MAGENTA_FULL_COLOR:
        case ESM_GREEN_MAGENTA_DUBOIS:
        case ESM_AMBER_BLUE_MONOCHROME:
        case ESM_AMBER_BLUE_HALF_COLOR:
        case ESM_AMBER_BLUE_FULL_COLOR:
        case ESM_AMBER_BLUE_DUBOIS:
        case ESM_RED_GREEN_MONOCHROME:
        case ESM_RED_BLUE_MONOCHROME:
            return(true);
        case ESM_CHECKERBOARD:
        case ESM_HDMI_FRAME_PACK:
            return(false);
    }

    ES_ERROR("not implemented");
    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
