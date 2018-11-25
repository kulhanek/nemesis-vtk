// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2018 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <StructureList.hpp>
#include <Structure.hpp>
#include <Project.hpp>
#include <AtomList.hpp>
#include <Atom.hpp>
#include <HistoryList.hpp>
#include <QString>
#include <QMessageBox>
#include <MainWindow.hpp>
#include <StructureSelection.hpp>

#include "SymmetryModule.hpp"
#include "SymmolWorkPanel.hpp"

#include <boost/algorithm/string/join.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* SymmolWorkPanelCB(void* p_data);

CExtUUID        SymmolWorkPanelID(
                    "{SYMMOL_WORK_PANEL:d95a2b28-0fa9-4e7e-a84e-13f838edf9d7}",
                    "Symmol");

CPluginObject   SymmolWorkPanelObject(&SymmetryPlugin,
                    SymmolWorkPanelID,WORK_PANEL_CAT,
                    ":/images/SymmetryWP/SymmolWorkPanel.svg",
                    SymmolWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* SymmolWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CSymmolWorkPanel requires active project");
        return(NULL);
    }

    QObject* p_build_wp = new CSymmolWorkPanel(p_project);
    return(p_build_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSymmolWorkPanel::CSymmolWorkPanel(CProject* p_project)
    : CWorkPanel(&SymmolWorkPanelObject,p_project,EWPR_TOOL)
{
    WidgetUI.setupUi(this);

    // set initial structure
    WidgetUI.structureW->setProject(p_project);
    WidgetUI.structureW->setObjectBaseMIMEType("structure.indexes");
    WidgetUI.structureW->setSelectionHandler(&SH_Structure);
    WidgetUI.structureW->setObject(p_project->GetStructures()->GetActiveStructure());

    // signals
    connect(WidgetUI.runPB,SIGNAL(clicked(bool)),
        this,SLOT(RunSymmol(void)));
    // -------------
    connect(WidgetUI.symmetrizePB,SIGNAL(clicked(bool)),
        this,SLOT(SymmetrizeStructure(void)));
    // -------------
    connect(WidgetUI.structureW,SIGNAL(OnObjectChanged(void)),
        this,SLOT(NewStructure(void)));
    // -------------
    connect(p_project->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
        this,SLOT(ClearAll(void)));

    // load work panel setup
    LoadWorkPanelSetup();

    // init data
    NewStructure();
}

//------------------------------------------------------------------------------

CSymmolWorkPanel::~CSymmolWorkPanel()
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSymmolWorkPanel::LoadWorkPanelSpecificData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }

    double ddummy;
    if( p_ele->GetAttribute("dcm",ddummy) ){
        WidgetUI.dcmSB->setValue(ddummy);
    }
    if( p_ele->GetAttribute("dcme",ddummy) ){
        WidgetUI.dcmeSB->setValue(ddummy);
    }

    bool bdummy;
    if( p_ele->GetAttribute("ca",bdummy) ){
        WidgetUI.useCoreAtomsCB->setChecked(bdummy);
    }
}

//------------------------------------------------------------------------------

void CSymmolWorkPanel::SaveWorkPanelSpecificData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }
    p_ele->SetAttribute("dcm",WidgetUI.dcmSB->value());
    p_ele->SetAttribute("dcme",WidgetUI.dcmeSB->value());
    p_ele->SetAttribute("ca",WidgetUI.useCoreAtomsCB->isChecked());

    CWorkPanel::SaveWorkPanelSpecificData(p_ele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSymmolWorkPanel::NewStructure(void)
{
    WidgetUI.runPB->setEnabled(WidgetUI.structureW->getObject() != NULL);
    ClearAll();
}

//------------------------------------------------------------------------------

void CSymmolWorkPanel::ClearAll(void)
{
    WidgetUI.symbolLE->setText("C1");
    WidgetUI.csmLE->setText("0.000");
    WidgetUI.rmsLE->setText("0.000");
    WidgetUI.symmetrizePB->setEnabled(false);
}

//------------------------------------------------------------------------------

void CSymmolWorkPanel::RunSymmol(void)
{

}

//------------------------------------------------------------------------------

void CSymmolWorkPanel::SymmetrizeStructure(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



