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

#include <AngleProperty.hpp>
#include <AnglePropertyDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* AnglePropertyDesignerCB(void* p_data);

CExtUUID        AnglePropertyDesignerID(
                    "{ANGLE_PROPERTY_DESIGNER:87714300-063b-4c9c-9a68-82337679e8fe}",
                    "Angle");

CPluginObject   AnglePropertyDesignerObject(&NemesisCorePlugin,
                    AnglePropertyDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/properties/Angle.svg",
                    AnglePropertyDesignerCB);

// -----------------------------------------------------------------------------

QObject* AnglePropertyDesignerCB(void* p_data)
{
    CAngleProperty* p_fmo = static_cast<CAngleProperty*>(p_data);
    QObject* p_object = new CAnglePropertyDesigner(p_fmo);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAnglePropertyDesigner::CAnglePropertyDesigner(CAngleProperty* p_fmo)
    : CProObjectDesigner(&AnglePropertyDesignerObject, p_fmo)
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

void CAnglePropertyDesigner::InitAllValues(void)
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

void CAnglePropertyDesigner::ApplyAllValues(void)
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

void CAnglePropertyDesigner::ButtonBoxClicked(QAbstractButton *button)
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

