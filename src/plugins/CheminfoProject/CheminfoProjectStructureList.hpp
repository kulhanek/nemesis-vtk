#ifndef CheminfoProjectStructureListH
#define CheminfoProjectStructureListH
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

#include "CheminfoProjectStructure.hpp"
#include "CheminfoProjectInfoList.hpp"

// -----------------------------------------------------------------------------

class CCheminfoProjectStructure;
class CCheminfoProjectStructureListProject;
class CCheminfoProject;
class CCheminfoProjectInfo;
class CProject;
class CXMLElement;

// -----------------------------------------------------------------------------

///  declare projects info list
class  CCheminfoProjectStructureList: public CProObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CCheminfoProjectStructureList(CProject* p_project,CCheminfoProjectInfo* p_info, CCheminfoProjectStructureListProject* p_listproj);
    ~CCheminfoProjectStructureList(void);

//------------------------------------------------------------------------------
    /// load projects info from database
    bool LoadStructureList (void);

//------------------------------------------------------------------------------
    void  SetProjectInfo(CCheminfoProjectInfo* p_info);
    /// create new projects info
    CCheminfoProjectStructure* CreateStructureWH(bool activate,const QString& name = QString(),
                                  const QString& descr = QString());

    /// create projects info from selected residues
    CCheminfoProjectStructure* CreateStructureFromSelectedResiduesWH(const QString& name = QString(),
                                  const QString& descr = QString());

    /// duplicate projects info, new projects info is activated
    CCheminfoProjectStructure* DuplicateStructureWH(CCheminfoProjectStructure* p_ostr);

    /// activate projects info - projects info must be owned by the list
    bool SetActiveStructureWH(CCheminfoProjectStructure* p_str);

    /// delete selected projects infos
    bool DeleteSelectedStructureWH(void);

    /// delete empty projects infos (e.g. projects infos without any atoms)
    bool DeleteEmptyStructureWH(void);

    /// delete all projects infos
    bool DeleteAllStructureWH(void);

    /// update sequence index by its order (e.g. it will remove gaps)
    bool SequenceIndexByOrderWH(void);

    /// merge projects infos
    bool MergeStructureWH(const QList<CCheminfoProjectStructure*>& projects_infos);

    /// merge all projects infos
    bool MergeAllStructureWH(void);

// executive methods -----------------------------------------------------------
    /// create empty projects info
    CCheminfoProjectStructure* CreateStructure(const QString& name = QString(),const QString& descr = QString(),
                                CHistoryNode* p_history=NULL);

    /// create projects info from XML stream
    CCheminfoProjectStructure* CreateStructure(CXMLElement* p_ele,CHistoryNode* p_history=NULL);

    /// duplicate projects info
    CCheminfoProjectStructure* DuplicateStructure(CCheminfoProjectStructure* p_ostr,bool activate, CHistoryNode* p_history=NULL);

    /// activate projects info - projects info must be owned by the list
    void SetActiveStructure(CCheminfoProjectStructure* p_str,CHistoryNode* p_history=NULL);

    /// delete selected projects infos
    void DeleteSelectedStructure(CHistoryNode* p_history=NULL);

    /// delete empty projects infos, e.g. projects infos that does not contain any atom
    void DeleteEmptyStructure(CHistoryNode* p_history=NULL);

    /// delete all projects infos
    void DeleteAllStructure(CHistoryNode* p_history=NULL);

    /// merge projects infos
    void MergeStructure(const QList<CCheminfoProjectStructure*>& projects_infos,CHistoryNode* p_history=NULL);

    /// clear projects infos
    void ClearStructure(void);

    /// sort projects infos by sequence index
    void SortStructure(void);

// informational methods -------------------------------------------------------
    /// return number of projects infos
    CCheminfoProjectInfo*       GetProjectInfo(void) const;
    /// get active structire
    CCheminfoProjectStructure*  GetActiveStructure(void) const;

    /// return number of projects infos
    int         GetNumberOfStructure(void) const;

    /// return number of selected projects infos
    int         GetNumberOfSelectedStructure(void) const;

    /// return number of empty projects infos
    int         GetNumberOfEmptyStructure(void) const;


// external notification -------------------------------------------------------
    /// block signals for massive update
    void BeginUpdate(void);

    /// end of massive update
    void EndUpdate(bool do_not_sort=false);

    /// emit OnCheminfoProjectStructureListChanged signal
    void EmitOnCheminfoProjectStructureListChanged(void);

// -----------------------------------------------------------------------------
signals:
    /// emmited when projects info list is changed
    void OnCheminfoProjectStructureListChanged(void);

// section of private data -----------------------------------------------------
private:
    CCheminfoProject*                     CheminfoProject;
    CCheminfoProjectStructureListProject* StructureListProject;
    CCheminfoProjectInfo*                 ProjectInfo;
    CCheminfoProjectStructure*            ActiveStructure;
    bool                      Changed;
    int                       UpdateLevel;


    friend class CCheminfoProjectStructure;

};

// -----------------------------------------------------------------------------

#endif

