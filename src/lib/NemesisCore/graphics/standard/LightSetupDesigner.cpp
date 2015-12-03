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

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <QColorDialog>
#include <ProjectList.hpp>
#include <GSDesignerGeneral.hpp>
#include <GSDesignerRefBy.hpp>
#include <NemesisCoreModule.hpp>
#include <LightSetup.hpp>
#include <LightSetupDesigner.hpp>
#include "LightSetupDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* LightSetupDesignerCB(void* p_data);

CExtUUID        LightSetupDesignerID(
                    "{LIGHT_SETUP_DESIGNER:04b5e85b-c295-48ec-87ca-3c8eefe4bb74}",
                    "Light");

CPluginObject   LightSetupDesignerObject(&NemesisCorePlugin,
                    LightSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/Light.svg",
                    LightSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* LightSetupDesignerCB(void* p_data)
{
    CLightSetup* p_ls = static_cast<CLightSetup*>(p_data);
    QObject* p_object = new CLightSetupDesigner(p_ls);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CLightSetupDesigner::CLightSetupDesigner(CLightSetup* p_ls)
    : CProObjectDesigner(&LightSetupDesignerObject,p_ls)
{
    Setup = p_ls;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    // signals -----------------------------------
    connect(WidgetUI.specularTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.diffuseTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.ambientTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));   
    // -------------
    connect(WidgetUI.spotExponentSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.spotCutoffSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.attenuation1SB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.attenuation2SB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.attenuation3SB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.coverColorTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.coverRadiusSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.coverHeightSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.coverTesselationSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
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

void CLightSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CLightSetupDesigner::ApplyAllValues(void)
{
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

void CLightSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CLightSetupDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    // light
    WidgetUI.diffuseTB->setColor(Setup->Diffuse);
    WidgetUI.specularTB->setColor(Setup->Specular);
    WidgetUI.ambientTB->setColor(Setup->Ambient);
    WidgetUI.spotExponentSB->setValue(Setup->SpotExponent);
    WidgetUI.spotCutoffSB->setValue(Setup->SpotCutoff);
    WidgetUI.attenuation1SB->setValue(Setup->Attenuation[0]);
    WidgetUI.attenuation2SB->setValue(Setup->Attenuation[1]);
    WidgetUI.attenuation3SB->setValue(Setup->Attenuation[2]);

    // cover
    WidgetUI.coverColorTB->setColor(Setup->CoverColor);
    WidgetUI.coverHeightSB->setInternalValue(Setup->CoverHeight);
    WidgetUI.coverRadiusSB->setInternalValue(Setup->CoverRadius);
    WidgetUI.coverTesselationSB->setValue(Setup->CoverTesselation);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CLightSetupDesigner::ApplyValues(void)
{
    // light
    Setup->Diffuse = WidgetUI.diffuseTB->getCColor();
    Setup->Specular = WidgetUI.specularTB->getCColor();
    Setup->Ambient = WidgetUI.ambientTB->getCColor();
    Setup->SpotExponent = WidgetUI.spotExponentSB->value();
    Setup->SpotCutoff = WidgetUI.spotCutoffSB->value();
    Setup->Attenuation[0] = WidgetUI.attenuation1SB->value();
    Setup->Attenuation[1] = WidgetUI.attenuation2SB->value();
    Setup->Attenuation[2] = WidgetUI.attenuation3SB->value();

    // cover
    Setup->CoverColor = WidgetUI.coverColorTB->getCColor();
    Setup->CoverHeight = WidgetUI.coverHeightSB->getInternalValue();
    Setup->CoverRadius = WidgetUI.coverRadiusSB->getInternalValue();
    Setup->CoverTesselation = WidgetUI.coverTesselationSB->value();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
