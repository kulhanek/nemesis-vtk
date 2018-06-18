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
#include <Project.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <ProjectList.hpp>
#include <GSDesignerGeneral.hpp>
#include <GSDesignerRefBy.hpp>
#include <QColorDialog>

#include <NemesisCoreModule.hpp>
#include <BackgroundSetup.hpp>
#include <BackgroundSetupDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* BackgroundSetupDesignerCB(void* p_data);

CExtUUID        BackgroundSetupDesignerID(
                    "{BACKGROUND_SETUP_DESIGNER:c5dbb780-166d-4b72-a66b-58a2ad1fa223}",
                    "Background");

CPluginObject   BackgroundSetupDesignerObject(&NemesisCorePlugin,
                    BackgroundSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/Background.svg",
                    BackgroundSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* BackgroundSetupDesignerCB(void* p_data)
{
    CBackgroundSetup* p_bs = static_cast<CBackgroundSetup*>(p_data);
    QObject* p_object = new CBackgroundSetupDesigner(p_bs);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBackgroundSetupDesigner::CBackgroundSetupDesigner(CBackgroundSetup* p_bs)
    : CProObjectDesigner(&BackgroundSetupDesignerObject,p_bs)
{
    Setup = p_bs;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    // signals -----------------------------------
    connect(WidgetUI.colorTB, SIGNAL(clicked(bool)),
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

void CBackgroundSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CBackgroundSetupDesigner::ApplyAllValues(void)
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

void CBackgroundSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CBackgroundSetupDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.colorTB->setColor(Setup->Color);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CBackgroundSetupDesigner::ApplyValues(void)
{
    Setup->Color = WidgetUI.colorTB->getCColor();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

