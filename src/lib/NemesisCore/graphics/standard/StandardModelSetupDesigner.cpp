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

#include <NemesisCoreModule.hpp>
#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <ProjectList.hpp>
#include <GSDesignerGeneral.hpp>
#include <GSDesignerRefBy.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>
#include <QMessageBox>

#include <StandardModelSetup.hpp>
#include <StandardModelSetupDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* StandardModelSetupDesignerCB(void* p_data);

CExtUUID        StandardModelSetupDesignerID(
                    "{STANDARD_MODEL_SETUP_DESIGNER:f2976c7e-687c-4658-a34b-c5a6c25df73d}",
                    "Standard Model");

CPluginObject   StandardModelSetupDesignerObject(&NemesisCorePlugin,
                    StandardModelSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/StandardModel.svg",
                    StandardModelSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* StandardModelSetupDesignerCB(void* p_data)
{
    CStandardModelSetup* p_ls = static_cast<CStandardModelSetup*>(p_data);
    QObject* p_object = new CStandardModelSetupDesigner(p_ls);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CStandardModelSetupDesigner::CStandardModelSetupDesigner(CStandardModelSetup* p_ls)
    : CProObjectDesigner(&StandardModelSetupDesignerObject,p_ls)
{
    Setup = p_ls;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    // units -------------------------------------
    WidgetUI.DSBARadius->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.SBARatio->setPhysicalQuantity(PQ_PERCENTAGE);
    WidgetUI.DSBBRadius->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.DSBBPitch->setPhysicalQuantity(PQ_DISTANCE);

    // extra setup -------------------------------
    WidgetUI.CBModel->addItem(tr("Sticks"));
    WidgetUI.CBModel->addItem(tr("Sticks and Balls"));
    WidgetUI.CBModel->addItem(tr("Tubes"));
    WidgetUI.CBModel->addItem(tr("Tubes and Balls"));
    WidgetUI.CBModel->addItem(tr("Points"));
    WidgetUI.CBModel->addItem(tr("Spheres"));

    WidgetUI.CBAtomType->addItem(tr("Points"));
    WidgetUI.CBAtomType->addItem(tr("Spheres"));
    WidgetUI.CBAtomType->addItem(tr("Icosahedrons"));

    WidgetUI.CBBondType->addItem(tr("Sticks"));
    WidgetUI.CBBondType->addItem(tr("Tubes"));

    ModelIndex = 0;

    // signals -----------------------------------
    connect(WidgetUI.CBModel, SIGNAL(currentIndexChanged(int)),
            this, SLOT(ModelIndexChanged(int)));
    // -------------
    connect(WidgetUI.CBAtomType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.CBBondType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.DSBARadius, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.SBARatio, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.SBAQualityA, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.DSBBRadius, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.SBBQualityA, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.ChBDiffuse, SIGNAL(stateChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.ChBShowOrder, SIGNAL(stateChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.DSBBPitch, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));
    // -------------
    connect(PQ_DISTANCE,SIGNAL(OnUnitChanged()),
            this,SLOT(InitValues()));

    // populate designer with data
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStandardModelSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CStandardModelSetupDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

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

void CStandardModelSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CStandardModelSetupDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    // object section
    CModelSettings settings;
    settings = Setup->Models[ModelIndex];

    // atoms
    WidgetUI.CBAtomType->setCurrentIndex(settings.Atoms.Type);
    WidgetUI.DSBARadius->setInternalValue(settings.Atoms.Radius);
    WidgetUI.SBARatio->setInternalValue(settings.Atoms.Ratio);
    WidgetUI.SBAQualityA->setValue(settings.Atoms.TessellationQuality);

    // bonds
    WidgetUI.CBBondType->setCurrentIndex(settings.Bonds.Type);
    WidgetUI.SBBQualityA->setValue(settings.Bonds.TessellationQuality);
    WidgetUI.ChBShowOrder->setChecked(settings.Bonds.ShowOrder);
    WidgetUI.ChBDiffuse->setChecked(settings.Bonds.Diffuse);
    WidgetUI.DSBBRadius->setInternalValue(settings.Bonds.Radius);
    WidgetUI.DSBBPitch->setInternalValue(settings.Bonds.Pitch);

    // we need to reset the changed flag because setCurrentIndex emit signals
    // that set the changed flag to true
    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CStandardModelSetupDesigner::ApplyValues(void)
{
    // object section
    CModelSettings settings;

    settings.Atoms.Type = WidgetUI.CBAtomType->currentIndex();
    settings.Atoms.Radius = WidgetUI.DSBARadius->getInternalValue();
    settings.Atoms.Ratio = WidgetUI.SBARatio->getInternalValue();
    settings.Atoms.TessellationQuality = WidgetUI.SBAQualityA->value();

    settings.Bonds.Type = WidgetUI.CBBondType->currentIndex();
    settings.Bonds.Radius = WidgetUI.DSBBRadius->getInternalValue();
    settings.Bonds.TessellationQuality = WidgetUI.SBBQualityA->value();
    settings.Bonds.Pitch = WidgetUI.DSBBPitch->getInternalValue();
    settings.Bonds.ShowOrder = WidgetUI.ChBShowOrder->isChecked();
    settings.Bonds.Diffuse = WidgetUI.ChBDiffuse->isChecked();

    Setup->Models[ModelIndex] = settings;
}

//------------------------------------------------------------------------------

void CStandardModelSetupDesigner::ModelIndexChanged(int index)
{
    if( ModelIndex == index ) return;

    if( IsChangedFlagSet() ){
        int result = QMessageBox::warning(this,tr("Change model"),
                tr("The setup was changed! Do you want to save it?"),
                QMessageBox::Yes|QMessageBox::No,
                QMessageBox::Yes);
        if( result == QMessageBox::No ){
            WidgetUI.CBModel->setCurrentIndex(ModelIndex);
            return;
        }
        // apply all values
        ApplyAllValues();
    }

    ModelIndex = index;

    // reload
    InitValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


