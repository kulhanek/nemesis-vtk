// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <GraphicsProfileList.hpp>
#include <GraphicsObjectList.hpp>

#include <PODesignerGeneral.hpp>
#include <GODesignerFlags.hpp>
#include <GODesignerObjects.hpp>
#include <GODesignerSetup.hpp>
#include <GODesignerRefBy.hpp>

#include <PhysicalQuantities.hpp>
#include <PointObject.hpp>
#include <Graphics.hpp>
#include <GraphicsProfile.hpp>
#include <VectorObject.hpp>

#include <GraphicsProfileObject.hpp>
#include <SpecAxesObject.hpp>
#include <SpecAxesObjectDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* SpecAxesObjectDesignerCB(void* p_data);

CExtUUID        SpecAxesObjectDesignerID(
                    "{SPEC_AXES_OBJECT_DESIGNER:d0800eb9-7864-4dcd-95fc-19af85f5372e}",
                    "Special Axes");

CPluginObject   SpecAxesObjectDesignerObject(&NemesisCorePlugin,
                    SpecAxesObjectDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/objects/SpecAxes.svg",
                    SpecAxesObjectDesignerCB);

// -----------------------------------------------------------------------------

GODESIGNER_ENTRY_POINT(SpecAxesObject);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSpecAxesObjectDesigner::CSpecAxesObjectDesigner(CSpecAxesObject* p_bo)
    : CProObjectDesigner(&SpecAxesObjectDesignerObject, p_bo)
{
    Object = p_bo;

    // graphics layout ---------------------------
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);
    Objects = new CGODesignerObjects(WidgetUI.objectsTab,Object,this);
    Setup = new CGODesignerSetup(WidgetUI.setupTab,Object,this);
    RefBy = new CGODesignerRefBy(WidgetUI.refByTab,Object,this);

    // units -------------------------------------
    WidgetUI.comXLA->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.comYLA->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.comZLA->setPhysicalQuantity(PQ_DISTANCE);

    // connect slots -----------------------------
    connect(WidgetUI.visibleCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    //------------------
    connect(WidgetUI.highlightedCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    //------------------
    connect(WidgetUI.applyMagOnVectorsCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    //------------------
    connect(WidgetUI.massWeightedCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    //------------------
    connect(WidgetUI.typeCB, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SetChangedFlagTrue(void)));
    //------------------
    connect(WidgetUI.eigenVectorAShowCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    //------------------
    connect(WidgetUI.eigenVectorBShowCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    //------------------
    connect(WidgetUI.eigenVectorCShowCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue(void)));
    //------------------
    connect(WidgetUI.showCOMAsPointPB, SIGNAL(clicked(bool)),
            this, SLOT(ShowCOMAsPoint(void)));
    //------------------
    connect(WidgetUI.showAAsVectorPB, SIGNAL(clicked(bool)),
            this, SLOT(ShowAAsVector(void)));
    //------------------
    connect(WidgetUI.showBAsVectorPB, SIGNAL(clicked(bool)),
            this, SLOT(ShowBAsVector(void)));
    //------------------
    connect(WidgetUI.showCAsVectorPB, SIGNAL(clicked(bool)),
            this, SLOT(ShowCAsVector(void)));
    //------------------
    connect(WidgetUI.buttonBox_1, SIGNAL(clicked (QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(WidgetUI.buttonBox_2, SIGNAL(clicked (QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(Object->GetProject()->GetHistory(), SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this, SLOT(ProjectLockChanged(EHistoryChangeMessage)));
    //------------------
    connect(Object, SIGNAL(OnStatusChanged(EStatusChanged)),
            this, SLOT(InitValues()));

    // init all values ---------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSpecAxesObjectDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    Setup->InitValues();
    RefBy->InitValues();
    InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CSpecAxesObjectDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_DESCRIPTION,"special axes change") == false ) return;

    Changing = true;
        General->ApplyValues();
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

void CSpecAxesObjectDesigner::InitValues(void)
{
    if( IsItChangingContent() == true ) return;

    bool set = Object->AreDataValid();

    WidgetUI.visibleCB->setChecked(Object->IsFlagSet(EPOF_VISIBLE));
    WidgetUI.highlightedCB->setChecked(Object->IsFlagSet(EPOF_SELECTED));
    WidgetUI.massWeightedCB->setChecked(Object->IsFlagSet<ESpecAxesObjectFlag>(ESAOF_MASS_WEIGHTED));
    WidgetUI.applyMagOnVectorsCB->setChecked(Object->IsFlagSet<ESpecAxesObjectFlag>(ESAOF_APPLY_MAGNITUDES));

    WidgetUI.typeCB->setCurrentIndex(Object->GetMode());

    WidgetUI.eigenVectorAXLE->setPhysicalQuantity(Object->GetVectorComponentPQ());
    WidgetUI.eigenVectorAYLE->setPhysicalQuantity(Object->GetVectorComponentPQ());
    WidgetUI.eigenVectorAZLE->setPhysicalQuantity(Object->GetVectorComponentPQ());

    WidgetUI.eigenVectorBXLE->setPhysicalQuantity(Object->GetVectorComponentPQ());
    WidgetUI.eigenVectorBYLE->setPhysicalQuantity(Object->GetVectorComponentPQ());
    WidgetUI.eigenVectorBZLE->setPhysicalQuantity(Object->GetVectorComponentPQ());

    WidgetUI.eigenVectorCXLE->setPhysicalQuantity(Object->GetVectorComponentPQ());
    WidgetUI.eigenVectorCYLE->setPhysicalQuantity(Object->GetVectorComponentPQ());
    WidgetUI.eigenVectorCZLE->setPhysicalQuantity(Object->GetVectorComponentPQ());

    WidgetUI.eigenValueALE->setPhysicalQuantity(Object->GetEigenValuePQ());
    WidgetUI.eigenValueBLE->setPhysicalQuantity(Object->GetEigenValuePQ());
    WidgetUI.eigenValueCLE->setPhysicalQuantity(Object->GetEigenValuePQ());

    WidgetUI.comXLA->setValidData(set);
    WidgetUI.comYLA->setValidData(set);
    WidgetUI.comZLA->setValidData(set);

    WidgetUI.eigenValueALE->setValidData(set);
    WidgetUI.eigenVectorAXLE->setValidData(set);
    WidgetUI.eigenVectorAYLE->setValidData(set);
    WidgetUI.eigenVectorAZLE->setValidData(set);

    WidgetUI.eigenValueBLE->setValidData(set);
    WidgetUI.eigenVectorBXLE->setValidData(set);
    WidgetUI.eigenVectorBYLE->setValidData(set);
    WidgetUI.eigenVectorBZLE->setValidData(set);

    WidgetUI.eigenValueCLE->setValidData(set);
    WidgetUI.eigenVectorCXLE->setValidData(set);
    WidgetUI.eigenVectorCYLE->setValidData(set);
    WidgetUI.eigenVectorCZLE->setValidData(set);

    WidgetUI.showCOMAsPointPB->setEnabled(set);
    WidgetUI.showAAsVectorPB->setEnabled(set);
    WidgetUI.showBAsVectorPB->setEnabled(set);
    WidgetUI.showCAsVectorPB->setEnabled(set);

    CPoint com = Object->GetCOM();
    WidgetUI.comXLA->setInternalValue(com.x);
    WidgetUI.comYLA->setInternalValue(com.y);
    WidgetUI.comZLA->setInternalValue(com.z);

    WidgetUI.eigenValueALE->setInternalValue(Object->GetEigenValueA());
    CPoint a = Object->GetEigenVectorA();
    WidgetUI.eigenVectorAXLE->setInternalValue(a.x);
    WidgetUI.eigenVectorAYLE->setInternalValue(a.y);
    WidgetUI.eigenVectorAZLE->setInternalValue(a.z);

    WidgetUI.eigenValueBLE->setInternalValue(Object->GetEigenValueB());
    CPoint b = Object->GetEigenVectorB();
    WidgetUI.eigenVectorBXLE->setInternalValue(b.x);
    WidgetUI.eigenVectorBYLE->setInternalValue(b.y);
    WidgetUI.eigenVectorBZLE->setInternalValue(b.z);

    WidgetUI.eigenValueCLE->setInternalValue(Object->GetEigenValueC());
    CPoint c = Object->GetEigenVectorC();
    WidgetUI.eigenVectorCXLE->setInternalValue(c.x);
    WidgetUI.eigenVectorCYLE->setInternalValue(c.y);
    WidgetUI.eigenVectorCZLE->setInternalValue(c.z);

    WidgetUI.eigenVectorAShowCB->setChecked(Object->IsFlagSet<ESpecAxesObjectFlag>(ESAOF_SHOW_A));
    WidgetUI.eigenVectorBShowCB->setChecked(Object->IsFlagSet<ESpecAxesObjectFlag>(ESAOF_SHOW_B));
    WidgetUI.eigenVectorCShowCB->setChecked(Object->IsFlagSet<ESpecAxesObjectFlag>(ESAOF_SHOW_C));

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CSpecAxesObjectDesigner::ApplyValues(void)
{
    CProObjectFlags flags = Object->GetFlags();

    SET_FLAG(flags,EPOF_VISIBLE,WidgetUI.visibleCB->isChecked());
    SET_FLAG(flags,EPOF_SELECTED,WidgetUI.highlightedCB->isChecked());
    SET_FLAG(flags,(EProObjectFlag)ESAOF_MASS_WEIGHTED,WidgetUI.massWeightedCB->isChecked());
    SET_FLAG(flags,(EProObjectFlag)ESAOF_APPLY_MAGNITUDES,WidgetUI.applyMagOnVectorsCB->isChecked());
    SET_FLAG(flags,(EProObjectFlag)ESAOF_SHOW_A,WidgetUI.eigenVectorAShowCB->isChecked());
    SET_FLAG(flags,(EProObjectFlag)ESAOF_SHOW_B,WidgetUI.eigenVectorBShowCB->isChecked());
    SET_FLAG(flags,(EProObjectFlag)ESAOF_SHOW_C,WidgetUI.eigenVectorCShowCB->isChecked());
    Object->SetFlagsWH(flags);

    Object->SetModeWH(static_cast<ESpecAxesObjectMode>(WidgetUI.typeCB->currentIndex()));
}

//------------------------------------------------------------------------------

void CSpecAxesObjectDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;

    bool set = ! Object->IsHistoryLocked(EHCL_GRAPHICS);
    WidgetUI.flagsGB->setEnabled(set);
    WidgetUI.dataTab->setEnabled(set);
}

//------------------------------------------------------------------------------

void CSpecAxesObjectDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( (WidgetUI.buttonBox_1->standardButton(button) == QDialogButtonBox::Reset) ||
        (WidgetUI.buttonBox_2->standardButton(button) == QDialogButtonBox::Reset) ) {
        InitAllValues();
        return;
    }

    if( (WidgetUI.buttonBox_1->standardButton(button) == QDialogButtonBox::Apply) ||
        (WidgetUI.buttonBox_2->standardButton(button) == QDialogButtonBox::Apply) ) {
        ApplyAllValues();
        return;
    }

    if( (WidgetUI.buttonBox_1->standardButton(button) == QDialogButtonBox::Close) ||
        (WidgetUI.buttonBox_2->standardButton(button) == QDialogButtonBox::Close) ) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSpecAxesObjectDesigner::ShowCOMAsPoint(void)
{
    CGraphics* p_graphics = NULL;
    if( Object->GetProject() ){
        p_graphics = Object->GetProject()->GetGraphics();
    }
    if( p_graphics == NULL ) return;

    CGraphicsProfile* p_profile = p_graphics->GetProfiles()->GetActiveProfile();
    if( p_profile == NULL ){
        ES_ERROR("no active profile");
        return;
    }

    CExtUUID point_id("{POINT_OBJECT:089f4df8-cf38-48a2-84e1-f0bac7aa6ffe}");

    CHistoryNode* p_hnode = Object->BeginChangeWH(EHCL_GRAPHICS,tr("show COM as point"));
    if( p_hnode == NULL ) return;

        CGraphicsObject* p_obj = p_graphics->GetObjects()->CreateObject(point_id,QString(),QString(),p_hnode);
        if( p_obj == NULL ) {
            CSmallString error;
            error << "unable to create object " << point_id.GetStringForm();
            RUNTIME_ERROR(error);
        }
        p_obj->AddObject(Object,p_hnode);
        p_profile->AddObject(p_obj,-1,p_hnode);

        CPointObject* p_point = dynamic_cast<CPointObject*>(p_obj);
        if( p_point ){
            p_point->SetPosition(Object->GetCOM(),p_hnode);
        }

    Object->EndChangeWH();
}

//------------------------------------------------------------------------------

void CSpecAxesObjectDesigner::ShowAAsVector(void)
{
    CGraphics* p_graphics = NULL;
    if( Object->GetProject() ){
        p_graphics = Object->GetProject()->GetGraphics();
    }
    if( p_graphics == NULL ) return;

    CGraphicsProfile* p_profile = p_graphics->GetProfiles()->GetActiveProfile();
    if( p_profile == NULL ){
        ES_ERROR("no active profile");
        return;
    }

    CExtUUID vector_id("{VECTOR_OBJECT:970f766f-c637-4107-8b25-2d76f558840f}");

    CHistoryNode* p_hnode = Object->BeginChangeWH(EHCL_GRAPHICS,tr("show A as vector"));
    if( p_hnode == NULL ) return;

        CGraphicsObject* p_obj = p_graphics->GetObjects()->CreateObject(vector_id,QString(),QString(),p_hnode);
        if( p_obj == NULL ) {
            CSmallString error;
            error << "unable to create object " << vector_id.GetStringForm();
            RUNTIME_ERROR(error);
        }
        p_obj->AddObject(Object,p_hnode);
        p_profile->AddObject(p_obj,-1,p_hnode);

        CVectorObject* p_vector = dynamic_cast<CVectorObject*>(p_obj);
        if( p_vector ){
            p_vector->SetPosition(Object->GetCOM(),p_hnode);
            p_vector->SetDirection(Object->GetEigenVectorA(),p_hnode);
        }

    Object->EndChangeWH();
}

//------------------------------------------------------------------------------

void CSpecAxesObjectDesigner::ShowBAsVector(void)
{
    CGraphics* p_graphics = NULL;
    if( Object->GetProject() ){
        p_graphics = Object->GetProject()->GetGraphics();
    }
    if( p_graphics == NULL ) return;

    CGraphicsProfile* p_profile = p_graphics->GetProfiles()->GetActiveProfile();
    if( p_profile == NULL ){
        ES_ERROR("no active profile");
        return;
    }

    CExtUUID vector_id("{VECTOR_OBJECT:970f766f-c637-4107-8b25-2d76f558840f}");

    CHistoryNode* p_hnode = Object->BeginChangeWH(EHCL_GRAPHICS,tr("show B as vector"));
    if( p_hnode == NULL ) return;

        CGraphicsObject* p_obj = p_graphics->GetObjects()->CreateObject(vector_id,QString(),QString(),p_hnode);
        if( p_obj == NULL ) {
            CSmallString error;
            error << "unable to create object " << vector_id.GetStringForm();
            RUNTIME_ERROR(error);
        }
        p_obj->AddObject(Object,p_hnode);
        p_profile->AddObject(p_obj,-1,p_hnode);

        CVectorObject* p_vector = dynamic_cast<CVectorObject*>(p_obj);
        if( p_vector ){
            p_vector->SetPosition(Object->GetCOM(),p_hnode);
            p_vector->SetDirection(Object->GetEigenVectorB(),p_hnode);
        }

    Object->EndChangeWH();
}

//------------------------------------------------------------------------------

void CSpecAxesObjectDesigner::ShowCAsVector(void)
{
    CGraphics* p_graphics = NULL;
    if( Object->GetProject() ){
        p_graphics = Object->GetProject()->GetGraphics();
    }
    if( p_graphics == NULL ) return;

    CGraphicsProfile* p_profile = p_graphics->GetProfiles()->GetActiveProfile();
    if( p_profile == NULL ){
        ES_ERROR("no active profile");
        return;
    }

    CExtUUID vector_id("{VECTOR_OBJECT:970f766f-c637-4107-8b25-2d76f558840f}");

    CHistoryNode* p_hnode = Object->BeginChangeWH(EHCL_GRAPHICS,tr("show C as vector"));
    if( p_hnode == NULL ) return;

        CGraphicsObject* p_obj = p_graphics->GetObjects()->CreateObject(vector_id,QString(),QString(),p_hnode);
        if( p_obj == NULL ) {
            CSmallString error;
            error << "unable to create object " << vector_id.GetStringForm();
            RUNTIME_ERROR(error);
        }
        p_obj->AddObject(Object,p_hnode);
        p_profile->AddObject(p_obj,-1,p_hnode);

        CVectorObject* p_vector = dynamic_cast<CVectorObject*>(p_obj);
        if( p_vector ){
            p_vector->SetPosition(Object->GetCOM(),p_hnode);
            p_vector->SetDirection(Object->GetEigenVectorC(),p_hnode);
        }

    Object->EndChangeWH();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


