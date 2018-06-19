// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

#include <CategoryUUID.hpp>
#include <DesktopSystem.hpp>
#include <ProjectList.hpp>
#include <Project.hpp>
#include <ErrorSystem.hpp>
#include <WorkPanelList.hpp>
#include <SelectionList.hpp>
#include <SelectionRequest.hpp>
#include <AtomListSelection.hpp>
#include <ResidueListSelection.hpp>

#include "SelectByMaskWorkPanel.hpp"
#include "StandardWPModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* SelectByMaskWorkPanelCB(void* p_data);

CExtUUID        SelectByMaskWorkPanelID(
                    "{SELECT_BY_MASK_WORK_PANEL:4501cda8-1db7-4634-8eeb-95ca0aa11e6a}",
                    "Select by Mask");

CPluginObject   SelectByMaskWorkPanelObject(&StandardWPPlugin,
                    SelectByMaskWorkPanelID,WORK_PANEL_CAT,
                    ":/images/StandardWP/SelectByMask.svg",
                    SelectByMaskWorkPanelCB);

CSelectByMaskWorkPanel* p_selbm_wp = NULL;

// -----------------------------------------------------------------------------

QObject* SelectByMaskWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("Active project required.");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(SelectByMaskWorkPanelID,p_project);
    if( p_wp == NULL ) {
        p_wp = new CSelectByMaskWorkPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSelectByMaskWorkPanel::CSelectByMaskWorkPanel(CProject* p_project)
    : CWorkPanel(&SelectByMaskWorkPanelObject,p_project,EWPR_TOOL)
{
    // set up ui file
    WidgetUI.setupUi(this);

    SelRequest = new CSelectionRequest(this,tr("Select by Mask"));

    // local events ------------------------------
    connect(WidgetUI.selectNoneTB,SIGNAL(clicked(bool)),
            this,SLOT(SelectNone(void)));
    // -------------
    connect(WidgetUI.clearTB,SIGNAL(clicked(bool)),
            this,SLOT(ClearMask(void)));
    // -------------
    connect(WidgetUI.typeTB,SIGNAL(toggled(bool)),
            this,SLOT(TypeChanged(bool)));
    // -------------
    connect(WidgetUI.executeTB,SIGNAL(clicked(bool)),
            this,SLOT(ExecuteMask(void)));
    // -------------
    connect(WidgetUI.maskLE,SIGNAL(returnPressed()),
            this,SLOT(ExecuteMask(void)));

    // load work panel setup
    LoadWorkPanelSetup();
}

// -----------------------------------------------------------------------------

CSelectByMaskWorkPanel::~CSelectByMaskWorkPanel(void)
{
    SaveWorkPanelSetup();
    p_selbm_wp = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSelectByMaskWorkPanel::SelectNone(void)
{
    SelRequest->SetRequest(NULL);
    GetProject()->RepaintProject();
}

//------------------------------------------------------------------------------

void CSelectByMaskWorkPanel::ClearMask(void)
{
    GetProject()->GetSelection()->ResetSelection();
    GetProject()->RepaintProject();
}

//------------------------------------------------------------------------------

void CSelectByMaskWorkPanel::TypeChanged(bool set)
{
    if( set ){
        WidgetUI.typeTB->setIcon(QIcon(":/images/StandardWP/SelectByMaskWorkPanel/SelectResidue.svg"));
    } else {
        WidgetUI.typeTB->setIcon(QIcon(":/images/StandardWP/SelectByMaskWorkPanel/SelectAtoms.svg"));
    }
}

//------------------------------------------------------------------------------

void CSelectByMaskWorkPanel::ExecuteMask(void)
{
    if( ! SelRequest->IsAttached() ){
        if( WidgetUI.typeTB->isChecked() ){
            SelRequest->SetRequest(GetProject()->GetSelection(),&SH_ResidueList);
        } else {
            SelRequest->SetRequest(GetProject()->GetSelection(),&SH_AtomList);
        }
    }
    bool result;
    if( WidgetUI.typeTB->isChecked() ){
        result = GetProject()->GetSelection()->SelectResiduesByMask(WidgetUI.maskLE->text(),EAMT_ASL);
    } else {
        result = GetProject()->GetSelection()->SelectAtomsByMask(WidgetUI.maskLE->text(),EAMT_ASL);
    }
    if( result ){
        GetProject()->RepaintProject();
    } else {
        GetProject()->TextNotification(ETNT_ERROR,tr("specified selection mask is invalid"),ETNT_ERROR_DELAY);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




