// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <ErrorSystem.hpp>
#include <ContainerModel.hpp>
#include <PropertyAtomList.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <ProObjectDesigner.hpp>
#include <CategoryUUID.hpp>
#include <Atom.hpp>
#include <SelectionRequest.hpp>
#include <AtomListSelection.hpp>
#include <XMLElement.hpp>

#include <PRDesignerAtoms.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPRDesignerAtoms::CPRDesignerAtoms(QWidget* p_owner,CPropertyAtomList* p_list,
                                       CProObjectDesigner* p_designer)
    : QWidget(p_owner)
{
    Atoms = p_list;
    if( Atoms == NULL ){
        INVALID_ARGUMENT("p_list is NULL");
    }
    Designer = p_designer;

    // set layout --------------------------------
    if( p_owner->layout() ) delete p_owner->layout();
    QVBoxLayout* p_layout = new QVBoxLayout();
    p_layout->setContentsMargins(0,0,0,0);
    p_layout->addWidget(this);
    p_owner->setLayout(p_layout);

    // load graphics -----------------------------
    WidgetUI.setupUi(this);

    // init model --------------------------------
    ObjectsModel = Atoms->GetContainerModel(this);
    WidgetUI.atomsTV->setModel(ObjectsModel);

    if( ObjectsModel != NULL ){
        for(int i=0; i < ObjectsModel->columnCount(); i++){
            WidgetUI.atomsTV->resizeColumnToContents(i);
        }
        // connect signals ---------------------------
        connect(ObjectsModel,SIGNAL(modelReset()),
                this,SLOT(AtomsTVReset()));
    }

    // update list status
    AtomsTVReset();

    // sel request
    SelRequest = new CSelectionRequest(this,tr("atom list"));

    //------------------
    connect(WidgetUI.atomsTV,SIGNAL(clicked(const QModelIndex&)),
            this,SLOT(AtomsTVClicked(const QModelIndex&)));
    //------------------
    connect(WidgetUI.atomsTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(AtomsTVDblClicked(const QModelIndex&)));
    //------------------
    connect(WidgetUI.removeAtomPB,SIGNAL(clicked(bool)),
            this,SLOT(RemoveAtom(void)));
    //------------------
    connect(WidgetUI.removeAllAtomsPB,SIGNAL(clicked(bool)),
            this,SLOT(RemoveAllAtoms(void)));
    //------------------
    connect(WidgetUI.selectAtomsPB,SIGNAL(clicked(bool)),
            this,SLOT(SelectAtoms(void)));
    //------------------
    connect(WidgetUI.endSelectionPB,SIGNAL(clicked(bool)),
            this,SLOT(EndSelection(void)));
    //------------------
    connect(WidgetUI.addSelectedAtomsPB,SIGNAL(clicked(bool)),
            this,SLOT(AddSelectedAtoms(void)));   
    //------------------
    connect(WidgetUI.removeSelectedAtomsPB,SIGNAL(clicked(bool)),
            this,SLOT(RemoveSelectedAtoms(void)));
    //------------------
    connect(WidgetUI.showAsPointPB,SIGNAL(clicked(bool)),
            this,SLOT(ShowAsPoint(void)));
    //------------------
    connect(Atoms->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));
    //------------------
    connect(Atoms->GetProject()->GetSelection(),SIGNAL(OnSelectionChanged(void)),
            this,SLOT(SelectionChanged(void)));

    // set initial values ------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    // do not init values here, the owner of this componet is
    // responsible to call InitValues explicitly
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPRDesignerAtoms::InitValues(void)
{
    // nothing to do here
}

//------------------------------------------------------------------------------

void CPRDesignerAtoms::LoadDesignerSetup(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }

    QByteArray view_state;
    p_ele->GetAttribute("objects_header_state",view_state);
    QHeaderView* p_header = WidgetUI.atomsTV->header();
    p_header->restoreState(view_state);
}

//------------------------------------------------------------------------------

void CPRDesignerAtoms::SaveDesignerSetup(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }

    QHeaderView* p_header = WidgetUI.atomsTV->header();
    p_ele->SetAttribute("objects_header_state",p_header->saveState());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPRDesignerAtoms::AtomsTVReset(void)
{
    WidgetUI.removeAtomPB->setEnabled(false);

    int num = 0;
    if( ObjectsModel != NULL ) {
        num = ObjectsModel->rowCount();
    }
    WidgetUI.removeAllAtomsPB->setEnabled(num > 0);
    WidgetUI.showAsPointPB->setEnabled(num > 0);

    QString text("%1");
    text = text.arg(num);
    WidgetUI.numOfAtomsLE->setText(text);
}

//------------------------------------------------------------------------------

void CPRDesignerAtoms::AtomsTVClicked(const QModelIndex& index)
{
    WidgetUI.removeAtomPB->setEnabled(true);
}

//------------------------------------------------------------------------------

void CPRDesignerAtoms::AtomsTVDblClicked(const QModelIndex& index)
{
    CProObject* p_obj = dynamic_cast<CProObject*>(ObjectsModel->GetItem(index));
    if( p_obj == NULL ) return;
    p_obj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CPRDesignerAtoms::RemoveAtom(void)
{
    CAtom* p_at = dynamic_cast<CAtom*>(ObjectsModel->GetItem(WidgetUI.atomsTV->currentIndex()));
    if( p_at == NULL ) return;
    Atoms->RemoveAtomWH(p_at);
}

//------------------------------------------------------------------------------

void CPRDesignerAtoms::RemoveAllAtoms(void)
{
    Atoms->RemoveAllAtomsWH();    
}

//------------------------------------------------------------------------------

void CPRDesignerAtoms::SelectAtoms(void)
{
    SelRequest->SetRequest(Atoms->GetProject()->GetSelection(),&SH_AtomList,"(property atom list)");
}

//------------------------------------------------------------------------------

void CPRDesignerAtoms::EndSelection(void)
{
    Atoms->GetProject()->GetSelection()->SetRequest(NULL);
}

//------------------------------------------------------------------------------

void CPRDesignerAtoms::AddSelectedAtoms(void)
{
    Atoms->AddSelectedAtomsWH();
    Atoms->GetProject()->GetSelection()->ResetSelection();
}

//------------------------------------------------------------------------------

void CPRDesignerAtoms::RemoveSelectedAtoms(void)
{
    Atoms->RemoveSelectedAtomsWH();
    if( Atoms->GetProject() ){
        Atoms->GetProject()->GetSelection()->ResetSelection();
    }
}

//------------------------------------------------------------------------------

void CPRDesignerAtoms::ShowAsPoint(void)
{
    Atoms->ShowAsPointWH();
}

//------------------------------------------------------------------------------

void CPRDesignerAtoms::SelectionChanged(void)
{
    bool set = Atoms->GetProject()->GetSelection()->NumOfSelectedObjects(AtomID);
    WidgetUI.addSelectedAtomsPB->setEnabled(set);
    WidgetUI.removeSelectedAtomsPB->setEnabled(set);
}

//------------------------------------------------------------------------------

void CPRDesignerAtoms::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    parentWidget()->setEnabled(! Atoms->IsHistoryLocked(EHCL_PROPERTY));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

