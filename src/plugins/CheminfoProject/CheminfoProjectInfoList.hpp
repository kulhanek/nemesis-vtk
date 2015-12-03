#ifndef CheminfoProjectInfoListH
#define CheminfoProjectInfoListH
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

#include "CheminfoProjectInfo.hpp"

// -----------------------------------------------------------------------------

class CCheminfoProjectInfo;
class CCheminfoProject;
class CProject;
class CXMLElement;

// -----------------------------------------------------------------------------

///  declare projects info list
class  CCheminfoProjectInfoList: public CProObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CCheminfoProjectInfoList(CProject* p_project);
    CCheminfoProjectInfoList(CProject* p_project,bool no_index);
    ~CCheminfoProjectInfoList(void);

//------------------------------------------------------------------------------
    /// load projects info from database
    bool LoadProjectInfoList (void);

//------------------------------------------------------------------------------
    /// set active structire
    void SetActiveProjectInfo(CCheminfoProjectInfo* p_info);
    /// create new projects info
    CCheminfoProjectInfo* CreateProjectInfoWH(bool activate,const QString& name = QString(),
                                  const QString& descr = QString());

    /// create projects info from selected residues
    CCheminfoProjectInfo* CreateProjectInfoFromSelectedResiduesWH(const QString& name = QString(),
                                  const QString& descr = QString());

    /// duplicate projects info, new projects info is activated
    CCheminfoProjectInfo* DuplicateProjectInfoWH(CCheminfoProjectInfo* p_ostr);

    /// activate projects info - projects info must be owned by the list
    bool SetActiveProjectInfoWH(CCheminfoProjectInfo* p_str);

    /// delete selected projects infos
    bool DeleteSelectedProjectInfoWH(void);

    /// delete empty projects infos (e.g. projects infos without any atoms)
    bool DeleteEmptyProjectInfoWH(void);

    /// delete all projects infos
    bool DeleteAllProjectInfoWH(void);

    /// update sequence index by its order (e.g. it will remove gaps)
    bool SequenceIndexByOrderWH(void);

    /// merge projects infos
    bool MergeProjectInfoWH(const QList<CCheminfoProjectInfo*>& projects_infos);

    /// merge all projects infos
    bool MergeAllProjectInfoWH(void);

// executive methods -----------------------------------------------------------
    /// create empty projects info
    CCheminfoProjectInfo* CreateProjectInfo(const QString& name = QString(),const QString& descr = QString(),
                                CHistoryNode* p_history=NULL);

    /// create projects info from XML stream
    CCheminfoProjectInfo* CreateProjectInfo(CXMLElement* p_ele,CHistoryNode* p_history=NULL);

    /// duplicate projects info
    CCheminfoProjectInfo* DuplicateProjectInfo(CCheminfoProjectInfo* p_ostr,bool activate, CHistoryNode* p_history=NULL);

    /// activate projects info - projects info must be owned by the list
    //void SetActiveProjectInfo(CCheminfoProjectInfo* p_str,CHistoryNode* p_history=NULL);

    /// delete selected projects infos
    void DeleteSelectedProjectInfo(CHistoryNode* p_history=NULL);

    /// delete empty projects infos, e.g. projects infos that does not contain any atom
    void DeleteEmptyProjectInfo(CHistoryNode* p_history=NULL);

    /// delete all projects infos
    void DeleteAllProjectInfo(CHistoryNode* p_history=NULL);

    /// merge projects infos
    void MergeProjectInfo(const QList<CCheminfoProjectInfo*>& projects_infos,CHistoryNode* p_history=NULL);

    /// clear projects infos
    void ClearProjectInfo(void);

    /// sort projects infos by sequence index
    void SortProjectInfo(void);

// informational methods -------------------------------------------------------
    /// get active structire
    CCheminfoProjectInfo*  GetActiveProjectInfo(void) const;

    /// return number of projects infos
    int         GetNumberOfProjectInfo(void) const;

    /// return number of selected projects infos
    int         GetNumberOfSelectedProjectInfo(void) const;

    /// return number of empty projects infos
    int         GetNumberOfEmptyProjectInfo(void) const;


// external notification -------------------------------------------------------
    /// block signals for massive update
    void BeginUpdate(void);

    /// end of massive update
    void EndUpdate(bool do_not_sort=false);

    /// emit OnCheminfoProjectInfoListChanged signal
    void EmitOnCheminfoProjectInfoListChanged(void);

// -----------------------------------------------------------------------------
signals:
    /// emmited when projects info list is changed
    void OnCheminfoProjectInfoListChanged(void);

// section of private data -----------------------------------------------------
private:
    CCheminfoProject*         CheminfoProject;
    CCheminfoProjectInfo*     ActiveProject;
    bool                      Changed;
    int                       UpdateLevel;


    friend class CCheminfoProjectInfo;

};

// -----------------------------------------------------------------------------

#endif

