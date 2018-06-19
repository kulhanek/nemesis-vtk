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
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <ContainerModel.hpp>
#include <PhysicalQuantity.hpp>
#include <PhysicalQuantities.hpp>
#include <StructureSelection.hpp>
#include <Structure.hpp>
#include <SelectionRequest.hpp>
#include <Atom.hpp>

#include <PODesignerGeneral.hpp>
#include <PODesignerRefBy.hpp>
#include <PRDesignerValue.hpp>
#include <PRDesignerGraphics.hpp>
#include <QMessageBox>

#include "RMSDToolsModule.hpp"
#include "MapCouple.hpp"
#include "RMSDProperty.hpp"
#include "RMSDPropertyDesigner.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* RMSDPropertyDesignerCB(void* p_data);

CExtUUID        RMSDPropertyDesignerID(
                    "{RMSD_PROPERTY_DESIGNER:08c82ce9-72b2-49bf-ad94-b9dfc4453b31}",
                    "RMSD");

CPluginObject   RMSDPropertyDesignerObject(&RMSDToolsPlugin,
                    RMSDPropertyDesignerID,DESIGNER_CAT,
                    RMSDPropertyDesignerCB);

// -----------------------------------------------------------------------------

QObject* RMSDPropertyDesignerCB(void* p_data)
{
    CRMSDProperty* p_fmo = static_cast<CRMSDProperty*>(p_data);
    QObject* p_object = new CRMSDPropertyDesigner(p_fmo);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRMSDPropertyDesigner::CRMSDPropertyDesigner(CRMSDProperty* p_fmo)
    : CProObjectDesigner(&RMSDPropertyDesignerObject, p_fmo)
{
    Property = p_fmo;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Property,this);
    Value = new CPRDesignerValue(WidgetUI.dataGB,Property,this);
    Graphics = new CPRDesignerGraphics(WidgetUI.graphicsGB,Property,this);
    RefBy = new CPODesignerRefBy(WidgetUI.refByTab,Property,this);

    // selection request
    SelRequest = new CSelectionRequest(this,"map list");

    // extra setup -------------------------------
    WidgetUI.templateLO->setProject(Property->GetProject());
    WidgetUI.templateLO->setSelectionHandler(&SH_Structure);
    // -----------------
    WidgetUI.targetLO->setProject(Property->GetProject());
    WidgetUI.targetLO->setSelectionHandler(&SH_Structure);
    // -----------------

    // units -------------------------------------
    WidgetUI.distanceTresholdSB->setPhysicalQuantity(PQ_DISTANCE);

    // default value
    WidgetUI.distanceTresholdSB->setInternalValue(0.5);

    // map model
    MapModel = Property->GetContainerModel(this);
    WidgetUI.mapTV->setModel(MapModel);

    if( MapModel != NULL ){
        connect(MapModel,SIGNAL(modelReset(void)),
                this,SLOT(MapTVReset(void)));
        for(int i=0; i < MapModel->columnCount(); i++){
            WidgetUI.mapTV->resizeColumnToContents(i);
        }
    }

    // update initial visualization
    MapTVReset();

    // connect slots -----------------------------
    // -------------
    connect(WidgetUI.allowOnlySameZCouplesCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.massWeightedCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.doNotFitCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.visibleCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    // -------------
    connect(WidgetUI.highlightedCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.templateLO,SIGNAL(OnObjectChanged()),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.templateLO,SIGNAL(OnObjectChanged()),
            this,SLOT(TemplateObjectChanged()));
    //------------------
    connect(WidgetUI.targetLO,SIGNAL(OnObjectChanged()),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.targetLO,SIGNAL(OnObjectChanged()),
            this,SLOT(TargetObjectChanged()));

    // -------------
    connect(WidgetUI.startManualMappingPB, SIGNAL(clicked(bool)),
            this, SLOT(StartManualMapping()));
    // -------------
    connect(WidgetUI.stopManualMappingPB, SIGNAL(clicked(bool)),
            this, SLOT(StopManualMapping()));
    //------------------
    connect(SelRequest,SIGNAL(OnSelectionCompleted()),
            this,SLOT(SelectionCompleted()));

    //------------------
    connect(WidgetUI.removeSelectedCouplePB,SIGNAL(clicked(bool)),
            this,SLOT(RemoveSelectedCouple()));
    //------------------
    connect(WidgetUI.removeHydrogenCouplesPB,SIGNAL(clicked(bool)),
            this,SLOT(RemoveHydrogenCouples()));
    //------------------
    connect(WidgetUI.removeAllCouplePB,SIGNAL(clicked(bool)),
            this,SLOT(RemoveAllCouples()));

    //------------------
    connect(WidgetUI.mappingBySerialIndexPB,SIGNAL(clicked(bool)),
            this,SLOT(MappingBySerIndx()));
    //------------------
    connect(WidgetUI.mappingByDistancePB,SIGNAL(clicked(bool)),
            this,SLOT(MappingByDistance()));

    //------------------
    connect(WidgetUI.alignPB,SIGNAL(clicked(bool)),
            this,SLOT(AlignTarget()));
    //------------------
    connect(WidgetUI.hideMappedAtomPB,SIGNAL(clicked(bool)),
            this,SLOT(HideAllMappedAtoms()));
    //------------------
    connect(WidgetUI.hideUnmappedAtomsPB,SIGNAL(clicked(bool)),
            this,SLOT(HideAllUnmappedAtoms()));
    //------------------
    connect(WidgetUI.showAllAtomsPB,SIGNAL(clicked(bool)),
            this,SLOT(ShowAllAtoms()));
    //------------------
    connect(WidgetUI.templateToTargetPB,SIGNAL(clicked(bool)),
            this,SLOT(MapTemplatePosToTarget()));
    //------------------
    connect(WidgetUI.targetToTemplatePB,SIGNAL(clicked(bool)),
            this,SLOT(MapTargetPosToTemplate()));
    //------------------
    connect(WidgetUI.imprintTemplateTopologyToTargetPB,SIGNAL(clicked(bool)),
            this,SLOT(ImprintTemplateTopologyToTarget()));
    //------------------
    connect(WidgetUI.imprintTargetTopologyToTemplatePB,SIGNAL(clicked(bool)),
            this,SLOT(ImprintTargetTopologyToTemplate()));
    //------------------
    connect(Property->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // -----------------
    connect(WidgetUI.buttonBox1, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));


    // check for initial locks
    ProjectLockChanged(EHCM_LOCK_LEVEL);

    // init all values ---------------------------
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRMSDPropertyDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    Value->InitValues();
    Graphics->InitValues();
    RefBy->InitValues();
    InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Property->BeginChangeWH(EHCL_COMPOSITE,Property->GetType().GetName()) == false ) return;

    Changing = true;
        General->ApplyValues();
        ApplyValues();
    Changing = false;

    Property->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Reset) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Reset)  ) {
        InitAllValues();
        return;
    }

    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Apply) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Apply)  ) {
        ApplyAllValues();
        return;
    }

    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Close) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Close)  ) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRMSDPropertyDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    //objects
    WidgetUI.templateLO->setObject(Property->GetTemplateStructure());
    WidgetUI.targetLO->setObject(Property->GetTargetStructure());

    // graphics
    WidgetUI.visibleCB->setChecked(Property->IsFlagSet(EPOF_VISIBLE));
    WidgetUI.highlightedCB->setChecked(Property->IsFlagSet(EPOF_SELECTED));

    // property
    WidgetUI.allowOnlySameZCouplesCB->setChecked(Property->IsFlagSet<ERMSDPropertyFlag>(ERMSDPF_ONLY_SAME_Z));
    WidgetUI.massWeightedCB->setChecked(Property->IsFlagSet<ERMSDPropertyFlag>(ERMSDPF_MASS_WEIGHTED));
    WidgetUI.doNotFitCB->setChecked(Property->IsFlagSet<ERMSDPropertyFlag>(ERMSDPF_DO_NOT_FIT));

    // disable map manipulation if target and template are not available
    bool set = (Property->GetTemplateStructure() != NULL) && (Property->GetTargetStructure() != NULL);
    WidgetUI.mapTab->setEnabled(set);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::ApplyValues(void)
{
    //objects
    Property->SetTemplateStructureWH(static_cast<CStructure*>(WidgetUI.templateLO->getObject()));
    Property->SetTargetStructureWH(static_cast<CStructure*>(WidgetUI.targetLO->getObject()));

    // all flags
    CProObjectFlags flags = Property->GetFlags();

    SET_FLAG(flags,EPOF_VISIBLE,WidgetUI.visibleCB->isChecked())
    SET_FLAG(flags,EPOF_SELECTED,WidgetUI.highlightedCB->isChecked())

    SET_FLAG(flags,(EProObjectFlag)ERMSDPF_ONLY_SAME_Z,WidgetUI.allowOnlySameZCouplesCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)ERMSDPF_MASS_WEIGHTED,WidgetUI.massWeightedCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)ERMSDPF_DO_NOT_FIT,WidgetUI.doNotFitCB->isChecked())

    Property->SetFlagsWH(flags);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRMSDPropertyDesigner::TemplateObjectChanged(void)
{
    CProObject* p_obj = WidgetUI.templateLO->getObject();
    if( (p_obj == WidgetUI.targetLO->getObject()) && (p_obj != NULL) ){
        // error
        QMessageBox::critical(this,tr("Error"),tr("Template and target structures cannot be identical!"),QMessageBox::Ok,QMessageBox::Ok);
        // set previous object
        WidgetUI.templateLO->setObject(Property->GetTemplateStructure());
    }
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::TargetObjectChanged(void)
{
    CProObject* p_obj = WidgetUI.targetLO->getObject();
    if( (p_obj == WidgetUI.templateLO->getObject()) && (p_obj != NULL) ){
        // error
        QMessageBox::critical(this,tr("Error"),tr("Template and target structures cannot be identical!"),QMessageBox::Ok,QMessageBox::Ok);
        // set previous object
        WidgetUI.targetLO->setObject(Property->GetTargetStructure());
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRMSDPropertyDesigner::StartManualMapping(void)
{
    SelRequest->SetRequest(Property->GetProject()->GetSelection(),Property,"to form map couple");
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::StopManualMapping(void)
{
    SelRequest->SetRequest(NULL);
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::SelectionCompleted(void)
{
    CSelectionList* p_sel = Property->GetProject()->GetSelection();
    if( p_sel->GetRequestType() == Property ){
        CAtom* p_atom1 = dynamic_cast<CAtom*>(p_sel->GetSelectedObject(0));
        CAtom* p_atom2 = dynamic_cast<CAtom*>(p_sel->GetSelectedObject(1));
        p_sel->ResetSelection();
        Property->AddCouple(p_atom1,p_atom2);
        Property->GetProject()->RepaintProject();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRMSDPropertyDesigner::AlignTarget(void)
{
    Property->AlignTargetWH(WidgetUI.massWeightedCB->isChecked());
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::HideAllMappedAtoms(void)
{
    Property->HideAllMappedAtomsWH();
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::HideAllUnmappedAtoms(void)
{
    Property->HideAllUnmappedAtomsWH();
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::ShowAllAtoms(void)
{
    Property->ShowAllAtomsWH();
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::MapTargetPosToTemplate(void)
{
    Property->MapTargetPosToTemplateWH();
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::MapTemplatePosToTarget(void)
{
    Property->MapTemplatePosToTargetWH();
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::ImprintTemplateTopologyToTarget(void)
{
    Property->ImprintTemplateTopologyToTargetWH();
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::ImprintTargetTopologyToTemplate(void)
{
    Property->ImprintTargetTopologyToTemplateWH();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRMSDPropertyDesigner::MapTVClicked(const QModelIndex& index)
{
    CMapCouple* p_obj = NULL;
    p_obj = dynamic_cast<CMapCouple*>(MapModel->GetItem(index));
    bool obj_enabled = p_obj != NULL;

    WidgetUI.removeSelectedCouplePB->setEnabled(obj_enabled);
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::MapTVReset(void)
{
    WidgetUI.mapCount1LE->setText(QString("%1").arg(Property->GetNumberOfCouples()));
    WidgetUI.mapCount2LE->setText(QString("%1").arg(Property->GetNumberOfCouples()));

    bool set1 = Property->GetNumberOfCouples() >= 4;
    bool set2 = Property->GetNumberOfCouples() > 0;
    WidgetUI.alignPB->setEnabled(set1);
    WidgetUI.hideMappedAtomPB->setEnabled(set2);
    WidgetUI.templateToTargetPB->setEnabled(set2);
    WidgetUI.targetToTemplatePB->setEnabled(set2);
    WidgetUI.imprintTemplateTopologyToTargetPB->setEnabled(set2);
    WidgetUI.imprintTargetTopologyToTemplatePB->setEnabled(set2);
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::RemoveSelectedCouple(void)
{
    CMapCouple* p_obj = dynamic_cast<CMapCouple*>(MapModel->GetItem(WidgetUI.mapTV->currentIndex()));
    if( p_obj == NULL ) return;
    p_obj->DeleteWH();
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::RemoveHydrogenCouples(void)
{
    Property->RemoveHydrogenCouplesWH();
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::RemoveAllCouples(void)
{
    Property->RemoveAllCouplesWH();
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::MappingBySerIndx(void)
{
    Property->AddMappingBySerIndxWH();
}

//------------------------------------------------------------------------------

void CRMSDPropertyDesigner::MappingByDistance(void)
{
    Property->AddMappingByDistanceWH(WidgetUI.distanceTresholdSB->getInternalValue());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRMSDPropertyDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    WidgetUI.structuresGB->setEnabled(! Property->GetProject()->IsHistoryLocked(EHCL_PROPERTY));
    WidgetUI.propFlagsGB->setEnabled(! Property->GetProject()->IsHistoryLocked(EHCL_PROPERTY));
    WidgetUI.mapTab->setEnabled(! Property->GetProject()->IsHistoryLocked(EHCL_PROPERTY));
    WidgetUI.graphFlagsGB->setEnabled(! Property->GetProject()->IsHistoryLocked(EHCL_GRAPHICS));

    bool set1 = (Property->GetNumberOfCouples() >= 4);
    bool set2 = (Property->GetNumberOfCouples() > 0);
    WidgetUI.alignPB->setEnabled(set1 && (! Property->GetProject()->IsHistoryLocked(EHCL_GEOMETRY)));
    WidgetUI.hideMappedAtomPB->setEnabled(set2 && (! Property->GetProject()->IsHistoryLocked(EHCL_GRAPHICS)));
    WidgetUI.hideUnmappedAtomsPB->setEnabled(! Property->GetProject()->IsHistoryLocked(EHCL_GRAPHICS));
    WidgetUI.showAllAtomsPB->setEnabled(! Property->GetProject()->IsHistoryLocked(EHCL_GRAPHICS));
    WidgetUI.templateToTargetPB->setEnabled(set2 && (! Property->GetProject()->IsHistoryLocked(EHCL_GEOMETRY)));
    WidgetUI.targetToTemplatePB->setEnabled(set2 && (! Property->GetProject()->IsHistoryLocked(EHCL_GEOMETRY)));
    WidgetUI.imprintTemplateTopologyToTargetPB->setEnabled(set2 && (! Property->GetProject()->IsHistoryLocked(EHCL_TOPOLOGY)));
    WidgetUI.imprintTargetTopologyToTemplatePB->setEnabled(set2 && (! Property->GetProject()->IsHistoryLocked(EHCL_TOPOLOGY)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




