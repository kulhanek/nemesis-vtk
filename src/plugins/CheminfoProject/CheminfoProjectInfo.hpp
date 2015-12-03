#ifndef CHEMINFOPROJECTINFO_HPP
#define CHEMINFOPROJECTINFO_HPP

// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <SmallString.hpp>
#include "CheminfoProject.hpp"

//------------------------------------------------------------------------------

class CCheminfoProjectInfoList;
class CCheminfoProject;
class CFirebirdTransaction;
class CFirebirdQuerySQL;

//------------------------------------------------------------------------------

class CCheminfoProjectInfo : public CProObject {
    Q_OBJECT
public:
    // constructors and destructors -----------------------------------------------
    /// Constructor.
    /*!
     * \param
     * CCheminfoProjectInfo object has to be coonected to an CCheminfoProjectInfoList.
    */
    CCheminfoProjectInfo(CCheminfoProjectInfoList *p_il);

    CCheminfoProjectInfo(CCheminfoProjectInfoList *p_il,bool no_index);

    /// Destructor.
    ~CCheminfoProjectInfo(void);

    // section of public data ----------------------------------------------------
public:

    /// Set info about project of name from db
    bool SetProjectInfo(const CSmallString& name);

    bool SetProjectInfo(const CSmallString &name, CFirebirdTransaction* trans);

    bool LoadStructureList(void);

    /// Get base list
    CCheminfoProjectInfoList*  GetProjectInfoList(void) const;

    /// Get info about project of name
    CCheminfoProjectInfo* GetProjectInfo(void);

    /// Get every info about project
    int           GetID(void);
    CSmallString  GetName(void);
    CSmallString  GetOperator(void);
    CSmallString  GetDescription(void);
    int           GetNumberOfProperties(void);
    CSmallString  GetProperties(void);
    int           GetNumberOfResults(void);

    void EmitOnStatusChanged(void);

signals:
    void OnStatusChanged(void);

    // section of private data ----------------------------------------------------
private:
    CCheminfoProject* CheminfoProject;
    int          ID;
    CSmallString Name;
    CSmallString Operator;
    CSmallString Description;
    int          NumberOfProperties;
    CSmallString Properties;
    int          NumberOfResults;

};

//------------------------------------------------------------------------------


#endif // CHEMINFOPROJECTINFO_HPP
