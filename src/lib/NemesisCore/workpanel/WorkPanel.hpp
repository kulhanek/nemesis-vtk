#ifndef WorkPanelH
#define WorkPanelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2008-2009 Petr Kulhanek, kulhanek@enzim.hu,
//                            Jakub Stepan, xstepan3@chemi.muni.cz
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
#include <WorkPanelContainer.hpp>

#include <QWidget>
#include <QMenu>

//------------------------------------------------------------------------------

class CXMLElement;
class CWorkPanel;
class CProject;

//------------------------------------------------------------------------------

enum EWorkPanelRole {
    EWPR_DIALOG,            // dialog
    EWPR_TOOL,              // tool work panel
    EWPR_WINDOW,            // window work panel
    EWPR_DESIGNER           // object designer
};

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CWorkPanel: public QWidget, public CComObject {
    Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    CWorkPanel(CPluginObject* p_objectinfo,CProject* p_project,EWorkPanelRole role);
    ~CWorkPanel(void);

// informational setup ---------------------------------------------------------
    /// get associated project
    CProject*   GetProject(void);

// visual setup ----------------------------------------------------------------
    /// put work panel into DockWidget container - it should be called BEFORE LoadWorkPanelSetup
    bool SetDockWidget(bool set);

    /// show work panel as a dialog - it should be called AFTER LoadWorkPanelSetup
    int  ShowAsDialog(void);

    /// set dialog results
    void SetDialogResult(int code);

    /// add user action into popup menu
    void AddPopupMenuAction(QAction* p_act);

    /// add user separator into popup menu
    void AddPopupMenuSeparator(void);

    /// show full screen
    void ShowFullScreen(void);

    /// show normal
    void ShowNormal(void);

// desktop subsystem -----------------------------------------------------------
    /// load work panel setup
    void LoadWorkPanelSetup(void);

    /// save work panel setup
    void SaveWorkPanelSetup(void);

    /// hide workpanel
    void Hide(void);

    /// show workpanel
    void Show(void);

    virtual void closeEvent(QCloseEvent *event);

// change flag -----------------------------------------------------------------
    /// set change flag
    void SetChangedFlag(bool set);

    /// is changed flag set?
    bool IsChangedFlagSet(void);

public slots:
    /// set work panel contents changed (for setup WP)
    void SetChangedFlagTrue(void);

// section of private data -----------------------------------------------------
protected:
    EWorkPanelRole      Role;
    CWorkPanelContainer Container;          // container of work panel window

    QMenu*              PopMenu;            // workpanel popup menu

    /// raised when window is docked/undocked from the main window
    virtual void WindowDockStatusChanged(bool docked);

// restore work panel setup ----------------------------------------------------
    /// load work panel setup
    /** by default it restore the window geometry and call LoadWorkPanelSpecificData
    */
    virtual void LoadWorkPanelSetup(CXMLElement* p_ele);

    /// save work panel setup
    /** by default it store window geometry and call SaveWorkPanelSpecificData
    */
    virtual void SaveWorkPanelSetup(CXMLElement* p_ele);

    /// restore specific setup of the work panel
    /** by default it restore data view headers, splitters and tab positions
    */
    virtual void LoadWorkPanelSpecificData(CXMLElement* p_ele);

    /// save specific setup of the work panel
    /** by default it store data about view headers, splitters and tab positions
    */
    virtual void SaveWorkPanelSpecificData(CXMLElement* p_ele);


private:
    CProject*           Project;
    bool                ContentsChanged;

    /// return corresponding work panel setup
    CXMLElement* GetWorkPanelSetup(void);

// window events --------------------------------
    /// handle pop-up menu
    virtual void contextMenuEvent(QContextMenuEvent *event);

    friend class CDesktopData;
    friend class CWorkPanelDockWidget;
};

//------------------------------------------------------------------------------

#endif
