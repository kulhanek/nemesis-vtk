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
#include <CheminfoProjectInfoList.hpp>
#include <CheminfoProjectInfoListDesigner.hpp>
#include "CheminfoProjectInfoListDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CheminfoProjectInfoListDesignerCB(void* p_data);

CExtUUID        CheminfoProjectInfoListDesignerID(
                    "{CHEMINFO_INFO_LIST_DESIGNER:f3d3c6af-6487-442b-b8a0-0db6a504f2e6}",
                    "Cheminfo Projects List Designer");

CPluginObject   CheminfoProjectInfoListDesignerObject(&CheminfoProjectPlugin,
                    CheminfoProjectInfoListDesignerID,
                    DESIGNER_CAT,
                    ":/images/CheminfoProject/12.cheminfo/CheminfoProjectInfoList.svg",
                    CheminfoProjectInfoListDesignerCB);

// -----------------------------------------------------------------------------

QObject* CheminfoProjectInfoListDesignerCB(void* p_data)
{
    CCheminfoProjectInfoList* p_pro_object = static_cast<CCheminfoProjectInfoList*>(p_data);
    QObject* p_object = new CCheminfoProjectInfoListDesigner(p_pro_object);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectInfoListDesigner::CCheminfoProjectInfoListDesigner(CCheminfoProjectInfoList* p_owner)
    : CProObjectDesigner(&CheminfoProjectInfoListDesignerObject,p_owner)
{   
    ProjectInfoList = p_owner;

    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,ProjectInfoList,this);
   // Projects = new CPODesignerCheminfoProjectInfoList(WidgetUI.projectsTab,ProjectInfoList,this);

    // signals -----------------------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));

    // init all values ---------------------------
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectInfoListDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
  //  Projects->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CCheminfoProjectInfoListDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    //if( ProjectInfoList->BeginChangeWH(EHCL_COMPOSITE,ProjectInfoList->GetType().GetName()) == false ) return;

    Changing = true;
        General->ApplyValues();
    //    Projects->ApplyValues();
    Changing = false;

    //ProjectInfoList->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectInfoListDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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










