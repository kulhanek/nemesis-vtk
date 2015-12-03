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

#include <PODesignerGeneral.hpp>
#include <GODesignerObjects.hpp>
#include <GODesignerSetup.hpp>
#include <GODesignerRefBy.hpp>

#include <GraphicsProfileObject.hpp>
#include <TextObject.hpp>
#include <TextObjectDesigner.hpp>
#include "TextObjectDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* TextObjectDesignerCB(void* p_data);

CExtUUID        TextObjectDesignerID(
                    "{TEXT_OBJECT_DESIGNER:bd5949fd-ddb4-4320-ab0d-63a7b4897019}",
                    "Text");

CPluginObject   TextObjectDesignerObject(&NemesisCorePlugin,
                    TextObjectDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/Text.svg",
                    TextObjectDesignerCB);

// -----------------------------------------------------------------------------

GODESIGNER_ENTRY_POINT(TextObject);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTextObjectDesigner::CTextObjectDesigner(CTextObject* p_fmo)
    : CProObjectDesigner(&TextObjectDesignerObject, p_fmo)
{
    Object = p_fmo;
    WidgetUI.setupUi(this);

    // set units ---------------------------------
    WidgetUI.positionXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.positionYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.positionZSB->setPhysicalQuantity(PQ_DISTANCE);

    WidgetUI.orientationXSB->setPhysicalQuantity(PQ_ANGLE);
    WidgetUI.orientationYSB->setPhysicalQuantity(PQ_ANGLE);
    WidgetUI.orientationZSB->setPhysicalQuantity(PQ_ANGLE);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Objects = new CGODesignerObjects(WidgetUI.objectsTab,Object,this);
    Setup = new CGODesignerSetup(WidgetUI.setupTab,Object,this);
    RefBy = new CGODesignerRefBy(WidgetUI.refByTab,Object,this);

    // connect slots -----------------------------
    connect(WidgetUI.visibleCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.highlightedCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.globalPositionCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.faceToUserCB, SIGNAL(clicked(bool)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.positionXSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.positionYSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.positionZSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.orientationXSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.orientationYSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.orientationZSB, SIGNAL(valueChanged(double)),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.textTE, SIGNAL(textChanged()),
            SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.buttonBox1, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(WidgetUI.buttonBox2, SIGNAL(clicked (QAbstractButton *)),
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

void CTextObjectDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();
    Setup->InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CTextObjectDesigner::ApplyAllValues(void)
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

void CTextObjectDesigner::ButtonBoxClicked(QAbstractButton *button)
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

void CTextObjectDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.visibleCB->setChecked(Object->IsFlagSet(EPOF_VISIBLE));
    WidgetUI.highlightedCB->setChecked(Object->IsFlagSet(EPOF_SELECTED));
    WidgetUI.globalPositionCB->setChecked(Object->IsFlagSet<ETextObjectFlag>(ETOF_GLOBAL_POSITION));
    WidgetUI.faceToUserCB->setChecked(Object->IsFlagSet<ETextObjectFlag>(ETOF_FACE_ORIENTATION));

    WidgetUI.textTE->setPlainText(Object->GetText());

    WidgetUI.positionXSB->setInternalValue(Object->GetPosition().x);
    WidgetUI.positionYSB->setInternalValue(Object->GetPosition().y);
    WidgetUI.positionZSB->setInternalValue(Object->GetPosition().z);

    WidgetUI.orientationXSB->setInternalValue(Object->GetOrientation().x);
    WidgetUI.orientationYSB->setInternalValue(Object->GetOrientation().y);
    WidgetUI.orientationZSB->setInternalValue(Object->GetOrientation().z);

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CTextObjectDesigner::ApplyValues(void)
{
    CProObjectFlags flags = Object->GetFlags();

    SET_FLAG(flags,EPOF_VISIBLE,WidgetUI.visibleCB->isChecked())
    SET_FLAG(flags,EPOF_SELECTED,WidgetUI.highlightedCB->isChecked())

    SET_FLAG(flags,(EProObjectFlag)ETOF_GLOBAL_POSITION,WidgetUI.globalPositionCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)ETOF_FACE_ORIENTATION,WidgetUI.faceToUserCB->isChecked())

    Object->SetFlagsWH(flags);

    Object->SetTextWH(WidgetUI.textTE->toPlainText());

    CPoint position;
    position.x = WidgetUI.positionXSB->getInternalValue();
    position.y = WidgetUI.positionYSB->getInternalValue();
    position.z = WidgetUI.positionZSB->getInternalValue();
    Object->SetPositionWH(position);

    CPoint orientation;
    orientation.x = WidgetUI.orientationXSB->getInternalValue();
    orientation.y = WidgetUI.orientationYSB->getInternalValue();
    orientation.z = WidgetUI.orientationZSB->getInternalValue();
    Object->SetOrientationWH(orientation);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTextObjectDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    bool set = ! Object->IsHistoryLocked(EHCL_GRAPHICS);
    WidgetUI.flagsGB->setEnabled(set);
    WidgetUI.textGB->setEnabled(set);
    WidgetUI.positionGB->setEnabled(set);
    WidgetUI.orientationGB->setEnabled(set);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

