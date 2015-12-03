#ifndef GraphicsManagerWorkPanelH
#define GraphicsManagerWorkPanelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <WorkPanel.hpp>
#include "ui_GraphicsManagerWorkPanel.h"

// -----------------------------------------------------------------------------

class CProject;
class CContainerModel;
class CGraphicsProfile;
class CGraphicsProfileList;

// -----------------------------------------------------------------------------

class CGraphicsManagerWorkPanel : public CWorkPanel {
    Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CGraphicsManagerWorkPanel(CProject* p_project);
    ~CGraphicsManagerWorkPanel();

// section of private data ----------------------------------------------------
private:
    Ui::GraphicsManagerWorkPanel    WidgetUI;
    CContainerModel*                ObjectsModel;
    CGraphicsProfileList*           ProfileList;
    CGraphicsProfile*               SelectedProfile;
    CContainerModel*                ProfileModel;

    /// raised when window is docked/undocked from the main window
    virtual void WindowDockStatusChanged(bool docked);

private slots:
    void HideObjectList(bool set);

    void OpenProjectDesigner(void);
    void ProjectNameChanged(CProject* p_project);

    void ObjectsListTVSelectionChanged(void);
    void ObjectsListTVDblClicked(const QModelIndex& index);
    void NewGraphicsObject(void);
    void ObjectShowHide(void);
    void DeleteObject(void);
    void ObjectInfo(void);

    void UpdateProfile(int index);

    void UpdateProfileList(void);
    void OpenProfileDesigner(void);
    void ActivateProfile(void);
    void NewProfile(void);
    void DeleteProfile(void);
    void ProfileInfo(void);

    void ProfileTVSelectionChanged(void);
    void ProfileTVDblClicked(const QModelIndex& index);

    void ProfileNewObject(void);
    void ProfileObjectAdd(void);
    void ProfileObjectUp(void);
    void ProfileObjectDown(void);
    void ProfileObjectShowHide(void);
    void ProfileObjectRemove(void);
    void ProfileObjectDelete(void);
    void ProfileObjectInfo(void);

    void ProjectLockChanged(EHistoryChangeMessage message);
};

// -----------------------------------------------------------------------------

#endif
