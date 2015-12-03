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

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <ProjectList.hpp>
#include <GSDesignerGeneral.hpp>
#include <GSDesignerRefBy.hpp>
#include <QColorDialog>
#include <NemesisCoreModule.hpp>
#include <PlaneSetup.hpp>
#include <PlaneSetupDesigner.hpp>
#include "PlaneSetupDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PlaneSetupDesignerCB(void* p_data);

CExtUUID        PlaneSetupDesignerID(
                    "{PLANE_SETUP_DESIGNER:1d7982a0-6d65-4e5e-a3b7-151e00456ff2}",
                    "Plane");

CPluginObject   PlaneSetupDesignerObject(&NemesisCorePlugin,
                    PlaneSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/Plane.svg",
                    PlaneSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* PlaneSetupDesignerCB(void* p_data)
{
    CPlaneSetup* p_setup = static_cast<CPlaneSetup*>(p_data);
    QObject* p_object = new CPlaneSetupDesigner(p_setup);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPlaneSetupDesigner::CPlaneSetupDesigner(CPlaneSetup* p_setup)
    : CProObjectDesigner(&PlaneSetupDesignerObject,p_setup)
{
    Setup = p_setup;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CGSDesignerGeneral(WidgetUI.generalGB,Setup,this);
    RefBy = new CGSDesignerRefBy(WidgetUI.refByTab,Setup,this);

    // signals -----------------------------------
    connect(WidgetUI.frontColorPB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.backColorPB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.numOfVerticesSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.transparencyCB, SIGNAL(clicked(bool)),
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

void CPlaneSetupDesigner::InitAllValues(void)
{
    if( IsItChangingContent() ) return;

    General->InitValues();
    InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CPlaneSetupDesigner::ApplyAllValues(void)
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

void CPlaneSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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

void CPlaneSetupDesigner::InitValues(void)
{
    WidgetUI.frontColorPB->setColor(Setup->FrontColor);
    WidgetUI.backColorPB->setColor(Setup->BackColor);
    WidgetUI.numOfVerticesSB->setValue(Setup->NumOfVertices);
    WidgetUI.transparencyCB->setChecked(Setup->Transparent);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CPlaneSetupDesigner::ApplyValues(void)
{
    Setup->FrontColor = WidgetUI.frontColorPB->getCColor();
    Setup->BackColor = WidgetUI.backColorPB->getCColor();
    Setup->NumOfVertices = WidgetUI.numOfVerticesSB->value();
    Setup->Transparent = WidgetUI.transparencyCB->isChecked();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


