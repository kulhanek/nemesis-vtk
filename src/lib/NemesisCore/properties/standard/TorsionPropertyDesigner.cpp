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

#include <PODesignerGeneral.hpp>
#include <PODesignerRefBy.hpp>
#include <PRDesignerValue.hpp>
#include <PRDesignerAtoms.hpp>
#include <PRDesignerGeoGraphics.hpp>

#include <TorsionProperty.hpp>
#include <TorsionPropertyDesigner.hpp>
#include "TorsionPropertyDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* TorsionPropertyDesignerCB(void* p_data);

CExtUUID        TorsionPropertyDesignerID(
                    "{TORSION_PROPERTY_DESIGNER:43df40b2-0da5-4669-98d2-54060244df49}",
                    "Torsion");

CPluginObject   TorsionPropertyDesignerObject(&NemesisCorePlugin,
                    TorsionPropertyDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/properties/Torsion.svg",
                    TorsionPropertyDesignerCB);

// -----------------------------------------------------------------------------

QObject* TorsionPropertyDesignerCB(void* p_data)
{
    CTorsionProperty* p_fmo = static_cast<CTorsionProperty*>(p_data);
    QObject* p_object = new CTorsionPropertyDesigner(p_fmo);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTorsionPropertyDesigner::CTorsionPropertyDesigner(CTorsionProperty* p_fmo)
    : CProObjectDesigner(&TorsionPropertyDesignerObject, p_fmo)
{
    Object = p_fmo;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Value = new CPRDesignerValue(WidgetUI.dataGB,Object,this);
    Graphics = new CPRDesignerGeoGraphics(WidgetUI.graphicsW,Object,this);
    PointA = new CPRDesignerAtoms(WidgetUI.pointATab,Object->GetPointA(),this);
    PointB = new CPRDesignerAtoms(WidgetUI.pointBTab,Object->GetPointB(),this);
    PointC = new CPRDesignerAtoms(WidgetUI.pointCTab,Object->GetPointC(),this);
    PointD = new CPRDesignerAtoms(WidgetUI.pointDTab,Object->GetPointD(),this);
    RefBy = new CPODesignerRefBy(WidgetUI.refByTab,Object,this);

    // connect slots -----------------------------
    connect(WidgetUI.buttonBox1, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    // -------------------------
    connect(WidgetUI.buttonBox2, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));

    // init all values ---------------------------
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTorsionPropertyDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    Graphics->InitValues();
    Value->InitValues();
    PointA->InitValues();
    PointB->InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CTorsionPropertyDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_COMPOSITE,Object->GetType().GetName()) == false ) return;

    Changing = true;
        General->ApplyValues();
        Graphics->ApplyValues();
    Changing = false;

    Object->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//------------------------------------------------------------------------------

void CTorsionPropertyDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Reset) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Reset) ) {
        InitAllValues();
        return;
    }

    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Apply) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Apply) ) {
        ApplyAllValues();
        return;
    }

    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Close) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Close) ) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

