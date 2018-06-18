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

#include <ErrorSystem.hpp>
#include <NemesisCoreModule.hpp>
#include <GraphicsObject.hpp>
#include <ProObjectDesigner.hpp>
#include <Project.hpp>
#include <SelectionRequest.hpp>
#include <SelectionList.hpp>
#include <Residue.hpp>

#include <QMimeData>

#include <ObjectLinkWidget.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObjectLinkWidget::QObjectLinkWidget(QWidget* p_owner)
    : QWidget(p_owner)
{
    // load graphics -----------------------------
    WidgetUI.setupUi(this);

    Project = NULL;
    SelHandler = NULL;
    Object = NULL;
    SelRequest = new CSelectionRequest(this,"link object selector");
    ResidueMode = false;
    AcceptChange = true;

    WidgetUI.removeObjectPB->hide();
    WidgetUI.selectObjectPB->hide();

    // connect signals ---------------------------
    connect(WidgetUI.objectNamePB,SIGNAL(clicked(bool)),
            this,SLOT(OpenObjectDesigner()));
    //------------------
    connect(WidgetUI.removeObjectPB,SIGNAL(clicked(bool)),
            this,SLOT(UnlinkObject()));
    //------------------
    connect(WidgetUI.selectObjectPB,SIGNAL(clicked(bool)),
            this,SLOT(SelectObject(bool)));
    //------------------
    connect(SelRequest,SIGNAL(OnSelectionCompleted()),
            this,SLOT(SelectionCompleted()));
    //------------------
    connect(SelRequest,SIGNAL(OnDetached()),
            this,SLOT(SelectionDetached()));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QObjectLinkWidget::setProject(CProject* p_project)
{
    Project = p_project;
}

//------------------------------------------------------------------------------

void QObjectLinkWidget::setSelectionHandler(CSelectionHandler* p_request)
{
    SelHandler = p_request;
    if( SelHandler ){
        WidgetUI.removeObjectPB->show();
        WidgetUI.selectObjectPB->show();
    } else {
        WidgetUI.removeObjectPB->hide();
        WidgetUI.selectObjectPB->hide();
    }
}

//------------------------------------------------------------------------------

void QObjectLinkWidget::setObjectBaseMIMEType(const QString& mime)
{
    if( Project == NULL ) return;
    MIMEType =  Project->GetProjectMIMEType(mime);
    WidgetUI.objectNamePB->setAcceptDrops(true);
}

//------------------------------------------------------------------------------

void QObjectLinkWidget::setObject(CProObject* p_object)
{
    AcceptChange = true;
    emit OnObjectToBeChanged(p_object);  // inform user about change action
    if( ! AcceptChange )  return;

    if( Object ){
        Object->disconnect(this);
    }

    Object = p_object;

    if( Object ){
        connect(Object,SIGNAL(OnStatusChanged(EStatusChanged)),
                this,SLOT(ObjectStatusChanged(EStatusChanged)));
        connect(Object,SIGNAL(destroyed(QObject*)),
                this,SLOT(ObjectDestroyed(void)));
    }
    ObjectStatusChanged(ESC_NAME);

    emit OnObjectChanged();
}

//------------------------------------------------------------------------------

void QObjectLinkWidget::setResidueMode(bool set)
{
    if( ResidueMode == set ) return;
    ResidueMode = set;
    ObjectStatusChanged(ESC_NAME);
}

//------------------------------------------------------------------------------

void QObjectLinkWidget::selectObjectManually(CProObject* p_object)
{
    if( Project == NULL ) return;
    if( SelHandler == NULL ) return;

    if( SelRequest->IsAttached() ){
        Project->GetSelection()->RegisterObject(CSelObject(p_object,0));
    }
}

//------------------------------------------------------------------------------

CProObject* QObjectLinkWidget::getObject(void)
{
    return(Object);
}

//------------------------------------------------------------------------------

void QObjectLinkWidget::rejectObjectChange(void)
{
    AcceptChange = false;
}

//------------------------------------------------------------------------------

void QObjectLinkWidget::showRemoveButton(bool set)
{
    if( set ){
        WidgetUI.removeObjectPB->show();
    } else {
        WidgetUI.removeObjectPB->hide();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QObjectLinkWidget::OpenObjectDesigner(void)
{
    if( Object ){
        Object->OpenObjectDesigner();
    }
}

//------------------------------------------------------------------------------

void QObjectLinkWidget::ObjectDestroyed(void)
{
    Object = NULL;
    WidgetUI.objectNamePB->setText("");
    emit OnObjectChanged();
}

//------------------------------------------------------------------------------

void QObjectLinkWidget::UnlinkObject(void)
{
    setObject(NULL);
}

//------------------------------------------------------------------------------

void QObjectLinkWidget::SelectObject(bool checked)
{
    if( Project == NULL ) return;
    if( SelHandler == NULL ) return;

    if( checked ){
        SelRequest->SetRequest(Project->GetSelection(),SelHandler,"to get link object");
    } else {
        SelRequest->SetRequest(Project->GetSelection());
    }
}

//------------------------------------------------------------------------------

void QObjectLinkWidget::ObjectStatusChanged(EStatusChanged status)
{
    if( Object == NULL ){
        WidgetUI.objectNamePB->setText("");
        return;
    }
    CResidue* p_res = dynamic_cast<CResidue*>(Object);
    if( ResidueMode && ( p_res != NULL) ) {
        if( status != ESC_DESCRIPTION ){
            WidgetUI.objectNamePB->setText(p_res->GetLongName());
        }
    } else {
        if( status == ESC_NAME ){
            WidgetUI.objectNamePB->setText(Object->GetName());
        }
    }
}

//------------------------------------------------------------------------------

void QObjectLinkWidget::SelectionCompleted(void)
{
    CProObject* p_object = Project->GetSelection()->PopSelectedObject();
    Project->GetSelection()->SetRequest(NULL);
    if( p_object ){
        setObject(p_object);
    }
    WidgetUI.selectObjectPB->setChecked(false);
}

//------------------------------------------------------------------------------

void QObjectLinkWidget::SelectionDetached(void)
{
    WidgetUI.selectObjectPB->setChecked(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QObjectLinkWidget::PBDragEnterEvent(QDragEnterEvent* p_event)
{
    if( Project == NULL ) return;

    // can we accept?
    if( p_event->mimeData()->hasFormat(MIMEType) ){
        p_event->acceptProposedAction();
    }
}

//------------------------------------------------------------------------------

void QObjectLinkWidget::PBDropEvent(QDropEvent* p_event)
{
    if( Project == NULL ) return;

    QByteArray encodedData = p_event->mimeData()->data(MIMEType);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while( ! stream.atEnd() ) {
        int index;
        stream >> index;
        CProObject* p_obj = Project->FindObject(index);
        if( p_obj == NULL ) continue;
        setObject(p_obj);
        break;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


