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

#include <QtGui>

#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>

#include <ErrorSystem.hpp>

#include <Structure.hpp>
#include <Residue.hpp>
#include <Project.hpp>
#include <ResidueList.hpp>
#include <ContainerModel.hpp>
#include <SelectionRequest.hpp>
#include <SelectionList.hpp>
#include <ResidueSelection.hpp>
#include <AtomSelection.hpp>
#include <Atom.hpp>
#include <AtomList.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>
#include <GraphicsObjectList.hpp>
#include <GraphicsProfileList.hpp>
#include <GraphicsObject.hpp>
#include <GraphicsView.hpp>

#include "BioToolsModule.hpp"
#include "ResidueWorkPanel.hpp"
#include "ResidueWorkPanel.moc"

//------------------------------------------------------------------------------

using namespace std;

//------------------------------------------------------------------------------

#if defined _WIN32 || defined __CYGWIN__
#undef AddAtom
#endif

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* ResidueWorkPanelCB(void* p_data);

CExtUUID        ResidueWorkPanelID(
                    "{RESIDUE_WORK_PANEL:983992af-e31d-417a-bc21-0b35a32d855b}",
                    "Residue Editor");

CPluginObject   ResidueWorkPanelObject(&BioToolsPlugin,
                    ResidueWorkPanelID,WORK_PANEL_CAT,
                    ":/images/BioTools/ResidueEditor.svg",
                    ResidueWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* ResidueWorkPanelCB(void* p_data)
{
    CProObject* p_obj = static_cast<CProObject*>(p_data);
    if( p_obj == NULL ){
        ES_ERROR("p_obj == NULL");
        return(NULL);
    }
    CResidue* p_res = dynamic_cast<CResidue*>(p_obj);
    CProject* p_pro;
    if( p_res ){
        p_pro = p_res->GetProject();
    } else {
        p_pro = dynamic_cast<CProject*>(p_obj);
    }
    if( p_pro == NULL ){
        ES_ERROR("p_pro == NULL");
        return(NULL);
    }

    QObject* p_object = new CResidueWorkPanel(p_pro,p_res);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CResidueWorkPanel::CResidueWorkPanel(CProject* p_pro,CResidue* p_res)
    : CWorkPanel(&ResidueWorkPanelObject,p_pro,EWPR_TOOL)
{
    WidgetUI.setupUi(this);
    AtomsModel = NULL;

    // extra setup -------------------------------
    WidgetUI.residueW->setProject(GetProject());
    WidgetUI.residueW->setObjectBaseMIMEType("residue.indexes");
    WidgetUI.residueW->setResidueMode(true);
    WidgetUI.residueW->setSelectionHandler(&SH_Residue);
    // -----------------
    WidgetUI.headObjectW->setProject(GetProject());
    WidgetUI.headObjectW->setObjectBaseMIMEType("atom.indexes");
    WidgetUI.headObjectW->setSelectionHandler(&SH_Atom);
    // -----------------
    WidgetUI.tailObjectW->setProject(GetProject());
    WidgetUI.tailObjectW->setObjectBaseMIMEType("atom.indexes");
    WidgetUI.tailObjectW->setSelectionHandler(&SH_Atom);

    // edit table header
    QStringList header_captions;
    QString     charge_column;
    charge_column = tr("Charge") + " [" + PQ_CHARGE->GetUnitName() + "]";
    header_captions << tr("Order") << tr("Local index") << tr("Name") << tr("Type") << charge_column;
    WidgetUI.editTW->setColumnCount(5);
    WidgetUI.editTW->setHorizontalHeaderLabels(header_captions);

    for(int i=0; i < 5; i++){
        WidgetUI.editTW->resizeColumnToContents(i);
    }

    UpdatingItems = false;

    WidgetUI.atomsTV->setDragDropMode(QAbstractItemView::DragOnly);
    WidgetUI.atomsTV->setDragEnabled(true);

    // local events ------------------------------
    connect(WidgetUI.residueW,SIGNAL(OnObjectChanged()),
            this,SLOT(ResidueChanged()));
    connect(WidgetUI.atomsTV, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(AtomsTVDblClicked(const QModelIndex&)));
    connect(WidgetUI.editTW, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(EditTWChanged(QTableWidgetItem*)));
    connect(WidgetUI.editTW, SIGNAL(itemSelectionChanged(void)),
            this, SLOT(ItemSelectionChanged(void)));

    // order -------------------------------------
    connect(WidgetUI.locIndxByOrderPB,SIGNAL(clicked(bool)),
            this,SLOT(LocIndxByOrder()));
    connect(WidgetUI.locIndxByMoleculesPB,SIGNAL(clicked(bool)),
            this,SLOT(LocIndxByMolecules()));
    connect(WidgetUI.aggregateHydrogensPB,SIGNAL(clicked(bool)),
            this,SLOT(AggregateHydrogens()));
    connect(WidgetUI.aggregateTerminalsPB,SIGNAL(clicked(bool)),
            this,SLOT(AggregateTerminals()));
    connect(WidgetUI.reorderByLocIndxPB,SIGNAL(clicked(bool)),
            this,SLOT(SortAtoms()));
    connect(WidgetUI.reverseLocIndxPB,SIGNAL(clicked(bool)),
            this,SLOT(ReverseOrder()));

    // builds
    connect(WidgetUI.addHydrogensPB,SIGNAL(clicked(bool)),
            this,SLOT(AddHydrogens()));
    connect(WidgetUI.removeHydrogensPB,SIGNAL(clicked(bool)),
            this,SLOT(RemoveHydrogens()));
    connect(WidgetUI.newResiduePB,SIGNAL(clicked(bool)),
            this,SLOT(NewResidue()));
    connect(WidgetUI.namesByLocIndxPB,SIGNAL(clicked(bool)),
            this,SLOT(NamesByLocIndx()));
    connect(WidgetUI.namesByHeavyAtomsPB,SIGNAL(clicked(bool)),
            this,SLOT(NamesByHeavyAtoms()));

    // show
    connect(WidgetUI.showOthersPB,SIGNAL(clicked(bool)),
            this,SLOT(ShowOthers()));
    connect(WidgetUI.hideOthersPB,SIGNAL(clicked(bool)),
            this,SLOT(HideOthers()));
    connect(WidgetUI.highlightAtomsCB,SIGNAL(clicked(bool)),
            this,SLOT(UpdateHighlightedAtoms()));
    connect(WidgetUI.showLabelsPB,SIGNAL(clicked(bool)),
            this,SLOT(ShowLabels()));

    // nicemole
    connect(WidgetUI.detectHTPB,SIGNAL(clicked(bool)),
            this,SLOT(DetectHeadTail()));
    connect(WidgetUI.headObjectW,SIGNAL(OnObjectToBeChanged(CProObject*)),
            this,SLOT(HeadAtomAboutToBeChanged(CProObject*)));
    connect(WidgetUI.tailObjectW,SIGNAL(OnObjectToBeChanged(CProObject*)),
            this,SLOT(TailAtomAboutToBeChanged(CProObject*)));

     connect(WidgetUI.rearrangeAtomsAndNamesPB,SIGNAL(clicked(bool)),
            this,SLOT(RearrangeAtomsAndNames()));
     connect(WidgetUI.rearrangeAtomsPB,SIGNAL(clicked(bool)),
            this,SLOT(RearrangeAtoms()));

    // units ---------------------------------------
    connect(PQ_CHARGE,SIGNAL(OnUnitChanged()),
            this,SLOT(UpdateEditTable()));
    if( GetProject() ){
        connect(GetProject()->GetHistory(), SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
                this, SLOT(ProjectLockChanged(EHistoryChangeMessage)));
    }

    // eventually attach residue
    Residue = NULL;
    WidgetUI.residueW->setObject(p_res);
    HideOthersActive = false;
    ShowSelectedActive = false;

    // load work panel setup
    LoadWorkPanelSetup();
    ProjectLockChanged(EHCM_LOCK_LEVEL);
}

//------------------------------------------------------------------------------

CResidueWorkPanel::~CResidueWorkPanel(void)
{
    // FIXME: might it crash here if the residue is deleted?
    WidgetUI.highlightAtomsCB->setChecked(false);
    if( Residue ){
        if( ShowSelectedActive ) UpdateHighlightedAtoms();
        if( HideOthersActive ) ShowOthers();
    }

    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CResidueWorkPanel::ResidueChanged(void)
{
    CResidue* p_res = static_cast<CResidue*>(WidgetUI.residueW->getObject());
    if( p_res ){
        ResidueUnregistered();
        if( p_res != NULL ) ResidueRegistered(p_res);
    } else {
        ResidueUnregistered();
    }
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::ResidueRegistered(CResidue* p_res)
{
    Residue = p_res;
    // connect signals
    connect(Residue,SIGNAL(OnAtomListChanged(void)),
            this,SLOT(UpdateEditTable()));

    // create residue model
    AtomsModel = Residue->GetContainerModel(this);
    WidgetUI.atomsTV->setModel(AtomsModel);

    if( AtomsModel != NULL ){
        for(int i=0; i < AtomsModel->columnCount(); i++){
            WidgetUI.atomsTV->resizeColumnToContents(i);
        }
    }

    // enable edit tools
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    WidgetUI.newResiduePB->setEnabled(false);

    UpdateEditTable();
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::ResidueUnregistered(void)
{
    WidgetUI.tailObjectW->setObject(NULL);
    WidgetUI.headObjectW->setObject(NULL);

    // unregister external events
    if( Residue != NULL ){
        Residue->disconnect(this);
        Residue->GetProject()->GetHistory()->disconnect(this);
        Residue = NULL;
    }

    // clean visualization
    WidgetUI.atomsTV->setModel(NULL);
    delete AtomsModel;
    AtomsModel = NULL;

    WidgetUI.newResiduePB->setEnabled(false);

    WidgetUI.editTW->clearContents();
    WidgetUI.editTW->setRowCount(0);
    WidgetUI.numOfAtomsLE->setText("");

    // disable edit tools
    ProjectLockChanged(EHCM_LOCK_LEVEL);
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    if( GetProject() == NULL ) return;

    bool rset = true;
    rset &= ! GetProject()->IsHistoryLocked(EHCL_TOPOLOGY);
    rset &= Residue != NULL;

    WidgetUI.basicTab->setEnabled(rset);
    WidgetUI.niceResTab->setEnabled(rset);

    bool gset = true;
    gset &= ! GetProject()->IsHistoryLocked(EHCL_GRAPHICS);
    gset &= Residue != NULL;
    WidgetUI.showTab->setEnabled(gset);
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::AtomsTVDblClicked(const QModelIndex& index)
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

void CResidueWorkPanel::UpdateEditTable(void)
{
    // SetLocIndexWH, etc in EditTWChanged emit signals that UpdateEditTable
    if( UpdatingItems ) return;
    UpdatingItems = true;

    WidgetUI.editTW->setRowCount(Residue->GetNumberOfAtoms());

    Qt::ItemFlags ro_flags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    Qt::ItemFlags no_flags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);

    int natoms = 0;
    int id = 0;
    foreach(CAtom* p_atom,Residue->GetAtoms()){
        QTableWidgetItem* p_item;
        int atom_idx = p_atom->GetIndex();

        natoms++;

        p_item = new QTableWidgetItem(QString().setNum(natoms),atom_idx);
        p_item->setFlags(ro_flags);
        WidgetUI.editTW->setItem(id, 0, p_item);

        p_item = new QTableWidgetItem(QString().setNum(p_atom->GetLocIndex()),atom_idx);
        p_item->setFlags(no_flags);
        WidgetUI.editTW->setItem(id, 1, p_item);

        p_item = new QTableWidgetItem(p_atom->GetName(),atom_idx);
        p_item->setFlags(no_flags);
        WidgetUI.editTW->setItem(id, 2, p_item);

        p_item = new QTableWidgetItem(p_atom->GetType(),atom_idx);
        p_item->setFlags(no_flags);
        WidgetUI.editTW->setItem(id, 3, p_item);

        p_item = new QTableWidgetItem(PQ_CHARGE->GetRealValueText(p_atom->GetCharge()),atom_idx);
        p_item->setFlags(no_flags);
        WidgetUI.editTW->setItem(id, 4, p_item);

        id++;
    }

    WidgetUI.numOfAtomsLE->setText(QString().setNum(natoms));

    UpdatingItems = false;
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::EditTWChanged(QTableWidgetItem* p_item)
{
    if( UpdatingItems ) return;
    if( Residue == NULL ) return;
    if( p_item == NULL ) return;

    // get atom
    CAtom* p_atom = Residue->FindAtomByIndex(p_item->type());
    if( p_atom == NULL ) return;

    UpdatingItems = true;

    if( p_item->column() == 1 ) {
        QString text = p_item->text();
        bool ok = false;
        int value = text.toInt(&ok);
        if( ok ){
            p_atom->SetLocIndexWH(value);
        }
        // change might be prohibited
        p_item->setText(QString().setNum(p_atom->GetLocIndex()));
    }

    if( p_item->column() == 2 ) {
        QString text = p_item->text();
        p_atom->SetNameWH(text);
        // change might be prohibited
        p_item->setText(p_atom->GetName());
    }

    if( p_item->column() == 3 ) {
        QString text = p_item->text();
        p_atom->SetTypeWH(text);
        // change might be prohibited
        p_item->setText(p_atom->GetType());
    }

    if( p_item->column() == 4 ) {
        QString text = p_item->text();
        bool ok = false;
        double value = text.toDouble(&ok);
        if( ok ){
            p_atom->SetChargeWH(value);
        }
        // change might be prohibited
        p_item->setText(PQ_CHARGE->GetRealValueText(p_atom->GetCharge()));
    }

    UpdatingItems = false;
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::ItemSelectionChanged(void)
{
    if( ! Residue ) return;

    QSet<CAtom*>    sel_atoms;

    // extract unique atoms
    foreach(QTableWidgetItem* p_item, WidgetUI.editTW->selectedItems()) {
        CAtom* p_atom = Residue->FindAtomByIndex(p_item->type());
        if( p_atom == NULL ) continue;
        WidgetUI.headObjectW->selectObjectManually(p_atom);
        WidgetUI.tailObjectW->selectObjectManually(p_atom);
        sel_atoms.insert(p_atom);
    }

    if( sel_atoms.count() > 0 ){
        WidgetUI.newResiduePB->setEnabled(true);
    }

    // ------------------------
    UpdateHighlightedAtoms();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CResidueWorkPanel::LocIndxByOrder(void)
{
    Residue->LocalIndexByOrderWH();
    WidgetUI.editTW->clearSelection();
    if( WidgetUI.autoReorderCB->isChecked() ){
        SortAtoms();
    }
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::LocIndxByMolecules(void)
{
    Residue->LocalIndexByMoleculesWH();
    WidgetUI.editTW->clearSelection();
    if( WidgetUI.autoReorderCB->isChecked() ){
        SortAtoms();
    }
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::AggregateHydrogens(void)
{
    Residue->AggregateHydrogensWH();
    WidgetUI.editTW->clearSelection();
    if( WidgetUI.autoReorderCB->isChecked() ){
        SortAtoms();
    }
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::AggregateTerminals(void)
{
    Residue->AggregateTerminalsWH();
    WidgetUI.editTW->clearSelection();
    if( WidgetUI.autoReorderCB->isChecked() ){
        SortAtoms();
    }
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::ReverseOrder(void)
{
    Residue->ReverseLocalIndexWH();
    WidgetUI.editTW->clearSelection();
    if( WidgetUI.autoReorderCB->isChecked() ){
        SortAtoms();
    }
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::SortAtoms(void)
{
    Residue->SortAtoms();

    if( WidgetUI.updateSerByLocIndexCB->isChecked() ){
        Residue->GetStructure()->GetAtoms()->SerialIndexBySeqAndLocIndexesWH();
    }

    WidgetUI.editTW->clearSelection();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CResidueWorkPanel::AddHydrogens(void)
{
    Residue->AddHydrogensWH(false);
    WidgetUI.editTW->clearSelection();
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::RemoveHydrogens(void)
{
    Residue->RemoveHydrogensWH();
    WidgetUI.editTW->clearSelection();
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::NewResidue(void)
{
    QSet<CAtom*>    sel_atoms;

    // extract unique atoms
    foreach(QTableWidgetItem* p_item, WidgetUI.editTW->selectedItems()) {
        CAtom* p_atom = Residue->FindAtomByIndex(p_item->type());
        if( p_atom == NULL ) continue;
        if( WidgetUI.highlightAtomsCB->isChecked() ){
            p_atom->SetFlag(EPOF_SELECTED,false);
        }
        sel_atoms.insert(p_atom);
    }

    // clear selection
    WidgetUI.editTW->clearSelection();

    if( sel_atoms.count() == 0 ) return;

    CHistoryNode* p_history = Residue->BeginChangeWH(EHCL_TOPOLOGY,"new residue");
    if( p_history == NULL ) return;

    // create new residue
    CResidue* p_res = Residue->GetResidues()->CreateResidue("UNK","",-1,p_history);

    // add selected atoms to is
    foreach(CAtom* p_atom,sel_atoms){
        p_res->AddAtom(p_atom,p_history);
    }

    Residue->EndChangeWH();

    // open object designer
    p_res->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::NamesByLocIndx(void)
{
    Residue->NamesByLocalIndexWH();
    WidgetUI.editTW->clearSelection();
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::NamesByHeavyAtoms(void)
{
    Residue->NamesByHeavyAtomsWH();
    WidgetUI.editTW->clearSelection();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CResidueWorkPanel::ShowOthers(void)
{
    CProject* p_project = Residue->GetProject();

    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,"show other residues");
    if( p_history == NULL ) return;

    foreach(QObject* p_qobj, Residue->GetStructure()->GetResidues()->children()) {
        CResidue* p_res = static_cast<CResidue*>(p_qobj);
        CProObjectFlags flags = p_res->GetFlags();
        SET_FLAG(flags,EPOF_VISIBLE,true);
        p_res->SetFlags(flags,p_history);
    }

    p_project->EndChangeWH();

    if( WidgetUI.autoFitSceneCB->isChecked() ){
        p_project->GetGraphics()->GetPrimaryView()->FitScene(true);
    }

    HideOthersActive = false;
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::HideOthers(void)
{
    CProject* p_project = Residue->GetProject();

    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,"hide other residues");
    if( p_history == NULL ) return;

    foreach(QObject* p_qobj, Residue->GetStructure()->GetResidues()->children()) {
        CResidue* p_res = static_cast<CResidue*>(p_qobj);
        CProObjectFlags flags = p_res->GetFlags();
        if( p_res == Residue ){
            SET_FLAG(flags,EPOF_VISIBLE,true);
        } else {
            SET_FLAG(flags,EPOF_VISIBLE,false);
        }
        p_res->SetFlags(flags,p_history);
    }

    p_project->EndChangeWH();

    if( WidgetUI.autoFitSceneCB->isChecked() ){
        p_project->GetGraphics()->GetPrimaryView()->FitScene(true);
    }

    HideOthersActive = true;
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::UpdateHighlightedAtoms(void)
{   
    QSet<CAtom*>    sel_atoms;

    // extract unique atoms
    foreach(QTableWidgetItem* p_item, WidgetUI.editTW->selectedItems()) {
        CAtom* p_atom = Residue->FindAtomByIndex(p_item->type());
        if( p_atom == NULL ) continue;
        sel_atoms.insert(p_atom);
    }

    // highlight atoms
    if( WidgetUI.highlightAtomsCB->isChecked() ){
        foreach(CAtom* p_atom, Residue->GetAtoms()) {
            p_atom->SetFlag(EPOF_SELECTED,sel_atoms.contains(p_atom));
        }
        ShowSelectedActive = true;
    } else {
        foreach(CAtom* p_atom, Residue->GetAtoms()) {
            p_atom->SetFlag(EPOF_SELECTED,false);
        }
        ShowSelectedActive = false;
    }

    // update scene
    Residue->GetProject()->RepaintProject();
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::ShowLabels(void)
{
    if( Residue == NULL ) return;

    CGraphicsProfile* p_profile = GetProject()->GetGraphics()->GetProfiles()->GetActiveProfile();
    if( p_profile == NULL ){
        ES_ERROR("no active graphics profile");
        return;
    }

    CHistoryNode* p_history = GetProject()->BeginChangeWH(EHCL_GRAPHICS,tr("add residue labels"));
    if( p_history == NULL ) return;

    CExtUUID objuuid;
    objuuid.LoadFromString("{ATOM_LABEL_OBJECT:9949e9ed-1c44-4bc6-b510-9f055893e2b1}");
    CGraphicsObject* p_obj =  p_profile->CreateObjectWH(objuuid);
    if( p_obj != NULL ){
        p_obj->AddObjectWH(Residue);
    }

    GetProject()->EndChangeWH();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CResidueWorkPanel::DetectHeadTail(void)
{
    WidgetUI.headObjectW->setObject(NULL);
    WidgetUI.tailObjectW->setObject(NULL);

    QList<CAtom*> connectors = Residue->GetConnectorAtoms();

    // we have two or more connectors - get first and last
    if( connectors.count() >= 2 ){
        WidgetUI.headObjectW->setObject(connectors.first());
        WidgetUI.tailObjectW->setObject(connectors.last());
        return;
    }

    bool head_set = false;
    bool tail_set = false;

    // we have only one connector
    if( connectors.count() == 1 ){
        // is it head or tail?
        int curr_index = 0;
        foreach(CAtom* p_atom, Residue->GetAtoms()) {
            if( p_atom == connectors.first() ){
                break;
            }
            curr_index++;
        }
        if( curr_index < Residue->GetNumberOfAtoms()/2 ){
            head_set = true;
            WidgetUI.headObjectW->setObject(connectors.first());
        } else {
            tail_set = true;
            WidgetUI.tailObjectW->setObject(connectors.first());
        }
    }

    if( head_set == false ){
        // get first heavy atom
        foreach(CAtom* p_atom, Residue->GetAtoms()) {
            if( p_atom->IsVirtual() == false ){
                WidgetUI.headObjectW->setObject(p_atom);
                break;
            }
        }
    }

    if( tail_set == false ){
        // get last heavy atom
        CAtom* p_last = NULL;
        foreach(CAtom* p_atom, Residue->GetAtoms()) {
            if( p_atom->IsVirtual() == false ){
                p_last = p_atom;
            }
        }
        WidgetUI.tailObjectW->setObject(p_last);
    }

}

//------------------------------------------------------------------------------

void CResidueWorkPanel::RearrangeAtomsAndNames(void)
{
    // find longest chain ---------------------------
    FindLongestChain();
    ReorderedAtoms = LongestChain;

    // and rebuild side chains ----------------------
    unsigned int rlen = ReorderedAtoms.size();
    bool changed = false;
    int iter=1;
    do{
        changed = true;
        RebuildSideChains();
        changed = rlen < ReorderedAtoms.size();
        rlen = ReorderedAtoms.size();
        iter++;
    } while(changed);

    // fix terminal atom order ---------------------
    if( WidgetUI.fixTerminalsCB->isChecked() ){
        FixTerminals();
    }

    // and finaly rebuild hydrogens -----------------
    RebuildHydrogens();

    CHistoryNode* p_history = Residue->BeginChangeWH(EHCL_TOPOLOGY,"rearrange residue");
    if( p_history == NULL ) return;

    for(unsigned int i=0; i < ReorderedAtoms.size(); i++){
        ReorderedAtoms[i]->SetLocIndex(i+1,p_history);
    }
    Residue->SortAtoms();
    if( WidgetUI.updateSerByLocIndexCB->isChecked() ){
        Residue->GetStructure()->GetAtoms()->SerialIndexBySeqAndLocIndexesWH();
    }
    Residue->NamesByHeavyAtomsWH();

    Residue->EndChangeWH();
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::RearrangeAtoms(void)
{
    // find longest chain ---------------------------
    FindLongestChain();
    ReorderedAtoms = LongestChain;

    // and rebuild side chains ----------------------
    unsigned int rlen = ReorderedAtoms.size();
    bool changed = false;
    int iter=1;
    do{
        changed = true;
        RebuildSideChains();
        changed = rlen < ReorderedAtoms.size();
        rlen = ReorderedAtoms.size();
        iter++;
    } while(changed);

    // fix terminal atom order ---------------------
    if( WidgetUI.fixTerminalsCB->isChecked() ){
        FixTerminals();
    }

    // and finaly rebuild hydrogens -----------------
    RebuildHydrogens();

    CHistoryNode* p_history = Residue->BeginChangeWH(EHCL_TOPOLOGY,"rearrange residue");
    if( p_history == NULL ) return;

    for(unsigned int i=0; i < ReorderedAtoms.size(); i++){
        ReorderedAtoms[i]->SetLocIndex(i+1,p_history);
    }
    Residue->SortAtoms();
    if( WidgetUI.updateSerByLocIndexCB->isChecked() ){
        Residue->GetStructure()->GetAtoms()->SerialIndexBySeqAndLocIndexesWH();
    }

    Residue->EndChangeWH();
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::HeadAtomAboutToBeChanged(CProObject* p_obj)
{
    if( p_obj == NULL ) return;

    bool accept = false;
    CAtom* p_atom = dynamic_cast<CAtom*>(p_obj);
    if( p_atom ){
        if( p_atom->GetResidue() == Residue ){
            accept = true;
        }
    }
    if( accept == false ){
        WidgetUI.headObjectW->rejectObjectChange();
        GetProject()->TextNotification(ETNT_ERROR,tr("the head atom is not from the edited residue"),ETNT_ERROR_DELAY);
    }
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::TailAtomAboutToBeChanged(CProObject* p_obj)
{
    if( p_obj == NULL ) return;

    bool accept = false;
    CAtom* p_atom = dynamic_cast<CAtom*>(p_obj);
    if( p_atom ){
        if( p_atom->GetResidue() == Residue ){
            accept = true;
        }
    }
    if( accept == false ){
        WidgetUI.tailObjectW->rejectObjectChange();
        GetProject()->TextNotification(ETNT_ERROR,tr("the tail atom is not from the edited residue"),ETNT_ERROR_DELAY);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CResidueWorkPanel::FindLongestChain(void)
{
    LongestChain.clear();
    ReorderedAtoms.clear();

    if( WidgetUI.headObjectW->getObject() == NULL ) {
        // try to fing longest path starting from all atoms in residue
        foreach(CAtom* p_atom, Residue->GetAtoms()) {
            if( p_atom->IsVirtual() )  continue;
            vector<CAtom*>  path;
            FindLongestChain(p_atom,path,LongestChain,true);
        }
    } else {
        vector<CAtom*>  path;
        FindLongestChain(static_cast<CAtom*>(WidgetUI.headObjectW->getObject()),path,LongestChain,true);
    }
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::FindLongestChain(CAtom* p_atom,std::vector<CAtom*> path,
                                        std::vector<CAtom*> &final_path,bool with_term)
{
    // add atom into path
    path.push_back(p_atom);

    if( with_term == true ){
        if( p_atom == static_cast<CAtom*>(WidgetUI.tailObjectW->getObject()) ){
            // update longest path
            if( path.size() > final_path.size() ) final_path = path;
            return;
        }
    }

    // discover neighbours
    foreach(CBond* p_bond, p_atom->GetBonds()) {
        CAtom* p_atom2 = p_bond->GetOppositeAtom(p_atom);
        if( p_atom2->GetResidue() != p_atom->GetResidue() ) return; // skip atoms from other residues
        if( p_atom2->IsVirtual() )  continue; // skip hydrogens

        // is it already part of path?
        bool found = false;
        for(unsigned int i=0; i < path.size(); i++){
            if( path[i] == p_atom2 ){
                found = true;
                break;
            }
        }
        if( ! found ){
            for( unsigned int i=0; i < ReorderedAtoms.size(); i++){
                if( ReorderedAtoms[i] == p_atom2 ){
                    found = true;
                    break;
                }
            }
        }
        if( found ) continue;
        FindLongestChain(p_atom2,path,final_path,with_term);
    }

    // update longest path
    if( static_cast<CAtom*>(WidgetUI.tailObjectW->getObject()) == NULL ){
        // update longest path
        if( path.size() > final_path.size() ) final_path = path;
        return;
    }
    if( with_term == false ){
        if( path.size() > final_path.size() ) final_path = path;
    }
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::RebuildSideChains(void)
{
    for(unsigned int i=0; i < ReorderedAtoms.size(); i++) {

        foreach(CBond* p_bond,ReorderedAtoms[i]->GetBonds()) {
            CAtom* p_atom = p_bond->GetOppositeAtom(ReorderedAtoms[i]);
            if( p_atom->IsVirtual() ) continue;
            bool found = false;
            for( unsigned int k=0; k < ReorderedAtoms.size(); k++){
                if( ReorderedAtoms[k] == p_atom ){
                    found = true;
                    break;
                }
            }
            if( found )  continue;   // atom is already in processed list

            // detect longest side chain
            vector<CAtom*>     path;
            FindLongestChain(p_atom,path,path,false);

            // insert it into the list
            for(unsigned int j=0; j < path.size(); j++){
                ReorderedAtoms.insert(ReorderedAtoms.begin()+i+j+1,path[j]);
            }
        }
    }
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::FixTerminals(void)
{
    vector<CAtom*> new_list;
    int num_of_fixes = 0;

    // shift single connected heavy atoms after their partners
    for(unsigned int i = 0; i < ReorderedAtoms.size(); i++) {
        int num_of_valences = 0;
        CAtom* p_master = NULL;
        foreach(CBond* p_bond,ReorderedAtoms[i]->GetBonds()) {
            CAtom* p_atom = p_bond->GetOppositeAtom(ReorderedAtoms[i]);
            if( p_atom->GetResidue() != ReorderedAtoms[i]->GetResidue() ) continue;
            p_master = p_atom;
            num_of_valences++;
        }
        if( num_of_valences == 1 ){
            // is master in the new list
            bool found = false;
            for(unsigned int j=0; j < new_list.size(); j++){
                if( new_list[j] == p_master ){
                    found = true;
                    break;
                }
            }
            if( ! found ) {
                num_of_fixes++;
                new_list.push_back(p_master);
            }
        }
        bool found = false;
        for(unsigned int j=0; j < new_list.size(); j++){
            if( new_list[j] == ReorderedAtoms[i] ){
                found = true;
                break;
            }
        }
        if( ! found ) {
            new_list.push_back(ReorderedAtoms[i]);
        }
    }

    ReorderedAtoms = new_list;
}

//------------------------------------------------------------------------------

void CResidueWorkPanel::RebuildHydrogens(void)
{
    for(unsigned int i = 0; i < ReorderedAtoms.size(); i++) {
        if( ReorderedAtoms[i]->IsVirtual() ) continue; // becuase of newly inserted hydrogens
        foreach(CBond* p_bond,ReorderedAtoms[i]->GetBonds()) {
            CAtom* p_atom = p_bond->GetOppositeAtom(ReorderedAtoms[i]);
            if( p_atom->GetResidue() != ReorderedAtoms[i]->GetResidue() ) continue;
            if( p_atom->IsVirtual() ){
                ReorderedAtoms.insert(ReorderedAtoms.begin()+i+1,p_atom);
            }
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




