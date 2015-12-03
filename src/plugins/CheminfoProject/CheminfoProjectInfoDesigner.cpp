// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <PluginObject.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <ProjectList.hpp>
#include <ContainerModel.hpp>
#include <Graphics.hpp>
#include <GraphicsProfileList.hpp>
#include <GraphicsObject.hpp>
#include <GraphicsObjectList.hpp>

#include <PODesignerGeneral.hpp>
#include <PODesignerFlags.hpp>
#include <PODesignerRefBy.hpp>
//#include "utils/PODesignerCheminfoProjectInfoList.hpp"

#include <CheminfoProjectModule.hpp>
#include <CheminfoProjectInfo.hpp>

#include <CheminfoProjectInfoDesigner.hpp>
#include "CheminfoProjectInfoDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CheminfoProjectInfoDesignerCB(void* p_data);

CExtUUID        CheminfoProjectInfoDesignerID(
                    "{CHEMINFO_INFO_DESIGNER:cd4ece70-9256-4d33-a8ef-600b5823c181}",
                    "Cheminfo Project Info Designer");

CPluginObject   CheminfoProjectInfoDesignerObject(&CheminfoProjectPlugin,
                    CheminfoProjectInfoDesignerID,DESIGNER_CAT,
                    ":/images/CheminfoProject/12.Cheminfo/ProjectInfo.svg",
                    CheminfoProjectInfoDesignerCB);

// -----------------------------------------------------------------------------

QObject* CheminfoProjectInfoDesignerCB(void* p_data)
{
    CCheminfoProjectInfo* p_str = static_cast<CCheminfoProjectInfo*>(p_data);
    QObject* p_object = new CCheminfoProjectInfoDesigner(p_str);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectInfoDesigner::CCheminfoProjectInfoDesigner(CCheminfoProjectInfo* p_chinfo)
    : CProObjectDesigner(&CheminfoProjectInfoDesignerObject,p_chinfo)
{
    Object = p_chinfo;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Flags = new CPODesignerFlags(WidgetUI.flagsGB,Object,this);
    RefBy = new CPODesignerRefBy(WidgetUI.refByTab,Object,this);

    // external events ---------------------------
    connect(Object, SIGNAL(OnStatusChanged(EStatusChanged)),
            this, SLOT(InitValues()));

    // change events -----------------------------

    // statistics ----------------
    connect(Object->GetProjectInfo(), SIGNAL(OnStatusChanged()),
            this, SLOT(UpdateStatistics()));

    // actions -------------------
    connect(WidgetUI.buttonBox_1, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));

    // initial values
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectInfoDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( (WidgetUI.buttonBox_1->standardButton(button) == QDialogButtonBox::Reset) ) {
        InitAllValues();
        return;
    }

    if( (WidgetUI.buttonBox_1->standardButton(button) == QDialogButtonBox::Apply) ) {
        ApplyAllValues();
        return;
    }

    if( (WidgetUI.buttonBox_1->standardButton(button) == QDialogButtonBox::Close) ) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectInfoDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    InitValues();

}

//------------------------------------------------------------------------------

void CCheminfoProjectInfoDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    Changing = true;
        ApplyValues();
    Changing = false;

    Object->EndChange(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectInfoDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    UpdateStatistics();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CCheminfoProjectInfoDesigner::ApplyValues(void)
{


}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectInfoDesigner::OpenProjectDesigner(void)
{
    CProject* p_proj;
    if((p_proj = Object->GetProject()) == NULL) {
        LOGIC_ERROR("Project == NULL");
    }
    p_proj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CCheminfoProjectInfoDesigner::ObjectTVDblClicked(const QModelIndex& index)
{
    QAbstractItemModel* p_amodel = (QAbstractItemModel*)(index.model());
    CContainerModel* p_model = dynamic_cast<CContainerModel*>(p_amodel);
    if( p_model == NULL ) return;
    CProObject* p_obj = dynamic_cast<CProObject*>(p_model->GetItem(index));
    if( p_obj == NULL ) return;
    p_obj->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectInfoDesigner::UpdateStatistics(void)
{


}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
