// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2016 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <GraphicsObject.hpp>

#include <PODesignerGeneral.hpp>
#include <GODesignerSetup.hpp>
#include <GODesignerRefBy.hpp>

#include <GraphicsProfileObject.hpp>

#include "AmberModule.hpp"
#include "GESPGridObject.hpp"
#include "GESPGridObjectDesigner.hpp"
#include "GESPGridObjectDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GESPGridObjectDesignerCB(void* p_data);

CExtUUID        GESPGridObjectDesignerID(
                    "{GESPGRID_OBJECT_DESIGNER:3cc93f37-7c4f-4c42-ae49-64e4cdeb2104}",
                    "GESPGrid");

CPluginObject   GESPGridObjectDesignerObject(&AmberPlugin,
                    GESPGridObjectDesignerID,DESIGNER_CAT,
                    ":/images/Amber/GESPGrid.svg",
                    GESPGridObjectDesignerCB);

// -----------------------------------------------------------------------------

GODESIGNER_ENTRY_POINT(GESPGridObject);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGESPGridObjectDesigner::CGESPGridObjectDesigner(CGraphicsObject* p_bo)
    : CProObjectDesigner(&GESPGridObjectDesignerObject, p_bo)
{
    Object = p_bo;

    // graphics layout ---------------------------
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Setup = new CGODesignerSetup(WidgetUI.setupTab,Object,this);
    RefBy = new CGODesignerRefBy(WidgetUI.refByTab,Object,this);

    // connect slots -----------------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));

    // init all values ---------------------------
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGESPGridObjectDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    Setup->InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGESPGridObjectDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_DESCRIPTION,"background change") == false ) return;

    Changing = true;
        General->ApplyValues();
    Changing = false;

    Object->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//------------------------------------------------------------------------------

void CGESPGridObjectDesigner::ButtonBoxClicked(QAbstractButton *button)
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


