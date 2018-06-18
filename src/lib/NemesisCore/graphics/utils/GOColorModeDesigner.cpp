// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>

#include <PODesignerGeneral.hpp>
#include <GODesignerObjects.hpp>
#include <GODesignerSetup.hpp>
#include <GODesignerRefBy.hpp>

#include <GOColorMode.hpp>
#include <GOColorModeDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GOColorModeDesignerCB(void* p_data);

CExtUUID        GOColorModeDesignerID(
                    "{GO_COLOR_MODE_DESIGNER:a307367c-eaba-4747-a141-82cdeff89f8d}",
                    "Model Color Mode");

CPluginObject   GOColorModeDesignerObject(&NemesisCorePlugin,
                    GOColorModeDesignerID,DESIGNER_CAT,
                    GOColorModeDesignerCB);

// -----------------------------------------------------------------------------

QObject* GOColorModeDesignerCB(void* p_data)
{
    CGOColorMode* p_fmo = static_cast<CGOColorMode*>(p_data);
    QObject* p_object = new CGOColorModeDesigner(p_fmo);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGOColorModeDesigner::CGOColorModeDesigner(CGOColorMode* p_fmo)
    : CProObjectDesigner(&GOColorModeDesignerObject, p_fmo)
{
    Object = p_fmo;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);

    //------------------
    connect(WidgetUI.modesCB,SIGNAL(currentIndexChanged(int)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.TBEleColor,SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.TBEleAmbient,SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.TBEleDiffuse,SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.TBEleEmission,SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.TBEleSpecular,SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.SBEleShininess,SIGNAL(valueChanged(int)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(Object, SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));

    // tabs ------------
    if( ! Object->IsColorModeEnabled(EGOCM_USER_COLOR) ){
        WidgetUI.userColorTab->hide();
    }

    // combo boxes -----
    WidgetUI.modesCB->clear();
    for(int i = 0; i < Object->GetNumberOfColorModes(); i++){
        WidgetUI.modesCB->addItem(Object->GetColorModeName(i));
    }

    // init all values ---------------------------
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGOColorModeDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGOColorModeDesigner::ApplyAllValues(void)
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

void CGOColorModeDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Reset ) {
        InitAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Apply ) {
        ApplyAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Close ) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGOColorModeDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    Changing = true;
        WidgetUI.modesCB->setCurrentIndex(Object->GetColorMode());
    Changing = false;

    // user color
    CElementColors color = Object->GetUserColor();
    WidgetUI.TBEleColor->setColor(color.Color);
    WidgetUI.TBEleAmbient->setColor(color.Ambient);
    WidgetUI.TBEleDiffuse->setColor(color.Diffuse);
    WidgetUI.TBEleEmission->setColor(color.Emission);
    WidgetUI.TBEleSpecular->setColor(color.Specular);
    WidgetUI.SBEleShininess->setValue(color.Shininess);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGOColorModeDesigner::ApplyValues(void)
{
    Object->SetColorModeWH(WidgetUI.modesCB->currentIndex());

    // user color
    CElementColors elem_color;

    elem_color.Color = WidgetUI.TBEleColor->getCColor();
    elem_color.Ambient = WidgetUI.TBEleAmbient->getCColor();
    elem_color.Diffuse = WidgetUI.TBEleDiffuse->getCColor();
    elem_color.Emission = WidgetUI.TBEleEmission->getCColor();
    elem_color.Specular = WidgetUI.TBEleSpecular->getCColor();

    elem_color.Shininess = WidgetUI.SBEleShininess->value();

    Object->SetUserColorWH(elem_color);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

