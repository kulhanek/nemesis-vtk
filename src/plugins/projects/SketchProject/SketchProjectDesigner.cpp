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

#include <QtGui>

#include "SketchProjectModule.hpp"
#include "SketchProjectDesigner.hpp"

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>

#include <PRDesignerGeneral.hpp>
#include <PODesignerStructures.hpp>

#include "SketchProject.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* SketchProjectDesignerCB(void* p_data);

CExtUUID        SketchProjectDesignerID(
                    "{SKETCH_PROJECT_DESIGNER:f67b31f6-3827-4399-ac84-f391c512b327}",
                    "Sketch Structure");

CPluginObject   SketchProjectDesignerObject(&SketchProjectPlugin,
                    SketchProjectDesignerID,
                    DESIGNER_CAT,
                    ":/images/SketchProject/SketchProject.svg",
                    SketchProjectDesignerCB);

// -----------------------------------------------------------------------------

QObject* SketchProjectDesignerCB(void* p_data)
{
    CSketchProject* p_pro_object = static_cast<CSketchProject*>(p_data);
    QObject* p_object = new CSketchProjectDesigner(p_pro_object);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSketchProjectDesigner::CSketchProjectDesigner(CSketchProject* p_owner)
    : CProObjectDesigner(&SketchProjectDesignerObject,p_owner)
{   
    Project = p_owner;

    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPRDesignerGeneral(WidgetUI.generalGB,Project,this);

    // signals -----------------------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));

    // init all values ---------------------------
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSketchProjectDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CSketchProjectDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Project->BeginChangeWH(EHCL_COMPOSITE,Project->GetType().GetName()) == NULL ) return;

    Changing = true;
        General->ApplyValues();
    Changing = false;

    Project->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSketchProjectDesigner::ButtonBoxClicked(QAbstractButton* p_button)
{
    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Reset ) {
        InitAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Apply ) {
        ApplyAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Close ) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================










