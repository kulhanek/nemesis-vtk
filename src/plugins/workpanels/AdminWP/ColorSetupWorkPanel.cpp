// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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

#include "ColorSetupWorkPanel.hpp"

#include "AdminWPModule.hpp"

#include <ElementColorsList.hpp>
#include <ElementColors.hpp>
#include <ErrorSystem.hpp>
#include <CategoryUUID.hpp>
#include <QMessageBox>

#include <ProjectList.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* ColorSetupWorkPanelCB(void* p_data);

CExtUUID        ColorSetupWorkPanelID(
                    "{COLOR_SETUP_WORK_PANEL:17c59331-de64-40a1-8854-2b6298cb5a9e}",
                    "Color setup",
                    "Panel with setup of colors for model and elements.");

CPluginObject   ColorSetupWorkPanelObject(&AdminWPPlugin,
                    ColorSetupWorkPanelID,WORK_PANEL_CAT,
                    ":images/AdminWP/ColorPreferences.svg",
                    ColorSetupWorkPanelCB);

CColorSetupWorkPanel* p_s6_window = NULL;

// -----------------------------------------------------------------------------

QObject* ColorSetupWorkPanelCB(void* p_data)
{
    if (p_s6_window != NULL) return(p_s6_window);
    p_s6_window = new CColorSetupWorkPanel;
    return(p_s6_window);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CColorSetupWorkPanel::CColorSetupWorkPanel()
    : CWorkPanel(&ColorSetupWorkPanelObject,NULL,EWPR_WINDOW)
{
    WidgetUI.setupUi(this);

    PeriodicTableWidget = new CPeriodicTableWidget(this);
    connect(PeriodicTableWidget,SIGNAL(OnElementChanged(int)),
            this,SLOT(SetNewElement(int)));

    WidgetUI.verticalLayout_1->insertWidget(0,PeriodicTableWidget);

    QButtonGroup* p_bg = new QButtonGroup(this);

    p_bg->addButton(WidgetUI.TBGlbAmbient);
    p_bg->addButton(WidgetUI.TBGlbDiffuse);
    p_bg->addButton(WidgetUI.TBGlbEmission);
    p_bg->addButton(WidgetUI.TBGlbSpecular);

    p_bg->addButton(WidgetUI.TBEleColor);
    p_bg->addButton(WidgetUI.TBEleAmbient);
    p_bg->addButton(WidgetUI.TBEleDiffuse);
    p_bg->addButton(WidgetUI.TBEleEmission);
    p_bg->addButton(WidgetUI.TBEleSpecular);

    p_bg->addButton(WidgetUI.TBSelColor);
    p_bg->addButton(WidgetUI.TBSelAmbient);
    p_bg->addButton(WidgetUI.TBSelDiffuse);
    p_bg->addButton(WidgetUI.TBSelEmission);
    p_bg->addButton(WidgetUI.TBSelSpecular);

    WidgetUI.CBType->addItem("Ambient");
    WidgetUI.CBType->addItem("Diffuse");
    WidgetUI.CBType->addItem("Specular");
    WidgetUI.CBType->addItem("Ambient and diffuse");

    connect(p_bg,SIGNAL(buttonClicked(QAbstractButton*)),
            this,SLOT(SetChangedFlagTrue(void)));

    connect(WidgetUI.applyBB_1,SIGNAL(clicked (QAbstractButton*)),
            this,SLOT(ButtonBoxClicked(QAbstractButton*)));
    connect(WidgetUI.applyBB_2,SIGNAL(clicked (QAbstractButton*)),
            this,SLOT(ButtonBoxClicked(QAbstractButton*)));
    connect(WidgetUI.mainBB,SIGNAL(clicked (QAbstractButton*)),
            this,SLOT(ButtonBoxClicked(QAbstractButton*)));

    // connect actions to SetChangedFlag
    connect(WidgetUI.SBEleShininess,SIGNAL(valueChanged(int)),
            this,SLOT(SetChangedFlagTrue(void)));
    connect(WidgetUI.SBGlbShininess,SIGNAL(valueChanged(int)),
            this,SLOT(SetChangedFlagTrue(void)));
    connect(WidgetUI.SBSelShininess,SIGNAL(valueChanged(int)),
            this,SLOT(SetChangedFlagTrue(void)));
    connect(WidgetUI.CBType,SIGNAL(currentIndexChanged(int)),
            this,SLOT(SetChangedFlagTrue(void)));

    // set initial colors from ElementColorList
    ReInitValues();

    // load work panel setup
    LoadWorkPanelSetup();
}

// -----------------------------------------------------------------------------

CColorSetupWorkPanel::~CColorSetupWorkPanel(void)
{
    SaveWorkPanelSetup();
    p_s6_window = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CColorSetupWorkPanel::UseValues(void)
{
    // global material
    ColorsList.GlobalMaterial.Ambient = WidgetUI.TBGlbAmbient->getCColor();
    ColorsList.GlobalMaterial.Diffuse = WidgetUI.TBGlbDiffuse->getCColor();
    ColorsList.GlobalMaterial.Emission = WidgetUI.TBGlbEmission->getCColor();
    ColorsList.GlobalMaterial.Specular = WidgetUI.TBGlbSpecular->getCColor();
    ColorsList.GlobalMaterial.Shininess = WidgetUI.SBGlbShininess->value();

    // selection material
    ColorsList.SelectionMaterial.Color = WidgetUI.TBSelColor->getCColor();
    ColorsList.SelectionMaterial.Ambient = WidgetUI.TBSelAmbient->getCColor();
    ColorsList.SelectionMaterial.Diffuse = WidgetUI.TBSelDiffuse->getCColor();
    ColorsList.SelectionMaterial.Emission = WidgetUI.TBSelEmission->getCColor();
    ColorsList.SelectionMaterial.Specular = WidgetUI.TBSelSpecular->getCColor();
    ColorsList.SelectionMaterial.Shininess = WidgetUI.SBSelShininess->value();

    switch(WidgetUI.CBType->currentIndex()) {
    case 0:
        ColorsList.MaterialMode = GL_AMBIENT;
        break;
    case 1:
        ColorsList.MaterialMode = GL_DIFFUSE;
        break;
    case 2:
        ColorsList.MaterialMode = GL_SPECULAR;
        break;
    case 3:
        ColorsList.MaterialMode = GL_AMBIENT_AND_DIFFUSE;
        break;
    default:
        ColorsList.MaterialMode = GL_AMBIENT;
        break;
    }

    CElementColors elem_color;

    // element color
    elem_color.Color = WidgetUI.TBEleColor->getCColor();
    elem_color.Ambient = WidgetUI.TBEleAmbient->getCColor();
    elem_color.Diffuse = WidgetUI.TBEleDiffuse->getCColor();
    elem_color.Emission = WidgetUI.TBEleEmission->getCColor();
    elem_color.Specular = WidgetUI.TBEleSpecular->getCColor();

    elem_color.Shininess = WidgetUI.SBEleShininess->value();

    ColorsList.SetElementColors(ActiveZ,elem_color);

    PeriodicTableWidget->UpdateColors();

    SetChangedFlag(false);
    return;
}

// -----------------------------------------------------------------------------

void CColorSetupWorkPanel::ReInitValues(void)
{
    WidgetUI.TBGlbAmbient->setColor(ColorsList.GlobalMaterial.Ambient);
    WidgetUI.TBGlbDiffuse->setColor(ColorsList.GlobalMaterial.Diffuse);
    WidgetUI.TBGlbEmission->setColor(ColorsList.GlobalMaterial.Emission);
    WidgetUI.TBGlbSpecular->setColor(ColorsList.GlobalMaterial.Specular);
    WidgetUI.SBGlbShininess->setValue(ColorsList.GlobalMaterial.Shininess);

    // type
    switch(ColorsList.MaterialMode) {
    case GL_AMBIENT:
        WidgetUI.CBType->setCurrentIndex(0);
        break;
    case GL_DIFFUSE:
        WidgetUI.CBType->setCurrentIndex(1);
        break;
    case GL_SPECULAR:
        WidgetUI.CBType->setCurrentIndex(2);
        break;
    case GL_AMBIENT_AND_DIFFUSE:
        WidgetUI.CBType->setCurrentIndex(3);
        break;
    default:
        WidgetUI.CBType->setCurrentIndex(0);
        break;
    }

    WidgetUI.TBSelColor->setColor(ColorsList.SelectionMaterial.Color);
    WidgetUI.TBSelAmbient->setColor(ColorsList.SelectionMaterial.Ambient);
    WidgetUI.TBSelDiffuse->setColor(ColorsList.SelectionMaterial.Diffuse);
    WidgetUI.TBSelEmission->setColor(ColorsList.SelectionMaterial.Emission);
    WidgetUI.TBSelSpecular->setColor(ColorsList.SelectionMaterial.Specular);

    WidgetUI.SBSelShininess->setValue(ColorsList.SelectionMaterial.Shininess);

    ReInitElementColors();
}

// -----------------------------------------------------------------------------

void CColorSetupWorkPanel::ReInitElementColors(void)
{
    ActiveZ = PeriodicTableWidget->GetActiveElement();

    QString text("Element: <b><sub>");
    text += QString::number(ActiveZ) + "</sub>";
    text += QString(PeriodicTable.GetSymbol(ActiveZ)) + " - ";
    text += QString(PeriodicTable.GetName(ActiveZ)) + "</b>";
    WidgetUI.LElement->setText(text);

    WidgetUI.TBEleColor->setColor(ColorsList.GetElementColors(ActiveZ).Color);
    WidgetUI.TBEleAmbient->setColor(ColorsList.GetElementColors(ActiveZ).Ambient);
    WidgetUI.TBEleDiffuse->setColor(ColorsList.GetElementColors(ActiveZ).Diffuse);
    WidgetUI.TBEleEmission->setColor(ColorsList.GetElementColors(ActiveZ).Emission);
    WidgetUI.TBEleSpecular->setColor(ColorsList.GetElementColors(ActiveZ).Specular);

    WidgetUI.SBEleShininess->setValue(ColorsList.GetElementColors(ActiveZ).Shininess);

    SetChangedFlag(false);
}

// -----------------------------------------------------------------------------

void CColorSetupWorkPanel::SetNewElement(int z)
{
    if( IsChangedFlagSet() == true ) {
        QMessageBox::StandardButton question = QMessageBox::warning(this,
                                               tr("Warning"), tr("Data has been changed!"),
                                               QMessageBox::Ignore | QMessageBox::Save | QMessageBox::Cancel);

        if( question == QMessageBox::Cancel ) {
            PeriodicTableWidget->ChangeElement(ActiveZ);
            return;
        }
        if( question == QMessageBox::Save ) {
            UseValues();
        }
    }

    ReInitElementColors();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CColorSetupWorkPanel::ButtonBoxClicked(QAbstractButton* p_button)
{
    if( p_button == NULL ) {
        INVALID_ARGUMENT("button == NULL");
    }

    // reset changes
    if( (WidgetUI.applyBB_1->standardButton(p_button) == QDialogButtonBox::Reset) ||
            (WidgetUI.applyBB_2->standardButton(p_button) == QDialogButtonBox::Reset) ) {
        ReInitValues();
        return;
    }

    // apply changes
    if( (WidgetUI.applyBB_1->standardButton(p_button) == QDialogButtonBox::Apply) ||
            (WidgetUI.applyBB_2->standardButton(p_button) == QDialogButtonBox::Apply) ) {
        UseValues();
        Projects->RepaintAllProjects();
        return;
    }

    // restore system dafault colors
    if( WidgetUI.mainBB->standardButton(p_button) == QDialogButtonBox::RestoreDefaults ) {
        ColorsList.LoadSystemColors();
        ReInitValues();
        PeriodicTableWidget->UpdateColors();
        Projects->RepaintAllProjects();
        return;
    }

    // apply changes and save them
    if(WidgetUI.mainBB->standardButton(p_button) == QDialogButtonBox::SaveAll) {
        UseValues();
        ColorsList.SaveUserColors();
        Projects->RepaintAllProjects();
        return;
    }

    // close workpanel
    if(WidgetUI.mainBB->standardButton(p_button) == QDialogButtonBox::Close) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


