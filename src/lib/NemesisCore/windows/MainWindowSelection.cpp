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

#include <ProjectList.hpp>
#include <Project.hpp>
#include <SelectionRequest.hpp>
#include <StructureListSelection.hpp>
#include <AtomsAndBondsSelection.hpp>
#include <AtomListSelection.hpp>
#include <BondListSelection.hpp>
#include <ResidueListSelection.hpp>
#include <GraphicsObjectListSelection.hpp>
#include <PropertyListSelection.hpp>
#include <RestraintListSelection.hpp>
#include <Structure.hpp>
#include <RegExpDialog.hpp>
#include <MaskDialog.hpp>
#include <SelBondOrderDialog.hpp>
#include <GraphicsTypeDialog.hpp>
#include <PeriodicTableWidget.hpp>
#include <MainWindow.hpp>
#include <QDialog>
#include <QMessageBox>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ConnectSelectionMenu(void)
{
    BIND_ACTION(actionSelectStructures);
    BIND_ACTION(actionSelectAtomsAndBonds);
    BIND_ACTION(actionSelectAtoms);
    BIND_ACTION(actionSelectBonds);
    BIND_ACTION(actionSelectResidues);
    BIND_ACTION(actionSelectRestraints);
    BIND_ACTION(actionSelectGraphicsObjects);
    BIND_ACTION(actionSelectProperties);

    CONNECT_ACTION(SelectNone);

    CONNECT_ACTION(InvertSelection);
    CONNECT_ACTION(ClearSelection);

    CONNECT_ACTION(SelectAtomsBySymbol);
    CONNECT_ACTION(SelectAtomsByName);
    CONNECT_ACTION(SelectAtomsByType);
    CONNECT_ACTION(SelectAtomsByMask);
    CONNECT_ACTION(SelectBondsByName);
    CONNECT_ACTION(SelectBondsByBondOrder);
    CONNECT_ACTION(SelectResiduesByName);
    CONNECT_ACTION(SelectResiduesByMask);
    CONNECT_ACTION(SelectGraphicsObjectsByType);

    BIND_ACTION(actionOnlyActiveStructureObjects);
    BIND_ACTION(actionAlreadySelectedObjectsAsSeed);
    BIND_ACTION(actionConsiderObjectsOfAllStructures);

    StrObjectSelMode = new QActionGroup(this);
    StrObjectSelMode->setExclusive(true);
    StrObjectSelMode->addAction(actionOnlyActiveStructureObjects);
    StrObjectSelMode->addAction(actionAlreadySelectedObjectsAsSeed);
    StrObjectSelMode->addAction(actionConsiderObjectsOfAllStructures);
    connect(StrObjectSelMode,SIGNAL(triggered(QAction *)),
            this,SLOT(StrObjectSelModeChanged(QAction*)));
    //-----------------
    connect(Project->GetSelection(),SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(UpdateStrObjectSelMode(void)));

    CONNECT_ACTION(CompleteAll);
    CONNECT_ACTION(CompleteActiveStructure);
    CONNECT_ACTION(CompleteMolecules);
    CONNECT_ACTION(CompleteValences);
    CONNECT_ACTION(CompleteBonds);
    CONNECT_ACTION(SelectionListWP);

    //=================
    connect(actionSelectStructures,SIGNAL(triggered(bool)),
            this,SLOT(SelectStructures(bool)));
    //-----------------
    connect(actionSelectAtoms,SIGNAL(triggered(bool)),
            this,SLOT(SelectAtoms(bool)));
    //-----------------
    connect(actionSelectBonds,SIGNAL(triggered(bool)),
            this,SLOT(SelectBonds(bool)));
    //-----------------
    connect(actionSelectAtomsAndBonds,SIGNAL(triggered(bool)),
            this,SLOT(SelectAtomsAndBonds(bool)));
    //-----------------
    connect(actionSelectResidues,SIGNAL(triggered(bool)),
            this,SLOT(SelectResidues(bool)));
    //-----------------
    connect(actionSelectRestraints,SIGNAL(triggered(bool)),
            this,SLOT(SelectRestraints(bool)));
    //-----------------
    connect(actionSelectGraphicsObjects,SIGNAL(triggered(bool)),
            this,SLOT(SelectGraphicsObjects(bool)));
    //-----------------
    connect(actionSelectProperties,SIGNAL(triggered(bool)),
            this,SLOT(SelectProperties(bool)));

    //=================
    connect(actionSelectionListWP,SIGNAL(triggered(bool)),
            this,SLOT(SelectionListWP()));

    // initial setup
    UpdateSelectionMenu();
    UpdateStrObjectSelMode();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::UpdateSelectionMenu(void)
{
    actionSelectStructures->setEnabled(true);
    actionSelectStructures->setChecked(false);
    actionSelectAtomsAndBonds->setEnabled(true);
    actionSelectAtomsAndBonds->setChecked(false);
    actionSelectAtoms->setEnabled(true);
    actionSelectAtoms->setChecked(false);
    actionSelectBonds->setEnabled(true);
    actionSelectBonds->setChecked(false);
    actionSelectResidues->setEnabled(true);
    actionSelectResidues->setChecked(false);
    actionSelectRestraints->setEnabled(true);
    actionSelectRestraints->setChecked(false);
    actionSelectGraphicsObjects->setEnabled(true);
    actionSelectGraphicsObjects->setChecked(false);
    actionSelectProperties->setEnabled(true);
    actionSelectProperties->setChecked(false);

    actionInvertSelection->setEnabled(false);
    actionClearSelection->setEnabled(false);
    actionSelectNone->setEnabled(false);

    actionSelectAtomsBySymbol->setEnabled(false);
    actionSelectAtomsByName->setEnabled(false);
    actionSelectAtomsByType->setEnabled(false);
    actionSelectAtomsByMask->setEnabled(false);
    actionSelectBondsByName->setEnabled(false);
    actionSelectBondsByBondOrder->setEnabled(false);
    actionSelectResiduesByName->setEnabled(false);
    actionSelectResiduesByMask->setEnabled(false);
    actionSelectGraphicsObjectsByType->setEnabled(false);

    actionCompleteAll->setEnabled(false);
    actionCompleteActiveStructure->setEnabled(false);
    actionCompleteMolecules->setEnabled(false);
    actionCompleteValences->setEnabled(false);
    actionCompleteBonds->setEnabled(false);
}

//------------------------------------------------------------------------------

void CMainWindow::UpdateStrObjectSelMode(void)
{
    switch( Project->GetSelection()->GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:
            actionOnlyActiveStructureObjects->setChecked(true);
        break;
        case ESOSM_STRUCTURES_FROM_OBJECTS:
            actionAlreadySelectedObjectsAsSeed->setChecked(true);
        break;
        case ESOSM_CONSIDER_ALL_STRUCTURES:
            actionConsiderObjectsOfAllStructures->setChecked(true);
        break;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::SelectionListWP(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{SELECTION_WORK_PANEL:afec4a64-48c0-4b7e-84d0-e6af678fb3df}");
    OpenToolPanel(mp_uuid,true);
}

//------------------------------------------------------------------------------

void CMainWindow::SelectStructures(bool checked)
{
    if( checked == false ) {
        SelectNone();
        return;
    }

    if( Project == NULL ) {
        SelRequest->SetRequest(NULL);
        return;
    }
    SelRequest->SetRequest(Project->GetSelection(),
                           &SH_StructureList,"(selection menu)");
    SelRequest->disconnect();
    connect(SelRequest,SIGNAL(OnRequestChanging(void)),
            this,SLOT(UpdateSelectionMenu(void)));
    connect(SelRequest,SIGNAL(OnDetached(void)),
            this,SLOT(UpdateSelectionMenu(void)));

    CSelObject selobj(Project->GetActiveStructure(),0);
    Project->GetSelection()->RegisterObject(selobj);

    Project->RepaintProject();

    actionSelectStructures->setChecked(true);
    actionSelectAtomsAndBonds->setChecked(false);
    actionSelectAtoms->setChecked(false);
    actionSelectBonds->setChecked(false);
    actionSelectResidues->setChecked(false);
    actionSelectRestraints->setChecked(false);
    actionSelectGraphicsObjects->setChecked(false);
    actionSelectProperties->setChecked(false);

    actionInvertSelection->setEnabled(false);
    actionClearSelection->setEnabled(true);

    actionSelectNone->setEnabled(true);

    actionSelectAtomsBySymbol->setEnabled(false);
    actionSelectAtomsByName->setEnabled(false);
    actionSelectAtomsByType->setEnabled(false);
    actionSelectAtomsByMask->setEnabled(false);
    actionSelectBondsByName->setEnabled(false);
    actionSelectBondsByBondOrder->setEnabled(false);
    actionSelectResiduesByName->setEnabled(false);
    actionSelectResiduesByMask->setEnabled(false);
    actionSelectGraphicsObjectsByType->setEnabled(false);

    actionCompleteAll->setEnabled(true);
    actionCompleteActiveStructure->setEnabled(false);
    actionCompleteMolecules->setEnabled(false);
    actionCompleteValences->setEnabled(false);
    actionCompleteBonds->setEnabled(false);
}

//------------------------------------------------------------------------------

void CMainWindow::SelectAtomsAndBonds(bool checked)
{
    if( checked == false ) {
        SelectNone();
        return;
    }

    if( Project == NULL ) {
        SelRequest->SetRequest(NULL);
        return;
    }
    SelRequest->SetRequest(Project->GetSelection(),
                           &SH_AtomsAndBonds,"(selection menu)");
    SelRequest->disconnect();
    connect(SelRequest,SIGNAL(OnRequestChanging(void)),
            this,SLOT(UpdateSelectionMenu(void)));
    connect(SelRequest,SIGNAL(OnDetached(void)),
            this,SLOT(UpdateSelectionMenu(void)));

    Project->RepaintProject();

    actionSelectStructures->setChecked(false);
    actionSelectAtomsAndBonds->setChecked(true);
    actionSelectAtoms->setChecked(false);
    actionSelectBonds->setChecked(false);
    actionSelectResidues->setChecked(false);
    actionSelectRestraints->setChecked(false);
    actionSelectGraphicsObjects->setChecked(false);
    actionSelectProperties->setChecked(false);

    actionInvertSelection->setEnabled(true);
    actionClearSelection->setEnabled(true);
    actionSelectNone->setEnabled(true);

    actionSelectAtomsBySymbol->setEnabled(true);
    actionSelectAtomsByName->setEnabled(true);
    actionSelectAtomsByType->setEnabled(true);
    actionSelectAtomsByMask->setEnabled(true);
    actionSelectBondsByName->setEnabled(true);
    actionSelectBondsByBondOrder->setEnabled(true);
    actionSelectResiduesByName->setEnabled(false);
    actionSelectResiduesByMask->setEnabled(false);
    actionSelectGraphicsObjectsByType->setEnabled(false);

    actionCompleteAll->setEnabled(true);
    actionCompleteActiveStructure->setEnabled(true);
    actionCompleteMolecules->setEnabled(true);
    actionCompleteValences->setEnabled(true);
    actionCompleteBonds->setEnabled(true);
}

//------------------------------------------------------------------------------

void CMainWindow::SelectAtoms(bool checked)
{
    if( checked == false ) {
        SelectNone();
        return;
    }

    if( Project == NULL ) {
        SelRequest->SetRequest(NULL);
        return;
    }
    SelRequest->SetRequest(Project->GetSelection(),
                           &SH_AtomList,"(selection menu)");
    SelRequest->disconnect();
    connect(SelRequest,SIGNAL(OnRequestChanging(void)),
            this,SLOT(UpdateSelectionMenu(void)));
    connect(SelRequest,SIGNAL(OnDetached(void)),
            this,SLOT(UpdateSelectionMenu(void)));

    Project->RepaintProject();

    actionSelectStructures->setChecked(false);
    actionSelectAtomsAndBonds->setChecked(false);
    actionSelectAtoms->setChecked(true);
    actionSelectBonds->setChecked(false);
    actionSelectResidues->setChecked(false);
    actionSelectRestraints->setChecked(false);
    actionSelectGraphicsObjects->setChecked(false);
    actionSelectProperties->setChecked(false);

    actionInvertSelection->setEnabled(true);
    actionClearSelection->setEnabled(true);
    actionSelectNone->setEnabled(true);

    actionSelectAtomsBySymbol->setEnabled(true);
    actionSelectAtomsByName->setEnabled(true);
    actionSelectAtomsByType->setEnabled(true);
    actionSelectAtomsByMask->setEnabled(true);
    actionSelectBondsByName->setEnabled(false);
    actionSelectBondsByBondOrder->setEnabled(false);
    actionSelectResiduesByName->setEnabled(false);
    actionSelectResiduesByMask->setEnabled(false);
    actionSelectGraphicsObjectsByType->setEnabled(false);

    actionCompleteAll->setEnabled(true);
    actionCompleteActiveStructure->setEnabled(true);
    actionCompleteMolecules->setEnabled(true);
    actionCompleteValences->setEnabled(false);
    actionCompleteBonds->setEnabled(false);
}

//------------------------------------------------------------------------------

void CMainWindow::SelectBonds(bool checked)
{
    if( checked == false ) {
        SelectNone();
        return;
    }

    if( Project == NULL ) {
        SelRequest->SetRequest(NULL);
        return;
    }
    SelRequest->SetRequest(Project->GetSelection(),
                           &SH_BondList,"(selection menu)");
    SelRequest->disconnect();
    connect(SelRequest,SIGNAL(OnRequestChanging(void)),
            this,SLOT(UpdateSelectionMenu(void)));
    connect(SelRequest,SIGNAL(OnDetached(void)),
            this,SLOT(UpdateSelectionMenu(void)));

    Project->RepaintProject();

    actionSelectStructures->setChecked(false);
    actionSelectAtomsAndBonds->setChecked(false);
    actionSelectAtoms->setChecked(false);
    actionSelectBonds->setChecked(true);
    actionSelectResidues->setChecked(false);
    actionSelectRestraints->setChecked(false);
    actionSelectGraphicsObjects->setChecked(false);
    actionSelectProperties->setChecked(false);

    actionInvertSelection->setEnabled(true);
    actionClearSelection->setEnabled(true);
    actionSelectNone->setEnabled(true);

    actionSelectAtomsBySymbol->setEnabled(false);
    actionSelectAtomsByName->setEnabled(false);
    actionSelectAtomsByType->setEnabled(false);
    actionSelectAtomsByMask->setEnabled(false);
    actionSelectBondsByName->setEnabled(true);
    actionSelectBondsByBondOrder->setEnabled(true);
    actionSelectResiduesByName->setEnabled(false);
    actionSelectResiduesByMask->setEnabled(false);
    actionSelectGraphicsObjectsByType->setEnabled(false);

    actionCompleteAll->setEnabled(true);
    actionCompleteActiveStructure->setEnabled(true);
    actionCompleteMolecules->setEnabled(false);
    actionCompleteValences->setEnabled(false);
    actionCompleteBonds->setEnabled(false);
}

//------------------------------------------------------------------------------

void CMainWindow::SelectResidues(bool checked)
{
    if( checked == false ) {
        SelectNone();
        return;
    }

    if( Project == NULL ) {
        SelRequest->SetRequest(NULL);
        return;
    }
    SelRequest->SetRequest(Project->GetSelection(),
                           &SH_ResidueList,"(selection menu)");
    SelRequest->disconnect();
    connect(SelRequest,SIGNAL(OnRequestChanging(void)),
            this,SLOT(UpdateSelectionMenu(void)));
    connect(SelRequest,SIGNAL(OnDetached(void)),
            this,SLOT(UpdateSelectionMenu(void)));

    Project->RepaintProject();

    actionSelectStructures->setChecked(false);
    actionSelectAtomsAndBonds->setChecked(false);
    actionSelectAtoms->setChecked(false);
    actionSelectBonds->setChecked(false);
    actionSelectResidues->setChecked(true);
    actionSelectRestraints->setChecked(false);
    actionSelectGraphicsObjects->setChecked(false);
    actionSelectProperties->setChecked(false);

    actionInvertSelection->setEnabled(true);
    actionClearSelection->setEnabled(true);
    actionSelectNone->setEnabled(true);

    actionSelectAtomsBySymbol->setEnabled(false);
    actionSelectAtomsByName->setEnabled(false);
    actionSelectAtomsByType->setEnabled(false);
    actionSelectAtomsByMask->setEnabled(false);
    actionSelectBondsByName->setEnabled(false);
    actionSelectBondsByBondOrder->setEnabled(false);
    actionSelectResiduesByName->setEnabled(true);
    actionSelectResiduesByMask->setEnabled(true);
    actionSelectGraphicsObjectsByType->setEnabled(false);

    actionCompleteAll->setEnabled(true);
    actionCompleteActiveStructure->setEnabled(true);
    actionCompleteMolecules->setEnabled(false);
    actionCompleteValences->setEnabled(false);
    actionCompleteBonds->setEnabled(false);
}

//------------------------------------------------------------------------------

void CMainWindow::SelectRestraints(bool checked)
{
    if( checked == false ) {
        SelectNone();
        return;
    }

    if( Project == NULL ) {
        SelRequest->SetRequest(NULL);
        return;
    }
    SelRequest->SetRequest(Project->GetSelection(),
                           &SH_RestraintList,"(selection menu)");
    SelRequest->disconnect();
    connect(SelRequest,SIGNAL(OnRequestChanging(void)),
            this,SLOT(UpdateSelectionMenu(void)));
    connect(SelRequest,SIGNAL(OnDetached(void)),
            this,SLOT(UpdateSelectionMenu(void)));

    Project->RepaintProject();

    actionSelectStructures->setChecked(false);
    actionSelectAtomsAndBonds->setChecked(false);
    actionSelectAtoms->setChecked(false);
    actionSelectBonds->setChecked(false);
    actionSelectResidues->setChecked(false);
    actionSelectRestraints->setChecked(true);
    actionSelectGraphicsObjects->setChecked(false);
    actionSelectProperties->setChecked(false);

    actionInvertSelection->setEnabled(true);
    actionClearSelection->setEnabled(true);
    actionSelectNone->setEnabled(true);

    actionSelectAtomsBySymbol->setEnabled(false);
    actionSelectAtomsByName->setEnabled(false);
    actionSelectAtomsByType->setEnabled(false);
    actionSelectAtomsByMask->setEnabled(false);
    actionSelectBondsByName->setEnabled(false);
    actionSelectBondsByBondOrder->setEnabled(false);
    actionSelectResiduesByName->setEnabled(false);
    actionSelectResiduesByMask->setEnabled(false);
    actionSelectGraphicsObjectsByType->setEnabled(false);

    actionCompleteAll->setEnabled(true);
    actionCompleteActiveStructure->setEnabled(true);
    actionCompleteMolecules->setEnabled(false);
    actionCompleteValences->setEnabled(false);
    actionCompleteBonds->setEnabled(false);
}

//------------------------------------------------------------------------------

void CMainWindow::SelectGraphicsObjects(bool checked)
{
    if( checked == false ) {
        SelectNone();
        return;
    }

    if( Project == NULL ) {
        SelRequest->SetRequest(NULL);
        return;
    }
    SelRequest->SetRequest(Project->GetSelection(),
                           &SH_GraphicsObjectList,"(selection menu)");
    SelRequest->disconnect();
    connect(SelRequest,SIGNAL(OnRequestChanging(void)),
            this,SLOT(UpdateSelectionMenu(void)));
    connect(SelRequest,SIGNAL(OnDetached(void)),
            this,SLOT(UpdateSelectionMenu(void)));

    Project->RepaintProject();

    actionSelectStructures->setChecked(false);
    actionSelectAtomsAndBonds->setChecked(false);
    actionSelectAtoms->setChecked(false);
    actionSelectBonds->setChecked(false);
    actionSelectResidues->setChecked(false);
    actionSelectRestraints->setChecked(false);
    actionSelectGraphicsObjects->setChecked(true);
    actionSelectProperties->setChecked(false);

    actionInvertSelection->setEnabled(true);
    actionClearSelection->setEnabled(true);
    actionSelectNone->setEnabled(true);

    actionSelectAtomsBySymbol->setEnabled(false);
    actionSelectAtomsByName->setEnabled(false);
    actionSelectAtomsByType->setEnabled(false);
    actionSelectAtomsByMask->setEnabled(false);
    actionSelectBondsByName->setEnabled(false);
    actionSelectBondsByBondOrder->setEnabled(false);
    actionSelectResiduesByName->setEnabled(false);
    actionSelectResiduesByMask->setEnabled(false);
    actionSelectGraphicsObjectsByType->setEnabled(true);

    actionCompleteAll->setEnabled(true);
    actionCompleteActiveStructure->setEnabled(false);
    actionCompleteMolecules->setEnabled(false);
    actionCompleteValences->setEnabled(false);
    actionCompleteBonds->setEnabled(false);
}

//------------------------------------------------------------------------------

void CMainWindow::SelectProperties(bool checked)
{
    if( checked == false ) {
        SelectNone();
        return;
    }

    if( Project == NULL ) {
        SelRequest->SetRequest(NULL);
        return;
    }
    SelRequest->SetRequest(Project->GetSelection(),
                           &SH_PropertyList,"(selection menu)");
    SelRequest->disconnect();
    connect(SelRequest,SIGNAL(OnRequestChanging(void)),
            this,SLOT(UpdateSelectionMenu(void)));
    connect(SelRequest,SIGNAL(OnDetached(void)),
            this,SLOT(UpdateSelectionMenu(void)));

    Project->RepaintProject();

    actionSelectStructures->setChecked(false);
    actionSelectAtomsAndBonds->setChecked(false);
    actionSelectAtoms->setChecked(false);
    actionSelectBonds->setChecked(false);
    actionSelectResidues->setChecked(false);
    actionSelectRestraints->setChecked(false);
    actionSelectGraphicsObjects->setChecked(false);
    actionSelectProperties->setChecked(true);

    actionInvertSelection->setEnabled(true);
    actionClearSelection->setEnabled(true);
    actionSelectNone->setEnabled(true);

    actionSelectAtomsBySymbol->setEnabled(false);
    actionSelectAtomsByName->setEnabled(false);
    actionSelectAtomsByType->setEnabled(false);
    actionSelectAtomsByMask->setEnabled(false);
    actionSelectBondsByName->setEnabled(false);
    actionSelectBondsByBondOrder->setEnabled(false);
    actionSelectResiduesByName->setEnabled(false);
    actionSelectResiduesByMask->setEnabled(false);
    actionSelectGraphicsObjectsByType->setEnabled(true);

    actionCompleteAll->setEnabled(true);
    actionCompleteActiveStructure->setEnabled(false);
    actionCompleteMolecules->setEnabled(false);
    actionCompleteValences->setEnabled(false);
    actionCompleteBonds->setEnabled(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::InvertSelection(void)
{
    CSelObject obj(NULL,SEL_INVERT);
    Project->GetSelection()->RegisterObject(obj);
    Project->RepaintProject();
}

//------------------------------------------------------------------------------

void CMainWindow::ClearSelection(void)
{
    Project->GetSelection()->ResetSelection();
    Project->RepaintProject();
}

//------------------------------------------------------------------------------

void CMainWindow::SelectNone(void)
{
    SelRequest->SetRequest(NULL);
    SelRequest->disconnect();
    UpdateSelectionMenu();
    Project->RepaintProject();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::SelectAtomsBySymbol(void)
{
    QDialog* p_dialog = new QDialog(this);
    CPeriodicTableWidget* p_ptb = new CPeriodicTableWidget(p_dialog);
    p_ptb->show();
    connect(p_ptb,SIGNAL(OnElementChanged(int)),
            p_dialog,SLOT(accept(void)));

    int resid = p_dialog->exec();
    if( resid == QDialog::Rejected ) {
        delete p_dialog;
        return;
    }

    int z = p_ptb->GetActiveElement();
    delete p_dialog;

    // select
    Project->GetSelection()->SelectAtomsByZ(z);
}

//------------------------------------------------------------------------------

void CMainWindow::SelectAtomsByName(void)
{
    CRegExpDialog* p_dialog = new CRegExpDialog(tr("Select atoms by name"),Project);
    if( p_dialog->ShowAsDialog() == QDialog::Accepted ){
        Project->GetSelection()->SelectAtomsByName(p_dialog->GetRegExp());
    }
    delete p_dialog;
}

//------------------------------------------------------------------------------

void CMainWindow::SelectAtomsByType(void)
{
    CRegExpDialog* p_dialog = new CRegExpDialog(tr("Select atoms by type"),Project);
    if( p_dialog->ShowAsDialog() == QDialog::Accepted ){
        Project->GetSelection()->SelectAtomsByType(p_dialog->GetRegExp());
    }
    delete p_dialog;
}

//------------------------------------------------------------------------------

void CMainWindow::SelectAtomsByMask(void)
{
    CMaskDialog* p_dialog;
    for(;;){
        p_dialog = new CMaskDialog(tr("Select atoms by mask"),Project);
        if( p_dialog->ShowAsDialog() == QDialog::Accepted ){
            if( Project->GetSelection()->SelectAtomsByMask(p_dialog->GetMask(),
                                                         p_dialog->GetMaskType()) == true ){
                break;
            }
            // error occured - inform user
            QMessageBox::critical(p_dialog,tr("Error"),tr("Entered mask is incorrect!"));
        } else {
            break;
        }
        delete p_dialog;
    }
    delete p_dialog;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::SelectBondsByName(void)
{
    CRegExpDialog* p_dialog = new CRegExpDialog(tr("Select bonds by name"),Project);
    if( p_dialog->ShowAsDialog() == QDialog::Accepted ){
        Project->GetSelection()->SelectBondsByName(p_dialog->GetRegExp());
    }
    delete p_dialog;
}

//------------------------------------------------------------------------------

void CMainWindow::SelectBondsByBondOrder(void)
{
    CSelBondOrderDialog* p_dialog = new CSelBondOrderDialog(tr("Select bonds by order"),Project);
    if( p_dialog->ShowAsDialog() == QDialog::Accepted ){
        Project->GetSelection()->SelectBondsByOrder(p_dialog->GetBondOrder(),p_dialog->GetOperator());
    }
    delete p_dialog;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::SelectResiduesByName(void)
{
    CRegExpDialog* p_dialog = new CRegExpDialog(tr("Select residues by name"),Project);
    if( p_dialog->ShowAsDialog() == QDialog::Accepted ){
        Project->GetSelection()->SelectResiduesByName(p_dialog->GetRegExp());
    }
    delete p_dialog;
}

//------------------------------------------------------------------------------

void CMainWindow::SelectResiduesByMask(void)
{
    CMaskDialog* p_dialog;
    for(;;){
        p_dialog = new CMaskDialog(tr("Select residues by mask"),Project);

        if( p_dialog->ShowAsDialog() == QDialog::Accepted ){
            if( Project->GetSelection()->SelectResiduesByMask(p_dialog->GetMask(),
                                                         p_dialog->GetMaskType()) == true ){
                break;
            }
            // error occured - inform user
            QMessageBox::critical(p_dialog,tr("Error"),tr("Entered mask is incorrect!"));
        } else {
            break;
        }
        delete p_dialog;
    }
    delete p_dialog;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::SelectGraphicsObjectsByType(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;

    CGraphicsTypeDialog* p_dialog = new CGraphicsTypeDialog(tr("Select graphics objects by type"),Project);
    if( p_dialog->ShowAsDialog() == QDialog::Accepted ){
        Project->GetSelection()->SelectGraphicsObjectsByType(p_dialog->GetType());
    }
    delete p_dialog;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::StrObjectSelModeChanged(QAction* p_action)
{
    if( p_action == actionOnlyActiveStructureObjects ){
        Project->GetSelection()->SetStrObjectSelMode(ESOSM_ACTIVE_STRUCTURE_ONLY);
    }

    if( p_action == actionAlreadySelectedObjectsAsSeed ){
        Project->GetSelection()->SetStrObjectSelMode(ESOSM_STRUCTURES_FROM_OBJECTS);
    }

    if( p_action == actionConsiderObjectsOfAllStructures ){
        Project->GetSelection()->SetStrObjectSelMode(ESOSM_CONSIDER_ALL_STRUCTURES);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::CompleteAll(void)
{
    CSelObject obj(NULL,SEL_COMPLETE_ALL);
    Project->GetSelection()->RegisterObject(obj);
    Project->RepaintProject();
}

//------------------------------------------------------------------------------

void CMainWindow::CompleteActiveStructure(void)
{
    CSelObject obj(NULL,SEL_COMPLETE_ACTIVE_STRUCTURE);
    Project->GetSelection()->RegisterObject(obj);
    Project->RepaintProject();
}

//------------------------------------------------------------------------------

void CMainWindow::CompleteMolecules(void)
{
    CSelObject obj(NULL,SEL_COMPLETE_MOLECULES);
    Project->GetSelection()->RegisterObject(obj);
    Project->RepaintProject();
}

//------------------------------------------------------------------------------

void CMainWindow::CompleteValences(void)
{
    CSelObject obj(NULL,SEL_COMPLETE_VALENCES);
    Project->GetSelection()->RegisterObject(obj);
    Project->RepaintProject();
}

//------------------------------------------------------------------------------

void CMainWindow::CompleteBonds(void)
{
    CSelObject obj(NULL,SEL_COMPLETE_BONDS);
    Project->GetSelection()->RegisterObject(obj);
    Project->RepaintProject();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


