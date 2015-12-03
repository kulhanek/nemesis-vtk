#ifndef ExtComObjectDesignerH
#define ExtComObjectDesignerH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ComObject.hpp>
#include <QWidget>
#include <QMenu>

//------------------------------------------------------------------------------

class CXMLElement;
class CExtComObject;
class CProject;

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CExtComObjectDesigner : public QWidget, public CComObject   {
Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CExtComObjectDesigner(CPluginObject* p_pluginobject,CExtComObject* p_owner);
    ~CExtComObjectDesigner(void);

// associated object ----------------------------------------------------------
    /// get owner object
    CExtComObject* GetExtComObject(void);

// designer flags -------------------------------------------------------------
    /// set changed flag
    void SetChangedFlag(bool set);

    /// is the content changed?
    bool IsChangedFlagSet(void);

    /// return true if the object content is changing
    bool IsItChangingContent(void);

// popup menu -----------------------------------------------------------------
    /// add user action into popup menu
    void AddPopupMenuAction(QAction* p_act);

    /// add user separator into popup menu
    void AddPopupMenuSeparator(void);

public slots:
    /// set changed flag to true
    void SetChangedFlagTrue(void);

// section of private data ----------------------------------------------------
private:
    CExtComObject*  Object;
    bool            ContentsChanged;
    QMenu*          PopMenu;

    // window events --------------------------------
    /// handle pop-up menu
    virtual void contextMenuEvent(QContextMenuEvent* event);

    /// save setup
    virtual void closeEvent(QCloseEvent* event);

    // desktop subsystem -----------------------------
    // refine control in accord with record in Desktop list or if
    // it does not exist this is loaded from generic desktop
    // it is called in CExtComObject::OpenObjectDesigner
    void LoadObjectDesignerSetup(void);

    // save workpanel data to generic desktop
    // do not call this method in object destructor!!
    // this is diferent to work panel!
    // it is called in closeEvent()
    void SaveObjectDesignerSetup(void);

    /// get main window if it is available
    virtual QWidget* GetMainWindow(void);

private slots:
    /// handle signals from save default position action
    void savePositionAsDefault(void);

    friend class CExtComObject;

// section of protected data ---------------------------------------------------
protected:
    bool        Changing;               // changing object state

    /// load object designer setup - wrapper
    void LoadObjectDesignerSetup(CXMLElement* p_ele);

    /// save object designer setup - wrapper
    void SaveObjectDesignerSetup(CXMLElement* p_ele);

    /// load object designer setup
    virtual void LoadObjectDesignerSpecificData(CXMLElement* p_ele);

    /// save object designer setup
    virtual void SaveObjectDesignerSpecificData(CXMLElement* p_ele);
};

//---------------------------------------------------------------------------
#endif
