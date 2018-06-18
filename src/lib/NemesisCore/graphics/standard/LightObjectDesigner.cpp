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
#include <PhysicalQuantity.hpp>
#include <PhysicalQuantities.hpp>
#include <PointSelection.hpp>

#include <PODesignerGeneral.hpp>
#include <GODesignerSetup.hpp>
#include <GODesignerRefBy.hpp>

#include <GraphicsProfileObject.hpp>
#include <LightObject.hpp>
#include <LightObjectDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* LightObjectDesignerCB(void* p_data);

CExtUUID        LightObjectDesignerID(
                    "{LIGHT_OBJECT_DESIGNER:8a81c1b3-51f1-438a-b95c-aafe6e823cac}",
                    "Light");

CPluginObject   LightObjectDesignerObject(&NemesisCorePlugin,
                    LightObjectDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/Light.svg",
                    LightObjectDesignerCB);

// -----------------------------------------------------------------------------

GODESIGNER_ENTRY_POINT(LightObject);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CLightObjectDesigner::CLightObjectDesigner(CLightObject* p_lo)
    : CProObjectDesigner(&LightObjectDesignerObject,p_lo)
{
    Object = p_lo;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Setup = new CGODesignerSetup(WidgetUI.setupTab,Object,this);
    RefBy = new CGODesignerRefBy(WidgetUI.refByTab,Object,this);

    // set units ---------------------------------
    WidgetUI.positionXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.positionYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.positionZSB->setPhysicalQuantity(PQ_DISTANCE);

    WidgetUI.orientationXSB->setPhysicalQuantity(PQ_ANGLE);
    WidgetUI.orientationYSB->setPhysicalQuantity(PQ_ANGLE);

    // change events -----------------------------
    connect(WidgetUI.visibleCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.primaryObjectCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.highlightedCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showCoverCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.spotEnabledCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.attenuationEnabledCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.globalPositionCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.positionXSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.positionYSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.positionZSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.orientationXSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.orientationYSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
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
    connect(Object->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // init all values ---------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CLightObjectDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();
    Setup->InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CLightObjectDesigner::ApplyAllValues(void)
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

void CLightObjectDesigner::ButtonBoxClicked(QAbstractButton *button)
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

void CLightObjectDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.visibleCB->setChecked(Object->IsFlagSet(EPOF_VISIBLE));
    WidgetUI.highlightedCB->setChecked(Object->IsFlagSet(EPOF_SELECTED));

    WidgetUI.globalPositionCB->setChecked(Object->IsFlagSet<ELightObjectFlag>(ELOF_GLOBAL_POSITION));
    WidgetUI.showCoverCB->setChecked(Object->IsFlagSet<ELightObjectFlag>(ELOF_SHOW_COVER));
    WidgetUI.attenuationEnabledCB->setChecked(Object->IsFlagSet<ELightObjectFlag>(ELOF_ATTENUATION_ENABLED));
    WidgetUI.spotEnabledCB->setChecked(Object->IsFlagSet<ELightObjectFlag>(ELOF_SPOT_ENABLED));

    WidgetUI.positionXSB->setInternalValue(Object->GetPosition().x);
    WidgetUI.positionYSB->setInternalValue(Object->GetPosition().y);
    WidgetUI.positionZSB->setInternalValue(Object->GetPosition().z);

    WidgetUI.orientationXSB->setInternalValue(Object->GetOrientation().x);
    WidgetUI.orientationYSB->setInternalValue(Object->GetOrientation().y);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CLightObjectDesigner::ApplyValues(void)
{
    CProObjectFlags flags = Object->GetFlags();

    SET_FLAG(flags,EPOF_VISIBLE,WidgetUI.visibleCB->isChecked())
    SET_FLAG(flags,EPOF_SELECTED,WidgetUI.highlightedCB->isChecked())

    SET_FLAG(flags,(EProObjectFlag)ELOF_GLOBAL_POSITION,WidgetUI.globalPositionCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)ELOF_SHOW_COVER,WidgetUI.showCoverCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)ELOF_ATTENUATION_ENABLED,WidgetUI.attenuationEnabledCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)ELOF_SPOT_ENABLED,WidgetUI.spotEnabledCB->isChecked())

    Object->SetFlagsWH(flags);

    CPoint position;
    position.x = WidgetUI.positionXSB->getInternalValue();
    position.y = WidgetUI.positionYSB->getInternalValue();
    position.z = WidgetUI.positionZSB->getInternalValue();
    Object->SetPositionWH(position);

    CPoint orientation;
    orientation.x = WidgetUI.orientationXSB->getInternalValue();
    orientation.y = WidgetUI.orientationYSB->getInternalValue();
    orientation.z = 0;
    Object->SetOrientationWH(orientation);
}

//------------------------------------------------------------------------------

void CLightObjectDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    WidgetUI.flagsGB->setEnabled(! Object->IsHistoryLocked(EHCL_GRAPHICS));
    WidgetUI.geometryTab->setEnabled(! Object->IsHistoryLocked(EHCL_GRAPHICS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

