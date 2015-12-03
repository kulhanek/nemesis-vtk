#ifndef RestraintListH
#define RestraintListH
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
#include <Restraint.hpp>
#include <Vector.hpp>
#include <QHash>

// -----------------------------------------------------------------------------

class CStructure;
class CProject;
class CXMLElement;

// -----------------------------------------------------------------------------

///  declare restraints list

class NEMESIS_CORE_PACKAGE CRestraintList: public CProObject {
    Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CRestraintList(CStructure *p_str);
    ~CRestraintList(void);

// methods with changes registered into history list ---------------------------
    /// create restraint
    CRestraint* CreateRestraintWH(const QString& name=QString(),const QString& descr=QString());

    /// delete all restraints from list
    bool DeleteAllRestraintsWH(void);

    /// delete invalid restraints from list
    bool DeleteInvalidRestraintsWH(void);

    /// delete selected restraints from list
    bool DeleteSelectedRestraintsWH(void);

    /// enable/disable all restraints
    bool EnableRestraintsWH(bool set);

// informational methods -------------------------------------------------------
    /// are restarints allowed
    bool IsEnabled(void);

    /// is property restrained?
    bool IsRestrained(CProperty* p_prop);

    /// get molecule pointer
    CStructure*  GetStructure(void) const;

    /// get number of restraints
    int GetNumberOfRestraints(void);

    /// energy and gradients of restarint
    double GetEnergy(CVector& gradients,const QHash<CAtom*,int>& map);

// executive methods -----------------------------------------------------------
    /// create new restraint
    CRestraint* CreateRestraint(const QString& name = QString(),
                                const QString& descr = QString(),
                                CHistoryNode* p_history = NULL);

    /// create new restraint
    CRestraint* CreateRestraint(CXMLElement* p_data,
                                CHistoryNode* p_history = NULL);

    /// enable/disable all restraints
    void EnableRestraints(bool set, CHistoryNode* p_history = NULL);

    /// remove registrations for all restraints
    void UnregisterAllRegisteredRestraints(CHistoryNode* p_history=NULL);

    /// change parentship of all restraints
    void MoveAllRestraintsFrom(CRestraintList* p_source,CHistoryNode* p_history=NULL);

// input/output methods --------------------------------------------------------
    /// load all restraints
    virtual void LoadData(CXMLElement* p_ele);

    /// save all restraints
    virtual void SaveData(CXMLElement* p_ele);

// external notification -------------------------------------------------------
    /// block signals for massive update
    void BeginUpdate(void);

    /// end of massive update
    void EndUpdate(void);

    /// list size changed
    void ListSizeChanged(void);

public slots:
    /// emit OnRestraintListChanged signal
    void EmitOnRestraintListChanged(void);

// -----------------------------------------------------------------------------
signals:
    /// emited when list is changed
    void OnRestraintListChanged(void);

// section of private data -----------------------------------------------------
private:
    // for list manipulations
    bool    Changed;
    int     UpdateLevel;

    friend class CRestraint;
};

// -----------------------------------------------------------------------------

#endif

