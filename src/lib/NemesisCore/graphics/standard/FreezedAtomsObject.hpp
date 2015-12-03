#ifndef FreezedAtomsObjectH
#define FreezedAtomsObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
class CFreezedAtomsSetup;
class CAtom;
class CStructureList;
class CStructure;

//------------------------------------------------------------------------------

extern CExtUUID FreezedAtomsObjectID;

//------------------------------------------------------------------------------

enum EFreezedAtomsObjectFlag {
    // user flags               = 0x00010000    // first user flag
    EFAOF_SHOW_HIDDEN           = 0x00010000
};

// -----------------------------------------------------------------------------

/// draw freezed atoms

class NEMESIS_CORE_PACKAGE CFreezedAtomsObject : public CGraphicsObject {
public:
// constructors and destructors ------------------------------------------------
    CFreezedAtomsObject(CGraphicsObjectList* p_gl);

// executive methods -----------------------------------------------------------
    /// draw object
    virtual void Draw(void);

// section of public data ------------------------------------------------------
private:
    CFreezedAtomsSetup*    Setup;

    /// draw structure list
    void DrawStructureList(CStructureList* p_sl);

    /// draw structure
    void DrawStructure(CStructure* p_mol);

    /// drav atom
    void DrawAtom(CAtom* p_atm);
};

//------------------------------------------------------------------------------

#endif
