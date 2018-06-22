#ifndef WorkPanelListH
#define WorkPanelListH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ExtComObject.hpp>
#include <QList>
#include <WorkPanel.hpp>

//------------------------------------------------------------------------------

///  list of all workpanels

class NEMESIS_CORE_PACKAGE CWorkPanelList : public CExtComObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CWorkPanelList(CExtComObject* p_parent);

// desktop ---------------------------------------------------------------------
    /// restore work panels
    void RestoreWorkPanels(CProject* p_project);

    /// show work panels
    void ShowWorkPanels(CProject* p_project);

    /// save project setup
    void SaveWorkPanelSetup(CProject* p_project);

// methods ---------------------------------------------------------------------
    /// remove all workpanels
    void RemoveAllWorkPanels(bool force);

    /// remove all project workpanels
    void RemoveAllProjectWorkPanels(CProject* p_project);

    /// remove all global workpanels, eg. not associated with project
    void RemoveAllGlobalWorkPanels(void);

    /// find first opened workpanel by its uuid
    CWorkPanel* Find(const CUUID& uuid);

    /// find first opened workpanel by its uuid
    CWorkPanel* Find(const CUUID& uuid,CProject* p_project);

    /// get number of WPs
    int  GetNumberOfWorkPanels(void) const;

// signals ---------------------------------------------------------------------
signals:
    /// emmited when WP is added into the list
    void OnWorkPanelAdded(CWorkPanel* p_wp);

    /// emmited when WP is removed from the list
    void OnWorkPanelRemoved(CWorkPanel* p_wp);

// section of private data -----------------------------------------------------
public:
    QList<CWorkPanel*> WorkPanels;

private:
    void WorkPanelAdded(CWorkPanel* p_wp);
    void WorkPanelRemoved(CWorkPanel* p_wp);

    friend class CWorkPanelModel;
    friend class CWorkPanel;
};

//------------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE CWorkPanelList* WorkPanels;

//------------------------------------------------------------------------------

#endif
