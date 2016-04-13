#ifndef GESPGridObjectH
#define GESPGridObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2016 Petr Kulhanek, kulhanek@chemi.muni.cz
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
class CGESPGridSetup;

//------------------------------------------------------------------------------

extern CExtUUID GESPGridObjectID;

class NEMESIS_CORE_PACKAGE CGESPGridPoint {
public:
    double x, y, z;
    double esp;
};


//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CGESPGridObject : public CGraphicsObject {
public:
// constructors and destructors -----------------------------------------------
    CGESPGridObject(CGraphicsObjectList* p_gl);

    /// add new ESP grid point
    void AddPoint(double x, double y, double z, double esp);

// executive methods -----------------------------------------------------------
    /// draw object
    virtual void Draw(void);

// section of private data -----------------------------------------------------
private:
    std::vector<CGESPGridPoint> Data;
    double MaxESP;
    double MinESP;

};

//------------------------------------------------------------------------------

#endif
