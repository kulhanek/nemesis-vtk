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

#include <NemesisCoreModule.hpp>
#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <ContainerModel.hpp>
#include <Structure.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>

#include <PODesignerGeneral.hpp>
#include <GODesignerObjects.hpp>
#include <GODesignerSetup.hpp>
#include <GODesignerRefBy.hpp>

#include <GraphicsProfileObject.hpp>
#include <PointObject.hpp>
#include <PointObjectDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PointObjectDesignerCB(void* p_data);

CExtUUID        PointObjectDesignerID(
                    "{POINT_OBJECT_DESIGNER:ef11d98a-3e39-4c9b-a4a1-8e0acc56239f}",
                    "Point");

CPluginObject   PointObjectDesignerObject(&NemesisCorePlugin,
                    PointObjectDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/Point.svg",
                    PointObjectDesignerCB);

// -----------------------------------------------------------------------------

GODESIGNER_ENTRY_POINT(PointObject);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPointObjectDesigner::CPointObjectDesigner(CPointObject* p_fmo)
    : CProObjectDesigner(&PointObjectDesignerObject, p_fmo)
{
    Object = p_fmo;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Objects = new CGODesignerObjects(WidgetUI.objectsTab,Object,this);
    Setup = new CGODesignerSetup(WidgetUI.setupTab,Object,this);
    RefBy = new CGODesignerRefBy(WidgetUI.refByTab,Object,this);

    // units -------------------------------------
    WidgetUI.posXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.posYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.posZSB->setPhysicalQuantity(PQ_DISTANCE);

    // connect slots -----------------------------
    connect(WidgetUI.visibleCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.highlightedCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showQuotationCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showCOGCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.posXSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.posYSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.posZSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.buttonBox1, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(WidgetUI.buttonBox2, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(Object, SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));
    //------------------
    connect(Object->GetProject()->GetHistory(), SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this, SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // init all values ---------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPointObjectDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();
    Objects->InitValues();
    Setup->InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CPointObjectDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_COMPOSITE,Object->GetType().GetName()) == false ) return;

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

void CPointObjectDesigner::ButtonBoxClicked(QAbstractButton *button)
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

void CPointObjectDesigner::InitValues(void)
{
    if( IsItChangingContent() == true ) return;

    WidgetUI.visibleCB->setChecked(Object->IsFlagSet(EPOF_VISIBLE));
    WidgetUI.highlightedCB->setChecked(Object->IsFlagSet(EPOF_SELECTED));
    WidgetUI.showQuotationCB->setChecked(Object->IsFlagSet((EProObjectFlag)ECPOF_SHOW_QUOTATION));
    WidgetUI.showCOGCB->setChecked(Object->IsFlagSet((EProObjectFlag)ECPOF_SHOW_COG));

    CPoint pos = Object->GetPosition();
    WidgetUI.posXSB->setInternalValue(pos.x);
    WidgetUI.posYSB->setInternalValue(pos.y);
    WidgetUI.posZSB->setInternalValue(pos.z);

    bool set = Object->IsAutonomousPoint();
    WidgetUI.posXSB->setEnabled(set);
    WidgetUI.posYSB->setEnabled(set);
    WidgetUI.posZSB->setEnabled(set);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CPointObjectDesigner::ApplyValues(void)
{
    CProObjectFlags flags = Object->GetFlags();
    SET_FLAG(flags,EPOF_VISIBLE,WidgetUI.visibleCB->isChecked());
    SET_FLAG(flags,EPOF_SELECTED,WidgetUI.highlightedCB->isChecked());
    SET_FLAG(flags,(EProObjectFlag)ECPOF_SHOW_QUOTATION,WidgetUI.showQuotationCB->isChecked());
    SET_FLAG(flags,(EProObjectFlag)ECPOF_SHOW_COG,WidgetUI.showCOGCB->isChecked());
    Object->SetFlagsWH(flags);

    if( Object->IsAutonomousPoint() ){
        CPoint pos;
        pos.x = WidgetUI.posXSB->getInternalValue();
        pos.y = WidgetUI.posYSB->getInternalValue();
        pos.z = WidgetUI.posZSB->getInternalValue();
        Object->SetPositionWH(pos);
    }
}

//------------------------------------------------------------------------------

void CPointObjectDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    bool locked = Object->IsHistoryLocked(EHCL_GRAPHICS);

    WidgetUI.flagsGB->setEnabled(!locked);
    WidgetUI.posTab->setEnabled(!locked);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

