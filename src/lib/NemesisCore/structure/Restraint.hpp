#ifndef RestraintH
#define RestraintH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <Point.hpp>
#include <ProObject.hpp>
#include <QVector>
#include <GeoMeasurement.hpp>

// -----------------------------------------------------------------------------

class CProject;
class CStructure;
class CRestraintList;
class CXMLElement;
class CPhysicalQuantity;
class CGraphicsObject;
class CProperty;

// ----------------------------------------------------------------------------

///  restraint implementation

class NEMESIS_CORE_PACKAGE CRestraint : public CProObject {
public:
// constructors and destructors ------------------------------------------------
    CRestraint(CRestraintList* p_bl);
    void RemoveFromBaseList(CHistoryNode* p_history=NULL);

// methods with changes registered into history list ---------------------------
    /// set associated property
    bool SetPropertyWH(CProperty* p_prop);

    /// set target restraint value
    bool SetTargetValueWH(double target);

    /// set force constant of harmonic restraint
    bool SetForceConstantWH(double force);

    /// set active flag
    bool SetEnabledWH(bool set);

    /// togle enabled/disabled flag
    bool ToggleEnabledDisabledWH(void);

    /// delete restraint from the list
    bool DeleteRestraintWH(void);

// informational methods ------------------------------------------------------
    /// get molecule owner
    CStructure* GetStructure(void) const;

    /// get bond list owner
    CRestraintList* GetRestraints(void) const;

    /// get associated property
    CProperty* GetProperty(void) const;

    /// get target restraint value
    double GetTargetValue(void);

    /// get current value
    double GetCurrentValue(void);

    /// get deviation value
    double GetDeviationValue(void);

    /// get force constant of harmonic restraint
    double GetForceConstant(void);

    /// is restraint enabled
    bool IsEnabled(void);

    /// get value unit
    CPhysicalQuantity* GetValueUnit(void);

    /// get force constant unit
    CPhysicalQuantity* GetForceConstantUnit(void);

// restraint setup -------------------------------------------------------------
    /// set name
    virtual void SetName(const QString& name,CHistoryNode* p_history=NULL);

    /// set description
    virtual void SetDescription(const QString& descrip,CHistoryNode* p_history=NULL);

    /// set associated property
    void SetProperty(CProperty* p_prop,CHistoryNode* p_history=NULL);

    /// set target restraint value
    void SetTargetValue(double target,CHistoryNode* p_history=NULL);

    /// set force constant of harmonic restraint
    void SetForceConstant(double force,CHistoryNode* p_history=NULL);

    /// set active flag
    void SetEnabled(bool set,CHistoryNode* p_history=NULL);

    /// change parent, see CRestraintList::MoveAllRestraintsFrom
    void ChangeParent(CRestraintList* p_newparent);

// restraint energy methods
    /// energy of restraint
    double GetEnergy(void);

    /// energy and gradients of restraint
    double GetEnergy(QVector<CAtomGrad>& grads);

// registered objects  ---------------------------------------------------------
public:
    virtual void RemoveFromRegistered(CProObject* p_object,
                                      CHistoryNode* p_history);

// input/output methods -------------------------------------------------------
    /// load restraint data
    virtual void LoadData(CXMLElement* p_ele);

    /// link object with given role - for delayed load
    virtual void LinkObject(const QString& role,CProObject* p_object);

    /// save restraint data
    virtual void SaveData(CXMLElement* p_ele);

// section of private data ----------------------------------------------------
protected:
    CProperty*          Property;
    CPhysicalQuantity*  ForceConstantPQ;

    double              TargetValue;
    double              ForceConstant;

protected:
    virtual ~CRestraint(void);
};

// -----------------------------------------------------------------------------

#endif

