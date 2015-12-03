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

#include <GSDesignerGeneral.hpp>
#include "GSDesignerGeneral.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGSDesignerGeneral::CGSDesignerGeneral(QWidget* p_owner,CGraphicsSetup* p_setup,
                                       CProObjectDesigner* p_designer)
    : QWidget(p_owner)
{    
    Setup = p_setup;
    if( Setup == NULL ){
        INVALID_ARGUMENT("p_setup is NULL");
    }
    Designer = p_designer;

    // insert to layout --------------------------
    if( p_owner->layout() ) delete p_owner->layout();
    QVBoxLayout* p_layout = new QVBoxLayout();
    p_layout->setContentsMargins(0,0,0,0);
    p_layout->addWidget(this);
    p_owner->setLayout(p_layout);

    // load graphics -----------------------------
    WidgetUI.setupUi(this);

    // connect signals ---------------------------
    if( Setup->GetProject() ){
        // project related signals
        connect(Setup->GetProject(),SIGNAL(OnStatusChanged(EStatusChanged)),
                this,SLOT(ProjectStatusChanged(EStatusChanged)));
        connect(WidgetUI.projectPB, SIGNAL(clicked()),
                Setup->GetProject(),SLOT(OpenObjectDesigner()));
        ProjectStatusChanged(ESC_NAME);
    } else {
        WidgetUI.projectPB->hide();
        WidgetUI.label1->hide();
        WidgetUI.nameLE->setEnabled(false);
    }

    if( (Setup->GetObjectID() > 0) && Setup->GetProject() ){
        connect(WidgetUI.objectPB, SIGNAL(clicked()),
                this,SLOT(OpenObjectDesigner()));
        QString text("%1");
        text = text.arg(Setup->GetObjectID());
        WidgetUI.objectPB->setText(text);
    } else {
        WidgetUI.objectPB->hide();
        WidgetUI.label2->hide();
    }

    // object related signals
    connect(Setup,SIGNAL(OnSetupChanged()),
            this,SLOT(InitValues()));
    //------------------
    connect(WidgetUI.nameLE, SIGNAL(textEdited(const QString &)),
            Designer,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.descriptionLE, SIGNAL(textEdited(const QString &)),
            Designer,SLOT(SetChangedFlagTrue()));

    // do not init values here, the owner of this componet is
    // responsible to call InitValues explicitly
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGSDesignerGeneral::InitValues(void)
{
    if( Designer->IsItChangingContent() ) return;

    WidgetUI.nameLE->setText(Setup->GetName());
    WidgetUI.descriptionLE->setText(Setup->GetDescription());
}

//------------------------------------------------------------------------------

void CGSDesignerGeneral::ApplyValues(void)
{
    Setup->SetName(WidgetUI.nameLE->text());
    Setup->SetDescription(WidgetUI.descriptionLE->text());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGSDesignerGeneral::ProjectStatusChanged(EStatusChanged status)
{
    if( status == ESC_NAME ){
        WidgetUI.projectPB->setText(Setup->GetProject()->GetName());
    }
}

//------------------------------------------------------------------------------

void CGSDesignerGeneral::OpenObjectDesigner(void)
{
    CProject* p_project = Setup->GetProject();
    if( p_project == NULL ) return;

    CProObject* p_object = NULL;
    if( Setup->GetObjectID() >= 0 ) {
        p_object = p_project->FindObject(Setup->GetObjectID());
    }
    if( p_object == NULL ) return;

    p_object->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

