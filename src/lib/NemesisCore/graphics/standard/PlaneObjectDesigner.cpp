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
#include <GraphicsObject.hpp>
#include <PhysicalQuantity.hpp>
#include <PhysicalQuantities.hpp>
#include <PointSelection.hpp>

#include <PODesignerGeneral.hpp>
#include <GODesignerObjects.hpp>
#include <GODesignerSetup.hpp>
#include <GODesignerRefBy.hpp>
#include <SelectionList.hpp>

#include <GraphicsProfileObject.hpp>
#include <PlaneObject.hpp>
#include <PlaneObjectDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PlaneObjectDesignerCB(void* p_data);

CExtUUID        PlaneObjectDesignerID(
                    "{PLANE_OBJECT_DESIGNER:08d2fb7d-b499-4e96-b45e-7a14d873f644}",
                    "Plane");

CPluginObject   PlaneObjectDesignerObject(&NemesisCorePlugin,
                    PlaneObjectDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/Plane.svg",
                    PlaneObjectDesignerCB);

// -----------------------------------------------------------------------------

GODESIGNER_ENTRY_POINT(PlaneObject);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPlaneObjectDesigner::CPlaneObjectDesigner(CPlaneObject* p_go)
    : CProObjectDesigner(&PlaneObjectDesignerObject, p_go)
{
    Object = p_go;

    // graphics layout ---------------------------
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Objects = new CGODesignerObjects(WidgetUI.objectsTab,Object,this);
    Setup = new CGODesignerSetup(WidgetUI.setupTab,Object,this);
    RefBy = new CGODesignerRefBy(WidgetUI.refByTab,Object,this);

    // extra setup -------------------------------
    WidgetUI.posObjectW->setProject(Object->GetProject());
    WidgetUI.posObjectW->setSelectionHandler(&SH_Point);

    WidgetUI.dirObjectW->setProject(Object->GetProject());
    WidgetUI.dirObjectW->setSelectionHandler(&SH_Point);

    // units -------------------------------------
    WidgetUI.posXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.posYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.posZSB->setPhysicalQuantity(PQ_DISTANCE);

    WidgetUI.dirXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.dirYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.dirZSB->setPhysicalQuantity(PQ_DISTANCE);

    WidgetUI.sizeSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.rotationSB->setPhysicalQuantity(PQ_ANGLE);

    // connect slots -----------------------------
    connect(WidgetUI.visibleCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.highlightedCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.fitToAtomsCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.doNotShowCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.posXSB, SIGNAL(valueChanged(double)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.posYSB, SIGNAL(valueChanged(double)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.posZSB, SIGNAL(valueChanged(double)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.dirXSB, SIGNAL(valueChanged(double)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.dirYSB, SIGNAL(valueChanged(double)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.dirZSB, SIGNAL(valueChanged(double)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.sizeSB, SIGNAL(valueChanged(double)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.rotationSB, SIGNAL(valueChanged(double)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.angularRB, SIGNAL(clicked(bool)),
            this,SLOT(ChangeToSpherical()));
    //------------------
    connect(WidgetUI.cartesianRB, SIGNAL(clicked(bool)),
            this,SLOT(ChangeToCartesian()));
    //------------------
    connect(WidgetUI.posObjectW, SIGNAL(OnObjectChanged()),
            this,SLOT(PositionObjectChanged()));
    //------------------
    connect(WidgetUI.posObjectW, SIGNAL(OnObjectChanged()),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.dirObjectW, SIGNAL(OnObjectChanged()),
            this,SLOT(DirectionObjectChanged()));
    //------------------
    connect(WidgetUI.dirObjectW, SIGNAL(OnObjectChanged()),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.planeClippingCB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.clippingPlaneIDSB, SIGNAL(valueChanged(int)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.buttonBox1, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(WidgetUI.buttonBox2, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(WidgetUI.buttonBox3, SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(WidgetUI.selectFrontAtomsTB, SIGNAL(clicked(bool)),
            this,SLOT(SelectAtomsAboveFrontFace()));
    //------------------
    connect(Object, SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));
    //------------------
    connect(Object->GetProject()->GetHistory(), SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this, SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // init all values ---------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPlaneObjectDesigner::InitAllValues(void)
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

void CPlaneObjectDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_DESCRIPTION,"background change") == false ) return;

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

void CPlaneObjectDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Reset) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Reset) ||
        (WidgetUI.buttonBox3->standardButton(button) == QDialogButtonBox::Reset) ) {
        InitAllValues();
        return;
    }

    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Apply) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Apply) ||
        (WidgetUI.buttonBox3->standardButton(button) == QDialogButtonBox::Apply) ) {
        ApplyAllValues();
        return;
    }

    if( (WidgetUI.buttonBox1->standardButton(button) == QDialogButtonBox::Close) ||
        (WidgetUI.buttonBox2->standardButton(button) == QDialogButtonBox::Close) ||
        (WidgetUI.buttonBox3->standardButton(button) == QDialogButtonBox::Close) ) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPlaneObjectDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.visibleCB->setChecked(Object->IsFlagSet(EPOF_VISIBLE));
    WidgetUI.highlightedCB->setChecked(Object->IsFlagSet(EPOF_SELECTED));

    WidgetUI.fitToAtomsCB->setChecked(Object->IsFlagSet<EPlaneObjectFlag>(EPOF_FIT_TO_ATOMS));
    WidgetUI.planeClippingCB->setChecked(Object->IsFlagSet<EPlaneObjectFlag>(EPOF_CLIP_PLANE));
    WidgetUI.doNotShowCB->setChecked(Object->IsFlagSet<EPlaneObjectFlag>(EPOF_DO_NOT_SHOW));

    CPoint pos = Object->GetPosition();

    WidgetUI.posXSB->setInternalValue(pos.x);
    WidgetUI.posYSB->setInternalValue(pos.y);
    WidgetUI.posZSB->setInternalValue(pos.z);

    WidgetUI.posObjectW->setObject(Object->GetPositionObject());

    CPoint dir = Object->GetDirection();

    if( WidgetUI.angularRB->isChecked() ){
        dir.ConvertToSpherical();
    }

    WidgetUI.dirXSB->setInternalValue(dir.x);
    WidgetUI.dirYSB->setInternalValue(dir.y);
    WidgetUI.dirZSB->setInternalValue(dir.z);

    WidgetUI.dirObjectW->setObject(Object->GetDirectionObject());

    WidgetUI.sizeSB->setInternalValue(Object->GetSize());
    WidgetUI.rotationSB->setInternalValue(Object->GetRotation());

    WidgetUI.clippingPlaneIDSB->setValue(Object->GetClippingPlaneID());

    // must be here becase posObjectW->setObject, dirObjectW->setObject
    // change this flag to true
    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CPlaneObjectDesigner::ApplyValues(void)
{
    CProObjectFlags flags = Object->GetFlags();

    SET_FLAG(flags,EPOF_VISIBLE,WidgetUI.visibleCB->isChecked())
    SET_FLAG(flags,EPOF_SELECTED,WidgetUI.highlightedCB->isChecked())

    SET_FLAG(flags,(EProObjectFlag)EPOF_FIT_TO_ATOMS,WidgetUI.fitToAtomsCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EPOF_CLIP_PLANE,WidgetUI.planeClippingCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EPOF_DO_NOT_SHOW,WidgetUI.doNotShowCB->isChecked())

    Object->SetFlagsWH(flags);

    CPoint pos;
    pos.x = WidgetUI.posXSB->getInternalValue();
    pos.y = WidgetUI.posYSB->getInternalValue();
    pos.z = WidgetUI.posZSB->getInternalValue();

    Object->SetPositionWH(WidgetUI.posObjectW->getObject());
    if( Object->GetPositionObject() == NULL ){
        Object->SetPositionWH(pos);
    }

    CPoint dir;
    dir.x = WidgetUI.dirXSB->getInternalValue();
    dir.y = WidgetUI.dirYSB->getInternalValue();
    dir.z = WidgetUI.dirZSB->getInternalValue();

    if( WidgetUI.angularRB->isChecked() ){
        dir.ConvertToCartesian();
    }

    Object->SetDirectionWH(WidgetUI.dirObjectW->getObject());
    if( Object->GetDirectionObject() == NULL ){
        Object->SetDirectionWH(dir);
    }

    Object->SetSizeWH(WidgetUI.sizeSB->getInternalValue());
    Object->SetRotationWH(WidgetUI.rotationSB->getInternalValue());

    Object->SetClippingPlaneIDWH(WidgetUI.clippingPlaneIDSB->value());
}

//------------------------------------------------------------------------------

void CPlaneObjectDesigner::PositionObjectChanged(void)
{
    bool set = Object->GetPositionObject() == NULL;
    WidgetUI.posXSB->setEnabled(set);
    WidgetUI.posYSB->setEnabled(set);
    WidgetUI.posZSB->setEnabled(set);
}

//------------------------------------------------------------------------------

void CPlaneObjectDesigner::DirectionObjectChanged(void)
{
    bool set = Object->GetDirectionObject() == NULL;
    WidgetUI.dirXSB->setEnabled(set);
    WidgetUI.dirYSB->setEnabled(set);
    WidgetUI.dirZSB->setEnabled(set);
}

//------------------------------------------------------------------------------

void CPlaneObjectDesigner::ChangeToSpherical(void)
{
    CPoint dir;

    dir.x = WidgetUI.dirXSB->getInternalValue();
    dir.y = WidgetUI.dirYSB->getInternalValue();
    dir.z = WidgetUI.dirZSB->getInternalValue();

    dir.ConvertToSpherical();

    WidgetUI.dirXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.dirYSB->setPhysicalQuantity(PQ_ANGLE);
    WidgetUI.dirZSB->setPhysicalQuantity(PQ_ANGLE);

    WidgetUI.dirXSB->setInternalValue(dir.x);
    WidgetUI.dirYSB->setInternalValue(dir.y);
    WidgetUI.dirZSB->setInternalValue(dir.z);
}

//------------------------------------------------------------------------------

void CPlaneObjectDesigner::ChangeToCartesian(void)
{
    CPoint dir;

    dir.x = WidgetUI.dirXSB->getInternalValue();
    dir.y = WidgetUI.dirYSB->getInternalValue();
    dir.z = WidgetUI.dirZSB->getInternalValue();

    dir.ConvertToCartesian();

    WidgetUI.dirXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.dirYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.dirZSB->setPhysicalQuantity(PQ_DISTANCE);

    WidgetUI.dirXSB->setInternalValue(dir.x);
    WidgetUI.dirYSB->setInternalValue(dir.y);
    WidgetUI.dirZSB->setInternalValue(dir.z);
}

//------------------------------------------------------------------------------

void CPlaneObjectDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;

    bool locked = Object->IsHistoryLocked(EHCL_GRAPHICS);
    WidgetUI.flagsGB->setEnabled(!locked);
    WidgetUI.geometryTab->setEnabled(!locked);
    WidgetUI.otherTab->setEnabled(!locked);
}

//------------------------------------------------------------------------------

void CPlaneObjectDesigner::SelectAtomsAboveFrontFace(void)
{
    CProject* p_proj = Object->GetProject();
    if( p_proj == NULL ) return;
    CPoint dir = Object->GetDirection();
    CPoint pos = Object->GetPosition();
    double d = -VectDot(dir,pos);
    p_proj->GetSelection()->SelectAtomsByPlane(dir.x,dir.y,dir.z,d,true);
    p_proj->RepaintProject();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


