// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ErrorSystem.hpp>
#include <CategoryUUID.hpp>
#include <DesktopSystem.hpp>
#include <ProjectList.hpp>
#include <Project.hpp>
#include <SelectionList.hpp>
#include <AtomSelection.hpp>
#include <BondSelection.hpp>
#include <ConnectAtomSelection.hpp>
#include <AtomSeqSelection.hpp>
#include <Atom.hpp>
#include <Bond.hpp>
#include <Structure.hpp>
#include <Fragment.hpp>
#include <GlobalSetup.hpp>
#include <Graphics.hpp>
#include <GraphicsProfile.hpp>
#include <PeriodicTableWidget.hpp>
#include <PeriodicTable.hpp>
#include <SelectionRequest.hpp>
#include <InserterList.hpp>
#include <OptimizerSetupList.hpp>
#include <Optimizer.hpp>
#include <WorkPanelList.hpp>

#include "StructurePaletteWorkPanel.moc"
#include "StructurePaletteWorkPanel.hpp"
#include "BioToolsModule.hpp"

#include <QGridLayout>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* StructurePaletteWorkPanelCB(void* p_data);

CExtUUID        StructurePaletteWorkPanelID(
                    "{STRUCTURE_PALETTE_WORK_PANEL:4055ad4a-4b46-4130-bad3-9192dee5c512}",
                    "Structure Palette");

CPluginObject   StructurePaletteWorkPanelObject(&BioToolsPlugin,
                     StructurePaletteWorkPanelID,WORK_PANEL_CAT,
                     ":images/BioTools/StructurePaletteWP.svg",
                     StructurePaletteWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* StructurePaletteWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CStructurePaletteWorkPanel requires active project");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(StructurePaletteWorkPanelID,p_project);
    if( p_wp == NULL ) {
        p_wp = new CStructurePaletteWorkPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CStructurePaletteWorkPanel::CStructurePaletteWorkPanel(CProject* p_project)
    : CWorkPanel(&StructurePaletteWorkPanelObject,p_project,EWPR_TOOL)
{
    // set up ui file
    WidgetUI.setupUi(this);

    connect(GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    if( layout() != NULL ) {
        QSpacerItem* p_sp = new QSpacerItem(20,5,QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout()->addItem(p_sp);
    }

    Action = EBA_NONE;

    // action group
    ActionGroup = new QButtonGroup(this);
    ActionGroup->setExclusive(true);
    ActionGroup->addButton(WidgetUI.nonePB);

    WidgetUI.nonePB->setChecked(true);
    WidgetUI.nonePB->hide();

    connect(ActionGroup,SIGNAL(buttonClicked(QAbstractButton*)),
            this,SLOT(ActionChange(QAbstractButton*)));

    connect(ActionGroup,SIGNAL(buttonClicked(QAbstractButton*)),
            this,SLOT(ActionChange(QAbstractButton*)));

    // create selection request
    SelRequest = new CSelectionRequest(this,tr("build panel"));
    connect(SelRequest,SIGNAL(OnSelectionCompleted(void)),
            this,SLOT(SelectionCompleted(void)));
    connect(SelRequest,SIGNAL(OnDetached(void)),
            this,SLOT(SelectionDetached(void)));

    // load structure palete
    LoadFragments();

    // set default action
    SetAction(EBA_NONE);

    // load work panel setup
    LoadWorkPanelSetup();

    // update locks
    ProjectLockChanged(EHCM_LOCK_LEVEL);
}

// -----------------------------------------------------------------------------

CStructurePaletteWorkPanel::~CStructurePaletteWorkPanel(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStructurePaletteWorkPanel::ActionChange(QAbstractButton *p_button)
{
    if( ActiveButton == p_button ) {
        CFragment* p_structure = NULL;
        if((p_structure = GetActiveFragment()) != NULL) {
            // change to next connector
            p_structure->SetNextConnector();
            // change icon
            QIcon ico = p_structure->GetConnectorIcon();
            QSize size = ico.actualSize(QSize(128,128));
            p_button->setIconSize(size+QSize(8,8));
            p_button->setIcon(ico);
        }
        return;
    }
    ActiveButton = p_button;

    // ---------------------------------
    SetAction(EBA_ADD_STRUCTURE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStructurePaletteWorkPanel::SetAction(EPaletteAction new_action)
{
    Action = new_action;

    switch(Action) {
    case EBA_NONE:
        WidgetUI.nonePB->setChecked(true);
        ActiveButton = NULL;
        SelRequest->SetRequest(NULL);
        break;
        //------------------------------
    case EBA_ADD_STRUCTURE:
        SelRequest->SetRequest(GetProject()->GetSelection(),&SH_ConnectAtom,"to attach a molecular fragment");
        break;
    }
    GetProject()->RepaintProject();
}

//------------------------------------------------------------------------------

void CStructurePaletteWorkPanel::SelectionCompleted(void)
{
    CSelectionList* p_sel = SelRequest->GetSelectionList();

    switch(Action) {
    case EBA_ADD_STRUCTURE: {
        CFragment* p_str = GetActiveFragment();
        if( p_str == NULL) break;

        CProObject* p_object = p_sel->PopSelectedObject();

        // attach structure to the selected atom
        CAtom* p_atom = dynamic_cast<CAtom*>(p_object);
        if( p_atom != NULL ) {
            Inserters->InsertStructureWH(p_str,p_atom);
        } else {
            // it has to be in else block since p_object can be destroyed by AttachStructureWH
            // insert structure to the molecule
            CStructure* p_mol = dynamic_cast<CStructure*>(p_object);
            if( p_mol != NULL ) {
                Inserters->InsertStructureWH(p_str,p_mol);
            }
        }
    }
    break;
    //------------------------------
    default:
        break;
    }

    p_sel->GetProject()->RepaintProject();
}

//------------------------------------------------------------------------------

void CStructurePaletteWorkPanel::SelectionDetached(void)
{
    SetAction(EBA_NONE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CStructurePaletteWorkPanel::LoadFragments(void)
{
    CFileName path = GlobalSetup->GetWorkPanelSetupPath(GetType());

    // ---------------------------------
    bool result = true;
    result &= PentoseFragments.Load(path / "pentoses.xml");
    result &= HexoseFragments.Load(path / "hexoses.xml");

    if( result == false ) {
        ES_ERROR("unable to load structures");
    }

    // ---------------------------------
    if( WidgetUI.pentosesTab->layout() != NULL ) {
        delete WidgetUI.pentosesTab->layout();
    }
    QGridLayout* p_gl = new QGridLayout(WidgetUI.pentosesTab);
    p_gl->setSpacing(2);
    p_gl->setContentsMargins(0,2,0,2);
    WidgetUI.pentosesTab->setLayout(p_gl);

    // add items
    int maxrow = 0;
    for(int i=0; i < PentoseFragments.GetNumberOfFragments(); i++) {
        QPushButton* p_pb = new QPushButton(WidgetUI.pentosesTab);
        p_pb->setProperty("pentose",i);
        p_pb->setCheckable(true);
        p_pb->setFocusPolicy(Qt::NoFocus);
        QIcon ico = PentoseFragments.GetFragment(i)->GetConnectorIcon();
        QSize size = ico.actualSize(QSize(128,128));
        p_pb->setIconSize(size+QSize(8,8));
        p_pb->setIcon(ico);
        ActionGroup->addButton(p_pb);

        int row = i / 4;
        if( row > maxrow ) maxrow  = row;
        int col = i % 4;
        p_gl->addWidget(p_pb,row,col,1,1);
    }
    QSpacerItem* p_sp = new QSpacerItem(20,5,QSizePolicy::Minimum, QSizePolicy::Expanding);
    p_gl->addItem(p_sp,maxrow+1,0,1,4);

    // ---------------------------------
    if( WidgetUI.hexosesTab->layout() != NULL ) {
        delete WidgetUI.hexosesTab->layout();
    }
    p_gl = new QGridLayout(WidgetUI.hexosesTab);
    p_gl->setSpacing(2);
    p_gl->setContentsMargins(0,2,0,2);
    WidgetUI.hexosesTab->setLayout(p_gl);

    maxrow = 0;
    for(int i=0; i < HexoseFragments.GetNumberOfFragments(); i++) {
        QPushButton* p_pb = new QPushButton(WidgetUI.hexosesTab);
        p_pb->setProperty("hexose",i);
        p_pb->setCheckable(true);
        p_pb->setFocusPolicy(Qt::NoFocus);
        QIcon ico = HexoseFragments.GetFragment(i)->GetConnectorIcon();
        QSize size = ico.actualSize(QSize(128,128));
        p_pb->setIconSize(size+QSize(8,8));
        p_pb->setIcon(ico);
        ActionGroup->addButton(p_pb);

        int row = i / 4 + 1;
        if( row > maxrow ) maxrow  = row;
        int col = i % 4;
        p_gl->addWidget(p_pb,row,col,1,1);
    }
    p_sp = new QSpacerItem(20,5,QSizePolicy::Minimum, QSizePolicy::Expanding);
    p_gl->addItem(p_sp,maxrow+1,0,1,4);

    return(result);
}

//------------------------------------------------------------------------------

CFragment* CStructurePaletteWorkPanel::GetActiveFragment(void)
{
    QAbstractButton* p_but = ActionGroup->checkedButton();
    if( p_but == NULL ) return(NULL);

    if( p_but->property("pentose").isValid() ) {
        int id = p_but->property("pentose").toInt();
        return( PentoseFragments.GetFragment(id));
    }

    if( p_but->property("hexose").isValid() ) {
        int id = p_but->property("hexose").toInt();
        return( HexoseFragments.GetFragment(id));
    }

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStructurePaletteWorkPanel::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;

    setEnabled( ! GetProject()->GetHistory()->IsLocked(EHCL_TOPOLOGY));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

