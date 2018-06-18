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

#include <NemesisCoreModule.hpp>
#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <ContainerModel.hpp>
#include <Structure.hpp>

#include <PODesignerGeneral.hpp>
#include <GODesignerObjects.hpp>
#include <GODesignerSetup.hpp>
#include <GODesignerRefBy.hpp>

#include <GraphicsProfileObject.hpp>
#include <PBCBoxObject.hpp>
#include <PBCBoxObjectDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PBCBoxObjectDesignerCB(void* p_data);

CExtUUID        PBCBoxObjectDesignerID(
                    "{PBC_BOX_OBJECT_DESIGNER:6de1864d-067f-4ee9-ad89-f4588e993e73}",
                    "PBCBox");

CPluginObject   PBCBoxObjectDesignerObject(&NemesisCorePlugin,
                    PBCBoxObjectDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/PBCBox.svg",
                    PBCBoxObjectDesignerCB);

// -----------------------------------------------------------------------------

GODESIGNER_ENTRY_POINT(PBCBoxObject);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPBCBoxObjectDesigner::CPBCBoxObjectDesigner(CPBCBoxObject* p_fmo)
    : CProObjectDesigner(&PBCBoxObjectDesignerObject, p_fmo)
{
    Object = p_fmo;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Objects = new CGODesignerObjects(WidgetUI.objectsTab,Object,this);
    Setup = new CGODesignerSetup(WidgetUI.setupTab,Object,this);
    RefBy = new CGODesignerRefBy(WidgetUI.refByTab,Object,this);

    // change events -----------------------------
    connect(WidgetUI.visibleCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.selectedCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showCenterCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showFamiliarCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.centerToOriginCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.kaSB, SIGNAL(valueChanged(int)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.kbSB, SIGNAL(valueChanged(int)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.kcSB, SIGNAL(valueChanged(int)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton *)),
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

void CPBCBoxObjectDesigner::InitAllValues(void)
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

void CPBCBoxObjectDesigner::ApplyAllValues(void)
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

void CPBCBoxObjectDesigner::ButtonBoxClicked(QAbstractButton *button)
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

void CPBCBoxObjectDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.visibleCB->setChecked(Object->IsFlagSet(EPOF_VISIBLE));
    WidgetUI.selectedCB->setChecked(Object->IsFlagSet(EPOF_SELECTED));
    WidgetUI.showCenterCB->setChecked(Object->IsFlagSet<EPBCBoxObjectFlag>(EPBCBOF_SHOW_CENTER));
    WidgetUI.showFamiliarCB->setChecked(Object->IsFlagSet<EPBCBoxObjectFlag>(EPBCBOF_SHOW_FAMILIAR));
    WidgetUI.centerToOriginCB->setChecked(Object->IsFlagSet<EPBCBoxObjectFlag>(EPBCBOF_CENTER_TO_ORIGIN));

    WidgetUI.kaSB->setValue(Object->GetKAOffset());
    WidgetUI.kbSB->setValue(Object->GetKBOffset());
    WidgetUI.kcSB->setValue(Object->GetKCOffset());

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CPBCBoxObjectDesigner::ApplyValues(void)
{
    CProObjectFlags flags = Object->GetFlags();

    SET_FLAG(flags,EPOF_VISIBLE,WidgetUI.visibleCB->isChecked())
    SET_FLAG(flags,EPOF_SELECTED,WidgetUI.selectedCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EPBCBOF_SHOW_CENTER,WidgetUI.showCenterCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EPBCBOF_SHOW_FAMILIAR,WidgetUI.showFamiliarCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EPBCBOF_CENTER_TO_ORIGIN,WidgetUI.centerToOriginCB->isChecked())

    Object->SetFlagsWH(flags);

    int ka = WidgetUI.kaSB->value();
    int kb = WidgetUI.kbSB->value();
    int kc = WidgetUI.kcSB->value();

    Object->SetKOffsetsWH(ka,kb,kc);
}

//------------------------------------------------------------------------------

void CPBCBoxObjectDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    bool set = ! Object->IsHistoryLocked(EHCL_GRAPHICS);
    WidgetUI.flagsGB->setEnabled(set);
    WidgetUI.positionGB->setEnabled(set);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

