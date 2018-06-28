#ifndef AtomListH
#define AtomListH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Jiri Klusak, anim4tor@mail.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ProObject.hpp>
#include <AtomData.hpp>
#include <IndexCounter.hpp>
#include <Transformation.hpp>

// ----------------------------------------------------------------------------

class CStructure;
class CProject;
class CXMLElement;
class CAtom;
class CSnapshot;

// -----------------------------------------------------------------------------

enum EGeometryScope {
    EGS_ALL_ATOMS,              // all atoms
    EGS_SELECTED_ATOMS,         // only selected atoms
    EGS_SELECTED_FRAGMENTS      // fragments containing selected atoms
};

// -----------------------------------------------------------------------------

enum EGeometryOrigin {
    EGO_ORIGIN,                 // cartesian origin (0,0,0)
    EGO_BOX_CENTER              // box center
};

// -----------------------------------------------------------------------------

enum EImageMode {
    EIM_BY_ATOMS,
    EIM_BY_RESIDUES,
    EIM_BY_MOLECULES
};

// -----------------------------------------------------------------------------

///  List of all atoms in molecule

class NEMESIS_CORE_PACKAGE CAtomList : public CProObject {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    CAtomList(CStructure* p_str);

// methods with changes registered into history list ---------------------------
    /// create atom
    CAtom* CreateAtomWH(int z,const CPoint& pos);

    /// add hydrogens
    bool AddHydrogensWH(bool polar_only=false);

    /// remove all hydrogens
    bool RemoveHydrogensWH(void);

    /// remove selected atoms - atached bonds are removed too
    bool RemoveSelectedAtomsWH(void);

    /// set serial index by sequence and local indexes
    bool SerialIndexBySeqAndLocIndexesWH(void);

    /// freeze all atoms
    bool FreezeAllAtomsWH(void);

    /// freeze heavy atoms
    bool FreezeHeavyAtomsWH(void);

    /// freeze selected atoms
    bool FreezeSelectedAtomsWH(void);

    /// unfreeze all atoms
    bool UnfreezeAllAtomsWH(void);

    /// unfreeze selected atoms
    bool UnfreezeSelectedAtomsWH(void);

// geometry manipulation -------------------------------------------------------
    /// move molecule to origin
    /** COM is calculated either from selected atoms or from all atoms
        atoms within the scope are transformed
    **/
    bool   MoveCOMToWH(EGeometryOrigin origin=EGO_ORIGIN,EGeometryScope scope=EGS_ALL_ATOMS,bool com_from_all_atoms = false);
    bool   MoveCOGToWH(EGeometryOrigin origin=EGO_ORIGIN,EGeometryScope scope=EGS_ALL_ATOMS,bool cog_from_all_atoms = false);

    /// mirror molecule according particular plane
    bool   MirrorInPlaneWH(EPlaneType mirror,EGeometryScope scope=EGS_ALL_ATOMS);
    bool   MirrorInPlaneWH(const CPoint& origin,const CPoint& normal,EGeometryScope scope);

    /// align principal axes
    bool   AlignPrincipalAxesWH(EAxisType axis,EGeometryScope scope=EGS_ALL_ATOMS);

    /// rotate around an exis
    bool   RotateByWH(EAxisType axis,double angle,EGeometryScope scope=EGS_ALL_ATOMS);
    bool   RotateByWH(const CPoint& orig,const CPoint& dir,double angle,EGeometryScope scope=EGS_ALL_ATOMS);

    /// change position of the whole molecule
    bool   MoveByWH(const CPoint& vect,EGeometryScope scope=EGS_ALL_ATOMS);

    /// image atoms to base cell
    bool   ImageCoordinatesWH(EImageMode mode,
                              bool origin, bool familiar,
                              EGeometryScope scope=EGS_ALL_ATOMS);

// executive methods without change registered to history list ----------------
    /// create new atom
    CAtom* CreateAtom(int Z,CHistoryNode* p_history=NULL);

    /// create new atom
    CAtom* CreateAtom(int Z,const CPoint& pos,CHistoryNode* p_history=NULL);

    /// create new atom
    CAtom* CreateAtom(CXMLElement* p_ele,CHistoryNode* p_history=NULL);

    /// create new atom
    CAtom* CreateAtom(const CAtomData& atom_data,CHistoryNode* p_history=NULL);

    /// freeze all atoms
    void FreezeAllAtoms(CHistoryNode* p_history=NULL);

    /// freeze heavy atoms
    void FreezeHeavyAtoms(CHistoryNode* p_history);

    /// freeze selected atoms
    void FreezeSelectedAtoms(CHistoryNode* p_history=NULL);

    /// unfreeze all atoms
    void UnfreezeAllAtoms(CHistoryNode* p_history=NULL);

    /// unfreeze selected atoms
    void UnfreezeSelectedAtoms(CHistoryNode* p_history=NULL);

    /// change parentship of all atoms
    void MoveAllAtomsFrom(CAtomList* p_source,CHistoryNode* p_history=NULL);

    /// sort atoms by serial index
    void SortAtoms(void);

    /// change position of the whole molecule
    void MoveBy(const CPoint& vect,EGeometryScope scope=EGS_ALL_ATOMS);

//-----------------------------------------------------------------------------
    /// return pointer to owner molecule
    CStructure*     GetStructure(void) const;

    /// return number of atoms
    int             GetNumberOfAtoms(void) const;

    /// find an atom by the serial index
    CAtom*          SearchBySerIndex(int seridx);

    /// get lower serial index
    int             GetLowSerIndex(void);

    /// get top serial index
    int             GetTopSerIndex(void);

    /// get mass center of all atoms
    const CPoint    GetCenterOfMass(void);

    /// get geometry center of all atoms
    const CPoint    GetCenterOfGeometry(void);

    /// is any atom selected, including residue selection?
    bool    IsAnyAtomSelected(void);

    /// return number of selected atom, only atoms are considered
    int     GetNumberOfSelectedAtoms(void);

    /// does structure contain invalid atoms (e.g. atoms not in residues?)
    bool    HasInvalidAtoms(void);

    /// remove registrations for all atoms
    void    UnregisterAllRegisteredAtoms(CHistoryNode* p_history=NULL);

// fragment operations ---------------------------------------------------------
    /// init fragment flags for all atoms
    void InitMoleculeFlagForAll(void);

    /// init fragments only from selected atoms
    void InitMoleculeFlagOnlyFromSelected(void);

    /// init fragments from selected atoms and interconnected atoms
    void InitMoleculeFlagFromSelected(void);

    /// clear fragment flags of all atoms
    void ClearMoleculeFlag(void);

// trajectory support ----------------------------------------------------------
    /// set associated snapshot
    void SetSnapshot(CSnapshot* p_snap);

    /// get associated snapshot
    CSnapshot*      GetSnapshot(void);

// input/output methods --------------------------------------------------------
    /// load all atoms with history recording
    void LoadData(CXMLElement* p_ele,CHistoryNode* p_history);

    /// load all atoms
    virtual void LoadData(CXMLElement* p_ele);

    /// save all atoms
    void SaveData(CXMLElement* p_ele,bool selected);

    /// save all atoms
    virtual void SaveData(CXMLElement* p_ele);

// external notification -------------------------------------------------------
    /// block signals for massive update
    void BeginUpdate(void);

    /// end of massive update
    void EndUpdate(bool do_not_sort=false);

    /// emit OnAtomListChanged signal
    void EmitOnAtomListChanged(void);

    /// list size changed
    void ListSizeChanged(bool do_not_sort=false);

// -----------------------------------------------------------------------------
signals:
    /// emmited when atom list is changed
    void OnAtomListChanged(void);

// section of private data -----------------------------------------------------
private:
    CIndexCounter   TopIndex;           // top index for newly created atoms
    bool            Changed;
    int             UpdateLevel;
    bool            ForceSorting;
    CSnapshot*      Snapshot;

// helper methods --------------------------------------------------------------
    void TransformAtomsBegin(EGeometryScope scope);
    void TransformAtomsEnd(const CTransformation& trans);

    /// image helpers
    void ImageByAtoms(bool origin,bool familiar);
    void ImageByResidues(bool origin,bool familiar);
    void ImageByMolecules(bool origin,bool familiar);

    /// used by SortAtoms
    static bool LessThanBySerIndex(CAtom* p_left,CAtom* p_right);

    friend class CAtom;
};

// -----------------------------------------------------------------------------

#endif

