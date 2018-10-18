// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <GODesignerFlags.hpp>
#include <GODesignerSetup.hpp>
#include <GODesignerRefBy.hpp>

#include <GraphicsProfileObject.hpp>
#include <GraphicsObjectLink.hpp>
#include <GraphicsObjectLinkDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GraphicsObjectLinkDesignerCB(void* p_data);

CExtUUID        GraphicsObjectLinkDesignerID(
                    "{GRAPHICS_OBJECT_LINK_DESIGNER:e79b6482-ddd9-44ab-9870-63bca5a51147}",
                    "Link");

CPluginObject   GraphicsObjectLinkDesignerObject(&NemesisCorePlugin,
                    GraphicsObjectLinkDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/Link.svg",
                    GraphicsObjectLinkDesignerCB);

// -----------------------------------------------------------------------------

GODESIGNER_ENTRY_POINT(GraphicsObjectLink);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsObjectLinkDesigner::CGraphicsObjectLinkDesigner(CGraphicsObjectLink* p_fmo)
    : CProObjectDesigner(&GraphicsObjectLinkDesignerObject, p_fmo)
{
    Object = p_fmo;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Flags = new CGODesignerFlags(WidgetUI.flagsGB,Object,this);
    Setup = new CGODesignerSetup(WidgetUI.setupTab,Object,this);
    RefBy = new CGODesignerRefBy(WidgetUI.refByTab,Object,this);

    // connect slots -----------------------------
    if( Object->GetEndObject() ){
        // project related signals
        connect(Object->GetEndObject(),SIGNAL(OnStatusChanged(EStatusChanged)),
                this,SLOT(EndObjectStatusChanged(EStatusChanged)));
        //------------------
        connect(WidgetUI.objectEndPB, SIGNAL(clicked()),
                Object->GetEndObject(),SLOT(OpenObjectDesigner()));
    }

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

void CGraphicsObjectLinkDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    Flags->InitValues();
    InitValues();
    Setup->InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGraphicsObjectLinkDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_COMPOSITE,Object->GetType().GetName()) == NULL ) return;

    Changing = true;
        General->ApplyValues();
        Flags->ApplyValues();
        ApplyValues();
    Changing = false;

    Object->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//------------------------------------------------------------------------------

void CGraphicsObjectLinkDesigner::ButtonBoxClicked(QAbstractButton *button)
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

void CGraphicsObjectLinkDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    if( Object->GetEndObject() ){
        WidgetUI.objectEndPB->setText(Object->GetEndObject()->GetName());
        WidgetUI.objectEndTypeLB->setText(Object->GetEndObject()->GetType().GetName());
        WidgetUI.objectEndCatLB->setText(Object->GetEndObject()->GetCategory().GetName());
    }

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGraphicsObjectLinkDesigner::ApplyValues(void)
{
    // nothing to do here
}

//------------------------------------------------------------------------------

void CGraphicsObjectLinkDesigner::EndObjectStatusChanged(EStatusChanged status)
{
    if( (status == ESC_NAME) && (Object->GetEndObject() != NULL) ){
        WidgetUI.objectEndPB->setText(Object->GetEndObject()->GetName());
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

