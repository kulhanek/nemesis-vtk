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
#include <XYZAxesSetup.hpp>
#include <XYZAxesSetupDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* XYZAxesSetupDesignerCB(void* p_data);

CExtUUID        XYZAxesSetupDesignerID(
                    "{XYZ_AXES_SETUP_DESIGNER:98a577b6-9d75-4814-9605-d9ac8f912b96}",
                    "XYZ Axes");

CPluginObject   XYZAxesSetupDesignerObject(&NemesisCorePlugin,
                    XYZAxesSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/XYZAxes.svg",
                    XYZAxesSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* XYZAxesSetupDesignerCB(void* p_data)
{
    CXYZAxesSetup* p_xyza = static_cast<CXYZAxesSetup*>(p_data);
    QObject* p_object = new CXYZAxesSetupDesigner(p_xyza);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CXYZAxesSetupDesigner::CXYZAxesSetupDesigner(CXYZAxesSetup* p_bo)
    : CProObjectDesigner(&XYZAxesSetupDesignerObject,p_bo)
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
    WidgetUI.magNXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.magNYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.magNZSB->setPhysicalQuantity(PQ_DISTANCE);

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
    connect(WidgetUI.nxcolorTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.nycolorTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.nzcolorTB, SIGNAL(clicked(bool)),
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
    connect(WidgetUI.magNXSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.magNYSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.magNZSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.showXAxisCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.showYAxisCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.showZAxisCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.showNXAxisCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.showNYAxisCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.showNZAxisCB, SIGNAL(clicked(bool)),
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

void CXYZAxesSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CXYZAxesSetupDesigner::ApplyAllValues(void)
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

void CXYZAxesSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CXYZAxesSetupDesigner::InitValues(void)
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

    WidgetUI.nxcolorTB->setColor(Setup->NXColor);
    WidgetUI.nycolorTB->setColor(Setup->NYColor);
    WidgetUI.nzcolorTB->setColor(Setup->NZColor);

    WidgetUI.magXSB->setInternalValue(Setup->MagnificationX);
    WidgetUI.magYSB->setInternalValue(Setup->MagnificationY);
    WidgetUI.magZSB->setInternalValue(Setup->MagnificationZ);

    WidgetUI.magNXSB->setInternalValue(Setup->MagnificationMX);
    WidgetUI.magNYSB->setInternalValue(Setup->MagnificationMY);
    WidgetUI.magNZSB->setInternalValue(Setup->MagnificationMZ);

    WidgetUI.showXAxisCB->setChecked(Setup->ShowX);
    WidgetUI.showYAxisCB->setChecked(Setup->ShowY);
    WidgetUI.showZAxisCB->setChecked(Setup->ShowZ);

    WidgetUI.showNXAxisCB->setChecked(Setup->ShowMX);
    WidgetUI.showNYAxisCB->setChecked(Setup->ShowMY);
    WidgetUI.showNZAxisCB->setChecked(Setup->ShowMZ);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CXYZAxesSetupDesigner::ApplyValues(void)
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

    Setup->NXColor = WidgetUI.nxcolorTB->getCColor();
    Setup->NYColor = WidgetUI.nycolorTB->getCColor();
    Setup->NZColor = WidgetUI.nzcolorTB->getCColor();

    Setup->MagnificationX = WidgetUI.magXSB->getInternalValue();
    Setup->MagnificationY = WidgetUI.magYSB->getInternalValue();
    Setup->MagnificationZ = WidgetUI.magZSB->getInternalValue();

    Setup->MagnificationMX = WidgetUI.magNXSB->getInternalValue();
    Setup->MagnificationMY = WidgetUI.magNYSB->getInternalValue();
    Setup->MagnificationMZ = WidgetUI.magNZSB->getInternalValue();

    Setup->ShowX = WidgetUI.showXAxisCB->isChecked();
    Setup->ShowY = WidgetUI.showYAxisCB->isChecked();
    Setup->ShowZ = WidgetUI.showZAxisCB->isChecked();

    Setup->ShowMX = WidgetUI.showNXAxisCB->isChecked();
    Setup->ShowMY = WidgetUI.showNYAxisCB->isChecked();
    Setup->ShowMZ = WidgetUI.showNZAxisCB->isChecked();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
