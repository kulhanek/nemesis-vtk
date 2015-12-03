// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include "DockingProjectModule.hpp"
#include "DockingProjectDesigner.hpp"
#include "DockingProjectDesigner.moc"

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>

#include <PODesignerGeneral.hpp>
#include <PODesignerStructures.hpp>

#include "DockingProject.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DockingProjectDesignerCB(void* p_data);

CExtUUID        DockingProjectDesignerID(
                    "{DOCKING_PROJECT_DESIGNER:c9299df0-0130-4304-ad68-22795c074f10}",
                    "Docking - Virtual screening");

CPluginObject   DockingProjectDesignerObject(&DockingProjectPlugin,
                    DockingProjectDesignerID,
                    DESIGNER_CAT,
                    ":/images/DockingProject/DockingProject.svg",
                    DockingProjectDesignerCB);

// -----------------------------------------------------------------------------

QObject* DockingProjectDesignerCB(void* p_data)
{
    CDockingProject* p_pro_object = static_cast<CDockingProject*>(p_data);
    QObject* p_object = new CDockingProjectDesigner(p_pro_object);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDockingProjectDesigner::CDockingProjectDesigner(CDockingProject* p_owner)
    : CProObjectDesigner(&DockingProjectDesignerObject,p_owner)
{   
    Project = p_owner;

    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Project,this);
    Structures = new CPODesignerStructures(WidgetUI.structuresTab,Project->GetStructures(),this);

    // signals -----------------------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));

    // init all values ---------------------------
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    Structures->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CDockingProjectDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Project->BeginChangeWH(EHCL_COMPOSITE,Project->GetType().GetName()) == false ) return;

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

void CDockingProjectDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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










