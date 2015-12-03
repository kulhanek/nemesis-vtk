#ifndef ObjectLinkWidgetH
#define ObjectLinkWidgetH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

#include <QWidget>
#include <ProObject.hpp>
#include "ui_ObjectLinkWidget.h"

//------------------------------------------------------------------------------

class CProject;
class CProObject;
class CSelectionRequest;
class CSelectionHandler;

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE QObjectLinkWidget : public QWidget {
Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    QObjectLinkWidget(QWidget* p_owner);

// executive methods -----------------------------------------------------------
    /// set project
    void setProject(CProject* p_project);

    /// set selection handler
    void setSelectionHandler(CSelectionHandler* p_request);

    /// set supported object MIME type (for drop actions)
    /** project must be already set by setProject()
    */
    void setObjectBaseMIMEType(const QString& baseid);

    /// set object
    void setObject(CProObject* p_object);

    /// set residue mode, e.g. show object name in alternative format
    void setResidueMode(bool set);

    /// select object manually
    void selectObjectManually(CProObject* p_object);

    /// reject object change by current change action, see OnObjectToBeChanged
    void rejectObjectChange(void);

    /// show remove button - it should be called after setSelectionHandler
    void showRemoveButton(bool set);

    /// get object
    CProObject* getObject(void);

    /// get object
    template <typename T1>
    inline T1 getObject(void);

signals:
    /// emited when object link is changed
    void OnObjectChanged(void);

    /// emited when object is about to be set by any action
    /** action can be canceled by rejectObjectChange();
    */
    void OnObjectToBeChanged(CProObject* p_object);

// section of private data -----------------------------------------------------
private:
    Ui::ObjectLinkWidget    WidgetUI;
    CProject*               Project;
    CSelectionHandler*      SelHandler;
    CProObject*             Object;
    CSelectionRequest*      SelRequest;
    bool                    ResidueMode;    // show name in alternative format
    QString                 MIMEType;
    bool                    AcceptChange;

    /// can we accept the event
    void PBDragEnterEvent(QDragEnterEvent* p_event);

    /// process drop event
    void PBDropEvent(QDropEvent* p_event);

    friend class QObjectLinkWidgetPushButton;

private slots:
    void OpenObjectDesigner(void);
    void ObjectDestroyed(void);
    void UnlinkObject(void);
    void SelectObject(bool checked);
    void SelectionDetached(void);
    void ObjectStatusChanged(EStatusChanged status);
    void SelectionCompleted(void);
};

//------------------------------------------------------------------------------

template <typename T1>
inline T1 QObjectLinkWidget::getObject(void)
{
    return( dynamic_cast<T1>(getObject()) );
}

//------------------------------------------------------------------------------

#endif
