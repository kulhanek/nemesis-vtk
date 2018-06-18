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
#include <PhysicalQuantity.hpp>
#include <PhysicalQuantities.hpp>
#include <PointSelection.hpp>

#include <PODesignerGeneral.hpp>
#include <GODesignerSetup.hpp>
#include <GODesignerRefBy.hpp>

#include <GraphicsProfileObject.hpp>
#include <BoxObject.hpp>
#include <BoxObjectDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* BoxObjectDesignerCB(void* p_data);

CExtUUID        BoxObjectDesignerID(
                    "{BOX_OBJECT_DESIGNER:d35c5137-b5bd-4afd-b65e-d11084a29bd9}",
                    "Box");

CPluginObject   BoxObjectDesignerObject(&NemesisCorePlugin,
                    BoxObjectDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/Box.svg",
                    BoxObjectDesignerCB);

// -----------------------------------------------------------------------------

GODESIGNER_ENTRY_POINT(BoxObject);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBoxObjectDesigner::CBoxObjectDesigner(CBoxObject* p_fmo)
    : CProObjectDesigner(&BoxObjectDesignerObject, p_fmo)
{
    Object = p_fmo;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Setup = new CGODesignerSetup(WidgetUI.setupTab,Object,this);
    RefBy = new CGODesignerRefBy(WidgetUI.refByTab,Object,this);

    // units -------------------------------------
    WidgetUI.posXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.posYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.posZSB->setPhysicalQuantity(PQ_DISTANCE);

    WidgetUI.dimXSB->setPhysicalQuantity(PQ_DIMENSION);
    WidgetUI.dimYSB->setPhysicalQuantity(PQ_DIMENSION);
    WidgetUI.dimZSB->setPhysicalQuantity(PQ_DIMENSION);

    WidgetUI.dirXSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.dirYSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.dirZSB->setPhysicalQuantity(PQ_DISTANCE);

    WidgetUI.rotationSB->setPhysicalQuantity(PQ_ANGLE);

    // extra setup -------------------------------
    WidgetUI.posObjectW->setProject(Object->GetProject());
    WidgetUI.posObjectW->setSelectionHandler(&SH_Point);

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
    connect(WidgetUI.cornerPosRB, SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.centerPosRB, SIGNAL(clicked(bool)),
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
    connect(WidgetUI.dimXSB, SIGNAL(valueChanged(double)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.dimYSB, SIGNAL(valueChanged(double)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.dimZSB, SIGNAL(valueChanged(double)),
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
    connect(WidgetUI.rotationSB, SIGNAL(valueChanged(double)),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.angularRB, SIGNAL(clicked(bool)),
            this,SLOT(ChangeToSpherical()));
    //------------------
    connect(WidgetUI.cartesianRB, SIGNAL(clicked(bool)),
            this,SLOT(ChangeToCartesian()));
    //------------------
    connect(WidgetUI.dirObjectW, SIGNAL(OnObjectChanged()),
            this,SLOT(DirectionObjectChanged()));
    //------------------
    connect(WidgetUI.dirObjectW, SIGNAL(OnObjectChanged()),
            this,SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.posObjectW, SIGNAL(OnObjectChanged()),
            this,SLOT(PositionObjectChanged()));
    //------------------
    connect(WidgetUI.posObjectW, SIGNAL(OnObjectChanged()),
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

void CBoxObjectDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();
    Setup->InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CBoxObjectDesigner::ApplyAllValues(void)
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

void CBoxObjectDesigner::ButtonBoxClicked(QAbstractButton *button)
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

void CBoxObjectDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    WidgetUI.visibleCB->setChecked(Object->IsFlagSet(EPOF_VISIBLE));
    WidgetUI.selectedCB->setChecked(Object->IsFlagSet(EPOF_SELECTED));
    WidgetUI.showCenterCB->setChecked(Object->IsFlagSet<EBoxObjectFlag>(EBCBOF_SHOW_CENTER));

    if( Object->IsFlagSet<EBoxObjectFlag>(EBCBOF_ORIGIN_IN_CENTER) ){
        WidgetUI.centerPosRB->setChecked(true);
    } else {
        WidgetUI.cornerPosRB->setChecked(true);
    }

    CPoint pos = Object->GetPosition();
    WidgetUI.posXSB->setInternalValue(pos.x);
    WidgetUI.posYSB->setInternalValue(pos.y);
    WidgetUI.posZSB->setInternalValue(pos.z);
    WidgetUI.posObjectW->setObject(Object->GetPositionObject());

    CPoint dim = Object->GetDimensions();
    WidgetUI.dimXSB->setInternalValue(dim.x);
    WidgetUI.dimYSB->setInternalValue(dim.y);
    WidgetUI.dimZSB->setInternalValue(dim.z);

    WidgetUI.rotationSB->setInternalValue(Object->GetRotation());

    CPoint dir = Object->GetDirection();

    if( WidgetUI.angularRB->isChecked() ){
        dir.ConvertToSpherical();
    }
    WidgetUI.dirXSB->setInternalValue(dir.x);
    WidgetUI.dirYSB->setInternalValue(dir.y);
    WidgetUI.dirZSB->setInternalValue(dir.z);

    WidgetUI.dirObjectW->setObject(Object->GetDirectionObject());

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CBoxObjectDesigner::ApplyValues(void)
{
    CProObjectFlags flags = Object->GetFlags();

    SET_FLAG(flags,EPOF_VISIBLE,WidgetUI.visibleCB->isChecked())
    SET_FLAG(flags,EPOF_SELECTED,WidgetUI.selectedCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EBCBOF_SHOW_CENTER,WidgetUI.showCenterCB->isChecked())
    SET_FLAG(flags,(EProObjectFlag)EBCBOF_ORIGIN_IN_CENTER,WidgetUI.centerPosRB->isChecked())

    Object->SetFlagsWH(flags);

    CPoint pos;
    pos.x = WidgetUI.posXSB->getInternalValue();
    pos.y = WidgetUI.posYSB->getInternalValue();
    pos.z = WidgetUI.posZSB->getInternalValue();

    Object->SetPositionWH(WidgetUI.posObjectW->getObject());
    if( Object->GetPositionObject() == NULL ){
        Object->SetPositionWH(pos);
    }

    CPoint dim;
    dim.x = WidgetUI.dimXSB->getInternalValue();
    dim.y = WidgetUI.dimYSB->getInternalValue();
    dim.z = WidgetUI.dimZSB->getInternalValue();

    Object->SetDimensionsWH(dim);
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

    Object->SetRotationWH(WidgetUI.rotationSB->getInternalValue());
}

//------------------------------------------------------------------------------

void CBoxObjectDesigner::ChangeToSpherical(void)
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

void CBoxObjectDesigner::ChangeToCartesian(void)
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

void CBoxObjectDesigner::PositionObjectChanged(void)
{
    bool set = Object->GetPositionObject() == NULL;
    WidgetUI.posXSB->setEnabled(set);
    WidgetUI.posYSB->setEnabled(set);
    WidgetUI.posZSB->setEnabled(set);
}

//------------------------------------------------------------------------------

void CBoxObjectDesigner::DirectionObjectChanged(void)
{
    bool set = Object->GetDirectionObject() == NULL;
    WidgetUI.dirXSB->setEnabled(set);
    WidgetUI.dirYSB->setEnabled(set);
    WidgetUI.dirZSB->setEnabled(set);
}

//------------------------------------------------------------------------------

void CBoxObjectDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    WidgetUI.flagsGB->setEnabled(! Object->IsHistoryLocked(EHCL_GRAPHICS));
    WidgetUI.geometryTab->setEnabled(! Object->IsHistoryLocked(EHCL_GRAPHICS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

