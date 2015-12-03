#ifndef CCheminfoProjectStructureListProjectH
#define CCheminfoProjectStructureListProjectH
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

#include "CheminfoProjectStructureList.hpp"
#include "CheminfoProjectInfoList.hpp"

// -----------------------------------------------------------------------------

class CCheminfoProjectStructureList;
class CCheminfoProject;
class CCheminfoProjectInfoList;
class CProject;
class CXMLElement;

// -----------------------------------------------------------------------------

///  declare projects info list
class  CCheminfoProjectStructureListProject: public CProObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CCheminfoProjectStructureListProject(CProject* p_project);
    CCheminfoProjectStructureListProject(CProject* p_project,bool no_index);
    ~CCheminfoProjectStructureListProject(void);

//------------------------------------------------------------------------------
    /// Set active structure
    void SetActiveStructureList(CCheminfoProjectStructureList* p_strlist);
    void SetProjectInfoList(CCheminfoProjectInfoList* p_info);

// informational methods -------------------------------------------------------
    /// Return number of projects infos
    CCheminfoProjectInfoList*           GetProjectInfoList(void) const;
    /// Get active structure
    CCheminfoProjectStructureList*  GetActiveStructureList(void) const;

// external notification -------------------------------------------------------
    /// Emit OnCheminfoProjectStructureListChanged signal
    void EmitOnCheminfoProjectStructureListProjectChanged(void);

// -----------------------------------------------------------------------------
signals:
    /// Emmited when projects info list is changed
    void OnCheminfoProjectStructureListProjectChanged(void);

// section of private data -----------------------------------------------------
private:
    CCheminfoProject*              CheminfoProject;
    CCheminfoProjectInfoList*      ProjectInfoList;
    CCheminfoProjectStructureList* ActiveStructureList;
    bool                           Changed;
    int                            UpdateLevel;

    friend class CCheminfoProjectStructureList;

};

// -----------------------------------------------------------------------------

#endif

