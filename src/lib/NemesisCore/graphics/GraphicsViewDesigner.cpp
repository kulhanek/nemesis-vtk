// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ContainerModel.hpp>
#include <GraphicsView.hpp>
#include <GraphicsProfileList.hpp>
#include <MainWindow.hpp>
#include <PluginDatabase.hpp>
#include <WorkPanel.hpp>
#include <PhysicalQuantity.hpp>
#include <PhysicalQuantities.hpp>

#include <PODesignerGeneral.hpp>

#include <GraphicsViewDesigner.hpp>
#include "GraphicsViewDesigner.moc"

// undef some strange windows macros
#ifdef WIN32
#undef near
#undef focal
#undef far
#undef fovy
#endif

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GraphicsViewDesignerCB(void* p_data);

CExtUUID        GraphicsViewDesignerID(
                    "{GRAPHICS_VIEW_DESIGNER:3fb5fdaa-ad95-4ea3-aa4b-bfd37baba051}",
                    "View");

CPluginObject   GraphicsViewDesignerObject(&NemesisCorePlugin,
                    GraphicsViewDesignerID,WORK_PANEL_CAT,
                    ":/images/NemesisCore/graphics/View.svg",
                    GraphicsViewDesignerCB);

// -----------------------------------------------------------------------------

QObject* GraphicsViewDesignerCB(void* p_data)
{
    CGraphicsView* p_gp = static_cast<CGraphicsView*>(p_data);
    QObject* p_object = new CGraphicsViewDesigner(p_gp);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsViewDesigner::CGraphicsViewDesigner(CGraphicsView* p_gp)
    : CProObjectDesigner(&GraphicsViewDesignerObject,p_gp)
{
    Object = p_gp;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);

    // units -------------------------------------
    WidgetUI.cxSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.cySB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.czSB->setPhysicalQuantity(PQ_DISTANCE);

    WidgetUI.txSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.tySB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.tzSB->setPhysicalQuantity(PQ_DISTANCE);

    WidgetUI.raSB->setPhysicalQuantity(PQ_ANGLE);
    WidgetUI.rbSB->setPhysicalQuantity(PQ_ANGLE);
    WidgetUI.rgSB->setPhysicalQuantity(PQ_ANGLE);

    // connect slots -----------------------------
    connect(WidgetUI.synchronizeWithPrimaryViewCB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.attachCanvasPB, SIGNAL(clicked(bool)),
            this, SLOT(AttachCanvas()));
    //------------------
    connect(WidgetUI.profilesCB, SIGNAL(currentIndexChanged(int)),
           this,  SLOT(SetChangedFlagTrue()));

    //--------------
    connect(WidgetUI.perspectiveRB,SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue(void)));
    //--------------
    connect(WidgetUI.orthographicRB,SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue(void)));
    //------------------
    connect(WidgetUI.nearPlaneSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.focalPlaneSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.farPlaneSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.fovSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));

    //--------------
    connect(WidgetUI.stereoModeCB,SIGNAL(currentIndexChanged(int)),
            this,SLOT(SetChangedFlagTrue(void)));
    //--------------
    connect(WidgetUI.stereoSwapEyesCB,SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue(void)));
    //------------------
    connect(WidgetUI.eyeSepSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));

    //--------------
    connect(WidgetUI.depthCueingCB,SIGNAL(clicked(bool)),
            this,SLOT(SetChangedFlagTrue(void)));
    //------------------
    connect(WidgetUI.dcDensitySB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.dcStartSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.dcEndSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.dcColorPB, SIGNAL(clicked(bool)),
            this, SLOT(SetChangedFlagTrue()));
    //------------------
    connect(WidgetUI.dcModeCB, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SetChangedFlagTrue()));

    //------------------
    connect(WidgetUI.manipAutofitPB, SIGNAL(clicked(bool)),
            this, SLOT(ManipAutofit()));
    //------------------
    connect(WidgetUI.manipResetPB, SIGNAL(clicked(bool)),
            this, SLOT(ManipReset()));

    //------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            this, SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(Object, SIGNAL(OnStatusChanged(EStatusChanged)),
            this, SLOT(InitValues()));
    //------------------
    connect(Object, SIGNAL(OnManipulatorChanged(void)),
            this, SLOT(ManipulatorChanged()));

    // init all values ---------------------------
    InitAllValues();
    ManipulatorChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsViewDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGraphicsViewDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    Changing = true;
        General->ApplyValues();
        ApplyValues();
    Changing = false;

    Object->GetProject()->RepaintProject();

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in above
}

//------------------------------------------------------------------------------

void CGraphicsViewDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Reset ) {
        InitAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Apply  ) {
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

void CGraphicsViewDesigner::InitValues(void)
{
    if( IsItChangingContent() ) return;

    // visualization ---------------------------------------
    WidgetUI.synchronizeWithPrimaryViewCB->setChecked(Object->IsSynchronizedWithPrimaryView());
    if( Object->IsPrimaryView() ){
        WidgetUI.synchronizeWithPrimaryViewCB->setEnabled(false);
        WidgetUI.attachCanvasPB->setEnabled(false);
        WidgetUI.attachedLA->setText(tr("primary view"));
    } else {
        WidgetUI.synchronizeWithPrimaryViewCB->setEnabled(true);
        if( Object->IsOpenGLCanvasAttached() ){
            WidgetUI.attachCanvasPB->setEnabled(false);
            WidgetUI.attachedLA->setText(tr("attached"));
        } else {
            WidgetUI.attachCanvasPB->setEnabled(true);
            WidgetUI.attachedLA->setText(tr("not attached"));
        }
    }

    WidgetUI.profilesCB->clear();
    WidgetUI.profilesCB->addItem(tr("active profile"));
    int pindx = 0;
    int cindx = 0;
    foreach(QObject* p_qobj, Object->GetProfiles()->children()){
        CGraphicsProfile* p_profile = static_cast<CGraphicsProfile*>(p_qobj);
        pindx++;
        WidgetUI.profilesCB->addItem(p_profile->GetName());
        if( p_profile == Object->GetUsedProfile() ){
            cindx = pindx;
        }
    }
    WidgetUI.profilesCB->setCurrentIndex(cindx);

    // projection ------------------------------------------
    switch(Object->GetProjectionMode()){
        case EPM_PERSPECTIVE:
            WidgetUI.perspectiveRB->setChecked(true);
            break;
        case EPM_ORTHOGRAPHIC:
            WidgetUI.orthographicRB->setChecked(true);
            break;
    }

    double near,focal,far,fov;
    Object->GetProjectionData(near,focal,far,fov);
    WidgetUI.nearPlaneSB->setValue(near);
    WidgetUI.focalPlaneSB->setValue(focal);
    WidgetUI.farPlaneSB->setValue(far);
    WidgetUI.fovSB->setValue(fov);

    // stereo ----------------------------------------------
    WidgetUI.stereoModeCB->clear();
    int cindex = 0;
    int aindex = 0;
    for(int i=0; i < CGraphicsViewStereo::GetNumberOfStereoModes(); i++){
        EStereoMode esm = static_cast<EStereoMode>(i);
        if( CGraphicsViewStereo::IsStereoModeAvailable(esm) ) {
            WidgetUI.stereoModeCB->addItem(CGraphicsViewStereo::GetStereoModeDescription(esm),QVariant(i));
            if( esm == Object->GetStereoMode() ) aindex = cindex;
            cindex++;
        }
    }
    WidgetUI.stereoModeCB->setCurrentIndex(aindex);

    WidgetUI.stereoSwapEyesCB->setEnabled(true);
    WidgetUI.stereoSwapEyesCB->setChecked(Object->AreStereoEyesSwapped());

    double esep;
    esep = Object->GetEyeSeparation();
    WidgetUI.eyeSepSB->setValue(esep);

    // depth cueing ---------------------------------------
    WidgetUI.depthCueingCB->setEnabled(true);
    WidgetUI.depthCueingCB->setChecked(Object->IsDepthCueingEnabled());

    GLenum dcm;
    double dcd,dcs,dce;
    CColor dcc;
    Object->GetDepthCueingData(dcm,dcd,dcs,dce,dcc);

    WidgetUI.dcDensitySB->setValue(dcd);
    WidgetUI.dcStartSB->setValue(dcs);
    WidgetUI.dcEndSB->setValue(dce);
    WidgetUI.dcColorPB->setColor(dcc);

    switch(dcm){
        case GL_LINEAR:
            WidgetUI.dcModeCB->setCurrentIndex(0);
            break;
        case GL_EXP:
            WidgetUI.dcModeCB->setCurrentIndex(1);
            break;
        case GL_EXP2:
            WidgetUI.dcModeCB->setCurrentIndex(2);
            break;
    }

    SetChangedFlag(false);

    // opengl
    WidgetUI.quadStereoCB->setChecked(Object->IsQuadBufferStereoAvailable());
    WidgetUI.multisamplingCB->setChecked(Object->IsMultiSamplingAvailable());
}

//------------------------------------------------------------------------------

void CGraphicsViewDesigner::ApplyValues(void)
{
    // visualization ---------------------------------------
    Object->SyncWithPrimaryView(WidgetUI.synchronizeWithPrimaryViewCB->isChecked());

    int pindx = 0;
    CGraphicsProfile* p_actprof = NULL;
    foreach(QObject* p_qobj, Object->GetProfiles()->children()){
        CGraphicsProfile* p_profile = static_cast<CGraphicsProfile*>(p_qobj);
        pindx++;
        if( pindx == WidgetUI.profilesCB->currentIndex() ){
            p_actprof = p_profile;
            break;
        }
    }
    Object->SetUsedProfile(p_actprof);

    // projection ------------------------------------------
    if( WidgetUI.perspectiveRB->isChecked() ){
        Object->SetProjectionMode(EPM_PERSPECTIVE);
    }
    if( WidgetUI.orthographicRB->isChecked() ){
        Object->SetProjectionMode(EPM_ORTHOGRAPHIC);
    }

    double near,focal,far,fov;
    near = WidgetUI.nearPlaneSB->value();
    focal = WidgetUI.focalPlaneSB->value();
    far = WidgetUI.farPlaneSB->value();
    fov = WidgetUI.fovSB->value();
    Object->SetProjectionData(near,focal,far,fov);

    // stereo ----------------------------------------------
    QVariant idata = WidgetUI.stereoModeCB->itemData(WidgetUI.stereoModeCB->currentIndex());
    EStereoMode esm = static_cast<EStereoMode>(idata.toInt());
    Object->SetStereoMode(esm);
    Object->StereoSwapEyes(WidgetUI.stereoSwapEyesCB->isChecked());

    double esep;
    esep = WidgetUI.eyeSepSB->value();
    Object->SetEyeSeparation(esep);

    // depth cueing ---------------------------------------
    Object->SetDepthCueing(WidgetUI.depthCueingCB->isChecked());

    GLenum dcm;
    double dcd,dcs,dce;
    CColor dcc;

    dcd = WidgetUI.dcDensitySB->value();
    dcs = WidgetUI.dcStartSB->value();
    dce = WidgetUI.dcEndSB->value();
    dcc = WidgetUI.dcColorPB->getCColor();

    switch(WidgetUI.dcModeCB->currentIndex()){
        case 0:
            dcm = GL_LINEAR;
            break;
        case 1:
            dcm = GL_EXP;
            break;
        case 2:
            dcm = GL_EXP2;
            break;
    }

    Object->SetDepthCueingData(dcm,dcd,dcs,dce,dcc);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsViewDesigner::AttachCanvas(void)
{
    Object->AttachShadowView();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsViewDesigner::ManipulatorChanged(void)
{

    CPoint c = Object->GetCentrum();
    WidgetUI.cxSB->setInternalValue(c.x);
    WidgetUI.cySB->setInternalValue(c.y);
    WidgetUI.czSB->setInternalValue(c.z);

    CPoint t = Object->GetPos();
    WidgetUI.txSB->setInternalValue(t.x);
    WidgetUI.tySB->setInternalValue(t.y);
    WidgetUI.tzSB->setInternalValue(t.z);

    CPoint r = Object->GetEulerAngles();
    WidgetUI.raSB->setInternalValue(r.x);
    WidgetUI.rbSB->setInternalValue(r.y);
    WidgetUI.rgSB->setInternalValue(r.z);

    WidgetUI.sSB->setValue(Object->GetScale());
}

//------------------------------------------------------------------------------

void CGraphicsViewDesigner::ManipAutofit(void)
{
    Object->FitScene();
}

//------------------------------------------------------------------------------

void CGraphicsViewDesigner::ManipReset(void)
{
    Object->ResetManip();
    Object->GetProject()->RepaintProject();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
