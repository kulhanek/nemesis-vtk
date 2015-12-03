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

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>

#include <PODesignerGeneral.hpp>

#include <CheminfoProjectModule.hpp>
#include <CheminfoProjectStructureList.hpp>
#include <CheminfoProjectStructureListDesigner.hpp>
#include "CheminfoProjectStructureListDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CheminfoProjectStructureListDesignerCB(void* p_data);

CExtUUID        CheminfoProjectStructureListDesignerID(
                    "{CHEMINFO_STRUCTURE_LIST_DESIGNER:8fb26c55-45cf-49ac-b9a5-a036fc046bc5}",
                    "Cheminfo Project Structure List Designer");

CPluginObject   CheminfoProjectStructureListDesignerObject(&CheminfoProjectPlugin,
                    CheminfoProjectStructureListDesignerID,
                    DESIGNER_CAT,
                    ":/images/CheminfoProject/12.cheminfo/CheminfoProjectStructureList.svg",
                    CheminfoProjectStructureListDesignerCB);

// -----------------------------------------------------------------------------

QObject* CheminfoProjectStructureListDesignerCB(void* p_data)
{
    CCheminfoProjectStructureList* p_pro_object = static_cast<CCheminfoProjectStructureList*>(p_data);
    QObject* p_object = new CCheminfoProjectStructureListDesigner(p_pro_object);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectStructureListDesigner::CCheminfoProjectStructureListDesigner(CCheminfoProjectStructureList* p_owner)
    : CProObjectDesigner(&CheminfoProjectStructureListDesignerObject,p_owner)
{   
    StructureList = p_owner;

    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,StructureList,this);
   // Projects = new CPODesignerCheminfoProjectStructureList(WidgetUI.projectsTab,StructureList,this);

    // signals -----------------------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));

    // init all values ---------------------------
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectStructureListDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
  //  Projects->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CCheminfoProjectStructureListDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    Changing = true;
        General->ApplyValues();
    Changing = false;

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectStructureListDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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










