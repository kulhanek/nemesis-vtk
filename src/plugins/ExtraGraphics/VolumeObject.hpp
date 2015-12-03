#ifndef VolumeObjectH
#define VolumeObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <GraphicsObject.hpp>

//------------------------------------------------------------------------------

class CGraphicsObjectList;
class CVolumeSetup;

typedef unsigned int GLuint;

//------------------------------------------------------------------------------

extern CExtUUID VolumeObjectID;

enum EVolumeObjectFlag {
    // user flags               = 0x00010000    // first user flag
    EFAOF_SHOW_HIDDEN           = 0x00010000,
};

// -----------------------------------------------------------------------------


class CVolumeObject : public CGraphicsObject {
public:
// constructors and destructors -----------------------------------------------
    CVolumeObject(CGraphicsObjectList* p_gl);

    //! can object be added
    virtual bool CanBeObjectAdded(CProObject* p_object);

    void linkProgram( GLuint program, const std::string& what );
    void compileShader( GLuint shader, const std::string& what );


// section of public data -----------------------------------------------------
private:
    CVolumeSetup*    Setup;

    //! draw object
    virtual void Draw(void);

        //! drav atom
    void DrawVolumeData(char* p_data);
};

//------------------------------------------------------------------------------

#endif
