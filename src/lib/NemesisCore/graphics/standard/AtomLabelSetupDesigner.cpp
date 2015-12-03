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
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <ProjectList.hpp>

#include <GSDesignerGeneral.hpp>
#include <GSDesignerRefBy.hpp>

#include <AtomLabelSetup.hpp>
#include <GlobalSetup.hpp>
#include <AtomLabelSetupDesigner.hpp>
#include "AtomLabelSetupDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* AtomLabelSetupDesignerCB(void* p_data);

CExtUUID        AtomLabelSetupDesignerID(
                    "{ATOM_LABEL_SETUP_DESIGNER:6e9351ea-3ace-4719-9af4-c6795e12d1da}",
                    "Atom Label");

CPluginObject   AtomLabelSetupDesignerObject(&NemesisCorePlugin,
                    AtomLabelSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/AtomLabel.svg",
                    AtomLabelSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* AtomLabelSetupDesignerCB(void* p_data)
{
    CAtomLabelSetup* p_setup = static_cast<CAtomLabelSetup*>(p_data);
    QObject* p_object = new CAtomLabelSetupDesigner(p_setup);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomLabelSetupDesigner::CAtomLabelSetupDesigner(CAtomLabelSetup* p_setup)
    : CProObjectDesigner(&AtomLabelSetupDesignerObject,p_setup)
{
    Setup = p_setup;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    WidgetUI.fontNameCB->setFontPath(QString(GlobalSetup->GetFontPath()));

    // signals -----------------------------------
    connect(WidgetUI.lineWidthSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.labelFontSizeSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.depthTestCB, SIGNAL(toggled(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.fontNameCB, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.labelColorPB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.lineColorPB, SIGNAL(clicked(bool)),
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

void CAtomLabelSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CAtomLabelSetupDesigner::ApplyAllValues(void)
{
    // apply all changes
    Changing = true;
        General->ApplyValues();
        ApplyValues();
        RefBy->ApplyValues();
        // update fontGL
        Setup->UpdateFTGLFont();
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

void CAtomLabelSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CAtomLabelSetupDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.fontNameCB->setFontName(Setup->LabelFontName);
    WidgetUI.labelFontSizeSB->setValue(Setup->LabelFontSize);
    WidgetUI.labelColorPB->setColor(Setup->LabelColor);
    WidgetUI.lineColorPB->setColor(Setup->LineColor);
    WidgetUI.lineWidthSB->setValue(Setup->LineWidth);
    WidgetUI.depthTestCB->setChecked(Setup->DepthTest);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CAtomLabelSetupDesigner::ApplyValues(void)
{
    Setup->LabelFontName = WidgetUI.fontNameCB->getFontName();
    Setup->LabelFontSize = WidgetUI.labelFontSizeSB->value();
    Setup->LabelColor = WidgetUI.labelColorPB->getCColor();
    Setup->LineColor = WidgetUI.lineColorPB->getCColor();
    Setup->LineWidth = WidgetUI.lineWidthSB->value();
    Setup->DepthTest = WidgetUI.depthTestCB->isChecked();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


