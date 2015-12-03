// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <Structure.hpp>
#include <Atom.hpp>
#include <AtomList.hpp>
#include <BondList.hpp>
#include <ResidueList.hpp>
#include <Residue.hpp>
#include <TextDialog.hpp>
#include <StructureList.hpp>
#include <QMessageBox>
#include <MainWindow.hpp>
#include <XMLDocument.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ConnectStructureMenu(void)
{
    // info
    CONNECT_ACTION(ActiveStructureInfo);
    CONNECT_ACTION(AllStructuresInfo);

    // structure
    CONNECT_ACTION(NewStructure);
    CONNECT_ACTION(CreateStructureFromSelectedResidues);
    CONNECT_ACTION(MergeAllStructures);
    CONNECT_ACTION(SortStructures);
    CONNECT_ACTION(DeleteSelectedStructures);
    CONNECT_ACTION(DeleteEmptyStructures);

    // residue
    CONNECT_ACTION(NewResidue);
    CONNECT_ACTION(NewResidueFromSelectedAtoms);
    CONNECT_ACTION(SortResidues);
    CONNECT_ACTION(DeleteSelectedResidues);
    CONNECT_ACTION(DeleteEmptyResidues);

    // atom
    CONNECT_ACTION(NewAtom);
    CONNECT_ACTION(SortAtoms);
    CONNECT_ACTION(DeleteSelectedAtoms);

    // bond
    CONNECT_ACTION(NewBond);
    CONNECT_ACTION(DeleteSelectedBonds);
    CONNECT_ACTION(DeleteInvalidBonds);

    // misc
    CONNECT_ACTION(InsertSMILES);
    CONNECT_ACTION(InsertInChI);
    CONNECT_ACTION(ConvertTo2D);
    CONNECT_ACTION(OpenAsBuildProject);
    CONNECT_ACTION(DuplicateStructure);
    CONNECT_ACTION(ClearStructure);

    // hydrogens
    CONNECT_ACTION(AddHydrogens);
    CONNECT_ACTION(AddPolarHydrogens);
    CONNECT_ACTION(RemoveHydrogens);

    // single bonds
    CONNECT_ACTION(AddBonds);
    CONNECT_ACTION(RemoveBonds);

    // panels
    CONNECT_ACTION(BuildWP);
    CONNECT_ACTION(ManageStructuresWP);

    //-----------------
    connect(Project->GetStructures(),SIGNAL(OnStructureListChanged(void)),
            this, SLOT(UpdateStructureMenu(void)));
    //-----------------
    connect(HistoryList,SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(UpdateStructureMenu()));

    UpdateStructureMenu();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::UpdateStructureMenu(void)
{
    bool set1 = ! Project->GetHistory()->IsHistoryLocked(EHCL_TOPOLOGY);
    bool set2 = ! Project->GetHistory()->IsHistoryLocked(EHCL_STRUCTURES);

    // info
    actionActiveStructureInfo->setEnabled(Project->GetActiveStructure() != NULL);
    actionAllStructuresInfo->setEnabled(true);

    // structure
    actionNewStructure->setEnabled(set2);
    actionCreateStructureFromSelectedResidues->setEnabled(set1);
    actionMergeAllStructures->setEnabled(set1);
    actionDeleteSelectedStructures->setEnabled(set2);
    actionDeleteEmptyStructures->setEnabled(set2);

    // residue
    actionNewResidue->setEnabled(set1);
    actionNewResidueFromSelectedAtoms->setEnabled(set1);
    actionDeleteSelectedResidues->setEnabled(set1);
    actionDeleteEmptyResidues->setEnabled(set1);

    // atom
    actionNewAtom->setEnabled(set1);
    actionDeleteSelectedAtoms->setEnabled(set1);

    // bond
    actionNewBond->setEnabled(set1);
    actionDeleteSelectedBonds->setEnabled(set1);
    actionDeleteInvalidBonds->setEnabled(set1);

    // misc
    actionInsertSMILES->setEnabled(set1);
    actionInsertInChI->setEnabled(set1);
    actionConvertTo2D->setEnabled(set1);
    actionClearStructure->setEnabled(set1);

    // hydrogens
    actionAddHydrogens->setEnabled(set1);
    actionAddPolarHydrogens->setEnabled(set1);
    actionRemoveHydrogens->setEnabled(set1);

    // single bonds
    actionAddBonds->setEnabled(set1);
    actionRemoveBonds->setEnabled(set1);

    // panels
    actionBuildWP->setEnabled(true);
    actionManageStructuresWP->setEnabled(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ActiveStructureInfo(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CMainWindow::AllStructuresInfo(void)
{
    Project->GetStructures()->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::NewStructure(void)
{
    Project->GetStructures()->CreateStructureWH(true);
}

//------------------------------------------------------------------------------

void CMainWindow::CreateStructureFromSelectedResidues(void)
{
    Project->GetStructures()->CreateStructureFromSelectedResiduesWH();
}

//------------------------------------------------------------------------------

void CMainWindow::MergeAllStructures(void)
{
    Project->GetStructures()->MergeAllStructuresWH();
}

//------------------------------------------------------------------------------

void CMainWindow::SortStructures(void)
{
    Project->GetStructures()->SortStructures();
}

//------------------------------------------------------------------------------

void CMainWindow::DeleteSelectedStructures(void)
{
    Project->GetStructures()->DeleteSelectedStructuresWH();
}

//------------------------------------------------------------------------------

void CMainWindow::DeleteEmptyStructures(void)
{
    Project->GetStructures()->DeleteEmptyStructuresWH();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::NewResidue(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    CResidue* p_res = p_str->GetResidues()->CreateResidueWH("UNK");
    if( p_res ){
        p_res->OpenObjectDesigner();
    }
}

//------------------------------------------------------------------------------

void CMainWindow::NewResidueFromSelectedAtoms(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    CResidue* p_res = p_str->GetResidues()->CreateResidueFromSelectedAtomsWH("UNK");
    if( p_res ){
        p_res->OpenObjectDesigner();
    }
}

//------------------------------------------------------------------------------

void CMainWindow::SortResidues(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetResidues()->SortResidues();
}

//------------------------------------------------------------------------------

void CMainWindow::DeleteSelectedResidues(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetResidues()->DeleteSelectedResiduesWH();
}

//------------------------------------------------------------------------------

void CMainWindow::DeleteEmptyResidues(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetResidues()->DeleteEmptyResiduesWH();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::NewAtom(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    CAtom* p_atm = p_str->GetAtoms()->CreateAtomWH(6,CPoint());
    if( p_atm ){
        p_atm->OpenObjectDesigner();
    }
}

//------------------------------------------------------------------------------

void CMainWindow::SortAtoms(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->SortAtoms();
}

//------------------------------------------------------------------------------

void CMainWindow::DeleteSelectedAtoms(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->RemoveSelectedAtomsWH();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::NewBond(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    CBond* p_bnd = p_str->GetBonds()->CreateBondWH(NULL,NULL,BO_SINGLE);
    if( p_bnd ) {
        p_bnd->OpenObjectDesigner();
    }
}

//------------------------------------------------------------------------------

void CMainWindow::DeleteSelectedBonds(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetBonds()->RemoveSelectedBondsWH();
}

//------------------------------------------------------------------------------

void CMainWindow::DeleteInvalidBonds(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetBonds()->RemoveInvalidBondsWH();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::InsertSMILES(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;

    CTextDialog* p_dialog = new CTextDialog(tr("Insert SMILES structure"),Project);
    if( p_dialog->ShowAsDialog() == QDialog::Accepted ){
        p_str->InsertSMILESWH(p_dialog->GetText());
    }
    delete p_dialog;
}

//------------------------------------------------------------------------------

void CMainWindow::InsertInChI(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;

    CTextDialog* p_dialog = new CTextDialog(tr("Insert InChI structure"),Project);
    if( p_dialog->ShowAsDialog() == QDialog::Accepted ){
        p_str->InsertInChIWH(p_dialog->GetText());
    }
    delete p_dialog;
}

//------------------------------------------------------------------------------

void CMainWindow::ConvertTo2D(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;

    if( p_str->GetAtoms()->GetNumberOfAtoms() == 0 ){
        Project->TextNotification(ETNT_WARNING,tr("active structure does not contain any atom"),ETNT_WARNING_DELAY);
        return;
    }

    // create build project
    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString("{SKETCH_PROJECT:6b047926-9a91-4e5f-884a-500db358ddb2}");
    CProject* p_project = Projects->NewProject(mp_uuid);
    if( p_project == NULL ){
        ES_ERROR("unable to create new sketch project");
        QMessageBox::critical(NULL, tr("Sketch Structure"),
                              tr("An error occurred during project opening!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        return;
    }

    // it is tricky to put data into sketch project
    // data must be loaded before project window is shown

    CXMLDocument xml_doc;
    CXMLElement* p_ele = xml_doc.GetChildElementByPath("data",true);

    // make data template
    p_project->SaveData(p_ele);

    // inject structure data
    p_ele->SetAttribute("smiles",p_str->GetSMILES());

    // inject new data
    p_project->LoadData(p_ele);

    // open main window
    p_project->ShowProject();
}

//------------------------------------------------------------------------------

void CMainWindow::OpenAsBuildProject(void)
{
    CStructure* p_ostr = Project->GetActiveStructure();
    if( p_ostr == NULL ) return;

    if( p_ostr->GetAtoms()->GetNumberOfAtoms() == 0 ){
        Project->TextNotification(ETNT_WARNING,tr("active structure does not contain any atom"),ETNT_WARNING_DELAY);
        return;
    }

    // create build project
    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString("{BUILD_PROJECT:b64d16f0-b73f-4747-9a13-212ab9a15d38}");
    CProject* p_project = Projects->NewProject(mp_uuid);
    if( p_project == NULL ){
        ES_ERROR("unable to create new build project");
        QMessageBox::critical(NULL, tr("Build Structure"),
                              tr("An error occurred during project opening!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        return;
    }

    CXMLDocument xml_doc;
    CXMLElement* p_ele = xml_doc.CreateChildElement("structure");

    // save the structure
    p_ostr->SaveStructureData(p_ele);

    // load the structure to the new project
    CStructure* p_nstr = p_project->GetStructures()->GetActiveStructure();
    if( p_nstr == NULL ){
        // show the project and quit
        p_project->ShowProject();
        return;
    }

    // load data
    p_project->BeginLinkProcedure();
    p_nstr->LoadStructureData(p_ele);
    p_project->EndLinkProcedure();

    // show the project
    p_project->ShowProject();
}

//------------------------------------------------------------------------------

void CMainWindow::DuplicateStructure(void)
{
    CStructure* p_ostr = Project->GetActiveStructure();
    if( p_ostr == NULL ) return;

    if( p_ostr->GetAtoms()->GetNumberOfAtoms() == 0 ){
        Project->TextNotification(ETNT_WARNING,tr("template structure does not contain any atom"),ETNT_WARNING_DELAY);
        return;
    }

    Project->GetStructures()->DuplicateStructureWH(p_ostr);
}

//------------------------------------------------------------------------------

void CMainWindow::ClearStructure(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->DeleteAllContentsWH();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::AddHydrogens(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->AddHydrogensWH();
}

//------------------------------------------------------------------------------

void CMainWindow::AddPolarHydrogens(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->AddHydrogensWH(true);
}

//------------------------------------------------------------------------------

void CMainWindow::RemoveHydrogens(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->RemoveHydrogensWH();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::AddBonds(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetBonds()->AddBondsWH();
}

//------------------------------------------------------------------------------

void CMainWindow::RemoveBonds(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetBonds()->RemoveBondsWH();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::BuildWP(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{BUILD_WORK_PANEL:e5c311d6-73f7-4091-a6e3-1d73024fe27d}");
    OpenToolPanel(mp_uuid,true);
}

//------------------------------------------------------------------------------

void CMainWindow::ManageStructuresWP(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{STRUCTURE_LIST_WORK_PANEL:a48ddecd-4a95-4dfe-96e1-dd2de8a0fd5e}");
    OpenToolPanel(mp_uuid,true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




