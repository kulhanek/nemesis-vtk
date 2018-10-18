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
#include <GODesignerColorMode.hpp>

#include <GraphicsProfileObject.hpp>
#include <FastModelObject.hpp>
#include <FastModelObjectDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* FastModelObjectDesignerCB(void* p_data);

CExtUUID        FastModelObjectDesignerID(
                    "{FAST_MODEL_OBJECT_DESIGNER:b0424633-0cda-4cc9-9e53-c6a160fe8c25}",
                    "Fast Model");

CPluginObject   FastModelObjectDesignerObject(&NemesisCorePlugin,
                    FastModelObjectDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/FastModel.svg",
                    FastModelObjectDesignerCB);

// -----------------------------------------------------------------------------

GODESIGNER_ENTRY_POINT(FastModelObject);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFastModelObjectDesigner::CFastModelObjectDesigner(CFastModelObject* p_fmo)
    : CProObjectDesigner(&FastModelObjectDesignerObject, p_fmo)
{
    Object = p_fmo;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Objects = new CGODesignerObjects(WidgetUI.objectsTab,Object,this);
    Setup = new CGODesignerSetup(WidgetUI.setupTab,Object,this);
    RefBy = new CGODesignerRefBy(WidgetUI.refByTab,Object,this);
    ColorMode = new CGODesignerColorMode(WidgetUI.colorModeW,Object->GetColorMode(),this);


    // connect slots -----------------------------
    connect(WidgetUI.visibleCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showHiddenCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showHydrogensCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.enablePBCCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.showAtomsOnlyCB, SIGNAL(clicked(bool)),
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

void CFastModelObjectDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();
    Objects->InitValues();
    Setup->InitValues();
    RefBy->InitValues();
    ColorMode->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CFastModelObjectDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_COMPOSITE,Object->GetType().GetName()) == NULL ) return;

    Changing = true;
        General->ApplyValues();
        ApplyValues();
        ColorMode->ApplyValues();
    Changing = false;

    Object->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//------------------------------------------------------------------------------

void CFastModelObjectDesigner::ButtonBoxClicked(QAbstractButton *button)
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

void CFastModelObjectDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.visibleCB->setChecked(Object->IsFlagSet(EPOF_VISIBLE));
    WidgetUI.showHydrogensCB->setChecked(Object->IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HYDROGENS));
    WidgetUI.showHiddenCB->setChecked(Object->IsFlagSet<EFastModelObjectFlag>(EFMOF_SHOW_HIDDEN));
    WidgetUI.enablePBCCB->setChecked(Object->IsFlagSet<EFastModelObjectFlag>(EFMOF_PBC_BONDS));
    WidgetUI.showAtomsOnlyCB->setChecked(Object->IsFlagSet<EFastModelObjectFlag>(EFMOF_ONLY_ATOMS));

    WidgetUI.kaSB->setValue(Object->GetKAOffset());
    WidgetUI.kbSB->setValue(Object->GetKBOffset());
    WidgetUI.kcSB->setValue(Object->GetKCOffset());

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CFastModelObjectDesigner::ApplyValues(void)
{
    CProObjectFlags flags = Object->GetFlags();
    SET_FLAG(flags,EPOF_VISIBLE,WidgetUI.visibleCB->isChecked());
    SET_FLAG(flags,(EProObjectFlag)EFMOF_SHOW_HYDROGENS,WidgetUI.showHydrogensCB->isChecked());
    SET_FLAG(flags,(EProObjectFlag)EFMOF_SHOW_HIDDEN,WidgetUI.showHiddenCB->isChecked());
    SET_FLAG(flags,(EProObjectFlag)EFMOF_PBC_BONDS,WidgetUI.enablePBCCB->isChecked());
    SET_FLAG(flags,(EProObjectFlag)EFMOF_ONLY_ATOMS,WidgetUI.showAtomsOnlyCB->isChecked());
    Object->SetFlagsWH(flags);

    int ka = WidgetUI.kaSB->value();
    int kb = WidgetUI.kbSB->value();
    int kc = WidgetUI.kcSB->value();

    Object->SetKOffsetsWH(ka,kb,kc);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFastModelObjectDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    bool set = ! Object->IsHistoryLocked(EHCL_GRAPHICS);
    WidgetUI.flagsGB->setEnabled(set);
    WidgetUI.pbcTab->setEnabled(set);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

