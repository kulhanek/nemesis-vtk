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
#include <GSDesignerGeneral.hpp>
#include <GSDesignerRefBy.hpp>
#include <ProjectList.hpp>
#include <NemesisCoreModule.hpp>
#include <FastModelSetup.hpp>
#include <FastModelSetupDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* FastModelSetupDesignerCB(void* p_data);

CExtUUID        FastModelSetupDesignerID(
                    "{FAST_MODEL_SETUP_DESIGNER:059ff399-8493-4bad-838e-2063a6f5f8d5}",
                    "Fast Model");

CPluginObject   FastModelSetupDesignerObject(&NemesisCorePlugin,
                    FastModelSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/FastModel.svg",
                    FastModelSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* FastModelSetupDesignerCB(void* p_data)
{
    CFastModelSetup* p_bo = static_cast<CFastModelSetup*>(p_data);
    QObject* p_object = new CFastModelSetupDesigner(p_bo);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFastModelSetupDesigner::CFastModelSetupDesigner(CFastModelSetup* p_bo)
    : CProObjectDesigner(&FastModelSetupDesignerObject,p_bo)
{
    Setup = p_bo;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    // signals -----------------------------------
    connect(WidgetUI.lineWidthCB, SIGNAL(valueChanged(int)),
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

void CFastModelSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CFastModelSetupDesigner::ApplyAllValues(void)
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

void CFastModelSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CFastModelSetupDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.lineWidthCB->setValue(Setup->LineWidth);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CFastModelSetupDesigner::ApplyValues(void)
{
    Setup->LineWidth = WidgetUI.lineWidthCB->value();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

