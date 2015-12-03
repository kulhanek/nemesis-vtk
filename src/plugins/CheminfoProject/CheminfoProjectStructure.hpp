#ifndef CheminfoProjectStructure_HPP
#define CheminfoProjectStructure_HPP

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
#include <vector>
#include <SmallString.hpp>
#include "CheminfoProject.hpp"

using namespace std;

//------------------------------------------------------------------------------

class CCheminfoProjectStructureList;
class CCheminfoProject;
class CFirebirdTransaction;
class CFirebirdQuerySQL;

//------------------------------------------------------------------------------

class CCheminfoProjectStructure : public CProObject {
    Q_OBJECT
public:
    // constructors and destructors -----------------------------------------------
    /// Constructor.
    /*!
     * \param
     * CCheminfoProjectStructure object has to be connected to an CCheminfoProjectStructureList.
    */
    CCheminfoProjectStructure(CCheminfoProjectStructureList *p_il);

    CCheminfoProjectStructure(CCheminfoProjectStructureList *p_il,bool no_index);

    /// Destructor.
    ~CCheminfoProjectStructure(void);

    // section of public data ----------------------------------------------------
public:

    /// Get info about project of name
    CCheminfoProjectStructure* GetStructure(void);

    CSmallString  GetID(void);
    int           GetFlag(void);
    CSmallString  GetInChiKey(void);
    int           GetNumberOfAtoms(void);
    int           GetPartialCharge(void);
    double        GetMolarMass(void);
    vector<double>* GetResults(void);

    /// Get base list
    CCheminfoProjectStructureList*  GetStructureList(void) const;
    /// Get base project info
    CCheminfoProjectInfo*  GetProjectInfo(void) const;

    // input/output methods --------------------------------------------------------

    /// Set info about project of name from database
    bool LoadStructure(const CSmallString& name);

    /// Set info about project of name from database in the same transaction
    bool LoadStructure(const CSmallString &name, CFirebirdTransaction* trans);

    // setting methods  ----------------------------------------------------------

    void SetProjectInfo(CCheminfoProjectInfo* p_info);

    void EmitOnStatusChanged(void);

signals:
    void OnStatusChanged(void);

    // section of private data ----------------------------------------------------
private:
    CCheminfoProject* CheminfoProject;
    CCheminfoProjectInfo* ProjectInfo;
    CSmallString ID;
    int          Flag;
    CSmallString InChiKey;
    int          NumberOfAtoms;
    int          PartialCharge;
    double       MolarMass;
    vector<double> Results;

};

//------------------------------------------------------------------------------


#endif // CheminfoProjectStructure_HPP
