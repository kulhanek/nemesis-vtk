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
#include <ProObjectDesigner.hpp>
#include <CategoryUUID.hpp>
#include <GraphicsProperty.hpp>
#include <GraphicsObjectLink.hpp>

#include <PRDesignerGraphics.hpp>
#include "PRDesignerGraphics.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPRDesignerGraphics::CPRDesignerGraphics(QWidget* p_owner,CGraphicsProperty* p_gpro,
                                       CProObjectDesigner* p_designer)
    : QWidget(p_owner)
{
    Property = p_gpro;
    if( Property == NULL ){
        INVALID_ARGUMENT("p_list is NULL");
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

    //------------------
    connect(Property,SIGNAL(OnGraphicsLinkUpdated(void)),
            this,SLOT(InitValues(void)));
    //------------------
    connect(WidgetUI.createLinkPB,SIGNAL(clicked(bool)),
            this,SLOT(CreateLink(void)));
    //------------------
    connect(WidgetUI.showHideLinkPB,SIGNAL(clicked(bool)),
            this,SLOT(ShowHideLink(void)));
    //------------------
    connect(WidgetUI.removeLinkPB,SIGNAL(clicked(bool)),
            this,SLOT(RemoveLink(void)));
    //------------------
    connect(Property->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // set initial values ------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    // do not init values here, the owner of this componet is
    // responsible to call InitValues explicitly
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPRDesignerGraphics::InitValues(void)
{   
    // connect slots -----------------------------
    if( Property->GetLinkObject() ){
        // disconnect previous bindings
        Property->GetLinkObject()->disconnect(this);
        WidgetUI.objectLinkPB->disconnect(Property->GetLinkObject());

        // project related signals
        connect(Property->GetLinkObject(),SIGNAL(OnStatusChanged(EStatusChanged)),
                this,SLOT(LinkObjectStatusChanged(EStatusChanged)));
        //------------------
        connect(WidgetUI.objectLinkPB, SIGNAL(clicked()),
                Property->GetLinkObject(),SLOT(OpenObjectDesigner()));

        // update link text
        WidgetUI.objectLinkPB->setText(Property->GetLinkObject()->GetName());
    } else {
        WidgetUI.objectLinkPB->setText("");
    }
}

//------------------------------------------------------------------------------

void CPRDesignerGraphics::LoadDesignerSetup(CXMLElement* p_ele)
{
    // nothing to do here
}

//------------------------------------------------------------------------------

void CPRDesignerGraphics::SaveDesignerSetup(CXMLElement* p_ele)
{
    // nothing to do here
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPRDesignerGraphics::CreateLink(void)
{
    Property->CreateGraphicsObjectWH();
}

//------------------------------------------------------------------------------

void CPRDesignerGraphics::ShowHideLink(void)
{
    Property->ToggleShowHideFlagWH();
}

//------------------------------------------------------------------------------

void CPRDesignerGraphics::RemoveLink(void)
{
    Property->RemoveGraphicsObjectWH();
}

//------------------------------------------------------------------------------

void CPRDesignerGraphics::LinkObjectStatusChanged(EStatusChanged status)
{
    if( (status == ESC_NAME) && (Property->GetLinkObject() != NULL) ){
        WidgetUI.objectLinkPB->setText(Property->GetLinkObject()->GetName());
    }
}

//------------------------------------------------------------------------------

void CPRDesignerGraphics::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    parentWidget()->setEnabled(! Property->IsHistoryLocked(EHCL_GRAPHICS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

