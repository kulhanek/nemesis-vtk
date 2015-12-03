#ifndef RMSDPropertyH
#define RMSDPropertyH
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

#include <GraphicsProperty.hpp>
#include <SelectionHandler.hpp>
#include <Transformation.hpp>

//------------------------------------------------------------------------------

class CPropertyList;
class CStructure;
class CAtom;
class CMapCouple;
class CRMSDPropertySetup;

//------------------------------------------------------------------------------

extern CExtUUID RMSDPropertyID;

//------------------------------------------------------------------------------

enum ERMSDPropertyFlag {
    // user flags               = 0x00010000    // first user flag
    ERMSDPF_ONLY_SAME_Z         = 0x00010000,   // only atoms of the same type can form a couple
    ERMSDPF_MASS_WEIGHTED       = 0x00020000,   // RMSD is mass weighted
    ERMSDPF_DO_NOT_FIT          = 0x00040000    // do not fit target to template
};

//------------------------------------------------------------------------------

///  Root-mean square deviation property

class CRMSDProperty : public CGraphicsProperty, public CSelectionHandler {
Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    CRMSDProperty(CPropertyList *p_bl);
    ~CRMSDProperty(void);

    void RemoveFromBaseList(CHistoryNode* p_history);

// executive operation ---------------------------------------------------------
    /// set template structure
    bool SetTemplateStructureWH(CStructure* p_str);

    /// set target structure
    bool SetTargetStructureWH(CStructure* p_str);

    /// set target structure
    CMapCouple* AddCoupleWH(CAtom* p_at1,CAtom* p_at2);

    /// remove all couples
    bool RemoveAllCouplesWH(void);

    /// remove hydrogen couples
    bool RemoveHydrogenCouplesWH(void);

    /// add mapping by serial index
    bool AddMappingBySerIndxWH(void);

    /// add mapping by distance
    bool AddMappingByDistanceWH(double treshold);

    /// align the target structure
    bool AlignTargetWH(bool mass_weighted);

    /// hide all map atoms
    bool HideAllMappedAtomsWH(void);

    /// hide all unmap atoms
    bool HideAllUnmappedAtomsWH(void);

    /// show all atoms
    bool ShowAllAtomsWH(void);

    /// map target pos to template
    bool MapTargetPosToTemplateWH(void);

    /// map template pos to target
    bool MapTemplatePosToTargetWH(void);

    /// imprint template topology to target
    bool ImprintTemplateTopologyToTargetWH(void);

    /// imprint target topology to template
    bool ImprintTargetTopologyToTemplateWH(void);

// setup operation -------------------------------------------------------------
    /// set template structure
    void SetTemplateStructure(CStructure* p_str,CHistoryNode* p_history=NULL);

    /// set target structure
    void SetTargetStructure(CStructure* p_str,CHistoryNode* p_history=NULL);

    /// add couple
    CMapCouple* AddCouple(CAtom* p_at1,CAtom* p_at2,CHistoryNode* p_history=NULL);

    /// add couple
    CMapCouple* AddCouple(CXMLElement* p_ele,CHistoryNode* p_history=NULL);

    /// remove all couples
    void RemoveAllCouples(CHistoryNode* p_history=NULL);

    /// remove hydrogen couples
    void RemoveHydrogenCouples(CHistoryNode* p_history=NULL);

    /// add mapping by serial index
    void AddMappingBySerIndx(CHistoryNode* p_history=NULL);

    /// add mapping by distance
    void AddMappingByDistance(double treshold,CHistoryNode* p_history=NULL);

    /// align the target structure
    void AlignTarget(bool mass_weighted,CHistoryNode* p_history=NULL);

    /// hide all map atoms
    void HideAllMappedAtoms(CHistoryNode* p_history=NULL);

    /// hide all unmap atoms
    void HideAllUnmappedAtoms(CHistoryNode* p_history=NULL);

    /// show all atoms
    void ShowAllAtoms(CHistoryNode* p_history=NULL);

    /// map target pos to template
    void MapTargetPosToTemplate(CHistoryNode* p_history=NULL);

    /// map template pos to target
    void MapTemplatePosToTarget(CHistoryNode* p_history=NULL);

    /// imprint template topology to target
    void ImprintTemplateTopologyToTarget(CHistoryNode* p_history=NULL);

    /// imprint target topology to template
    void ImprintTargetTopologyToTemplate(CHistoryNode* p_history=NULL);

// information methods ---------------------------------------------------------
    /// return pointer to template structure
    CStructure* GetTemplateStructure(void) const;

    /// return pointer to target structure
    CStructure* GetTargetStructure(void) const;

    /// return number of atom couples
    int GetNumberOfCouples(void);

    /// return number of atom couples
    int GetNumberOfHydrogenCouples(void);

    /// are all couples mass consisten
    bool AreAllCouplesMassConsistent(void);

    /// are already a registered couple?
    CMapCouple* AreCoupled(CAtom* p_at1,CAtom* p_at2);

    /// is it part of any couple?
    bool IsInMap(CAtom* p_at1);

    /// is property completed?
    virtual bool IsReady(void);

    /// get property value - scalar value
    virtual double  GetScalarValue(void);

// input/output methods --------------------------------------------------------
    /// load atom data
    virtual void LoadData(CXMLElement* p_ele);

    /// save atom data
    virtual void SaveData(CXMLElement* p_ele);

// math ------------------------------------------------------------------------
    /// return transformation matrix to align both structures
    CTransformation GetAlignTransform(bool mass_weighted);

    /// return rmsd among two structures
    double  GetRMSD(bool mass_weighted);

// registered objects  ---------------------------------------------------------
    /// handle object removal from the profile list
    virtual void RemoveFromRegistered(CProObject* p_object,
                                      CHistoryNode* p_history);

// -----------------------------------------------------------------------------
signals:
    /// emmited when RMSDProperty map is changed
    void OnRMSDPropertyMapChanged(void);

public:
    /// emit OnRMSDPropertyMapChanged signal
    void EmitOnRMSDPropertyMapChanged(void);

// section of private data -----------------------------------------------------
private:
    CStructure*         Template;
    CStructure*         Target;
    CRMSDPropertySetup* Setup;

// handler main method ---------------------------
    virtual ESelResult RegisterObject(CSelectionList* p_sel,const CSelObject& obj);

// handler description
    virtual const QString GetHandlerDescription(void) const;

// graphics --------------------------------------
    virtual void Draw(void);
};

//------------------------------------------------------------------------------

#endif
