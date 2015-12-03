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

#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <GraphicsObject.hpp>
#include <ProObjectDesigner.hpp>

#include <PRDesignerGeneral.hpp>
#include "PRDesignerGeneral.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPRDesignerGeneral::CPRDesignerGeneral(QWidget* p_owner,CProject* p_object,
                                       CProObjectDesigner* p_designer)
    : QWidget(p_owner)
{
    Object = p_object;
    if( Object == NULL ){
        INVALID_ARGUMENT("p_object is NULL");
    }
    Designer = p_designer;

    // set layout --------------------------------
    if( p_owner->layout() ) delete p_owner->layout();
    QVBoxLayout* p_layout = new QVBoxLayout();
    p_layout->setContentsMargins(0,0,0,0);
    p_layout->addWidget(this);
    p_owner->setLayout(p_layout);

    // load graphics -----------------------------
    WidgetUI.setupUi(this);

    // connect signals ---------------------------
    // object related signals
    connect(Object,SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(ObjectStatusChanged(EStatusChanged)));
    //------------------
    connect(WidgetUI.descriptionTE, SIGNAL(textChanged()),
            Designer,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(Object->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // set initial values ------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    // do not init values here, the owner of this componet is
    // responsible to call InitValues explicitly
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPRDesignerGeneral::InitValues(void)
{
    if( Designer->IsItChangingContent() ) return;

    WidgetUI.nameLE->setText(Object->GetName());
    WidgetUI.pathLE->setText(QString(Object->GetPath()));
    WidgetUI.descriptionTE->setText(Object->GetDescription());
}

//------------------------------------------------------------------------------

void CPRDesignerGeneral::ApplyValues(void)
{
    Object->SetDescriptionWH(WidgetUI.descriptionTE->toPlainText());
}

//------------------------------------------------------------------------------

void CPRDesignerGeneral::LoadDesignerSetup(CXMLElement* p_ele)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CPRDesignerGeneral::SaveDesignerSetup(CXMLElement* p_ele)
{
    // nothing to be here
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPRDesignerGeneral::ObjectStatusChanged(EStatusChanged status)
{
    if( status == ESC_NAME ){
        WidgetUI.nameLE->setText(Object->GetName());
    }
    if( status == ESC_DESCRIPTION ){
        WidgetUI.descriptionTE->setText(Object->GetDescription());
    }
    if( status == ESC_OTHER ){
        WidgetUI.pathLE->setText(QString(Object->GetPath()));
    }
}

//------------------------------------------------------------------------------

void CPRDesignerGeneral::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    parentWidget()->setEnabled(! Object->IsHistoryLocked(EHCL_DESCRIPTION));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

