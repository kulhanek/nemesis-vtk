#ifndef PropertyListH
#define PropertyListH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ProObject.hpp>

// -----------------------------------------------------------------------------

class CProperty;

// -----------------------------------------------------------------------------

///  List of all properties in project

class NEMESIS_CORE_PACKAGE CPropertyList : public CProObject {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    CPropertyList(CProject* p_project);
    CPropertyList(CProject* p_project,bool no_index);
    virtual ~CPropertyList(void);

// methods with changes registered into history list ---------------------------
    /// create property
    CProperty* CreatePropertyWH(
                    const CUUID& objectuuid,
                    const QString& name  = QString(),
                    const QString& descr = QString());

    /// delete all properties
    bool DeleteAllPropertiesWH(void);

    /// delete invalid properties
    bool DeleteInvalidPropertiesWH(void);

    /// delete selected properties
    bool DeleteSelectedPropertiesWH(void);

// executive methods without change registration to history list --------------
    /// create property
    CProperty* CreateProperty(const CUUID& objectuuid,
                    const QString& name = QString(),
                    const QString& descr = QString(),
                    CHistoryNode* p_history = NULL);

    /// create new graphics object
    CProperty* CreateProperty(CXMLElement* p_data,CHistoryNode* p_history = NULL);

    /// delete all properties
    void DeleteAllProperties(CHistoryNode* p_history);

    /// delete invalid properties
    void DeleteInvalidProperties(CHistoryNode* p_history);

    /// delete selected properties
    void DeleteSelectedProperties(CHistoryNode* p_history);

    /// clear all properties
    void ClearProperties(void);

// executive methods without change registered to history list -----------------
    /// get number of properties
    int GetNumberOfProperties(void);

    /// get number of selected properties
    int GetNumberOfSelectedProperties(void);

    /// get number of invalid properties
    int GetNumberOfInvalidProperties(void);

    /// get property
    CProperty* GetProperty(int index);

// input/output methods --------------------------------------------------------
    /// load all structures
    virtual void LoadData(CXMLElement* p_ele);

    /// save all structures
    virtual void SaveData(CXMLElement* p_ele);

// external notification -------------------------------------------------------
    /// emit OnPropertyListChanged signal
    void EmitOnPropertyListChanged(void);

// -----------------------------------------------------------------------------
signals:
    /// emmited when property list is changed
    void OnPropertyListChanged(void);

// section of private data ----------------------------------------------------
private:
    friend class CProperty;
};

// -----------------------------------------------------------------------------

#endif

