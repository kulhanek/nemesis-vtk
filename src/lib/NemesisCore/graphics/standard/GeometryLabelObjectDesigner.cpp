// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <NemesisCoreModule.hpp>
#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <GraphicsObject.hpp>

#include <PODesignerGeneral.hpp>
#include <GODesignerObjects.hpp>
#include <GODesignerSetup.hpp>
#include <GODesignerRefBy.hpp>

#include <PhysicalQuantity.hpp>
#include <PhysicalQuantities.hpp>

#include <GraphicsProfileObject.hpp>
#include <GeometryLabelObject.hpp>
#include <GeometryLabelObjectDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GeometryLabelObjectDesignerCB(void* p_data);

CExtUUID        GeometryLabelObjectDesignerID(
                    "{GEOMETRY_LABEL_OBJECT_DESIGNER:d069a9a4-3e5c-4930-975c-2d807da589da}",
                    "Geometry Label");

CPluginObject   GeometryLabelObjectDesignerObject(&NemesisCorePlugin,
                    GeometryLabelObjectDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/GeometryLabel.svg",
                    GeometryLabelObjectDesignerCB);

// -----------------------------------------------------------------------------

GODESIGNER_ENTRY_POINT(GeometryLabelObject);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGeometryLabelObjectDesigner::CGeometryLabelObjectDesigner(CGeometryLabelObject* p_bo)
    : CProObjectDesigner(&GeometryLabelObjectDesignerObject, p_bo)
{
    Object = p_bo;

    // graphics layout ---------------------------
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Objects = new CGODesignerObjects(WidgetUI.objectsTab,Object,this);
    Setup = new CGODesignerSetup(WidgetUI.setupTab,Object,this);
    RefBy = new CGODesignerRefBy(WidgetUI.refByTab,Object,this);

    // units -------------------------------------
    WidgetUI.offsetXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.offsetYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.offsetZSB->setPhysicalQuantity(PQ_DISTANCE);

    // connect slots -----------------------------
    connect(WidgetUI.visibleCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.highlightedCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showTextCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showQuotationCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.relativePosCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.offsetXSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.offsetYSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.offsetZSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.buttonBox1, SIGNAL(clicked (QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(WidgetUI.buttonBox2, SIGNAL(clicked (QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));

    //------------------
    connect(Object, SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));
    //------------------
    connect(Object->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // init all values ---------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeometryLabelObjectDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    Objects->InitValues();
    Setup->InitValues();
    RefBy->InitValues();
    InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGeometryLabelObjectDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_DESCRIPTION,"background change") == false ) return;

    Changing = true;
        General->ApplyValues();
        ApplyValues();
    Changing = false;

    Object->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//------------------------------------------------------------------------------

void CGeometryLabelObjectDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Reset) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Reset) ) {
        InitAllValues();
        return;
    }

    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Apply) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Apply) ) {
        ApplyAllValues();
        return;
    }

    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Close) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Close) ) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeometryLabelObjectDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.visibleCB->setChecked(Object->IsFlagSet(EPOF_VISIBLE));
    WidgetUI.highlightedCB->setChecked(Object->IsFlagSet(EPOF_SELECTED));

    WidgetUI.showTextCB->setChecked(Object->IsFlagSet((EProObjectFlag)ESMOF_SHOW_TEXT));
    WidgetUI.relativePosCB->setChecked(Object->IsFlagSet((EProObjectFlag)ESMOF_RELATIVE_TEXT_POS));
    WidgetUI.showQuotationCB->setChecked(Object->IsFlagSet((EProObjectFlag)ESMOF_SHOW_QUOTATION));

    WidgetUI.offsetXSB->setInternalValue(Object->GetOffset().x);
    WidgetUI.offsetYSB->setInternalValue(Object->GetOffset().y);
    WidgetUI.offsetZSB->setInternalValue(Object->GetOffset().z);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGeometryLabelObjectDesigner::ApplyValues(void)
{
    CProObjectFlags flags = Object->GetFlags();

    SET_FLAG(flags,EPOF_VISIBLE,WidgetUI.visibleCB->isChecked())
    SET_FLAG(flags,EPOF_SELECTED,WidgetUI.highlightedCB->isChecked())

    SET_FLAG(flags,(EProObjectFlag)ESMOF_SHOW_TEXT,WidgetUI.showTextCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)ESMOF_RELATIVE_TEXT_POS,WidgetUI.relativePosCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)ESMOF_SHOW_QUOTATION,WidgetUI.showQuotationCB->isChecked())

    Object->SetFlagsWH(flags);

    CPoint offset;
    offset.x = WidgetUI.offsetXSB->getInternalValue();
    offset.y = WidgetUI.offsetYSB->getInternalValue();
    offset.z = WidgetUI.offsetZSB->getInternalValue();

    Object->SetOffsetWH(offset);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeometryLabelObjectDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    bool set = ! Object->IsHistoryLocked(EHCL_GRAPHICS);
    WidgetUI.flagsGB->setEnabled(set);
    WidgetUI.offsetGB->setEnabled(set);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


