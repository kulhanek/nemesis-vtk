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
#include <NemesisCoreModule.hpp>

#include <ErrorSystem.hpp>

#include <Structure.hpp>
#include <Residue.hpp>
#include <Project.hpp>
#include <ResidueList.hpp>
#include <ContainerModel.hpp>

#include <WorkPanel.hpp>
#include <PluginDatabase.hpp>

#include <PODesignerGeneral.hpp>
#include <PODesignerFlags.hpp>
#include <PODesignerRefBy.hpp>

#include <ResidueDesigner.hpp>
#include "ResidueDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* ResidueDesignerCB(void* p_data);

CExtUUID        ResidueDesignerID(
                    "{RESIDUE_DESIGNER:991e5c1a-a4d8-4e7f-b786-7043ca5a6d12}",
                    "Residue");

CPluginObject   ResidueDesignerObject(&NemesisCorePlugin,
                    ResidueDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/structure/Residue.svg",
                    ResidueDesignerCB);

// -----------------------------------------------------------------------------

QObject* ResidueDesignerCB(void* p_data)
{
    CResidue* p_res = static_cast<CResidue*>(p_data);
    QObject* p_object = new CResidueDesigner(p_res);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CResidueDesigner::CResidueDesigner(CResidue* p_res)
    : CProObjectDesigner(&ResidueDesignerObject,p_res)
{
    Object = p_res;

    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Flags = new CPODesignerFlags(WidgetUI.flagsGB,Object,this);
    RefBy = new CPODesignerRefBy(WidgetUI.refByTab,Object,this);

    // models --------------------------------
    AtomsModel = Object->GetContainerModel(this);
    WidgetUI.atomsTV->setModel(AtomsModel);

    WidgetUI.atomsTV->setDragDropMode(QAbstractItemView::DragOnly);
    WidgetUI.atomsTV->setDragEnabled(true);

    if( AtomsModel != NULL ){
        for(int i=0; i < AtomsModel->columnCount(); i++){
            WidgetUI.atomsTV->resizeColumnToContents(i);
        }
    }

    // external events ---------------------------
    connect(Object,SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));
    connect(Object,SIGNAL(OnAtomListChanged(void)),
            this,SLOT(UpdateStatistics()));
    connect(Object->GetProject()->GetHistory(), SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this, SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // change events -----------------------------
    connect(WidgetUI.seqNumberSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.chainLE, SIGNAL(textEdited(const QString &)),
            SLOT(SetChangedFlagTrue()));

    // local actions -----------------------------
    connect(WidgetUI.atomsTV, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(ObjectTVDblClicked(const QModelIndex&)));
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));
    connect(WidgetUI.editPB, SIGNAL(clicked()),
            SLOT(EditResidue()));

    // initial values
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
}

//------------------------------------------------------------------------------

void CResidueDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Reset ) {
        InitAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Apply ) {
        ApplyAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Close ) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CResidueDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    Flags->InitValues();
    RefBy->InitValues();
    InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CResidueDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_COMPOSITE,Object->CComObject::GetType().GetName()) == false ) return;

    Changing = true;
        General->ApplyValues();
        Flags->ApplyValues();
        RefBy->ApplyValues();
        ApplyValues();
    Changing = false;

    Object->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CResidueDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.structureW->setObject(Object->GetStructure());

    // load other data
    WidgetUI.seqNumberSB->setValue(Object->GetSeqIndex());
    WidgetUI.chainLE->setText(Object->GetChain());

    // atom counts
    UpdateStatistics();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CResidueDesigner::ApplyValues(void)
{
    Object->SetSeqIndexWH(WidgetUI.seqNumberSB->value());
    Object->SetChainWH(WidgetUI.chainLE->text());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CResidueDesigner::UpdateStatistics(void)
{
    QString text;
    text.setNum(Object->GetNumberOfAtoms());
    WidgetUI.numOfAtomLE->setText(text);
}

//------------------------------------------------------------------------------

void CResidueDesigner::ObjectTVDblClicked(const QModelIndex& index)
{
    QAbstractItemModel* p_amodel = (QAbstractItemModel*)(index.model());
    CContainerModel* p_model = dynamic_cast<CContainerModel*>(p_amodel);
    if( p_model == NULL ) return;
    CProObject* p_obj = dynamic_cast<CProObject*>(p_model->GetItem(index));
    if( p_obj == NULL ) return;
    p_obj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CResidueDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    WidgetUI.specificGB->setEnabled(! Object->IsHistoryLocked(EHCL_TOPOLOGY));
}

//------------------------------------------------------------------------------

void CResidueDesigner::EditResidue(void)
{
    CExtUUID object_uuid;
    object_uuid.LoadFromString("{RESIDUE_WORK_PANEL:983992af-e31d-417a-bc21-0b35a32d855b}");

    CWorkPanel* p_wp = static_cast<CWorkPanel*>(PluginDatabase.CreateObject(object_uuid,Object));
    if( p_wp == NULL ) {
        CSmallString error;
        error << "unable to create tool object " << object_uuid.GetStringForm();
        ES_ERROR(error);
        return;
    }
    p_wp->Show();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




