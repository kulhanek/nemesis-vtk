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
#include <ProObject.hpp>
#include <ProObjectDesigner.hpp>
#include <SelectionList.hpp>

#include <PODesignerGeneral.hpp>
#include "PODesignerGeneral.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPODesignerGeneral::CPODesignerGeneral(QWidget* p_owner,CProObject* p_object,
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
    if( Object->GetProject() ){
        // project related signals
        connect(Object->GetProject(),SIGNAL(OnStatusChanged(EStatusChanged)),
                this,SLOT(ProjectStatusChanged(EStatusChanged)));
        connect(WidgetUI.projectPB, SIGNAL(clicked()),
                Object->GetProject(),SLOT(OpenObjectDesigner()));
    }
    // object related signals
    connect(Object,SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(ObjectStatusChanged(EStatusChanged)));
    //------------------
    connect(WidgetUI.nameLE, SIGNAL(textEdited(const QString &)),
            Designer,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.selectPB, SIGNAL(clicked(bool)),
            this,SLOT(TryToSelect()));

    //------------------
    connect(WidgetUI.descriptionLE, SIGNAL(textEdited(const QString &)),
            Designer,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(Object->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // set initial values ------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    // do not init values here, the owner of this component is
    // responsible to call InitValues explicitly
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPODesignerGeneral::InitValues(void)
{
    if( Designer->IsItChangingContent() ) return;

    if( Object->GetProject() != NULL) {
        WidgetUI.projectPB->setText(Object->GetProject()->GetName());
    } else {
        WidgetUI.projectPB->setText("none");
    }

    WidgetUI.nameLE->setText(Object->GetName());
    WidgetUI.nameLE->setEnabled(!Object->IsFlagSet(EPOF_RO_NAME));
    WidgetUI.descriptionLE->setText(Object->GetDescription());
    WidgetUI.descriptionLE->setEnabled(!Object->IsFlagSet(EPOF_RO_DESCRIPTION));
}

//------------------------------------------------------------------------------

void CPODesignerGeneral::ApplyValues(void)
{
    Object->SetNameWH(WidgetUI.nameLE->text());
    Object->SetDescriptionWH(WidgetUI.descriptionLE->text());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPODesignerGeneral::ProjectStatusChanged(EStatusChanged status)
{
    if( status == ESC_NAME ){
        WidgetUI.projectPB->setText(Object->GetProject()->GetName());
    }
}

//------------------------------------------------------------------------------

void CPODesignerGeneral::ObjectStatusChanged(EStatusChanged status)
{
    if( status == ESC_NAME ){
        WidgetUI.nameLE->setText(Object->GetName());
    }
    if( status == ESC_DESCRIPTION ){
        WidgetUI.descriptionLE->setText(Object->GetDescription());
    }
}

//------------------------------------------------------------------------------

void CPODesignerGeneral::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    parentWidget()->setEnabled(! Object->IsHistoryLocked(EHCL_DESCRIPTION));
}

//------------------------------------------------------------------------------

void CPODesignerGeneral::TryToSelect(void)
{
    CSelObject sobj(Object,0);
    CProject* p_project = Object->GetProject(); // object might be destroyed by the selection
    p_project->GetSelection()->RegisterObject(sobj);
    p_project->RepaintProject();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

