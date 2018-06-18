// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <QColorDialog>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>

#include <GSDesignerGeneral.hpp>
#include <GSDesignerRefBy.hpp>

#include <ProjectList.hpp>
#include <SpecAxesSetup.hpp>
#include <SpecAxesSetupDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* SpecAxesSetupDesignerCB(void* p_data);

CExtUUID        SpecAxesSetupDesignerID(
                    "{SPEC_AXES_SETUP_DESIGNER:f2c5a2ae-1514-439d-bcba-9c85344462d9}",
                    "Special Axes");

CPluginObject   SpecAxesSetupDesignerObject(&NemesisCorePlugin,
                    SpecAxesSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/SpecAxes.svg",
                    SpecAxesSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* SpecAxesSetupDesignerCB(void* p_data)
{
    CSpecAxesSetup* p_xyza = static_cast<CSpecAxesSetup*>(p_data);
    QObject* p_object = new CSpecAxesSetupDesigner(p_xyza);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSpecAxesSetupDesigner::CSpecAxesSetupDesigner(CSpecAxesSetup* p_bo)
    : CProObjectDesigner(&SpecAxesSetupDesignerObject,p_bo)
{
    Setup = p_bo;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    // units -------------------------------------
    WidgetUI.axisradiusSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.coneradiusSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.coneheightSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.magXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.magYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.magZSB->setPhysicalQuantity(PQ_DISTANCE);

    // signals -----------------------------------
    connect(WidgetUI.axisradiusSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.cylindertesselationSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.coneradiusSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.coneheightSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.conetesselationSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.conexcolorTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.coneycolorTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.conezcolorTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.pxcolorTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.pycolorTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.pzcolorTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.magXSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.magYSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.magZSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.buttonBox1, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));
    // -------------
    connect(WidgetUI.buttonBox2, SIGNAL(clicked(QAbstractButton*)),
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

void CSpecAxesSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CSpecAxesSetupDesigner::ApplyAllValues(void)
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

void CSpecAxesSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
{
    if(WidgetUI.buttonBox1->standardButton(p_button) == QDialogButtonBox::Reset) {
        InitAllValues();
    }

    if(WidgetUI.buttonBox1->standardButton(p_button) == QDialogButtonBox::Apply) {
        ApplyAllValues();
    }

    if(WidgetUI.buttonBox1->standardButton(p_button) == QDialogButtonBox::Close) {
        close();
    }

    if(WidgetUI.buttonBox2->standardButton(p_button) == QDialogButtonBox::Reset) {
        InitAllValues();
    }

    if(WidgetUI.buttonBox2->standardButton(p_button) == QDialogButtonBox::Apply) {
        ApplyAllValues();
    }

    if(WidgetUI.buttonBox2->standardButton(p_button) == QDialogButtonBox::Close) {
        close();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSpecAxesSetupDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.axisradiusSB->setValue(Setup->AxisRadius);
    WidgetUI.cylindertesselationSB->setValue(Setup->CylinderTesselation);
    WidgetUI.coneradiusSB->setValue(Setup->ConeRadius);
    WidgetUI.coneheightSB->setValue(Setup->ConeHeight);
    WidgetUI.conetesselationSB->setValue(Setup->ConeTesselation);

    WidgetUI.conexcolorTB->setColor(Setup->ConeXColor);
    WidgetUI.coneycolorTB->setColor(Setup->ConeYColor);
    WidgetUI.conezcolorTB->setColor(Setup->ConeZColor);

    WidgetUI.pxcolorTB->setColor(Setup->PXColor);
    WidgetUI.pycolorTB->setColor(Setup->PYColor);
    WidgetUI.pzcolorTB->setColor(Setup->PZColor);

    WidgetUI.magXSB->setInternalValue(Setup->MagnificationX);
    WidgetUI.magYSB->setInternalValue(Setup->MagnificationY);
    WidgetUI.magZSB->setInternalValue(Setup->MagnificationZ);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CSpecAxesSetupDesigner::ApplyValues(void)
{
    Setup->AxisRadius = WidgetUI.axisradiusSB->value();
    Setup->CylinderTesselation = WidgetUI.cylindertesselationSB->value();
    Setup->ConeRadius = WidgetUI.coneradiusSB->value();
    Setup->ConeHeight = WidgetUI.coneheightSB->value();
    Setup->ConeTesselation = WidgetUI.conetesselationSB->value();

    Setup->ConeXColor = WidgetUI.conexcolorTB->getCColor();
    Setup->ConeYColor = WidgetUI.coneycolorTB->getCColor();
    Setup->ConeZColor = WidgetUI.conezcolorTB->getCColor();

    Setup->PXColor = WidgetUI.pxcolorTB->getCColor();
    Setup->PYColor = WidgetUI.pycolorTB->getCColor();
    Setup->PZColor = WidgetUI.pzcolorTB->getCColor();

    Setup->MagnificationX = WidgetUI.magXSB->getInternalValue();
    Setup->MagnificationY = WidgetUI.magYSB->getInternalValue();
    Setup->MagnificationZ = WidgetUI.magZSB->getInternalValue();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
