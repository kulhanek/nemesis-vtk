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
#include <PhysicalQuantity.hpp>
#include <PhysicalQuantities.hpp>

#include <PODesignerGeneral.hpp>
#include <PODesignerRefBy.hpp>
#include <PRDesignerValue.hpp>
#include <PRDesignerAtoms.hpp>
#include <PRDesignerGeoGraphics.hpp>

#include <DistanceToPositionProperty.hpp>
#include <DistanceToPositionPropertyDesigner.hpp>
#include "DistanceToPositionPropertyDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DistanceToPositionPropertyDesignerCB(void* p_data);

CExtUUID        DistanceToPositionPropertyDesignerID(
                    "{DISTANCE_TO_POSITION_PROPERTY_DESIGNER:4278c14a-3ef3-41c8-b02a-6f7777239d2b}",
                    "Distance to position");

CPluginObject   DistanceToPositionPropertyDesignerObject(&NemesisCorePlugin,
                    DistanceToPositionPropertyDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/properties/DistanceToPosition.svg",
                    DistanceToPositionPropertyDesignerCB);

// -----------------------------------------------------------------------------

QObject* DistanceToPositionPropertyDesignerCB(void* p_data)
{
    CDistanceToPositionProperty* p_fmo = static_cast<CDistanceToPositionProperty*>(p_data);
    QObject* p_object = new CDistanceToPositionPropertyDesigner(p_fmo);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDistanceToPositionPropertyDesigner::CDistanceToPositionPropertyDesigner(CDistanceToPositionProperty* p_fmo)
    : CProObjectDesigner(&DistanceToPositionPropertyDesignerObject, p_fmo)
{
    Object = p_fmo;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Value = new CPRDesignerValue(WidgetUI.dataGB,Object,this);
    Graphics = new CPRDesignerGeoGraphics(WidgetUI.graphicsW,Object,this);
    PointA = new CPRDesignerAtoms(WidgetUI.pointATab,Object->GetPointA(),this);
    RefBy = new CPODesignerRefBy(WidgetUI.refByTab,Object,this);

    // units -------------------------------------
    WidgetUI.posXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.posYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.posZSB->setPhysicalQuantity(PQ_DISTANCE);

    // connect slots -----------------------------
    connect(Object, SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));
    // -------------------------
    connect(WidgetUI.posXSB, SIGNAL(valueChanged(double)),
            this,SLOT(SetChangedFlagTrue()));
    // -------------------------
    connect(WidgetUI.posYSB, SIGNAL(valueChanged(double)),
            this,SLOT(SetChangedFlagTrue()));
    // -------------------------
    connect(WidgetUI.posZSB, SIGNAL(valueChanged(double)),
            this,SLOT(SetChangedFlagTrue()));
    // -------------------------
    connect(WidgetUI.buttonBox1, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    // -------------------------
    connect(WidgetUI.buttonBox2, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    // -------------------------
    connect(WidgetUI.buttonBox3, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));

    // init all values ---------------------------
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDistanceToPositionPropertyDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    Graphics->InitValues();
    Value->InitValues();
    PointA->InitValues();
    RefBy->InitValues();

    InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CDistanceToPositionPropertyDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_COMPOSITE,Object->GetType().GetName()) == false ) return;

    Changing = true;
        General->ApplyValues();
        Graphics->ApplyValues();
        ApplyValues();
    Changing = false;

    Object->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//------------------------------------------------------------------------------

void CDistanceToPositionPropertyDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    CPoint pos = Object->GetPointB();

    WidgetUI.posXSB->setInternalValue(pos.x);
    WidgetUI.posYSB->setInternalValue(pos.y);
    WidgetUI.posZSB->setInternalValue(pos.z);
}

//------------------------------------------------------------------------------

void CDistanceToPositionPropertyDesigner::ApplyValues(void)
{
    CPoint pos;
    pos.x = WidgetUI.posXSB->getInternalValue();
    pos.y = WidgetUI.posYSB->getInternalValue();
    pos.z = WidgetUI.posZSB->getInternalValue();

    Object->SetPointBWH(pos);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDistanceToPositionPropertyDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Reset) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Reset) ||
        (WidgetUI.buttonBox3->standardButton(button) == QDialogButtonBox::Reset)  ) {
        InitAllValues();
        return;
    }

    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Apply) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Apply) ||
        (WidgetUI.buttonBox3->standardButton(button) == QDialogButtonBox::Apply) ) {
        ApplyAllValues();
        return;
    }

    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Close) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Close) ||
        (WidgetUI.buttonBox3->standardButton(button) == QDialogButtonBox::Close) ) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

