// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <NemesisCoreModule.hpp>
#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>
#include <GraphicsProfileObject.hpp>

#include <PODesignerGeneral.hpp>
#include <GODesignerObjects.hpp>
#include <GODesignerSetup.hpp>
#include <GODesignerRefBy.hpp>

#include <AtomLabelObject.hpp>
#include <AtomLabelObjectDesigner.hpp>
#include "AtomLabelObjectDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* AtomLabelObjectDesignerCB(void* p_data);

CExtUUID        AtomLabelObjectDesignerID(
                    "{ATOM_LABEL_OBJECT_DESIGNER:d9ec8ab4-6655-460e-ad20-d9c4b7d66074}",
                    "Atom label");

CPluginObject   AtomLabelObjectDesignerObject(&NemesisCorePlugin,
                    AtomLabelObjectDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/AtomLabel.svg",
                    AtomLabelObjectDesignerCB);

// -----------------------------------------------------------------------------

GODESIGNER_ENTRY_POINT(AtomLabelObject);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomLabelObjectDesigner::CAtomLabelObjectDesigner(CAtomLabelObject* p_fmo)
    : CProObjectDesigner(&AtomLabelObjectDesignerObject, p_fmo)
{
    Object = p_fmo;
    WidgetUI.setupUi(this);

    // set units ---------------------------------
    WidgetUI.offsetXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.offsetYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.offsetZSB->setPhysicalQuantity(PQ_DISTANCE);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Objects = new CGODesignerObjects(WidgetUI.objectsTab,Object,this);
    Setup = new CGODesignerSetup(WidgetUI.setupTab,Object,this);
    RefBy = new CGODesignerRefBy(WidgetUI.refByTab,Object,this);

    // connect slots -----------------------------
    connect(WidgetUI.visibleCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showHiddenCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.highlightedCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showHydrogensCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showNameCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showTypeCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showChargeCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showResidueCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showCustomCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showCustomCB, SIGNAL(clicked(bool)),
            SLOT(CustomFormatClicked(bool)));
    //------------------
    connect(WidgetUI.showLineCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.offsetXSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.offsetYSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.offsetZSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.customFormatLE, SIGNAL(textChanged(const QString&)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.buttonBox1, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(WidgetUI.buttonBox2, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(Object, SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));
    //------------------
    connect(Object->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // init all values ---------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CAtomLabelObjectDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();
    Objects->InitValues();
    Setup->InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CAtomLabelObjectDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_COMPOSITE,Object->GetType().GetName()) == false ) return;

    Changing = true;
        General->ApplyValues();
        ApplyValues();
    Changing = false;

    Object->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//------------------------------------------------------------------------------

void CAtomLabelObjectDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Reset) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Reset) ) {
        InitAllValues();
        return;
    }

    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Apply) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Apply) ) {
        ApplyAllValues();
        return;
    }

    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Close) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Close) ) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CAtomLabelObjectDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.visibleCB->setChecked(Object->IsFlagSet(EPOF_VISIBLE));
    WidgetUI.highlightedCB->setChecked(Object->IsFlagSet(EPOF_SELECTED));

    WidgetUI.showHiddenCB->setChecked(Object->IsFlagSet((EProObjectFlag)EALOF_SHOW_HIDDEN));
    WidgetUI.showNameCB->setChecked(Object->IsFlagSet((EProObjectFlag)EALOF_SHOW_NAME));
    WidgetUI.showTypeCB->setChecked(Object->IsFlagSet((EProObjectFlag)EALOF_SHOW_TYPE));
    WidgetUI.showChargeCB->setChecked(Object->IsFlagSet((EProObjectFlag)EALOF_SHOW_CHARGE));
    WidgetUI.showResidueCB->setChecked(Object->IsFlagSet((EProObjectFlag)EALOF_SHOW_RESNAME));
    WidgetUI.showCustomCB->setChecked(Object->IsFlagSet((EProObjectFlag)EALOF_SHOW_CUSTOM));
    WidgetUI.showHydrogensCB->setChecked(Object->IsFlagSet((EProObjectFlag)EALOF_SHOW_HYDROGENS));

    CustomFormatClicked(Object->IsFlagSet((EProObjectFlag)EALOF_SHOW_CUSTOM));

    WidgetUI.showLineCB->setChecked(Object->IsFlagSet((EProObjectFlag)EALOF_SHOW_LINE));

    WidgetUI.customFormatLE->setText(Object->GetCustomFormat());

    WidgetUI.offsetXSB->setInternalValue(Object->GetOffset().x);
    WidgetUI.offsetYSB->setInternalValue(Object->GetOffset().y);
    WidgetUI.offsetZSB->setInternalValue(Object->GetOffset().z);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CAtomLabelObjectDesigner::ApplyValues(void)
{
    CProObjectFlags flags = Object->GetFlags();

    SET_FLAG(flags,EPOF_VISIBLE,WidgetUI.visibleCB->isChecked())
    SET_FLAG(flags,EPOF_SELECTED,WidgetUI.highlightedCB->isChecked())

    SET_FLAG(flags,(EProObjectFlag)EALOF_SHOW_HIDDEN,WidgetUI.showHiddenCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EALOF_SHOW_NAME,WidgetUI.showNameCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EALOF_SHOW_TYPE,WidgetUI.showTypeCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EALOF_SHOW_CHARGE,WidgetUI.showChargeCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EALOF_SHOW_RESNAME,WidgetUI.showResidueCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EALOF_SHOW_CUSTOM,WidgetUI.showCustomCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EALOF_SHOW_LINE,WidgetUI.showLineCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EALOF_SHOW_HYDROGENS,WidgetUI.showHydrogensCB->isChecked())

    Object->SetFlagsWH(flags);

    Object->SetCustomFormatWH(WidgetUI.customFormatLE->text());

    CPoint offset;
    offset.x = WidgetUI.offsetXSB->getInternalValue();
    offset.y = WidgetUI.offsetYSB->getInternalValue();
    offset.z = WidgetUI.offsetZSB->getInternalValue();

    Object->SetOffsetWH(offset);
}

//------------------------------------------------------------------------------

void CAtomLabelObjectDesigner::CustomFormatClicked(bool checked)
{
    if( checked ){
        WidgetUI.showNameCB->setChecked(false);
        WidgetUI.showTypeCB->setChecked(false);
        WidgetUI.showChargeCB->setChecked(false);
        WidgetUI.showResidueCB->setChecked(false);
        WidgetUI.showNameCB->setEnabled(false);
        WidgetUI.showTypeCB->setEnabled(false);
        WidgetUI.showChargeCB->setEnabled(false);
        WidgetUI.showResidueCB->setEnabled(false);
    } else {
        WidgetUI.showNameCB->setEnabled(true);
        WidgetUI.showTypeCB->setEnabled(true);
        WidgetUI.showChargeCB->setEnabled(true);
        WidgetUI.showResidueCB->setEnabled(true);
    }
}

//------------------------------------------------------------------------------

void CAtomLabelObjectDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    bool set = ! Object->IsHistoryLocked(EHCL_GRAPHICS);
    WidgetUI.flagsGB->setEnabled(set);
    WidgetUI.offsetGB->setEnabled(set);
    WidgetUI.customFormatLE->setEnabled(set);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

