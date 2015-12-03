// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include "RMSDToolsModule.hpp"
#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <ProjectList.hpp>
#include <GlobalSetup.hpp>

#include <GSDesignerGeneral.hpp>
#include <GSDesignerRefBy.hpp>

#include "RMSDPropertySetup.hpp"
#include "RMSDPropertySetupDesigner.hpp"
#include "RMSDPropertySetupDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* RMSDPropertySetupDesignerCB(void* p_data);

CExtUUID        RMSDPropertySetupDesignerID(
                    "{RMSD_PROPERTY_SETUP_DESIGNER:6d9a554d-f9db-4933-bb44-7b2a2a6d5e70}",
                    "RMSD");

CPluginObject   RMSDPropertySetupDesignerObject(&RMSDToolsPlugin,
                    RMSDPropertySetupDesignerID,DESIGNER_CAT,
                    ":/images/RMSDTools/RMSD.svg",
                    RMSDPropertySetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* RMSDPropertySetupDesignerCB(void* p_data)
{
    CRMSDPropertySetup* p_setup = static_cast<CRMSDPropertySetup*>(p_data);
    QObject* p_object = new CRMSDPropertySetupDesigner(p_setup);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRMSDPropertySetupDesigner::CRMSDPropertySetupDesigner(CRMSDPropertySetup* p_setup)
    : CProObjectDesigner(&RMSDPropertySetupDesignerObject,p_setup)
{
    Setup = p_setup;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    // signals -----------------------------------
    connect(WidgetUI.lineWidthSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.lineColorPB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.lineStippleFactorSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.lineStipplePatternCB, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
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

void CRMSDPropertySetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CRMSDPropertySetupDesigner::ApplyAllValues(void)
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

void CRMSDPropertySetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CRMSDPropertySetupDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.lineColorPB->setColor(Setup->LineColor);
    WidgetUI.lineWidthSB->setValue(Setup->LineWidth);
    WidgetUI.lineStippleFactorSB->setValue(Setup->LineStippleFactor);
    WidgetUI.lineStipplePatternCB->setStipplePattern(Setup->LineStipplePattern);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CRMSDPropertySetupDesigner::ApplyValues(void)
{
    Setup->LineColor = WidgetUI.lineColorPB->getCColor();
    Setup->LineWidth = WidgetUI.lineWidthSB->value();
    Setup->LineStippleFactor = WidgetUI.lineStippleFactorSB->value();
    Setup->LineStipplePattern = WidgetUI.lineStipplePatternCB->getStipplePattern();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


