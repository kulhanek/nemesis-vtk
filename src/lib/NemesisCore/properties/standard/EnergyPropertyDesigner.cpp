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
#include <PODesignerRefBy.hpp>

#include <EnergyProperty.hpp>
#include <EnergyPropertyDesigner.hpp>
#include "EnergyPropertyDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* EnergyPropertyDesignerCB(void* p_data);

CExtUUID        EnergyPropertyDesignerID(
                    "{ENERGY_PROPERTY_DESIGNER:9d508488-088c-401b-98fc-078d51a45db2}",
                    "Energy");

CPluginObject   EnergyPropertyDesignerObject(&NemesisCorePlugin,
                    EnergyPropertyDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/properties/Energy.svg",
                    EnergyPropertyDesignerCB);

// -----------------------------------------------------------------------------

QObject* EnergyPropertyDesignerCB(void* p_data)
{
    CEnergyProperty* p_fmo = static_cast<CEnergyProperty*>(p_data);
    QObject* p_object = new CEnergyPropertyDesigner(p_fmo);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CEnergyPropertyDesigner::CEnergyPropertyDesigner(CEnergyProperty* p_fmo)
    : CProObjectDesigner(&EnergyPropertyDesignerObject, p_fmo)
{
    Object = p_fmo;
    WidgetUI.setupUi(this);

    // set units ---------------------------------
    WidgetUI.energySB->setPhysicalQuantity(Object->GetPropertyUnit());

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    RefBy = new CPODesignerRefBy(WidgetUI.refByTab,Object,this);

    // connect slots -----------------------------
    connect(WidgetUI.energySB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.methodLE, SIGNAL(textChanged(const QString&)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(Object, SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));

    // init all values ---------------------------
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CEnergyPropertyDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CEnergyPropertyDesigner::ApplyAllValues(void)
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

void CEnergyPropertyDesigner::ButtonBoxClicked(QAbstractButton *button)
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

void CEnergyPropertyDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.energySB->setInternalValue(Object->GetEnergy());
    WidgetUI.methodLE->setText(Object->GetMethod());

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CEnergyPropertyDesigner::ApplyValues(void)
{
    Object->SetEnergyWH(WidgetUI.energySB->getInternalValue());
    Object->SetMethodWH(WidgetUI.methodLE->text());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

