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
#include <GeoProperty.hpp>
#include <PRDesignerGraphics.hpp>
#include <PhysicalQuantity.hpp>
#include <PhysicalQuantities.hpp>

#include <PRDesignerGeoGraphics.hpp>
#include "PRDesignerGeoGraphics.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPRDesignerGeoGraphics::CPRDesignerGeoGraphics(QWidget* p_owner,CGeoProperty* p_prop,
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

    // graphics link menu
    Graphics = new CPRDesignerGraphics(WidgetUI.graphicsGB,Property,Designer);

    // units -------------------------------------
    WidgetUI.labelXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.labelYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.labelZSB->setPhysicalQuantity(PQ_DISTANCE);

    // connect signals ---------------------------
    // object related signals
    connect(WidgetUI.visibleCB, SIGNAL(clicked(bool)),
            Designer,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.highlightedCB, SIGNAL(clicked(bool)),
            Designer,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showLabelCB, SIGNAL(clicked(bool)),
            Designer,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showQuotationCB, SIGNAL(clicked(bool)),
            Designer,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.relPosCB, SIGNAL(clicked(bool)),
            Designer,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showCOMsCB, SIGNAL(clicked(bool)),
            Designer,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showCOMsQuotationCB, SIGNAL(clicked(bool)),
            Designer,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.labelXSB, SIGNAL(valueChanged(double)),
            Designer,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.labelYSB, SIGNAL(valueChanged(double)),
            Designer,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.labelZSB, SIGNAL(valueChanged(double)),
            Designer,SLOT(SetChangedFlagTrue()));

    //------------------
    connect(Property, SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));

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

void CPRDesignerGeoGraphics::InitValues(void)
{
    if( Designer->IsItChangingContent() ) return;

    Graphics->InitValues();

    WidgetUI.visibleCB->setChecked(Property->IsFlagSet(EPOF_VISIBLE));
    WidgetUI.highlightedCB->setChecked(Property->IsFlagSet(EPOF_SELECTED));

    WidgetUI.showLabelCB->setChecked(Property->IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_SHOW_LABEL));
    WidgetUI.showQuotationCB->setChecked(Property->IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_SHOW_LABEL_QUOTATION));
    WidgetUI.relPosCB->setChecked(Property->IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_RELATIVE_LABEL_POS));
    WidgetUI.showCOMsCB->setChecked(Property->IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_SHOW_COM));
    WidgetUI.showCOMsQuotationCB->setChecked(Property->IsFlagSet<EGeoPropertyObjectFlag>(EGPOF_SHOW_COM_QUOTATION));

    CPoint pos = Property->GetLabelPosition();

    WidgetUI.labelXSB->setInternalValue(pos.x);
    WidgetUI.labelYSB->setInternalValue(pos.y);
    WidgetUI.labelZSB->setInternalValue(pos.z);
}

//------------------------------------------------------------------------------

void CPRDesignerGeoGraphics::ApplyValues(void)
{
    CProObjectFlags flags = Property->GetFlags();
    SET_FLAG(flags,EPOF_VISIBLE,WidgetUI.visibleCB->isChecked());
    SET_FLAG(flags,EPOF_SELECTED,WidgetUI.highlightedCB->isChecked());

    SET_FLAG(flags,(EProObjectFlag)EGPOF_SHOW_LABEL,WidgetUI.showLabelCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EGPOF_SHOW_LABEL_QUOTATION,WidgetUI.showQuotationCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EGPOF_RELATIVE_LABEL_POS,WidgetUI.relPosCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EGPOF_SHOW_COM,WidgetUI.showCOMsCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EGPOF_SHOW_COM_QUOTATION,WidgetUI.showCOMsQuotationCB->isChecked())

    Property->SetFlagsWH(flags);

    CPoint pos;
    pos.x = WidgetUI.labelXSB->getInternalValue();
    pos.y = WidgetUI.labelYSB->getInternalValue();
    pos.z = WidgetUI.labelZSB->getInternalValue();

    Property->SetLabelPositionWH(pos);
}

//------------------------------------------------------------------------------

void CPRDesignerGeoGraphics::LoadDesignerSetup(CXMLElement* p_ele)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CPRDesignerGeoGraphics::SaveDesignerSetup(CXMLElement* p_ele)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CPRDesignerGeoGraphics::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    setEnabled(! Property->IsHistoryLocked(EHCL_PROPERTY));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

