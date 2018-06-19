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
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>
#include <QColorDialog>
#include <FreezedAtomsSetup.hpp>
#include <GSDesignerGeneral.hpp>
#include <GSDesignerRefBy.hpp>
#include <FreezedAtomsSetupDesigner.hpp>
#include "FreezedAtomsSetupDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* FreezedAtomsSetupDesignerCB(void* p_data);

CExtUUID        FreezedAtomsSetupDesignerID(
                    "{FREEZED_ATOM_SETUP_DESIGNER:e4e2f3b8-e9f6-4d7c-8749-30a834752c70}",
                    "Freezed Atoms");

CPluginObject   FreezedAtomsSetupDesignerObject(&NemesisCorePlugin,
                    FreezedAtomsSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/FreezedAtoms.svg",
                    FreezedAtomsSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* FreezedAtomsSetupDesignerCB(void* p_data)
{
    CFreezedAtomsSetup* p_setup = static_cast<CFreezedAtomsSetup*>(p_data);
    QObject* p_object = new CFreezedAtomsSetupDesigner(p_setup);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFreezedAtomsSetupDesigner::CFreezedAtomsSetupDesigner(CFreezedAtomsSetup* p_setup)
    : CProObjectDesigner(&FreezedAtomsSetupDesignerObject,p_setup)
{
    Setup = p_setup;
    WidgetUI.setupUi(this);
    // it must be here becuase the title is set in WidgetUI.setupUi(this)
    setWindowTitle(GetType().GetName());

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    // units -------------------------------------
    WidgetUI.ratioSB->setPhysicalQuantity(PQ_PERCENTAGE);

    // signals -----------------------------------
    connect(WidgetUI.colorTB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.widthSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.ratioSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));
    // -------------
    connect(Setup, SIGNAL(OnSetupChanged()),
            this, SLOT(InitValues()));

    // init all values ---------------------------
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFreezedAtomsSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CFreezedAtomsSetupDesigner::ApplyAllValues(void)
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

//------------------------------------------------------------------------------

void CFreezedAtomsSetupDesigner::LoadObjectDesignerSetup(CXMLElement* p_ele)
{
    General->LoadDesignerSetup(p_ele);
    RefBy->LoadDesignerSetup(p_ele);
}

//------------------------------------------------------------------------------

void CFreezedAtomsSetupDesigner::SaveObjectDesignerSetup(CXMLElement* p_ele)
{
    General->SaveDesignerSetup(p_ele);
    RefBy->SaveDesignerSetup(p_ele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFreezedAtomsSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CFreezedAtomsSetupDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.colorTB->setColor(Setup->Color);
    WidgetUI.widthSB->setValue(Setup->Width);
    WidgetUI.ratioSB->setInternalValue(Setup->Ratio);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CFreezedAtomsSetupDesigner::ApplyValues(void)
{    
    Setup->Color = WidgetUI.colorTB->getCColor();
    Setup->Width = WidgetUI.widthSB->value();
    Setup->Ratio = WidgetUI.ratioSB->getInternalValue();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


