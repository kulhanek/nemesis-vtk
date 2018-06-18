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

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <ProjectList.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>
#include <GSDesignerGeneral.hpp>
#include <GSDesignerRefBy.hpp>
#include <QColorDialog>
#include <NemesisCoreModule.hpp>
#include <PBCBoxSetup.hpp>
#include <PBCBoxSetupDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PBCBoxSetupDesignerCB(void* p_data);

CExtUUID        PBCBoxSetupDesignerID(
                    "{PBC_BOX_SETUP_DESIGNER:bf876246-908b-4fdc-84cf-2c723c3103a6}",
                    "PBC Box");

CPluginObject   PBCBoxSetupDesignerObject(&NemesisCorePlugin,
                    PBCBoxSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/PBCBox.svg",
                    PBCBoxSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* PBCBoxSetupDesignerCB(void* p_data)
{
    CPBCBoxSetup* p_setup = static_cast<CPBCBoxSetup*>(p_data);
    QObject* p_object = new CPBCBoxSetupDesigner(p_setup);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPBCBoxSetupDesigner::CPBCBoxSetupDesigner(CPBCBoxSetup* p_setup)
    : CProObjectDesigner(&PBCBoxSetupDesignerObject,p_setup)
{
    Setup = p_setup;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    // units -------------------------------------
    WidgetUI.lineWidthSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.pointSizeSB->setPhysicalQuantity(PQ_DISTANCE);

    // signals -----------------------------------
    connect(WidgetUI.lineColorPB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.lineWidthSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.lineStipplePatternCB, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.lineStippleFactorSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.showTubesCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.pointColorPB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.pointWidthSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.pointSizeSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));
    // -------------
    connect(Setup, SIGNAL(OnSetupChanged()),
            this, SLOT(InitValues()));

    // populate designer with data
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPBCBoxSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CPBCBoxSetupDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    // apply all changes
    Changing = true;
        General->ApplyValues();
        ApplyValues();
        RefBy->ApplyValues();
        // emit signal about the change - it must be here
        Setup->EmitOnSetupChanged();
    Changing = false;

    // some changes might be prohibited
    InitAllValues();

    // repaint all projects
    Projects->RepaintAllProjects();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPBCBoxSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
{
    if(WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Reset) {
        InitAllValues();
    }

    if(WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Apply) {
        ApplyAllValues();
    }

    if(WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Close) {
        close();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPBCBoxSetupDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.lineColorPB->setColor(Setup->LineColor);
    WidgetUI.lineWidthSB->setValue(Setup->LineWidth);
    WidgetUI.lineStipplePatternCB->setStipplePattern(Setup->LineStipplePattern);
    WidgetUI.lineStippleFactorSB->setValue(Setup->LineStippleFactor );
    WidgetUI.showTubesCB->setChecked(Setup->UseTubes);

    WidgetUI.pointColorPB->setColor(Setup->PointColor);
    WidgetUI.pointSizeSB->setInternalValue(Setup->PointSize);
    WidgetUI.pointWidthSB->setValue(Setup->PointWidth);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CPBCBoxSetupDesigner::ApplyValues(void)
{
    Setup->LineColor = WidgetUI.lineColorPB->getCColor();
    Setup->LineWidth = WidgetUI.lineWidthSB->value();
    Setup->LineStipplePattern = WidgetUI.lineStipplePatternCB->getStipplePattern();
    Setup->LineStippleFactor = WidgetUI.lineStippleFactorSB->value();
    Setup->UseTubes = WidgetUI.showTubesCB->isChecked();

    Setup->PointColor = WidgetUI.pointColorPB->getCColor();
    Setup->PointWidth = WidgetUI.pointWidthSB->value();
    Setup->PointSize = WidgetUI.pointSizeSB->getInternalValue();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


