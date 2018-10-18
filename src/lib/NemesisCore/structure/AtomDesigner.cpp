// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <Project.hpp>
#include <ExtUUID.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>

#include <PeriodicTableWidget.hpp>
#include <PeriodicTable.hpp>
#include <PhysicalQuantities.hpp>
#include <Structure.hpp>
#include <AtomList.hpp>
#include <Residue.hpp>
#include <ContainerModel.hpp>
#include <StructureList.hpp>
#include <ResidueSelection.hpp>

#include <PODesignerGeneral.hpp>
#include <PODesignerFlags.hpp>
#include <PODesignerRefBy.hpp>

#include <QDialog>

#include <AtomDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* AtomDesignerCB(void* p_data);

CExtUUID        AtomDesignerID(
                    "{ATOM_DESIGNER:56ff4ac4-67b2-4c8c-88bd-5ace4e14bd4e}",
                    "Atom");

CPluginObject   AtomDesignerObject(&NemesisCorePlugin,
                    AtomDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/structure/Atom.svg",
                    AtomDesignerCB);

// -----------------------------------------------------------------------------

QObject* AtomDesignerCB(void* p_data)
{
    CAtom* p_atom = static_cast<CAtom*>(p_data);
    QObject* p_object = new CAtomDesigner(p_atom);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomDesigner::CAtomDesigner(CAtom* p_atom)
    : CProObjectDesigner(&AtomDesignerObject,p_atom)
{
    Object = p_atom;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Flags = new CPODesignerFlags(WidgetUI.flagsGB,Object,this);
    RefBy = new CPODesignerRefBy(WidgetUI.refByTab,Object,this);

    WidgetUI.residueW->setProject(Object->GetProject());
    WidgetUI.residueW->setSelectionHandler(&SH_Residue);
    WidgetUI.residueW->setObjectBaseMIMEType("residue.indexes");
    WidgetUI.residueW->setResidueMode(true);

    // models ------------------------------------
    Bonds = Object->GetRegBondsContainerModel(this);
    WidgetUI.bondsTV->setModel(Bonds);
    WidgetUI.bondsTV->setDragDropMode(QAbstractItemView::DragOnly);
    WidgetUI.bondsTV->setDragEnabled(true);

    if( Bonds != NULL ){
        for(int i=0; i < Bonds->columnCount(); i++){
            WidgetUI.bondsTV->resizeColumnToContents(i);
        }
    }

    // external events ---------------------------    
    connect(Object,SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));
    connect(Object, SIGNAL(OnBondRegistered(CBond*)),
            this, SLOT(UpdateStatistics()));
    connect(Object, SIGNAL(OnBondUnregistered(CBond*)),
            this, SLOT(UpdateStatistics()));
    connect(Object->GetProject()->GetHistory(), SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this, SLOT(ProjectLockChanged(EHistoryChangeMessage)));
    connect(Object->GetProject()->GetStructures(),SIGNAL(OnGeometryChangeTick(void)),
            this,SLOT(GeometryChanged(void)));
    connect(Object,SIGNAL(destroyed(QObject *)),
            this,SLOT(ObjectDestroyed()));

    // change events -----------------------------
    connect(WidgetUI.residueW, SIGNAL(OnObjectChanged(void)),
            SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.residueW, SIGNAL(OnObjectToBeChanged(CProObject*)),
            SLOT(ResidueAboutToBeChanged(CProObject*)));
    connect(WidgetUI.typeLE, SIGNAL(textEdited(const QString &)),
            SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.serialIndexSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.localIndexSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.chargeSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));

    connect(WidgetUI.xSB, SIGNAL(valueChanged(const QString &)),
            SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.ySB, SIGNAL(valueChanged(const QString &)),
            SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.zSB, SIGNAL(valueChanged(const QString &)),
            SLOT(SetChangedFlagTrue()));

    connect(WidgetUI.vxSB, SIGNAL(valueChanged(const QString &)),
            SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.vySB, SIGNAL(valueChanged(const QString &)),
            SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.vzSB, SIGNAL(valueChanged(const QString &)),
            SLOT(SetChangedFlagTrue()));

    // local actions -----------------------------
    connect(WidgetUI.symbolPB, SIGNAL(clicked()),
            SLOT(OpenPeriodicTableDialog()));
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));
    connect(WidgetUI.bondsTV, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(ObjectTVDblClicked(const QModelIndex&)));

    // units -------------------------------------
    WidgetUI.chargeSB->setPhysicalQuantity(PQ_CHARGE);
    WidgetUI.xSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.ySB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.zSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.vxSB->setPhysicalQuantity(PQ_VELOCITY);
    WidgetUI.vySB->setPhysicalQuantity(PQ_VELOCITY);
    WidgetUI.vzSB->setPhysicalQuantity(PQ_VELOCITY);

    // initial values
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CAtomDesigner::ButtonBoxClicked(QAbstractButton *button)
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

//------------------------------------------------------------------------------

void CAtomDesigner::ObjectDestroyed(void)
{
    // send to selection buffer might lead to object destruction, we need to remove
    // any external notification that might access deleted object
    // remove geometry change notification
    Object->GetProject()->GetStructures()->disconnect(this);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CAtomDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    Flags->InitValues();
    RefBy->InitValues();
    InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CAtomDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_COMPOSITE,Object->CComObject::GetType().GetName()) == NULL ) return;

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

void CAtomDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.structureW->setObject(Object->GetStructure());
    WidgetUI.residueW->setObject(Object->GetResidue()); 

    // z
    Z = Object->GetZ();
    WidgetUI.symbolPB->setText(QString(PeriodicTable.GetSymbol(Z)));

    // type
    WidgetUI.typeLE->setText(Object->GetType());

    // indexes
    WidgetUI.serialIndexSB->setValue(Object->GetSerIndex());
    WidgetUI.localIndexSB->setValue(Object->GetLocIndex());

    // charge
    WidgetUI.chargeSB->setInternalValue(Object->GetCharge());

    // set position values
    WidgetUI.xSB->setInternalValue(Object->GetPos().x);
    WidgetUI.ySB->setInternalValue(Object->GetPos().y);
    WidgetUI.zSB->setInternalValue(Object->GetPos().z);

    // set velocities values
    WidgetUI.vxSB->setInternalValue(Object->GetVel().x);
    WidgetUI.vySB->setInternalValue(Object->GetVel().y);
    WidgetUI.vzSB->setInternalValue(Object->GetVel().z);

    // update bond statistics
    UpdateStatistics();

    // set unchanged
    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CAtomDesigner::GeometryChanged(void)
{
    // set position values
    WidgetUI.xSB->setInternalValue(Object->GetPos().x);
    WidgetUI.ySB->setInternalValue(Object->GetPos().y);
    WidgetUI.zSB->setInternalValue(Object->GetPos().z);

    // set velocities values
    WidgetUI.vxSB->setInternalValue(Object->GetVel().x);
    WidgetUI.vySB->setInternalValue(Object->GetVel().y);
    WidgetUI.vzSB->setInternalValue(Object->GetVel().z);
}

//------------------------------------------------------------------------------

void CAtomDesigner::ApplyValues(void)
{
    Object->SetResidueWH(WidgetUI.residueW->getObject<CResidue*>());

    Object->SetZWH(Z);

    Object->SetTypeWH(WidgetUI.typeLE->text());
    Object->SetSerIndexWH(WidgetUI.serialIndexSB->value());
    Object->SetLocIndexWH(WidgetUI.localIndexSB->value());
    Object->SetChargeWH(WidgetUI.chargeSB->getInternalValue());

    double x, y, z;
    x = WidgetUI.xSB->getInternalValue();
    y = WidgetUI.ySB->getInternalValue();
    z = WidgetUI.zSB->getInternalValue();

    if( WidgetUI.moveWholeMolChBox->isChecked() ) {
        CPoint opos = Object->GetPos();
        if( Object->GetAtoms() != NULL ) {
            Object->GetAtoms()->MoveByWH(CPoint(x,y,z)-opos);
        }
    } else {
        Object->SetPosWH(CPoint(x,y,z));
    }

    x = WidgetUI.vxSB->getInternalValue();
    y = WidgetUI.vySB->getInternalValue();
    z = WidgetUI.vzSB->getInternalValue();
    Object->SetVelWH(CPoint(x,y,z));
}

//------------------------------------------------------------------------------

void CAtomDesigner::OpenPeriodicTableDialog(void)
{
    QDialog* p_dialog = new QDialog(this);
    if(p_dialog == NULL) return;

    p_dialog->setWindowTitle(tr("Periodic Table"));

    CPeriodicTableWidget* p_table = new CPeriodicTableWidget(p_dialog);
    if( p_table == NULL) {
        p_dialog->close();
        return;
    }

    p_table->ChangeElement(Z);
    p_dialog->setLayout(new QVBoxLayout());
    p_dialog->layout()->addWidget(p_table);

    QDialogButtonBox* p_buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, p_dialog);
    p_dialog->layout()->addWidget(p_buttonBox);
    p_dialog->setToolTip("Choose atom element from table ...");

    connect(p_buttonBox, SIGNAL(accepted()),
            p_dialog, SLOT(accept()));
    connect(p_buttonBox, SIGNAL(rejected()),
            p_dialog, SLOT(reject()));

    int result = p_dialog->exec();
    if(result == 1) {
        Z = p_table->GetActiveElement();
        WidgetUI.symbolPB->setText(QString(PeriodicTable.GetSymbol(Z)));
        SetChangedFlag(true);
    }
}

//------------------------------------------------------------------------------

void CAtomDesigner::ObjectTVDblClicked(const QModelIndex& index)
{
    QAbstractItemModel* p_amodel = (QAbstractItemModel*)(index.model());
    CContainerModel* p_model = dynamic_cast<CContainerModel*>(p_amodel);
    if( p_model == NULL ) return;
    CProObject* p_obj = dynamic_cast<CProObject*>(p_model->GetItem(index));
    if( p_obj == NULL ) return;
    p_obj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CAtomDesigner::UpdateStatistics(void)
{
    QString text;
    text.setNum(Object->GetNumberOfBonds());
    WidgetUI.numOfBondsLE->setText(text);
}

//------------------------------------------------------------------------------

void CAtomDesigner::ResidueAboutToBeChanged(CProObject* p_obj)
{
    if( p_obj == NULL ) return;
    bool accept = false;
    CResidue* p_res = dynamic_cast<CResidue*>(p_obj);
    if( p_res ){
        if( p_res->GetStructure() == Object->GetStructure() ){
            accept = true;
        }
    }
    if( accept == false ){
        WidgetUI.residueW->rejectObjectChange();
        Object->GetProject()->TextNotification(ETNT_ERROR,tr("the residue is not from the same structure that owns the atom"),ETNT_ERROR_DELAY);
    }
}

//------------------------------------------------------------------------------

void CAtomDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;

    WidgetUI.specificGB->setEnabled(! Object->IsHistoryLocked(EHCL_TOPOLOGY));
    WidgetUI.geometryTab->setEnabled(! Object->IsHistoryLocked(EHCL_GEOMETRY));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
