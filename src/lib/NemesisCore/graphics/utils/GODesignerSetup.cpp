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

#include <GODesignerSetup.hpp>
#include "GODesignerSetup.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGODesignerSetup::CGODesignerSetup(QWidget* p_owner,CGraphicsObject* p_object,
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
    // rather connect local slot than setup slot
    // because setup object can be changed
    connect(WidgetUI.setupNamePB,SIGNAL(clicked()),
            this,SLOT(OpenSetupDesigner()));
    // -------------
    connect(WidgetUI.globalSetupCB,SIGNAL(toggled(bool)),
            this,SLOT(SetGlobalSetup(bool)));
    // -------------
    connect(WidgetUI.projectSetupCB,SIGNAL(toggled(bool)),
            this,SLOT(SetProjectSetup(bool)));
    // -------------
    connect(WidgetUI.removeProjectSetupPB,SIGNAL(clicked()),
            this,SLOT(RemoveProjectSetup()));
    // -------------
    connect(WidgetUI.objectSetupCB,SIGNAL(toggled(bool)),
            this,SLOT(SetObjectSetup(bool)));
    // -------------
    connect(WidgetUI.removeObjectSetupPB,SIGNAL(clicked()),
            this,SLOT(RemoveObjectSetup()));
    // -------------
    connect(Object,SIGNAL(OnSetupChanged()),
            this,SLOT(InitValues()));
    //------------------
    connect(Object->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // set initial values ------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    // do not init values here, the owner of this componet is
    // responsible to call InitValues explicitly
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGODesignerSetup::InitValues(void)
{
    if( Designer->IsItChangingContent() ) return;

    CGraphicsSetup* p_setup = Object->GetSetup();

    if( p_setup == NULL ) return; // invalid setup

    WidgetUI.setupNamePB->setText(p_setup->GetName());

    switch(p_setup->GetSetupMode()){
        case EGSM_GLOBAL_SETUP:
            WidgetUI.globalSetupCB->setChecked(true);
            break;
        case EGSM_PROJECT_SETUP:
            WidgetUI.projectSetupCB->setChecked(true);
            break;
        case EGSM_OBJECT_SETUP:
            WidgetUI.objectSetupCB->setChecked(true);
            break;
    }

    WidgetUI.removeProjectSetupPB->setEnabled(Object->HasProjectSetup());
    WidgetUI.removeObjectSetupPB->setEnabled(Object->HasObjectSetup());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGODesignerSetup::OpenSetupDesigner(void)
{
    CGraphicsSetup* p_setup = Object->GetSetup();
    if( p_setup == NULL ) return;
    p_setup->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CGODesignerSetup::SetGlobalSetup(bool checked)
{
    if( checked == false ) return;
    Object->SetGlobalSetup();
}

//------------------------------------------------------------------------------

void CGODesignerSetup::SetProjectSetup(bool checked)
{
    if( checked == false ) return;
    Object->SetProjectSetup();
}

//------------------------------------------------------------------------------

void CGODesignerSetup::RemoveProjectSetup(void)
{
    // TODO - confirm dialog
    Object->RemoveProjectSetup();
}

//------------------------------------------------------------------------------

void CGODesignerSetup::SetObjectSetup(bool checked)
{
    if( checked == false ) return;
    Object->SetObjectSetup();
}

//------------------------------------------------------------------------------

void CGODesignerSetup::RemoveObjectSetup(void)
{
    // TODO - confirm dialog
    Object->RemoveObjectSetup();
}

//------------------------------------------------------------------------------

void CGODesignerSetup::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    parentWidget()->setEnabled(! Object->IsHistoryLocked(EHCL_GRAPHICS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

