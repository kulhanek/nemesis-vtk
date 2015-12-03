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

#include <NemesisCoreModule.hpp>
#include <StructureList.hpp>
#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>

#include <PODesignerGeneral.hpp>
#include <PODesignerStructures.hpp>

#include <StructureListDesigner.hpp>
#include "StructureListDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* StructureListDesignerCB(void* p_data);

CExtUUID        StructureListDesignerID(
                    "{STRUCTURE_LIST_DESIGNER:4c262f61-566e-4136-a3ed-15a3bb4b4be5}",
                    "Structures");

CPluginObject   StructureListDesignerObject(&NemesisCorePlugin,
                    StructureListDesignerID,
                    DESIGNER_CAT,
                    ":/images/NemesisCore/structure/StructureList.svg",
                    StructureListDesignerCB);

// -----------------------------------------------------------------------------

QObject* StructureListDesignerCB(void* p_data)
{
    CStructureList* p_pro_object = static_cast<CStructureList*>(p_data);
    QObject* p_object = new CStructureListDesigner(p_pro_object);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CStructureListDesigner::CStructureListDesigner(CStructureList* p_owner)
    : CProObjectDesigner(&StructureListDesignerObject,p_owner)
{   
    List = p_owner;

    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,List,this);
    Structures = new CPODesignerStructures(WidgetUI.structuresTab,List,this);

    // signals -----------------------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));

    // init all values ---------------------------
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStructureListDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    Structures->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CStructureListDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( List->BeginChangeWH(EHCL_COMPOSITE,List->GetType().GetName()) == false ) return;

    Changing = true;
        General->ApplyValues();
        Structures->ApplyValues();
    Changing = false;

    List->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStructureListDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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










