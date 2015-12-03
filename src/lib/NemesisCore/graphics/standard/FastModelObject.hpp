#ifndef FastModelObjectH
#define FastModelObjectH
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
#include <GraphicsObject.hpp>
#include <ProObject.hpp>
#include <SimpleList.hpp>

// -----------------------------------------------------------------------------

class CStructureList;
class CStructure;
class CAtom;
class CBond;
class CXMLElement;
class CFastModelSetup;
class CResidue;
class CGOColorMode;
class CElementColors;

//------------------------------------------------------------------------------

extern CExtUUID FastModelObjectID;

//------------------------------------------------------------------------------

enum EFastModelObjectFlag {
    // user flags               = 0x00010000    // first user flag
    EFMOF_SHOW_HYDROGENS        = 0x00010000,
    EFMOF_SHOW_HIDDEN           = 0x00020000,
    EFMOF_PBC_BONDS             = 0x00040000,   // apply PBC on bonds
    EFMOF_ONLY_ATOMS            = 0x00080000    // show only atoms
};

// -----------------------------------------------------------------------------

/// fast model for structure

class NEMESIS_CORE_PACKAGE CFastModelObject : public CGraphicsObject {
public:
// constructors and destructors -----------------------------------------------
    CFastModelObject(CGraphicsObjectList* p_gl);

// methods with changes registered into history list ---------------------------
    /// set k-offsets
    bool SetKOffsetsWH(int ka,int kb,int kc);

// executive functions --------------------------------------------------------
    /// draw model
    virtual void Draw(void);

    /// set k-offsets
    void SetKOffsets(int ka,int kb,int kc,CHistoryNode* p_history=NULL);

// informational methods ------------------------------------------------------
    /// calculate visible object metrics
    virtual void GetObjectMetrics(CObjMetrics& metrics);

    /// return color mode object
    CGOColorMode*   GetColorMode(void);

    /// get ka offset
    int GetKAOffset(void);

    /// get kb offset
    int GetKBOffset(void);

    /// get kc offset
    int GetKCOffset(void);

// input/outpu methods ---------------------------------------------------------
    /// load object specific data
    virtual void LoadData(CXMLElement* p_ele);

    /// save object specific data
    virtual void SaveData(CXMLElement* p_ele);

// section of private data ----------------------------------------------------
private:
    CGOColorMode*   ColorMode;
    int             KA;
    int             KB;
    int             KC;

// tmp data
    CPoint          koffset; // PBCOffset
    void SetPBCOffset(CStructure* p_str);

// fast draw ------------------------------------
    void FastDrawStructureList(CStructureList* p_strlist);
    void FastDrawStructure(CStructure* p_str);
    void FastDrawResidue(CResidue* p_res);
    void FastDrawAtom(CAtom* p_atom);
    void FastDrawBond(CBond* p_bond);

    void GetStructureListMetrics(CStructureList* p_strlist,CObjMetrics& metrics);
    void GetStructureMetrics(CStructure* p_str,CObjMetrics& metrics);
    void GetResidueMetrics(CResidue* p_res,CObjMetrics& metrics);
    void GetAtomMetrics(CAtom* p_atom,CObjMetrics& metrics);
    void GetBondMetrics(CBond* p_bond,CObjMetrics& metrics);

    void DrawCoreBond(CAtom* p_atom1,CAtom* p_atom2,
                    CElementColors* p_color1,CElementColors* p_color2,
                    CSimplePoint<float>& pos1,
                    CSimplePoint<float>& posM,
                    CSimplePoint<float>& pos2);
};

//---------------------------------------------------------------------------

#endif
