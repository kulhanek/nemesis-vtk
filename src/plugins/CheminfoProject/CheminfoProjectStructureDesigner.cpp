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
//#include "utils/PODesignerCheminfoProjectStructureList.hpp"

#include <CheminfoProjectModule.hpp>
#include <CheminfoProjectStructure.hpp>

#include <CheminfoProjectStructureDesigner.hpp>
#include "CheminfoProjectStructureDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CheminfoProjectStructureDesignerCB(void* p_data);

CExtUUID        CheminfoProjectStructureDesignerID(
                    "{CHEMINFO_STRUCTURE_DESIGNER:41a5d99d-e9d8-4a86-bab5-87a7e698a201}",
                    "Cheminfo Project Structure Designer");

CPluginObject   CheminfoProjectStructureDesignerObject(&CheminfoProjectPlugin,
                    CheminfoProjectStructureDesignerID,DESIGNER_CAT,
                    ":/images/CheminfoProject/12.Cheminfo/Structure.svg",
                    CheminfoProjectStructureDesignerCB);

// -----------------------------------------------------------------------------

QObject* CheminfoProjectStructureDesignerCB(void* p_data)
{
    CCheminfoProjectStructure* p_str = static_cast<CCheminfoProjectStructure*>(p_data);
    QObject* p_object = new CCheminfoProjectStructureDesigner(p_str);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectStructureDesigner::CCheminfoProjectStructureDesigner(CCheminfoProjectStructure* p_chinfo)
    : CProObjectDesigner(&CheminfoProjectStructureDesignerObject,p_chinfo)
{
    Object = p_chinfo;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Flags = new CPODesignerFlags(WidgetUI.flagsGB,Object,this);
    RefBy = new CPODesignerRefBy(WidgetUI.refByTab,Object,this);

    // init models -------------------------------

//    StructureModel = Object->GetStructure()->GetContainerModel(this);
//    WidgetUI.structureTV->setModel(StructureModel);
//    WidgetUI.structureTV->setDragDropMode(QAbstractItemView::DragOnly);
//    WidgetUI.structureTV->setDragEnabled(true);

//    if( StructureModel != NULL ){
//        for(int i=0; i < StructureModel->columnCount(); i++){
//            WidgetUI.structureTV->resizeColumnToContents(i);
//        }
//    }

    // external events ---------------------------
    connect(Object, SIGNAL(OnStatusChanged(EStatusChanged)),
            this, SLOT(InitValues()));

    // change events -----------------------------

    // statistics ----------------
    connect(Object->GetStructure(), SIGNAL(OnStatusChanged()),
            this, SLOT(UpdateStatistics()));

    // designer from TreeView ----
//    connect(WidgetUI.structureTV, SIGNAL(doubleClicked(const QModelIndex&)),
//            this, SLOT(ObjectTVDblClicked(const QModelIndex&)));

    // actions -------------------
    connect(WidgetUI.buttonBox_1, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));

    // initial values
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectStructureDesigner::ButtonBoxClicked(QAbstractButton *button)
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

void CCheminfoProjectStructureDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    InitValues();

}

//------------------------------------------------------------------------------

void CCheminfoProjectStructureDesigner::ApplyAllValues(void)
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

void CCheminfoProjectStructureDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    UpdateStatistics();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CCheminfoProjectStructureDesigner::ApplyValues(void)
{


}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectStructureDesigner::OpenStructureDesigner(void)
{
    CProject* p_proj;
    if((p_proj = Object->GetProject()) == NULL) {
        LOGIC_ERROR("Project == NULL");
    }
    p_proj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CCheminfoProjectStructureDesigner::ObjectTVDblClicked(const QModelIndex& index)
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

void CCheminfoProjectStructureDesigner::UpdateStatistics(void)
{


}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
