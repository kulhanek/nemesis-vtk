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
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>

#include <ErrorSystem.hpp>
#include <StructureSelection.hpp>
#include <Structure.hpp>

#include "ExtraWPModule.hpp"
#include "SuperCellBuilderWorkPanel.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* SuperCellBuilderWorkPanelCB(void* p_data);

CExtUUID        SuperCellBuilderWorkPanelID(
                    "{SUPER_CELL_BUILDER_WORK_PANEL:ab06478c-26ca-49e3-871e-73239ea1042f}",
                    "Super Cell Builder");

CPluginObject   SuperCellBuilderWorkPanelObject(&ExtraWPPlugin,
                    SuperCellBuilderWorkPanelID,WORK_PANEL_CAT,
                    ":/images/ExtraWP/SuperCellBuilder.svg",
                    SuperCellBuilderWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* SuperCellBuilderWorkPanelCB(void* p_data)
{
    CProObject* p_obj = static_cast<CProObject*>(p_data);
    if( p_obj == NULL ){
        ES_ERROR("p_obj == NULL");
        return(NULL);
    }
    CStructure* p_str = dynamic_cast<CStructure*>(p_obj);
    CProject* p_pro;
    if( p_str ){
        p_pro = p_str->GetProject();
    } else {
        p_pro = dynamic_cast<CProject*>(p_obj);
        p_str = p_pro->GetActiveStructure();
    }
    if( p_pro == NULL ){
        ES_ERROR("p_pro == NULL");
        return(NULL);
    }
    QObject* p_object = new CSuperCellBuilderWorkPanel(p_pro,p_str);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSuperCellBuilderWorkPanel::CSuperCellBuilderWorkPanel(CProject* p_pro,CStructure* p_str)
    : CWorkPanel(&SuperCellBuilderWorkPanelObject,p_pro,EWPR_TOOL)
{
    WidgetUI.setupUi(this);

    // extra setup -------------------------------
    WidgetUI.structureW->setProject(GetProject());
    WidgetUI.structureW->setSelectionHandler(&SH_Structure);
    WidgetUI.structureW->setObjectBaseMIMEType("structure.indexes");

    // local events ------------------------------
    connect(WidgetUI.structureW,SIGNAL(OnObjectChanged()),
            this,SLOT(StructureChanged()));

    connect(WidgetUI.repeatASB,SIGNAL(valueChanged(int)),
            this,SLOT(StructureChanged()));
    connect(WidgetUI.repeatBSB,SIGNAL(valueChanged(int)),
            this,SLOT(StructureChanged()));
    connect(WidgetUI.repeatCSB,SIGNAL(valueChanged(int)),
            this,SLOT(StructureChanged()));

    connect(WidgetUI.vectorKA,SIGNAL(valueChanged(int)),
            this,SLOT(StructureChanged()));
    connect(WidgetUI.vectorKB,SIGNAL(valueChanged(int)),
            this,SLOT(StructureChanged()));
    connect(WidgetUI.vectorKC,SIGNAL(valueChanged(int)),
            this,SLOT(StructureChanged()));

    connect(WidgetUI.repeatN,SIGNAL(valueChanged(int)),
            this,SLOT(StructureChanged()));

    connect(WidgetUI.buildSuperCellPB1,SIGNAL(clicked(bool)),
            this, SLOT(BuildSuperCell1(void)));
    connect(WidgetUI.buildSuperCellPB2,SIGNAL(clicked(bool)),
            this, SLOT(BuildSuperCell2(void)));

    if( GetProject() ){
        connect(GetProject()->GetHistory(), SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
                this, SLOT(ProjectLockChanged(EHistoryChangeMessage)));
    }

    WidgetUI.structureW->setObject(p_str);

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CSuperCellBuilderWorkPanel::~CSuperCellBuilderWorkPanel(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSuperCellBuilderWorkPanel::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    setEnabled(! GetProject()->IsHistoryLocked(EHCL_TOPOLOGY));
}

//------------------------------------------------------------------------------

void CSuperCellBuilderWorkPanel::StructureChanged(void)
{
    bool set1 = true;
    set1 &= WidgetUI.structureW->getObject() != NULL;
    set1 &= (WidgetUI.repeatASB->value() > 1) || (WidgetUI.repeatBSB->value() > 1) || (WidgetUI.repeatCSB->value() > 1);
    WidgetUI.buildSuperCellPB1->setEnabled(set1);

    bool set2 = true;
    set2 &= WidgetUI.structureW->getObject() != NULL;
    set2 &= !((WidgetUI.vectorKA->value() == 0) && (WidgetUI.vectorKB->value() == 0) && (WidgetUI.vectorKC->value() == 0));
    set2 &= (WidgetUI.repeatN->value() > 1);
    WidgetUI.buildSuperCellPB2->setEnabled(set2);
}

//------------------------------------------------------------------------------

void CSuperCellBuilderWorkPanel::BuildSuperCell1(void)
{
    if( WidgetUI.structureW->getObject() == NULL ) return;
    WidgetUI.structureW->getObject<CStructure*>()->BuildSuperCellWH(WidgetUI.repeatASB->value(),WidgetUI.repeatBSB->value(),WidgetUI.repeatCSB->value());
    WidgetUI.repeatASB->setValue(1);
    WidgetUI.repeatBSB->setValue(1);
    WidgetUI.repeatCSB->setValue(1);
}

//------------------------------------------------------------------------------

void CSuperCellBuilderWorkPanel::BuildSuperCell2(void)
{
    if( WidgetUI.structureW->getObject() == NULL ) return;
    WidgetUI.structureW->getObject<CStructure*>()->BuildSuperCellWH(WidgetUI.vectorKA->value(),WidgetUI.vectorKB->value(),WidgetUI.vectorKC->value(),WidgetUI.repeatN->value());
    WidgetUI.repeatN->setValue(1);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




