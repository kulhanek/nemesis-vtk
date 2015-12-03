#ifndef GraphicsViewStereoH
#define GraphicsViewStereoH
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

#include <NemesisCoreMainHeader.hpp>
#include <gls/gls.h>
#include <QString>
#include <map>

//------------------------------------------------------------------------------

/// supported stereo modes
enum EStereoMode {
    ESM_OFF = 0,
    ESM_QUAD_BUFFER_STEREO,
    ESM_MONO_LEFT,
    ESM_MONO_RIGHT,
    ESM_LEFT_RIGHT,
    ESM_TOP_BOTTOM,
    ESM_HDMI_FRAME_PACK,
    ESM_EVEN_ODD_ROWS,
    ESM_EVEN_ODD_COLUMNS,
    ESM_CHECKERBOARD,
    ESM_RED_CYAN_MONOCHROME,
    ESM_RED_CYAN_HALF_COLOR,
    ESM_RED_CYAN_FULL_COLOR,
    ESM_RED_CYAN_DUBOIS,
    ESM_GREEN_MAGENTA_MONOCHROME,
    ESM_GREEN_MAGENTA_HALF_COLOR,
    ESM_GREEN_MAGENTA_FULL_COLOR,
    ESM_GREEN_MAGENTA_DUBOIS,
    ESM_AMBER_BLUE_MONOCHROME,
    ESM_AMBER_BLUE_HALF_COLOR,
    ESM_AMBER_BLUE_FULL_COLOR,
    ESM_AMBER_BLUE_DUBOIS,
    ESM_RED_GREEN_MONOCHROME,
    ESM_RED_BLUE_MONOCHROME
};

//------------------------------------------------------------------------------

/// graphic view stereo modes

class NEMESIS_CORE_PACKAGE CGraphicsViewStereo  {
public:
// executive methods -----------------------------------------------------------
    /// init database of GLSmodes
    static void InitGLSModes(void);

// information methods ---------------------------------------------------------
    /// number of stereo modes
    static int GetNumberOfStereoModes(void);

    /// return description of stereo mode
    static const QString GetStereoModeDescription(EStereoMode mode);

    /// return description of stereo mode
    static void GetStereoModeDescription(EStereoMode mode,QString& category,QString& descr);

    /// is stereo mode available
    static bool IsStereoModeAvailable(EStereoMode stereo);

    /// translate to GLS stereo mode
    static GLSmode  GetGLSMode(EStereoMode mode);

// section of private data -----------------------------------------------------
private:
    static std::map<EStereoMode,GLSmode>   GLSModes;
    static std::map<EStereoMode,QString>   GLSModeDescrs;
    static std::map<EStereoMode,QString>   GLSModeSDescrs;
    static std::map<EStereoMode,QString>   GLSModeCDescrs;
    static bool                            HasQuadStereo;
};

//------------------------------------------------------------------------------

#endif
