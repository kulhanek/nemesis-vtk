// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <Property.hpp>
#include <StructureList.hpp>

#include <PRDesignerValue.hpp>
#include "PRDesignerValue.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPRDesignerValue::CPRDesignerValue(QWidget* p_owner,CProperty* p_prop,
                                   CProObjectDesigner* p_designer)
    : QWidget(p_owner)
{
    Property = p_prop;
    if( Property == NULL ){
        INVALID_ARGUMENT("Property is NULL");
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

    // set units ---------------------------------
    WidgetUI.valueLE->setPhysicalQuantity(Property->GetPropertyUnit());

    // connect slots -----------------------------
    connect(Property, SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));

    connect(WidgetUI.restrainPB, SIGNAL(clicked(bool)),
            this,SLOT(RestrainProperty()));
    //------------------
    CStructureList* p_sl = Property->GetProject()->GetStructures();
    if( p_sl != NULL ) {
        connect(p_sl,SIGNAL(OnGeometryChangeTick(void)),
                this,SLOT(InitValues(void)));
    }
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

void CPRDesignerValue::InitValues(void)
{
    if( Designer->IsItChangingContent() ) return;

    WidgetUI.valueLE->setInternalValue(Property->GetScalarValue());

    if( Property->IsReady() ){
        WidgetUI.dnrLA->hide();
    } else {
        WidgetUI.dnrLA->show();
    }

    WidgetUI.restrainPB->setEnabled(Property->IsReady());
}

//------------------------------------------------------------------------------

void CPRDesignerValue::LoadDesignerSetup(CXMLElement* p_ele)
{
    // nothing to do here
}

//------------------------------------------------------------------------------

void CPRDesignerValue::SaveDesignerSetup(CXMLElement* p_ele)
{
    // nothing to do here
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPRDesignerValue::RestrainProperty(void)
{
    Property->RestrainPropertyWH();
}

//------------------------------------------------------------------------------

void CPRDesignerValue::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    parentWidget()->setEnabled(! Property->IsHistoryLocked(EHCL_PROPERTY));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

