// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <NemesisCoreModule.hpp>

#include <ErrorSystem.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>
#include <Atom.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <ContainerModel.hpp>
#include <StructureList.hpp>

#include <PODesignerGeneral.hpp>
#include <PODesignerFlags.hpp>
#include <PODesignerRefBy.hpp>

#include <AtomSelection.hpp>

#include <BondDesigner.hpp>
#include "BondDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* BondDesignerCB(void* p_data);

CExtUUID        BondDesignerID(
                    "{BOND_DESIGNER:d9d57aa3-19b7-453c-a2ec-97cccce7e8d1}",
                    "Bond");

CPluginObject   BondDesignerObject(&NemesisCorePlugin,
                    BondDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/structure/Bond.svg",
                    BondDesignerCB);

// -----------------------------------------------------------------------------

QObject* BondDesignerCB(void* p_data)
{
    CBond* p_bond = static_cast<CBond*>(p_data);
    QObject* p_object = new CBondDesigner(p_bond);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBondDesigner::CBondDesigner(CBond* p_bond)
    : CProObjectDesigner(&BondDesignerObject,p_bond)
{
    Object = p_bond;

    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Flags = new CPODesignerFlags(WidgetUI.flagsGB,Object,this);
    RefBy = new CPODesignerRefBy(WidgetUI.refByTab,Object,this);

    // this button is not visible
    WidgetUI.bondButton0_0->hide();

    // atoms can be selected
    WidgetUI.atom1W->setProject(Object->GetProject());
    WidgetUI.atom1W->setSelectionHandler(&SH_Atom);
    WidgetUI.atom1W->setObjectBaseMIMEType("atom.indexes");

    WidgetUI.atom2W->setProject(Object->GetProject());
    WidgetUI.atom2W->setSelectionHandler(&SH_Atom);
    WidgetUI.atom2W->setObjectBaseMIMEType("atom.indexes");

    // external events ---------------------------
    connect(Object->GetStructure(),SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));
    connect(Object,SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));
    if( Object->GetProject() ){
        connect(Object->GetProject()->GetHistory(), SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
                this, SLOT(ProjectLockChanged(EHistoryChangeMessage)));
    }
    if( Object->GetProject()->GetStructures() ){
        connect(Object->GetProject()->GetStructures(),SIGNAL(OnGeometryChangeTick(void)),
                this,SLOT(GeometryChanged(void)));
    }
    connect(Object,SIGNAL(destroyed(QObject *)),
            this,SLOT(ObjectDestroyed()));

    // change events -----------------------------
    connect(WidgetUI.typeLE, SIGNAL(textEdited(const QString &)),
            SLOT(SetChangedFlagTrue()));

    connect(WidgetUI.lengthLE, SIGNAL(textEdited(const QString &)),
            SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.bondButton1_0, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.bondButton1_5, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.bondButton2_0, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.bondButton2_5, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.bondButton3_0, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.atom1W, SIGNAL(OnObjectChanged(void)),
            SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.atom1W,SIGNAL(OnObjectToBeChanged(CProObject*)),
            this,SLOT(FirstAtomAboutToBeChanged(CProObject*)));
    connect(WidgetUI.atom2W, SIGNAL(OnObjectChanged(void)),
            SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.atom2W,SIGNAL(OnObjectToBeChanged(CProObject*)),
            this,SLOT(SecondAtomAboutToBeChanged(CProObject*)));

    // local actions -----------------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));

    // units
    WidgetUI.lengthLE->setPhysicalQuantity(PQ_DISTANCE);

    // initial values
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBondDesigner::ButtonBoxClicked(QAbstractButton *button)
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

void CBondDesigner::ObjectDestroyed(void)
{
    // send to selection buffer might lead to object destruction, we need to remove
    // any external notification that might access deleted object
    // remove geometry change notification
    Object->GetProject()->GetStructures()->disconnect(this);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBondDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    Flags->InitValues();
    RefBy->InitValues();
    InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CBondDesigner::ApplyAllValues(void)
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

void CBondDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.structureW->setObject(Object->GetStructure());
    WidgetUI.atom1W->setObject(Object->GetFirstAtom());
    WidgetUI.atom2W->setObject(Object->GetSecondAtom());

    // update length and unit
    WidgetUI.lengthLE->setInternalValue(Object->GetLength());
    WidgetUI.typeLE->setText(Object->GetBondType());

    // update bond order
    switch(Object->GetBondOrder()) {
        case BO_NONE :
        case BO_WEAK :
            WidgetUI.bondButton0_0->setChecked(true);
            break;
        case BO_SINGLE :
            WidgetUI.bondButton1_0->setChecked(true);
            break;
        case BO_SINGLE_H :
            WidgetUI.bondButton1_5->setChecked(true);
            break;
        case BO_DOUBLE :
            WidgetUI.bondButton2_0->setChecked(true);
            break;
        case BO_DOUBLE_H :
            WidgetUI.bondButton2_5->setChecked(true);
            break;
        case BO_TRIPLE :
            WidgetUI.bondButton3_0->setChecked(true);
            break;
    }

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CBondDesigner::GeometryChanged(void)
{
    WidgetUI.lengthLE->setInternalValue(Object->GetLength());
}

//------------------------------------------------------------------------------

void CBondDesigner::ApplyValues(void)
{
    Object->SetFirstAtomWH(WidgetUI.atom1W->getObject<CAtom*>());
    Object->SetSecondAtomWH(WidgetUI.atom2W->getObject<CAtom*>());

    Object->SetBondTypeWH(WidgetUI.typeLE->text());

    if( WidgetUI.bondButton1_0->isChecked() ) {
        Object->SetOrderWH(BO_SINGLE);
    } else if ( WidgetUI.bondButton1_5->isChecked() ) {
        Object->SetOrderWH(BO_SINGLE_H);
    } else if( WidgetUI.bondButton2_0->isChecked() ) {
        Object->SetOrderWH(BO_DOUBLE);
    } else if( WidgetUI.bondButton2_5->isChecked() ) {
        Object->SetOrderWH(BO_DOUBLE_H);
    } else if( WidgetUI.bondButton3_0->isChecked() ) {
        Object->SetOrderWH(BO_TRIPLE);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBondDesigner::FirstAtomAboutToBeChanged(CProObject* p_obj)
{
    if( p_obj == NULL ) return;
    bool accept = false;
    CAtom* p_atom = dynamic_cast<CAtom*>(p_obj);
    if( p_atom ){
        if( p_atom->GetStructure() == Object->GetStructure() ){
            accept = true;
        }
    }
    if( p_obj != WidgetUI.atom2W->getObject() ){
        if( accept == false ){
            WidgetUI.atom1W->rejectObjectChange();
            Object->GetProject()->TextNotification(ETNT_ERROR,tr("the first atom must be from the same structure that owns the bond"),ETNT_ERROR_DELAY);
        }
    } else {
        WidgetUI.atom1W->rejectObjectChange();
        Object->GetProject()->TextNotification(ETNT_ERROR,tr("the first atom must be different from the second one"),ETNT_ERROR_DELAY);
    }
}

//------------------------------------------------------------------------------

void CBondDesigner::SecondAtomAboutToBeChanged(CProObject* p_obj)
{
    if( p_obj == NULL ) return;
    bool accept = false;
    CAtom* p_atom = dynamic_cast<CAtom*>(p_obj);
    if( p_atom ){
        if( p_atom->GetStructure() == Object->GetStructure() ){
            accept = true;
        }
    }
    if( p_obj != WidgetUI.atom1W->getObject() ){
        if( accept == false ){
            WidgetUI.atom2W->rejectObjectChange();
            Object->GetProject()->TextNotification(ETNT_ERROR,tr("the second atom must be from the same structure that owns the bond"),ETNT_ERROR_DELAY);
        }
    } else {
        WidgetUI.atom1W->rejectObjectChange();
        Object->GetProject()->TextNotification(ETNT_ERROR,tr("the second atom must be different from the first one"),ETNT_ERROR_DELAY);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CBondDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    WidgetUI.specificGB->setEnabled(! Object->IsHistoryLocked(EHCL_TOPOLOGY));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


