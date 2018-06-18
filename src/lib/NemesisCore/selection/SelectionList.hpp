#ifndef SelectionListH
#define SelectionListH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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
#include <GLSelection.hpp>
#include <Bond.hpp>
#include <GeoMeasurement.hpp>
#include <AtomMask.hpp>

//------------------------------------------------------------------------------

class CProject;
class CSelectionRequest;
class CSelectionHandler;
class CStructure;

//------------------------------------------------------------------------------

enum ESelStatus {
    ESS_REQUEST_DETACHED,           // selection request detached
    ESS_LIST_INITIALIZED,           // selection list initialized (reset, or new request)
    ESS_NONE_SELECTED,              // none object selected
    ESS_SELECTED_OBJECTS_CHANGED,   // list of selected objects changed
    ESS_SELECTED_OBJECTS_END        // selection completed
};

//------------------------------------------------------------------------------

enum EStrObjectSelMode {
    ESOSM_ACTIVE_STRUCTURE_ONLY,    // only active structure objects should be used
    ESOSM_STRUCTURES_FROM_OBJECTS,  // determine structures from already selected objects
    ESOSM_CONSIDER_ALL_STRUCTURES   // all structures should be considered
};

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CSelectionList : public CProObject {
    Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CSelectionList(CProject* p_project,bool no_index);
    ~CSelectionList(void);

// set selection request ------------------------------------------------------
    /// set request
    void SetRequest(CSelectionRequest* p_request);

    /// register primitive object
    void RegisterObject(const CSelObject& obj);

    /// reset selection
    void ResetSelection(void);

    /// select atoms by z
    void SelectAtomsByZ(int z);

    /// select atoms by z
    void SelectAtomsByZ(int z,CStructure* p_mol);

    /// select atoms by name
    void SelectAtomsByName(const QRegExp& regexp);

    /// select atoms by name
    void SelectAtomsByName(const QRegExp& regexp,CStructure* p_mol);

    /// select atoms by type
    void SelectAtomsByType(const QRegExp& regexp);

    /// select atoms by type
    void SelectAtomsByType(const QRegExp& regexp,CStructure* p_mol);

    /// select atoms by mask
    bool SelectAtomsByMask(const QString& mask,EAtomMaskType type);

    /// select atoms by mask
    bool SelectAtomsByMask(const QString& mask,EAtomMaskType type,CStructure* p_mol);

    /// select atoms by plane
    void SelectAtomsByPlane(double a,double b,double c, double d,bool front);

    /// select atoms by plane
    void SelectAtomsByPlane(double a,double b,double c, double d,bool front,CStructure* p_mol);

    /// select bonds by name
    void SelectBondsByName(const QRegExp& regexp);

    /// select bonds by name
    void SelectBondsByName(const QRegExp& regexp,CStructure* p_mol);

    /// select bonds by bond order
    void SelectBondsByOrder(EBondOrder order,EComparisonOperator oper);

    /// select bonds by bond order
    void SelectBondsByOrder(EBondOrder order,EComparisonOperator oper,CStructure* p_mol);

    /// select residues by name
    void SelectResiduesByName(const QRegExp& regexp);

    /// select residues by name
    void SelectResiduesByName(const QRegExp& regexp,CStructure* p_mol);

    /// select residues by mask
    bool SelectResiduesByMask(const QString& mask,EAtomMaskType type);

    /// select residues by mask
    bool SelectResiduesByMask(const QString& mask,EAtomMaskType type,CStructure* p_mol);

    /// select graphics objects by type
    void SelectGraphicsObjectsByType(const CUUID& type);

// local filters ---------------------------------------------------------------
    /// set structure selection mode
    void SetStrObjectSelMode(EStrObjectSelMode mode);

    /// get structure selection mode
    EStrObjectSelMode GetStrObjectSelMode(void) const;

// informational methods -------------------------------------------------------
    /// get current active request
    CSelectionHandler*  GetRequestType(void);

    /// get current active request in string form
    const QString       GetRequestTypeString(void);

    /// get reason for a selection
    const QString       GetRequestReason(void);

    /// get requestor name
    const QString       GetRequestorName(void);

    /// get selection status
    ESelStatus          GetStatus(void);

// selected object methods ----------------------------------------------------
    /// number of selected objects
    int NumOfSelectedObjects(void);

    /// number of selected objects of given type or belonging to specified category
    int NumOfSelectedObjects(const CUUID& type);

    /// get selected object (pointer is important for SelectionModel)
    CSelObject* GetSelectedSelObject(int index);

    /// pop first selected object from the list
    CSelObject  PopSelectedSelObject(void);

    /// get selected object
    CProObject* GetSelectedObject(int index);

    /// pop first selected object from the list
    CProObject* PopSelectedObject(void);

// signals --------------------------------------------------------------------
signals:
    /// emmited when selection request is changed
    void OnSelectionRequestChanged(void);

    /// emmited when selection request is reseted
    void OnSelectionInitialized(void);

    /// emmited when new object is added into the selection list
    void OnObjectAdded(const CSelObject& obj);

    /// emmited when selected object is going to be removed from the selection list
    void OnObjectRemoved(const CSelObject& obj);

    /// emmited when selection request is fullfilled
    void OnSelectionCompleted(void);

    /// emmited when selection is changed
    void OnSelectionChanged(void);

// methods used by selection handlers -----------------------------------------
public:
    /// add object into the list and set all necessary connection for it
    void AddObject(const CSelObject& obj);

    /// remove object from the list and remove all connection for it
    void RemoveObject(const CSelObject& obj);

    /// is in list?
    bool IsInList(const CSelObject& obj);

    /// is in list?
    bool IsInList(CProObject* p_obj);

// section of private data ----------------------------------------------------
private:
    CSelectionRequest*      Request;
    QList<CSelObject>       SelectedObjects;
    ESelStatus              Status;
    EStrObjectSelMode       StrSelMode;

private slots:
    void ObjectDestroyed(QObject* p_obj);

    friend class CDirectionSelection;
};

// -----------------------------------------------------------------------------

#endif

