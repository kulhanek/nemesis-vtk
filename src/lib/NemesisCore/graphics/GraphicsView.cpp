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

#include <GraphicsView.hpp>
#include <ErrorSystem.hpp>
#include <PluginDatabase.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <XMLElement.hpp>
#include <Graphics.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLSelection.hpp>
#include <QGLPixelBuffer>
#include <GraphicsViewList.hpp>
#include <GraphicsProfileList.hpp>
#include <WorkPanel.hpp>
#include <GraphicsShadowView.hpp>
#include <FTGLFontCache.hpp>

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

CExtUUID        GraphicsViewID(
                    "{GRAPHICS_VIEW:ec35dc44-7f02-4814-b3b2-c712f258aec6}",
                    "View");

CPluginObject   GraphicsViewObject(&NemesisCorePlugin,
                    GraphicsViewID,GRAPHICS_CAT,
                    ":/images/NemesisCore/graphics/View.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsView::CGraphicsView(CGraphicsViewList* p_owner,bool primary)
    : CProObject(&GraphicsViewObject,p_owner,p_owner->GetProject(),primary)
{
    ConFlags |= EECOF_SUB_CONTAINERS;
    ConFlags |= EECOF_DYNAMICAL_CONTENTS;

    Profile = NULL;
    DrawGLCanvas = NULL;
    SyncToPrimaryView = false;
    SelAreaSize = 4;
    SelBuffSize = 50000;
    SelBuffer = new GLuint[SelBuffSize];

    FitSceneTimer = new QTimer;
    connect(FitSceneTimer,SIGNAL(timeout(void)),
            this,SLOT(FitSceneTick(void)));

    ProjectionMode = EPM_PERSPECTIVE;

    // setting camera properties
    Near    =    1;         // near plane
    Focal   =   25;         // focal plane
    Far     =  400;         // far plane
    Fovy    =   45;         // horizontal field of view
    EyeSep  =    5;         // eye separation

    StereoMode = ESM_OFF;
    SwapEyes = false;

    Position.x =  Focal;
    Position.y =  0.0;
    Position.z =  0.0;

    Reference.x =   0.0;
    Reference.y =   0.0;
    Reference.z =   0.0;

    ViewUp.x = 0;
    ViewUp.y = 0;
    ViewUp.z = 1;

    Width = 0;
    Height = 0;

    DepthCueing = false;
    DCMode = GL_EXP2;
    DCDensity = 0.02;
    DCColor.SetRGB(1.,1.,1.);

    ShadowView = NULL;

    connect(GetProfiles(),SIGNAL(OnGraphicsProfileListChanged(void)),
            this,SLOT(GraphicsProfileListChanged(void)));

    SyncWithPrimaryView(true);

    // init glsl modes
    CGraphicsViewStereo::InitGLSModes();
}

//------------------------------------------------------------------------------

CGraphicsView::~CGraphicsView(void)
{
    if( DrawGLCanvas ){
        DrawGLCanvas->SetGraphicsView(NULL);
        DrawGLCanvas = NULL;
    }

    CGraphicsViewList* p_list = GetViews();
    setParent(NULL);    // remove object from the list
    if( p_list ) p_list->EmitOnGraphicsViewListChanged();

    // release resources
    if( SelBuffer ){
        delete[] SelBuffer;
        SelBuffer = NULL;
    }
    if( FitSceneTimer ) {
        delete FitSceneTimer;
        FitSceneTimer = NULL;
    }
    // we cannot destroy gls context here - OpenGL must be active when calling glsDestroyContext
//    if( StereoContext ){
//        glsDestroyContext(StereoContext);
//        StereoContext = NULL;
//    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsView::Delete(void)
{
    if( IsPrimaryView() ) return;
    // primary view cannot be deleted
    delete this;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsViewList* CGraphicsView::GetViews(void) const
{
    return(dynamic_cast<CGraphicsViewList*>(parent()));
}

//------------------------------------------------------------------------------

CGraphics* CGraphicsView::GetGraphics(void) const
{
    return(GetViews()->GetGraphics());
}

//------------------------------------------------------------------------------

CGraphicsProfileList* CGraphicsView::GetProfiles(void) const
{
    return(GetGraphics()->GetProfiles());
}

//------------------------------------------------------------------------------

bool CGraphicsView::IsOpenGLCanvasAttached(void)
{
    return(DrawGLCanvas != NULL);
}

//------------------------------------------------------------------------------

bool CGraphicsView::IsPrimaryView(void)
{
    return(IsFlagSet(EPOF_PRIMARY_VIEW));
}

//------------------------------------------------------------------------------

CGraphicsProfile* CGraphicsView::GetActiveProfile(void)
{
    if( Profile != NULL ) return(Profile);
    return(GetProfiles()->GetActiveProfile());
}

//------------------------------------------------------------------------------

CGraphicsProfile* CGraphicsView::GetUsedProfile(void)
{
    return(Profile);
}

//------------------------------------------------------------------------------

int CGraphicsView::GetWidth(void)
{
    if( DrawGLCanvas == NULL ) return(0);
    return(DrawGLCanvas->width());
}

//------------------------------------------------------------------------------

int CGraphicsView::GetHeight(void)
{
    if( DrawGLCanvas == NULL ) return(0);
    return(DrawGLCanvas->height());
}

//------------------------------------------------------------------------------

bool CGraphicsView::IsQuadBufferStereoAvailable(void)
{
    if( DrawGLCanvas == NULL ) return(0);
    return(DrawGLCanvas->GetFormat().stereo());
}

//------------------------------------------------------------------------------

bool CGraphicsView::IsMultiSamplingAvailable(void)
{
    if( DrawGLCanvas == NULL ) return(0);
    return(DrawGLCanvas->GetFormat().samples() > 1);
}

//------------------------------------------------------------------------------

CXMLElement* CGraphicsView::GetShadowData(bool create)
{
    return(ShadowData.GetChildElementByPath("shadow",create));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsView::SyncWithPrimaryView(bool set)
{
    if( set == SyncToPrimaryView ) return;
    SyncToPrimaryView = set;
    if( SyncToPrimaryView == true ){
        if( GetViews() &&  GetViews()->GetPrimaryView() ){
            // copy manipulator setup
            SetPos(GetViews()->GetPrimaryView()->GetPos());
            SetTrans(GetViews()->GetPrimaryView()->GetTrans());
            SetCentrum(GetViews()->GetPrimaryView()->GetCentrum());
        }
    }

    emit OnStatusChanged(ESC_OTHER);

    if( DrawGLCanvas ){
        Repaint(EDL_USER_DRAW);
    }
}

//------------------------------------------------------------------------------

bool CGraphicsView::IsSynchronizedWithPrimaryView(void) const
{
    return(SyncToPrimaryView);
}

//------------------------------------------------------------------------------

void CGraphicsView::SetProjectionMode(EProjectionMode mode)
{
    if( ProjectionMode == mode ) return;
    ProjectionMode = mode;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CGraphicsView::SetProjectionData(double near,double focal,double far,double fovy)
{
    if( (Near == near) && (Focal == focal) && (Far == far) && (Fovy == fovy) ) return;
    Near = near;
    Focal = focal;
    Far = far;
    Fovy = fovy;
    Position.x =  Focal;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

EProjectionMode CGraphicsView::GetProjectionMode(void)
{
    return(ProjectionMode);
}

//------------------------------------------------------------------------------

void CGraphicsView::GetProjectionData(double& near,double& focal,double& far,double& fovy)
{
    near = Near;
    focal = Focal;
    far = Far;
    fovy = Fovy;
}

//------------------------------------------------------------------------------

void CGraphicsView::SetStereoMode(EStereoMode mode)
{
    if( StereoMode == mode ) return;
    StereoMode = mode;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CGraphicsView::SetEyeSeparation(double esep)
{
    if( EyeSep == esep ) return;
    EyeSep = esep;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

EStereoMode CGraphicsView::GetStereoMode(void)
{
    return(StereoMode);
}

//------------------------------------------------------------------------------

double CGraphicsView::GetEyeSeparation(void)
{
    return(EyeSep);
}

//------------------------------------------------------------------------------

void CGraphicsView::StereoSwapEyes(bool swap)
{
    if( SwapEyes == swap ) return;
    SwapEyes = swap;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

bool CGraphicsView::AreStereoEyesSwapped(void)
{
    return(SwapEyes );
}

//------------------------------------------------------------------------------

void CGraphicsView::SetDepthCueing(bool set)
{
    if( DepthCueing == set ) return;
    DepthCueing = set;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CGraphicsView::SetDepthCueingData(GLenum dcm,double dcd, double dcs, double dce,const CColor& dcc)
{
    if( (DCMode == dcm) && (DCDensity == dcd) && (DCStart == dcs) && (DCEnd == dce) && (DCColor == dcc) ) return;

    DCMode = dcm;
    DCDensity = dcd;
    DCStart = dcs;
    DCEnd = dce;
    DCColor = dcc;

    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

bool CGraphicsView::IsDepthCueingEnabled(void)
{
    return(DepthCueing);
}

//------------------------------------------------------------------------------

void CGraphicsView::GetDepthCueingData(GLenum& dcm,double& dcd, double& dcs, double& dce,CColor& dcc)
{
    dcm = DCMode;
    dcd = DCDensity;
    dcs = DCStart;
    dce = DCEnd;
    dcc = DCColor;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsView::SetUsedProfile(CGraphicsProfile* p_profile)
{
    if( Profile == p_profile ) return;

    Profile = p_profile;
    // premature object destruction is solved in GraphicsProfileListChanged
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CGraphicsView::GraphicsProfileListChanged(void)
{
    if( ! GetProfiles()->children().contains(Profile) ){
        Profile = NULL;
    }
}

//------------------------------------------------------------------------------

void CGraphicsView::FitScene(bool animated)
{
    if( GetActiveProfile() == NULL ) return;
    if( FitSceneTimer->isActive() ) return;

    // get bounding box
    CObjMetrics metrics;
    GetActiveProfile()->GetSceneMetrics(metrics);

    // calculate centre
    CPoint cog = (metrics.GetLowPoint()+metrics.GetHighPoint()) / 2.0;
    double diameter = Size(metrics.GetLowPoint()-metrics.GetHighPoint());

    int num_of_levels = 5;
    int tot_time = 500;     // in miliseconds

    // calculate ticks
    TransTick = GetTrans();
    TransTick.Invert(); // after transformation trans has to be unit matrix

    if( animated ){

        TickIntervals = 1;
        for(int i=0; i < num_of_levels; i++) {
            TickIntervals *= 2;
            TransTick.SquareRoot();
        }

        PosTick  = -GetPos() / TickIntervals;
        PosTick -= CPoint(diameter,0,0) / TickIntervals;
        CentreTick = (cog - GetCentrum()) / TickIntervals;
        ScaleTick = (1.0 - GetScale()) / TickIntervals;

        FitSceneTimer->setInterval(tot_time/TickIntervals);
        FitSceneTimer->start();
    } else {
        CTransformation trans = GetTrans();
        trans *= TransTick;
        BeginUpdate();
            Move(-GetPos()-CPoint(diameter,0,0),this);
            MoveCentrum(cog - GetCentrum(),this);
            SetTrans(trans);
            SetScale(1.0);
        EndUpdate();
        if( IsPrimaryView() || IsSynchronizedWithPrimaryView() ){
            GetViews()->SynchroniseAssociatedViews(this);
        }
        Repaint(EDL_MANIP_DRAW);
    }
}

//------------------------------------------------------------------------------

void CGraphicsView::FitScene(CProObject* p_object, bool animated)
{
    if( GetActiveProfile() == NULL ) return;
    if( FitSceneTimer->isActive() ) return;

    // get bounding box
    CObjMetrics metrics;
    p_object->GetObjectMetrics(metrics);

    // calculate centre
    CPoint cog = (metrics.GetLowPoint()+metrics.GetHighPoint()) / 2.0;
    // set rotation centre on the calculted center
    SetRotationCentre(cog,false);
    double diameter = Size(metrics.GetLowPoint()-metrics.GetHighPoint());

    int num_of_levels = 5;
    int tot_time = 500;     // in miliseconds

    // calculate ticks
    TransTick = GetTrans();
    TransTick.Invert(); // after transformation trans has to be unit matrix

    if( animated ){

        TickIntervals = 1;
        for(int i=0; i < num_of_levels; i++) {
            TickIntervals *= 2;
            TransTick.SquareRoot();
        }

        PosTick  = -GetPos() / TickIntervals;
        PosTick -= CPoint(diameter,0,0) / TickIntervals;
        CentreTick = (cog - GetCentrum()) / TickIntervals;
        ScaleTick = (1.0 - GetScale()) / TickIntervals;

        FitSceneTimer->setInterval(tot_time/TickIntervals);
        FitSceneTimer->start();
    } else {
        CTransformation trans = GetTrans();
        trans *= TransTick;
        BeginUpdate();
            Move(-GetPos()-CPoint(diameter,0,0),this);
            MoveCentrum(cog - GetCentrum(),this);
            SetTrans(trans);
            SetScale(1.0);
        EndUpdate();
        if( IsPrimaryView() || IsSynchronizedWithPrimaryView() ){
            GetViews()->SynchroniseAssociatedViews(this);
        }
        Repaint(EDL_MANIP_DRAW);
    }
}

//------------------------------------------------------------------------------

void CGraphicsView::SetRotationCentre(const CPoint& centre,bool animated)
{
    if( GetActiveProfile() == NULL ) return;
    if( FitSceneTimer->isActive() ) return;

    TickIntervals = 2*2*2*2*2;
    int tot_time = 500;     // in miliseconds

    // calculate ticks
    TransTick.SetUnit();

    if( animated ){
        PosTick = CPoint();
        CentreTick = (centre - GetCentrum()) / TickIntervals;
        ScaleTick = 0.0;

        FitSceneTimer->setInterval(tot_time/TickIntervals);
        FitSceneTimer->start();
    } else {
        BeginUpdate();
            MoveCentrum(centre - GetCentrum(),this);
        EndUpdate();
        if( IsPrimaryView() || IsSynchronizedWithPrimaryView() ){
            GetViews()->SynchroniseAssociatedViews(this);
        }
        Repaint(EDL_MANIP_DRAW);
    }
}

//------------------------------------------------------------------------------

void CGraphicsView::FitSceneTick(void)
{
    TickIntervals--;
    if( TickIntervals == 0 ) FitSceneTimer->stop();

    BeginUpdate();
        Move(PosTick,this);
        MoveCentrum(CentreTick,this);
        CTransformation trans = GetTrans();
        trans *= TransTick;
        SetTrans(trans);
        ChangeScale(ScaleTick,this);
    EndUpdate();
    if( IsPrimaryView() || IsSynchronizedWithPrimaryView() ){
        GetViews()->SynchroniseAssociatedViews(this);
    }

    Repaint(EDL_MANIP_DRAW);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QImage CGraphicsView::Render(int width,int height)
{
    QImage image;

    if( GetActiveProfile() == NULL ) {
        ES_ERROR("ActiveProfile is NULL");
        return(image);
    }
    if( IsOpenGLCanvasAttached() == false ) {
        ES_ERROR("DrawGLCanvas is NULL");
        return(image);
    }

    if( width <= 0 ) width = DrawGLCanvas->width();
    if( height <= 0 ) height = DrawGLCanvas->height();

    Width = width;
    Height = height;

    CGraphicsCommonView* p_tmp = DrawGLCanvas;
    DrawGLCanvas = NULL;

    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(true);

    QGLPixelBuffer pbuffer(Width,Height,format,NULL);
    pbuffer.makeCurrent();
    DrawGL();
    FTGLFontCache.DestroyFonts();

    image = pbuffer.toImage();

    DrawGLCanvas = p_tmp;

    return(image);
}

//------------------------------------------------------------------------------

void CGraphicsView::Repaint(EDrawLevel level)
{
    if( DrawGLCanvas == NULL ) {
        ES_ERROR("DrawGLCanvas is NULL");
        return;
    }
    if( IsSynchronizedWithPrimaryView() ){
        GetViews()->RepaintAssociatedViews(this);
    }
    DrawGLCanvas->UpdateScene();
}

//------------------------------------------------------------------------------

void CGraphicsView::RepaintOnlyThisView(void)
{
    if( DrawGLCanvas == NULL ) {
        ES_ERROR("DrawGLCanvas is NULL");
        return;
    }
    DrawGLCanvas->UpdateScene();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsView::AttachShadowView(void)
{
    if( IsOpenGLCanvasAttached() ) return;
    new CGraphicsShadowView(this);
}

//------------------------------------------------------------------------------

void CGraphicsView::TryToAttachShadowView(void)
{
    if( IsOpenGLCanvasAttached() ) return;

    CXMLElement* p_ele = GetShadowData();
    if( p_ele == NULL ) return;

    bool attached = false;
    p_ele->GetAttribute("attached",attached);

    if( attached ) AttachShadowView();
}

//------------------------------------------------------------------------------

// this must be called when OpenGL context is active

void CGraphicsView::AttachOpenGLCanvas(CGraphicsCommonView* p_scene)
{
    DrawGLCanvas = p_scene;

    emit OnStatusChanged(ESC_OTHER);
    if( GetViews() ) GetViews()->EmitOnGraphicsViewListChanged();
}

//------------------------------------------------------------------------------

// this must be called when OpenGL context is active

void CGraphicsView::DetachOpenGLCanvas(void)
{
    // destroy fonts associated with the context
    FTGLFontCache.DestroyFonts();

    if( DrawGLCanvas ){
        DrawGLCanvas->SetGraphicsView(NULL);
    }

    DrawGLCanvas = NULL;
    emit OnStatusChanged(ESC_OTHER);
    if( GetViews() ) GetViews()->EmitOnGraphicsViewListChanged();
}

//------------------------------------------------------------------------------

void CGraphicsView::CloseShadowView(void)
{
    if( ShadowView ){
        delete ShadowView;
    }
}

//------------------------------------------------------------------------------

void CGraphicsView::SetCursor(const QCursor& cursor)
{
    if( DrawGLCanvas ){
        DrawGLCanvas->setCursor(cursor);
    }
}

//------------------------------------------------------------------------------

void CGraphicsView::DrawGL(void)
{
// do not test here as it is not set in Render Mode
//   if( DrawGLCanvas == NULL ) return;
//   if( StereoContext == NULL ) return;

    GetViews()->RegisterCurrentView(this);

    try{
        // GL init
        GLfloat lmodel_ambient[] = { 0.0, 0.0, 0.0, 1.0 };

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_MULTISAMPLE);

        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

        glEnable(GL_LIGHTING);
#ifdef WIN32
        glEnable(GL_NORMALIZE);
#else
        glEnable(GL_RESCALE_NORMAL);
#endif
        glEnable(GL_COLOR_MATERIAL);
        glShadeModel(GL_SMOOTH);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        //glEnable(GL_CULL_FACE);
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        GLfloat shininess = 80;

        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,&shininess );
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);

        // disable all lights
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
        glDisable(GL_LIGHT3);
        glDisable(GL_LIGHT4);
        glDisable(GL_LIGHT5);
        glDisable(GL_LIGHT6);
        glDisable(GL_LIGHT7);

        // setup mode
        glRenderMode(GL_RENDER);

        if( StereoMode == ESM_OFF ){
            glDrawBuffer(GL_BACK);
            InitMono();
            ManipDraw();
        }
    } catch(...){
        ES_ERROR("exception in CGraphicsView::DrawGL");
    }

    GetViews()->RegisterCurrentView(NULL);
}

//------------------------------------------------------------------------------

const CSelObject CGraphicsView::SelectObject(int mousex,int mousey)
{
    if( DrawGLCanvas == NULL ) return(CSelObject());

    // selection is disabled in stereo mode
    if( StereoMode != ESM_OFF ){
        GetProject()->TextNotification(ETNT_WARNING,"selection is disabled in the stereo mode",ETNT_WARNING_DELAY);
        return(CSelObject());
    }

    // this is problematic part ...
    // NVIDIA - cause problems with swapBuffer(), wrong rendering
    // Intel - cause later crashes
    // see inside for workaround
    DrawGLCanvas->ActivateGLContext();

    GetViews()->RegisterCurrentView(this);

    try{
        // workaround - see GLGetMode();
        LoadedObjects.Clear();

        // set selection buffer
        glSelectBuffer(SelBuffSize,SelBuffer);

        LoadedObjects.RegisterMasterObject(GetProfiles()->GetSelectionMasterObject());

        // init DrawGLCanvas
        glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);
        glShadeModel(GL_SMOOTH);

        glRenderMode(GL_SELECT);
        glInitNames();
        glPushName((GLint)~0);

        // init camera
        InitMonoSelection(mousex,mousey,4,4);
        ManipDraw();

    } catch(...){
        ES_ERROR("exception in CGraphicsView::DrawGL");
    }

    GetViews()->RegisterCurrentView(NULL);

    // get object
    int top_in_buffer = glRenderMode(GL_RENDER);

    // this is problematic part ...
    // see inside for workaround
    DrawGLCanvas->DoneGLContext();

    if( top_in_buffer < 1 ) {
        CSelObject obj = LoadedObjects.FindObject(0);
        LoadedObjects.Clear();
        return(obj);    // return master object
    }

    int object_id = -1;
    int numofnames = SelBuffer[0];
    unsigned int minz = SelBuffer[1];
    object_id = SelBuffer[numofnames+2];

    int pos = numofnames + 3;
    for(int i=0; i<top_in_buffer-1; i++) {
        numofnames = SelBuffer[pos];
        if( minz > SelBuffer[pos+1] ) {
            minz = SelBuffer[pos+1];
            object_id = SelBuffer[numofnames+pos+2];
        }
        pos += numofnames + 3;
    }

    CSelObject obj = LoadedObjects.FindObject(object_id);
    LoadedObjects.Clear();
    return(obj);
}

//------------------------------------------------------------------------------

void CGraphicsView::InitMono(void)
{
    // Misc stuff
    double aspect  = 1.0;
    if( DrawGLCanvas ) {
        if( DrawGLCanvas->height() > 0 ){
            aspect = DrawGLCanvas->width() / (double)DrawGLCanvas->height();
        }
        glViewport(0,0,DrawGLCanvas->width(),DrawGLCanvas->height());
    } else {
        if( Height > 0 ){
            aspect = Width / (double)Height;
        }
        glViewport(0,0,Width,Height);
    }

    double radians = (M_PI / 180.0) * Fovy / 2.0;
    double wd2     = Near * tan(radians);

    double left, right, top, bottom;
    left    = -aspect * wd2;
    right   =  aspect * wd2;
    top     =  wd2;
    bottom  = -wd2;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    switch(ProjectionMode){
        case EPM_PERSPECTIVE:
            glFrustum(left,right,bottom,top,Near,Far);
            break;
        case EPM_ORTHOGRAPHIC:
            glOrtho(left,right,bottom,top,Near,Far);
            break;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(Position.x,Position.y,Position.z,
              Reference.x,Reference.y,Reference.z,
              ViewUp.x,ViewUp.y,ViewUp.z);
}

//------------------------------------------------------------------------------

void CGraphicsView::InitMonoSelection(int x,int y,int w,int h)
{
    // Misc stuff
    double aspect  = 1.0;
    if( DrawGLCanvas->height() > 0 ){
        aspect = DrawGLCanvas->width() / (double)DrawGLCanvas->height();
    }
    double radians = (M_PI / 180.0) * Fovy / 2.0;
    double wd2     = Near * tan(radians);

    double left, right, top, bottom;
    left    = -aspect * wd2;
    right   =  aspect * wd2;
    top     =  wd2;
    bottom  = -wd2;

    glViewport(0,0,DrawGLCanvas->width(),DrawGLCanvas->height());

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    if( (w == 0) && (h == 0) ) {
        gluPickMatrix(x,DrawGLCanvas->height()-y, 4, 4, vp);
    } else {
        gluPickMatrix(x-w/2,DrawGLCanvas->height()-y+h/2, w, h, vp);
    }

    switch(ProjectionMode){
        case EPM_PERSPECTIVE:
            glFrustum(left,right,bottom,top,Near,Far);
            break;
        case EPM_ORTHOGRAPHIC:
            glOrtho(left,right,bottom,top,Near,Far);
            break;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(Position.x,Position.y,Position.z,
              Reference.x,Reference.y,Reference.z,
              ViewUp.x,ViewUp.y,ViewUp.z);
}

//------------------------------------------------------------------------------

void CGraphicsView::Move(const CPoint& dmov,CGraphicsView* p_view)
{
    CGraphicsViewManipulator::Move(dmov,p_view);

    if( IsPrimaryView() || IsSynchronizedWithPrimaryView() ){
        GetViews()->SynchroniseAssociatedViews(this);
    }
}

//------------------------------------------------------------------------------

void CGraphicsView::Rotate(const CPoint& drot,CGraphicsView* p_view)
{
    CGraphicsViewManipulator::Rotate(drot,p_view);

    if( IsPrimaryView() || IsSynchronizedWithPrimaryView() ){
        GetViews()->SynchroniseAssociatedViews(this);
    }
}

//------------------------------------------------------------------------------

void CGraphicsView::ChangeScale(double increment,CGraphicsView* p_view)
{
    CGraphicsViewManipulator::ChangeScale(increment,p_view);

    if( IsPrimaryView() || IsSynchronizedWithPrimaryView() ){
        GetViews()->SynchroniseAssociatedViews(this);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsView::SetName(const QString& name,CHistoryNode* p_history)
{
    CProObject::SetName(name,p_history);
    if( GetViews() ) GetViews()->EmitOnGraphicsViewListChanged();
}

//------------------------------------------------------------------------------

void CGraphicsView::SetDescription(const QString& descrip,CHistoryNode* p_history)
{
    CProObject::SetDescription(descrip,p_history);
    if( GetViews() ) GetViews()->EmitOnGraphicsViewListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsView::EmitManipulatorChanged(void)
{
    emit OnManipulatorChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsView::LoadData(CXMLElement* p_ele)
{
    // check input data --------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "view" ) {
        LOGIC_ERROR("p_ele is not 'view'");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    // get projections
    p_ele->GetAttribute("prm",ProjectionMode);
    p_ele->GetAttribute("nep",Near);
    p_ele->GetAttribute("fop",Focal);
    p_ele->GetAttribute("fap",Far);
    p_ele->GetAttribute("fov",Fovy);

    // get stereo mode
    p_ele->GetAttribute("stm",StereoMode);
    p_ele->GetAttribute("swp",SwapEyes);
    p_ele->GetAttribute("eyp",EyeSep);

    // get depth cueing
    p_ele->GetAttribute("dcen",DepthCueing);
    p_ele->GetAttribute("dcm",DCMode);
    p_ele->GetAttribute("dcd",DCDensity);
    p_ele->GetAttribute("dcs",DCStart);
    p_ele->GetAttribute("dce",DCEnd);
    DCColor.Load("dcc",p_ele);

    // manipulator ----------------------------------
    CXMLElement* p_mele = p_ele->GetFirstChildElement("manip");
    if( p_mele ) {
        LoadManip(p_mele);
    }
}

//------------------------------------------------------------------------------

void CGraphicsView::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "view" ) {
        LOGIC_ERROR("p_ele is not 'view'");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    // save projections
    p_ele->SetAttribute("prm",ProjectionMode);
    p_ele->SetAttribute("nep",Near);
    p_ele->SetAttribute("fop",Focal);
    p_ele->SetAttribute("fap",Far);
    p_ele->SetAttribute("fov",Fovy);

    // save stereo mode
    p_ele->SetAttribute("stm",StereoMode);
    p_ele->SetAttribute("swp",SwapEyes);
    p_ele->SetAttribute("eyp",EyeSep);

    // save depth cueing
    p_ele->SetAttribute("dcen",DepthCueing);
    p_ele->SetAttribute("dcm",DCMode);
    p_ele->SetAttribute("dcd",DCDensity);
    p_ele->SetAttribute("dcs",DCStart);
    p_ele->SetAttribute("dce",DCEnd);
    DCColor.Save("dcc",p_ele);

    // manipulator ----------------------------------
    CXMLElement* p_mele = p_ele->CreateChildElement("manip");
    SaveManip(p_mele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsView::RawDraw(void)
{
    if( (GetActiveProfile() == NULL) || GetProfiles()->GetDataManipulationMode() ) {
        glClearColor(0.0,0.0,0.0,0.0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }

    // depth cueing
    if( DepthCueing ){
        glEnable(GL_FOG);
        glFogi(GL_FOG_MODE,DCMode);
        glFogfv(GL_FOG_COLOR,DCColor);
        glFogf(GL_FOG_DENSITY,DCDensity);
        glFogf(GL_FOG_START,DCStart);
        glFogf(GL_FOG_END,DCEnd);
        glHint(GL_FOG_HINT,GL_NICEST);
    } else {
        glDisable(GL_FOG);
    }

    if( GetProfiles()->GetDataManipulationMode() ){
        // TODO
        // paint here informative message about data manipulation
    } else {
        // normal scene draw
        if( GetActiveProfile() ){
            GetActiveProfile()->Draw();
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
