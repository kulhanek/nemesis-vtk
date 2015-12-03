#ifndef StandardModelSetupH
#define StandardModelSetupH
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

#include <NemesisCoreMainHeader.hpp>
#include <GraphicsSetup.hpp>
#include <ElementColors.hpp>
#include <ElementColors.hpp>
#include <Manipulator.hpp>

// -----------------------------------------------------------------------------

class CXMLElement;

// -----------------------------------------------------------------------------

/// enumeration of all supported models by standard model object

enum EModel {
    MODEL_NO_MODEL=-1,
    MODEL_STICKS,
    MODEL_STICKS_AND_BALLS,
    MODEL_TUBES,
    MODEL_TUBES_AND_BALLS,
    MODEL_POINTS,
    MODEL_SPHERES,
    MODEL_NUM_OF_MODELS
};

// -----------------------------------------------------------------------------

/// parameters of atom drawing

class CAtomModelSettings {
public:
// constructors and destructors -----------------------------------------------
    CAtomModelSettings(void);

// input/output methods -------------------------------------------------------
    void LoadData(CXMLElement* p_ele);
    void SaveData(CXMLElement* p_ele);

// section of public data -----------------------------------------------------
public:
    int     Type;                   // 0 - points, 1 - use gluSphere, 2 - icosahedron
    float   Radius;                 // radius of isolated atoms
    float   Ratio;                  // ratio between vdw radius and graphic radius
    int     TessellationQuality;    // quality of gluSphere or icosahedron
};

// -----------------------------------------------------------------------------

/// parameters of bond drawing

class CBondModelSettings {
public:
// constructors and destructors -----------------------------------------------
    CBondModelSettings(void);

// input/output methods -------------------------------------------------------
    void LoadData(CXMLElement* p_ele);
    void SaveData(CXMLElement* p_ele);

// section of public data -----------------------------------------------------
public:
    int     Type;        // 0 - lines, 1 - use gluCone, 2 - icosahedron
    float   Radius;      // tube radius
    float   Pitch;       // pitch for bonds with order higher than 1
    bool    ShowOrder;   // show order for tubes
    bool    Diffuse;     // continuous color transfer or van der Waals division
    int     TessellationQuality; // quality of gluCone or icosahedron
};

// -----------------------------------------------------------------------------

/// model settings

class CModelSettings {
public:
// constructors and destructors -----------------------------------------------
    CModelSettings(void);

// input/output methods -------------------------------------------------------
    void LoadData(CXMLElement* p_ele);
    void SaveData(CXMLElement* p_ele);

// section of public data -----------------------------------------------------
public:
    CAtomModelSettings      Atoms;
    CBondModelSettings      Bonds;
};

// -----------------------------------------------------------------------------

/// standard model setup

class CStandardModelSetup : public CGraphicsSetup {
public:
// constructors and destructors -----------------------------------------------
    CStandardModelSetup(CProObject* p_owner);

// input/output methods -------------------------------------------------------
    virtual void LoadData(CXMLElement* p_ele);
    virtual void SaveData(CXMLElement* p_ele);

// section of public data -----------------------------------------------------
public:
    CModelSettings  Models[MODEL_NUM_OF_MODELS];
};

// -----------------------------------------------------------------------------

#endif
