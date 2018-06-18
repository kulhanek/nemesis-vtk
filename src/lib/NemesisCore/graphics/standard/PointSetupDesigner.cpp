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
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <ProjectList.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>
#include <GSDesignerGeneral.hpp>
#include <GSDesignerRefBy.hpp>
#include <QColorDialog>
#include <NemesisCoreModule.hpp>
#include <PointSetup.hpp>
#include <PointSetupDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PointSetupDesignerCB(void* p_data);

CExtUUID        PointSetupDesignerID(
                    "{POINT_SETUP_DESIGNER:a8c1c6b8-4408-43fd-9eff-4fd416fde43b}",
                    "Point");

CPluginObject   PointSetupDesignerObject(&NemesisCorePlugin,
                    PointSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/Point.svg",
                    PointSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* PointSetupDesignerCB(void* p_data)
{
    CPointSetup* p_setup = static_cast<CPointSetup*>(p_data);
    QObject* p_object = new CPointSetupDesigner(p_setup);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPointSetupDesigner::CPointSetupDesigner(CPointSetup* p_setup)
    : CProObjectDesigner(&PointSetupDesignerObject,p_setup)
{
    Setup = p_setup;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    // units -------------------------------------
    WidgetUI.pointSizeSB->setPhysicalQuantity(PQ_DISTANCE);

    // signals -----------------------------------
    connect(WidgetUI.pointColorPB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.pointWidthSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.pointSizeSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.quotationColorPB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.quotationWidthSB, SIGNAL(valueChanged(int)),
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

void CPointSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CPointSetupDesigner::ApplyAllValues(void)
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

void CPointSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CPointSetupDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.pointColorPB->setColor(Setup->PointColor);
    WidgetUI.pointSizeSB->setInternalValue(Setup->PointSize);
    WidgetUI.pointWidthSB->setValue(Setup->PointWidth);

    WidgetUI.quotationColorPB->setColor(Setup->QuotationColor);
    WidgetUI.quotationWidthSB->setValue(Setup->QuotationWidth);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CPointSetupDesigner::ApplyValues(void)
{
    Setup->PointColor = WidgetUI.pointColorPB->getCColor();
    Setup->PointWidth = WidgetUI.pointWidthSB->value();
    Setup->PointSize = WidgetUI.pointSizeSB->getInternalValue();

    Setup->QuotationColor = WidgetUI.quotationColorPB->getCColor();
    Setup->QuotationWidth = WidgetUI.quotationWidthSB->value();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


