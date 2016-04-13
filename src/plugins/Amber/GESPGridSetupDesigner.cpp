// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2016 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include "AmberModule.hpp"
#include "GESPGridSetup.hpp"
#include "GESPGridSetupDesigner.hpp"
#include "GESPGridSetupDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GESPGridSetupDesignerCB(void* p_data);

CExtUUID        GESPGridSetupDesignerID(
                    "{GESPGRID_SETUP_DESIGNER:d6309161-5715-4260-9a02-db8c956c96bc}",
                    "GESPGrid");

CPluginObject   GESPGridSetupDesignerObject(&AmberPlugin,
                    GESPGridSetupDesignerID,DESIGNER_CAT,
                    ":/images/Amber/GESPGrid.svg",
                    GESPGridSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* GESPGridSetupDesignerCB(void* p_data)
{
    CGESPGridSetup* p_bs = static_cast<CGESPGridSetup*>(p_data);
    QObject* p_object = new CGESPGridSetupDesigner(p_bs);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGESPGridSetupDesigner::CGESPGridSetupDesigner(CGESPGridSetup* p_bs)
    : CProObjectDesigner(&GESPGridSetupDesignerObject,p_bs)
{
    Setup = p_bs;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    // signals -----------------------------------
    connect(WidgetUI.posColorTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.negColorTB, SIGNAL(clicked(bool)),
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

void CGESPGridSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGESPGridSetupDesigner::ApplyAllValues(void)
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

void CGESPGridSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CGESPGridSetupDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.posColorTB->setColor(Setup->PESPColor);
    WidgetUI.negColorTB->setColor(Setup->NESPColor);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGESPGridSetupDesigner::ApplyValues(void)
{
    Setup->PESPColor = WidgetUI.posColorTB->getCColor();
    Setup->NESPColor = WidgetUI.negColorTB->getCColor();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

