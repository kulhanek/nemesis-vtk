// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2011 Jiri Klusak, anim4tor@mail.muni.cz
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
#include <PhysicalQuantity.hpp>
#include <PhysicalQuantities.hpp>
#include <GlobalSetup.hpp>

#include <GSDesignerGeneral.hpp>
#include <GSDesignerRefBy.hpp>

#include <GeometryLabelSetup.hpp>
#include <GeometryLabelSetupDesigner.hpp>
#include "GeometryLabelSetupDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GeometryLabelSetupDesignerCB(void* p_data);

CExtUUID        GeometryLabelSetupDesignerID(
                    "{GEOMETRY_LABEL_SETUP_DESIGNER:3bf1c462-5337-4b19-a851-db4f52c79940}",
                    "Geometry Label");

CPluginObject   GeometryLabelSetupDesignerObject(&NemesisCorePlugin,
                    GeometryLabelSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/GeometryLabel.svg",
                    GeometryLabelSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* GeometryLabelSetupDesignerCB(void* p_data)
{
    CGeometryLabelSetup* p_setup = static_cast<CGeometryLabelSetup*>(p_data);
    QObject* p_object = new CGeometryLabelSetupDesigner(p_setup);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGeometryLabelSetupDesigner::CGeometryLabelSetupDesigner(CGeometryLabelSetup* p_setup)
    : CProObjectDesigner(&GeometryLabelSetupDesignerObject,p_setup)
{
    Setup = p_setup;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    // units -------------------------------------
    WidgetUI.angleArcScalingSB->setPhysicalQuantity(PQ_DISTANCE);

    // extra setup -------------------------------
    WidgetUI.fontNameCB->setFontPath(QString(GlobalSetup->GetFontPath()));

    // signals -----------------------------------
    connect(WidgetUI.lineColorPB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.lineFactorSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.lineStipplePatternCB, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.lineWidthSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.angleArcScalingSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.quotationColorPB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.quotationStippleFactorSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.quotationStipplePatternCB, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.quotationWidthSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.labelColorPB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.fontNameCB, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.fontSizeSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.showUnitCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.depthTestCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    // -------------
    connect(WidgetUI.buttonBox1, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));
    // -------------
    connect(WidgetUI.buttonBox2, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));
    // -------------
    connect(WidgetUI.buttonBox3, SIGNAL(clicked(QAbstractButton*)),
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

void CGeometryLabelSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGeometryLabelSetupDesigner::ApplyAllValues(void)
{
    // apply all changes
    Changing = true;
        General->ApplyValues();
        ApplyValues();
        RefBy->ApplyValues();
        // update font
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

void CGeometryLabelSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
{
    if( (WidgetUI.buttonBox1->standardButton(p_button) == QDialogButtonBox::Reset) ||
        (WidgetUI.buttonBox2->standardButton(p_button) == QDialogButtonBox::Reset) ||
        (WidgetUI.buttonBox3->standardButton(p_button) == QDialogButtonBox::Reset) ) {
        InitAllValues();
    }

    if( (WidgetUI.buttonBox1->standardButton(p_button) == QDialogButtonBox::Apply) ||
        (WidgetUI.buttonBox2->standardButton(p_button) == QDialogButtonBox::Apply) ||
        (WidgetUI.buttonBox3->standardButton(p_button) == QDialogButtonBox::Apply) ) {
        ApplyAllValues();
    }

    if( (WidgetUI.buttonBox1->standardButton(p_button) == QDialogButtonBox::Close) ||
        (WidgetUI.buttonBox2->standardButton(p_button) == QDialogButtonBox::Close) ||
        (WidgetUI.buttonBox3->standardButton(p_button) == QDialogButtonBox::Close) ) {
        close();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGeometryLabelSetupDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.lineColorPB->setColor(Setup->LineColor);
    WidgetUI.lineWidthSB->setValue(Setup->LineWidth);
    WidgetUI.lineFactorSB->setValue(Setup->LineStippleFactor);
    WidgetUI.lineStipplePatternCB->setStipplePattern(Setup->LineStipplePattern);
    WidgetUI.angleArcScalingSB->setInternalValue(Setup->AngleArcScope);

    WidgetUI.quotationColorPB->setColor(Setup->QuotationColor);
    WidgetUI.quotationWidthSB->setValue(Setup->QuotationLineWidth);
    WidgetUI.quotationStippleFactorSB->setValue(Setup->QuotationStippleFactor);
    WidgetUI.quotationStipplePatternCB->setStipplePattern(Setup->QuotationStipplePattern);

    WidgetUI.labelColorPB->setColor(Setup->LabelColor);
    WidgetUI.fontNameCB->setFontName(Setup->LabelFontName);
    WidgetUI.fontSizeSB->setValue(Setup->LabelFontSize);
    WidgetUI.showUnitCB->setChecked(Setup->ShowUnit);
    WidgetUI.depthTestCB->setChecked(Setup->DepthTest);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGeometryLabelSetupDesigner::ApplyValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    Setup->LineColor = WidgetUI.lineColorPB->getCColor();
    Setup->LineWidth = WidgetUI.lineWidthSB->value();
    Setup->LineStippleFactor = WidgetUI.lineFactorSB->value();
    Setup->LineStipplePattern = WidgetUI.lineStipplePatternCB->getStipplePattern();
    Setup->AngleArcScope = WidgetUI.angleArcScalingSB->getInternalValue();

    Setup->QuotationColor = WidgetUI.quotationColorPB->getCColor();
    Setup->QuotationLineWidth = WidgetUI.quotationWidthSB->value();
    Setup->QuotationStippleFactor = WidgetUI.quotationStippleFactorSB->value();
    Setup->QuotationStipplePattern = WidgetUI.quotationStipplePatternCB->getStipplePattern();

    Setup->LabelColor = WidgetUI.labelColorPB->getCColor();
    Setup->LabelFontName = WidgetUI.fontNameCB->getFontName();
    Setup->LabelFontSize = WidgetUI.fontSizeSB->value();
    Setup->DepthTest = WidgetUI.depthTestCB->isChecked();
    Setup->ShowUnit = WidgetUI.showUnitCB->isChecked();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


