#ifndef StandardModelObjectH
#define StandardModelObjectH
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
#include <GraphicsUtil.hpp>
#include <SimpleList.hpp>
#include <Bond.hpp>
#include <StandardModelSetup.hpp>

// -----------------------------------------------------------------------------

class CStructureList;
class CStructure;
class CAtom;
class CBond;
class CResidue;
class CStandardModelSetup;
class CHistoryItem;
class CGOColorMode;

//------------------------------------------------------------------------------

extern CExtUUID StandardModelObjectID;

enum EStandardModelObjectFlag {
    // user flags               = 0x00010000    // first user flag
    ESMOF_SHOW_HYDROGENS        = 0x00010000,
    ESMOF_SHOW_HIDDEN           = 0x00020000,
    ESMOF_PBC_BONDS             = 0x00040000
};

// -----------------------------------------------------------------------------

/// standard model for molecule

class NEMESIS_CORE_PACKAGE CStandardModelObject : public CGraphicsObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CStandardModelObject(CGraphicsObjectList* p_gl);

// methods with changes registered into history list ---------------------------
    /// change used model
    bool SetModelWH(EModel model);

    /// set k-offsets
    bool SetKOffsetsWH(int ka,int kb,int kc);

// executive functions ---------------------------------------------------------
    /// draw model
    virtual void Draw(void);

    /// set used model setup
    void SetModel(EModel model,CHistoryNode* p_history=NULL);

    /// set k-offsets
    void SetKOffsets(int ka,int kb,int kc,CHistoryNode* p_history=NULL);

// informational methods -------------------------------------------------------
    /// return model
    EModel  GetModel(void);

    /// return color mode object
    CGOColorMode*   GetColorMode(void);

    /// get ka offset
    int GetKAOffset(void);

    /// get kb offset
    int GetKBOffset(void);

    /// get kc offset
    int GetKCOffset(void);

    /// calculate visible object metrics
    virtual void GetObjectMetrics(CObjMetrics& metrics);

// input/outpu methods ---------------------------------------------------------
    /// load object specific data
    virtual void LoadData(CXMLElement* p_ele);

    /// save object specific data
    virtual void SaveData(CXMLElement* p_ele);

// section of private data -----------------------------------------------------
private:
    CStandardModelSetup*        Setup;
    CAtomModelSettings*         AtomsSet;               // atom setup
    CBondModelSettings*         BondsSet;               // bond setup
    CModelSettings*             ModelSet;               // model settings
    EModel                      Model;                  // model
    int                         KA;
    int                         KB;
    int                         KC;

// helper objects -------------------------------
    CSphere                     Sphere;
    CHalfSphere                 HalfSphere;
    CCylinder                   Cylinder;
    GLUquadricObj*              ExtQuad;
    CGOColorMode*               ColorMode;

// tmp data
    CPoint          koffset; // PBCOffset
    void SetPBCOffset(CStructure* p_str);

    void UpdateSetup(void);
    virtual void SetupChanged(void);
    void SetInitialColorScheme(void);

// draw individual objects ----------------------
    void DrawStructureList(CStructureList* p_sl);
    void DrawStructure(CStructure* p_str);
    void DrawResidue(CResidue* p_res);
    void DrawAtom(CAtom* p_atom);
    void DrawBond(CBond* p_bond);   

// metrics support ------------------------------
    void GetStructureListMetrics(CStructureList* p_sl,CObjMetrics& metrics);
    void GetStructureMetrics(CStructure* p_str,CObjMetrics& metrics);
    void GetResidueMetrics(CResidue* p_res,CObjMetrics& metrics);
    void GetAtomMetrics(CAtom* p_atom,CObjMetrics& metrics);
    void GetBondMetrics(CBond* p_bond,CObjMetrics& metrics);

// helper methods -------------------------------

    void DrawRawBond(CSimplePoint<float>& pos1,CSimplePoint<float>& pos2,
                     CSimplePoint<float>& posm,
                     CElementColors* color1,CElementColors* color2,
                     EBondOrder order,const CSimplePoint<float>& ordervect,
                     bool selected);
    //-------------------------
    void DrawTube(CSimplePoint<float>& p1,
                       CSimplePoint<float>& p2,
                       float radius,
                       const CElementColors* color1,
                       const CElementColors* color2);
    void DrawTube(CSimplePoint<float>& p1,
                       CSimplePoint<float>& pm,
                       CSimplePoint<float>& p2,
                       float radius,
                       const CElementColors* color1,
                       const CElementColors* color2);
    void DrawTube(CSimplePoint<float>& p1,
                       CSimplePoint<float>& p2,
                       float radius,
                       const CElementColors* color1,
                       const CElementColors* color2,
                       EBondOrder order,
                       const CSimplePoint<float> &ordervect);
    void DrawTube(CSimplePoint<float>& p1,
                       CSimplePoint<float>& pm,
                       CSimplePoint<float>& p2,
                       float radius,
                       const CElementColors* color1,
                       const CElementColors* color2,
                       EBondOrder order,
                       const CSimplePoint<float>& ordervect);
    //-------------------------
    void DrawStick(CSimplePoint<float>& p1,
                       CSimplePoint<float>& p2,
                       const CElementColors* color1,
                       const CElementColors* color2);
    void DrawStick(CSimplePoint<float>& p1,
                       CSimplePoint<float>& pm,
                       CSimplePoint<float>& p2,
                       const CElementColors* color1,
                       const CElementColors* color2);
    void DrawStick(CSimplePoint<float>& p1,
                       CSimplePoint<float>& p2,
                       const CElementColors* color1,
                       const CElementColors* color2,
                       EBondOrder order,
                       const CSimplePoint<float> &ordervect);
    void DrawStick(CSimplePoint<float>& p1,
                       CSimplePoint<float>& pm,
                       CSimplePoint<float>& p2,
                       const CElementColors* color1,
                       const CElementColors* color2,
                       EBondOrder order,
                       const CSimplePoint<float>& ordervect);
    //-------------------------
    void DrawSphere(float radius,int slices,int stacks);
};

//------------------------------------------------------------------------------

#endif
