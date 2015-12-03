#ifndef SpecAxesObjectH
#define SpecAxesObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008-2009 Petr Kulhanek, kulhanek@enzim.hu,
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
#include <GL/gl.h>
#include <GL/glu.h>
#include <GraphicsUtil.hpp>
#include <FortranMatrix.hpp>
#include <Vector.hpp>

//------------------------------------------------------------------------------

class CSpecAxesSetup;
class CPhysicalQuantity;
class CStructure;
class CResidue;
class CAtom;

//------------------------------------------------------------------------------

enum ESpecAxesObjectFlag {
    // user flags               = 0x00010000    // first user flag
    ESAOF_SHOW_A                = 0x00010000,
    ESAOF_SHOW_B                = 0x00020000,
    ESAOF_SHOW_C                = 0x00040000,
    ESAOF_MASS_WEIGHTED         = 0x00080000,
    ESAOF_APPLY_MAGNITUDES      = 0x00100000
};

//------------------------------------------------------------------------------

enum ESpecAxesObjectMode {
    ESAOM_PMT = 0,              // principal moments of inertia
    ESAOM_RGT = 1               // principal moments of the gyration tensor
};

//------------------------------------------------------------------------------

/// draw special axes

class NEMESIS_CORE_PACKAGE CSpecAxesObject : public CGraphicsObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CSpecAxesObject(CGraphicsObjectList* p_gl);

// seetup methods --------------------------------------------------------------
    /// set mode
    bool SetModeWH(ESpecAxesObjectMode mode);

    /// set mode
    void SetMode(ESpecAxesObjectMode mode,CHistoryNode* p_history=NULL);

// executive methods -----------------------------------------------------------
    /// draw object
    virtual void Draw(void);

    /// calculate visible object metrics
    virtual void GetObjectMetrics(CObjMetrics& metrics);

// information methods ---------------------------------------------------------
    /// return current mode
    ESpecAxesObjectMode GetMode(void);

    /// get vector components unit
    CPhysicalQuantity* GetVectorComponentPQ(void);

    /// get eigenvalue unit
    CPhysicalQuantity* GetEigenValuePQ(void);

    /// return center of mass
    const CPoint& GetCOM(void);

    /// return direction of eigenvector A
    const CPoint GetEigenVectorA(void);

    /// return direction of eigenvector B
    const CPoint GetEigenVectorB(void);

    /// return direction of eigenvector C
    const CPoint GetEigenVectorC(void);

    /// return eigenvalue A
    double GetEigenValueA(void);

    /// return eigenvalue B
    double GetEigenValueB(void);

    /// return eigenvalue C
    double GetEigenValueC(void);

    /// are data valid
    bool AreDataValid(void);

// input/outpu methods ---------------------------------------------------------
    /// load object specific data
    virtual void LoadData(CXMLElement* p_ele);

    /// save object specific data
    virtual void SaveData(CXMLElement* p_ele);

// section of private data -----------------------------------------------------
private:
    CCylinder           Cylinder;
    CCylinder           Cone;
    CSpecAxesSetup*     Setup;
    ESpecAxesObjectMode Mode;
    double              TotMass;
    CPoint              COM;
    CFortranMatrix      Tensor;
    CVector             EigenValues;
    CPhysicalQuantity*  PQ_PMI;
    CPhysicalQuantity*  PQ_RGT;

    /// this is called every time when the object list is changed
    virtual void ObjectListChanged(void);

    /// this is called every time when the object list is changed
    virtual void ObjectListObjectAdded(CProObject* p_obj);

    /// this is called every time when the object list is changed
    virtual void ObjectListObjectRemoved(CProObject* p_obj);

    /// set object flags
    virtual void SetFlags(const CProObjectFlags& flags,CHistoryNode* p_history=NULL);

    /// update color setup
    void SetInitialColorScheme(void);

    /// draw one axis
    void DrawAxis(const CPoint& origin,const CPoint& vect,float value,float pmag,
                  CColor* ppart,CColor* conec);

    /// update Cylinder and Cone
    virtual void SetupChanged(void);

    /// update COM by structure
    void AddCOMStructure(CStructure* p_str);

    /// update COM by residues
    void AddCOMResidue(CResidue* p_str);

    /// update COM by atom
    void AddCOMAtom(CAtom* p_str);

    /// update tensor by structure
    void AddAxesStructure(CStructure* p_str);

    /// update tensor by residues
    void AddAxesResidue(CResidue* p_str);

    /// update tensor by atom
    void AddAxesAtom(CAtom* p_str);

private slots:
    /// update data
    void UpdateData(void);
};

//---------------------------------------------------------------------------

#endif
