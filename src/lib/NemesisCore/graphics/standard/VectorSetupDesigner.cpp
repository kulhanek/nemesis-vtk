// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Jiri Klusak, anim4tor@mail.muni.cz
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
#include <VectorSetup.hpp>
#include <VectorSetupDesigner.hpp>

#include "VectorSetupDesigner.moc"


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* VectorSetupDesignerCB(void* p_data);

CExtUUID        VectorSetupDesignerID(
                    "{VECTOR_SETUP_DESIGNER:5d6459ba-bcf2-4c0e-a071-b374183f28e6}",
                    "Vector");

CPluginObject   VectorSetupDesignerObject(&NemesisCorePlugin,
                    VectorSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/Vector.svg",
                    VectorSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* VectorSetupDesignerCB(void* p_data)
{
    CVectorSetup* p_setup = static_cast<CVectorSetup*>(p_data);
    QObject* p_object = new CVectorSetupDesigner(p_setup);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CVectorSetupDesigner::CVectorSetupDesigner(CVectorSetup* p_setup)
    : CProObjectDesigner(&VectorSetupDesignerObject,p_setup)
{
    Setup = p_setup;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    // units -------------------------------------
    WidgetUI.axisRadiusSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.coneRadiusSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.coneHeightSB->setPhysicalQuantity(PQ_DISTANCE);

    // signals -----------------------------------
    connect(WidgetUI.axisQualitySB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.axisRadiusSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.axisDiffuseCB, SIGNAL(toggled(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.coneQualitySB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.coneRadiusSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.coneHeightSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.axisColorTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.coneColorTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));
    // -------------
    connect(PQ_DISTANCE,SIGNAL(OnUnitChanged()),
            this,SLOT(InitValues()));

    // populate designer with data
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CVectorSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CVectorSetupDesigner::ApplyAllValues(void)
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

void CVectorSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CVectorSetupDesigner::InitValues(void)
{
    WidgetUI.axisRadiusSB->setInternalValue(Setup->AxisRadius);
    WidgetUI.axisQualitySB->setValue(Setup->AxisTesselation);
    WidgetUI.axisColorTB->setColor(Setup->AxisColor);

    WidgetUI.axisDiffuseCB->setChecked(Setup->AxisDifuseColor);

    WidgetUI.coneRadiusSB->setInternalValue(Setup->ConeRadius);
    WidgetUI.coneHeightSB->setInternalValue(Setup->ConeHeight);
    WidgetUI.coneQualitySB->setValue(Setup->ConeTesselation);
    WidgetUI.coneColorTB->setColor(Setup->ConeColor);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CVectorSetupDesigner::ApplyValues(void)
{
    Setup->AxisRadius = WidgetUI.axisRadiusSB->getInternalValue();
    Setup->AxisTesselation = WidgetUI.axisQualitySB->value();
    Setup->AxisColor = WidgetUI.axisColorTB->getCColor();

    Setup->AxisDifuseColor = WidgetUI.axisDiffuseCB->isChecked();

    Setup->ConeRadius = WidgetUI.coneRadiusSB->getInternalValue();
    Setup->ConeHeight = WidgetUI.coneHeightSB->getInternalValue();
    Setup->ConeTesselation = WidgetUI.coneQualitySB->value();
    Setup->ConeColor = WidgetUI.coneColorTB->getCColor();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


