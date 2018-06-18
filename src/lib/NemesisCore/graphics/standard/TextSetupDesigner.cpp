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

#include <TextSetup.hpp>
#include <GlobalSetup.hpp>
#include <TextSetupDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* TextSetupDesignerCB(void* p_data);

CExtUUID        TextSetupDesignerID(
                    "{TEXT_SETUP_DESIGNER:4394f8b3-4dc1-416e-b696-e9af19b3c69c}",
                    "Text");

CPluginObject   TextSetupDesignerObject(&NemesisCorePlugin,
                    TextSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/Text.svg",
                    TextSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* TextSetupDesignerCB(void* p_data)
{
    CTextSetup* p_setup = static_cast<CTextSetup*>(p_data);
    QObject* p_object = new CTextSetupDesigner(p_setup);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTextSetupDesigner::CTextSetupDesigner(CTextSetup* p_setup)
    : CProObjectDesigner(&TextSetupDesignerObject,p_setup)
{
    Setup = p_setup;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    WidgetUI.fontNameCB->setFontPath(QString(GlobalSetup->GetFontPath()));

    // signals -----------------------------------
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

void CTextSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CTextSetupDesigner::ApplyAllValues(void)
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

void CTextSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CTextSetupDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.fontNameCB->setFontName(Setup->LabelFontName);
    WidgetUI.labelFontSizeSB->setValue(Setup->LabelFontSize);
    WidgetUI.labelColorPB->setColor(Setup->LabelColor);
    WidgetUI.depthTestCB->setChecked(Setup->DepthTest);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CTextSetupDesigner::ApplyValues(void)
{
    Setup->LabelFontName = WidgetUI.fontNameCB->getFontName();
    Setup->LabelFontSize = WidgetUI.labelFontSizeSB->value();
    Setup->LabelColor = WidgetUI.labelColorPB->getCColor();
    Setup->DepthTest = WidgetUI.depthTestCB->isChecked();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


