// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

#include <ProjectList.hpp>
#include <Project.hpp>
#include <Graphics.hpp>
#include <GraphicsProfileList.hpp>
#include <StandardModelObject.hpp>
#include <GraphicsObjectList.hpp>
#include <GraphicsViewList.hpp>
#include <MainWindow.hpp>
#include <SelectionList.hpp>
#include <Atom.hpp>
#include <Bond.hpp>
#include <Residue.hpp>
#include <Structure.hpp>
#include <StructureList.hpp>
#include <QMenu>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ConnectGraphicsMenu(void)
{
    CONNECT_ACTION(AutofitScene);

    BIND_ACTION(actionPerspective);
    BIND_ACTION(actionOrthographic);

    CONNECT_ACTION(StereoSwapEyes);
    CONNECT_ACTION(DepthCueing);

    CONNECT_ACTION(NewObjectGlobalScope);
    CONNECT_ACTION(NewObjectActiveProfile);
    CONNECT_ACTION(ObjectListInfo);

    CONNECT_ACTION(NewProfile);
    CONNECT_ACTION(ActiveProfileInfo);
    CONNECT_ACTION(ProfileListInfo);

    CONNECT_ACTION(NewView);
    CONNECT_ACTION(PrimaryViewInfo);
    CONNECT_ACTION(ViewListInfo);

    CONNECT_ACTION(GraphicsManagerWP);
    CONNECT_ACTION(ProfileManagerWP);

    BIND_MENU(menuStereo);

    //====================================
    // create groups
    ProjectionGroup = new QActionGroup(this);
    ProjectionGroup->addAction(actionPerspective);
    ProjectionGroup->addAction(actionOrthographic);
    connect(ProjectionGroup,SIGNAL(triggered(QAction *)),
            this,SLOT(ProjectionChanged(QAction*)));

    StereoGroup = new QActionGroup(this);
    QAction* p_sep = new QAction(menuStereo);
    p_sep->setSeparator(true);
    menuStereo->insertAction(actionStereoSwapEyes,p_sep);
    for(int i=0; i < CGraphicsViewStereo::GetNumberOfStereoModes(); i++){
        EStereoMode esm = static_cast<EStereoMode>(i);
        QString cat;
        QString desc;
        CGraphicsViewStereo::GetStereoModeDescription(esm,cat,desc);
        QAction* p_act;
        QMenu*   p_submenu = menuStereo;
        if( cat != "" ) {
            // find sub menu
            foreach(QMenu* p_menu,menuStereo->findChildren<QMenu*>()){
                if( p_menu->title() == cat ){
                    p_submenu = p_menu;
                    break;
                }
            }
            if( p_submenu == menuStereo ){
                p_submenu = new QMenu(cat,menuStereo);
                menuStereo->addMenu(p_submenu);
            }
        }
        p_act = new QAction(desc,p_submenu);

        p_act->setEnabled(CGraphicsViewStereo::IsStereoModeAvailable(esm));
        p_act->setData(i);
        p_act->setCheckable(true);
        p_submenu->addAction(p_act);
        StereoGroup->addAction(p_act);
    }

    connect(StereoGroup,SIGNAL(triggered(QAction *)),
            this,SLOT(StereoChanged(QAction*)));

    //-----------------
    connect(ProfileList,SIGNAL(OnGraphicsProfileListChanged(void)),
            this, SLOT(UpdateGraphicsMenu(void)));
    //-----------------
    connect(Project->GetGraphics()->GetPrimaryView(),SIGNAL(OnStatusChanged(EStatusChanged)),
            this, SLOT(UpdateGraphicsMenu(void)));
    //-----------------
    connect(HistoryList,SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(UpdateGraphicsMenu()));

    UpdateGraphicsMenu();
}

//------------------------------------------------------------------------------

void CMainWindow::UpdateGraphicsMenu(void)
{
    bool set = true;
    CLockLevels curr_levels = Project->GetHistory()->GetLockModeLevels();
    set &= ! curr_levels.testFlag(EHCL_GRAPHICS);

    // -------------------
    actionNewObjectGlobalScope->setEnabled(set);
    actionNewObjectActiveProfile->setEnabled(set);
    actionObjectListInfo->setEnabled(set);

    // -------------------
    actionNewProfile->setEnabled(set);
    actionProfileListInfo->setEnabled(set);
    actionActiveProfileInfo->setEnabled(set);

    // this is allowed to change even if the graphics is disabled
    CGraphicsView* p_gw = Project->GetGraphics()->GetPrimaryView();

    switch(p_gw->GetProjectionMode()){
        case EPM_PERSPECTIVE:
            actionPerspective->setChecked(true);
            break;
        case EPM_ORTHOGRAPHIC:
            actionOrthographic->setChecked(true);
            break;
    }

    foreach(QAction* p_act,StereoGroup->actions()){
        int i = p_act->data().toInt();
        EStereoMode esm = static_cast<EStereoMode>(i);
        p_act->setChecked(esm == p_gw->GetStereoMode());
    }

    actionStereoSwapEyes->setEnabled(true);
    actionStereoSwapEyes->setChecked(p_gw->AreStereoEyesSwapped());

    actionDepthCueing->setEnabled(true);
    actionDepthCueing->setChecked(p_gw->IsDepthCueingEnabled());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::GraphicsManagerWP(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{GRAPHICS_MANAGER_WORK_PANEL:b202f126-3322-4cbe-8400-01899b71a818}");
    OpenToolPanel(mp_uuid,true);
}

//------------------------------------------------------------------------------

void CMainWindow::ProfileManagerWP(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{ACTIVE_PROFILE_MANAGER_WORK_PANEL:340c9972-f129-4337-a375-e14f11632857}");
    OpenToolPanel(mp_uuid,true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::NewObjectGlobalScope(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{NEW_GRAPHICS_OBJECT_WORK_PANEL:b0568e3a-c78b-44f3-aee5-1e4739cc4be9}");
    ShowAsDialog(mp_uuid,Project->GetGraphics()->GetObjects());
}

//------------------------------------------------------------------------------

void CMainWindow::NewObjectActiveProfile(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{NEW_GRAPHICS_OBJECT_WORK_PANEL:b0568e3a-c78b-44f3-aee5-1e4739cc4be9}");
    ShowAsDialog(mp_uuid,Project->GetGraphics()->GetProfiles()->GetActiveProfile());
}

//------------------------------------------------------------------------------


void CMainWindow::ObjectListInfo(void)
{
    Project->GetGraphics()->GetObjects()->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::NewProfile(void)
{
    CProObject* p_profile = Project->GetGraphics()->GetProfiles()->CreateProfileWH();
    if( p_profile ){
        p_profile->OpenObjectDesigner();
    }
}

//------------------------------------------------------------------------------

void CMainWindow::ActiveProfileInfo(void)
{
    CGraphicsProfile* p_profile = Project->GetGraphics()->GetProfiles()->GetActiveProfile();
    if( p_profile == NULL ) return;
    p_profile->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CMainWindow::ProfileListInfo(void)
{
    Project->GetGraphics()->GetProfiles()->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::NewView(void)
{
    CGraphicsView* p_view = Project->GetGraphics()->GetViews()->CreateView();
    if( p_view ){
        p_view->AttachShadowView();
    }
}

//------------------------------------------------------------------------------

void CMainWindow::PrimaryViewInfo(void)
{
    Project->GetGraphics()->GetPrimaryView()->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CMainWindow::ViewListInfo(void)
{
    Project->GetGraphics()->GetViews()->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::AutofitScene(void)
{
    CGraphicsView* p_view = Project->GetGraphics()->GetPrimaryView();
    if( p_view ){
        p_view->FitScene();
    }
}

//------------------------------------------------------------------------------

void CMainWindow::ProjectionChanged(QAction* p_action)
{
    if( p_action == actionPerspective ){
        Project->GetGraphics()->GetPrimaryView()->SetProjectionMode(EPM_PERSPECTIVE);
    }

    if( p_action == actionOrthographic ){
        Project->GetGraphics()->GetPrimaryView()->SetProjectionMode(EPM_ORTHOGRAPHIC);
    }
    Project->RepaintProject();
}

//------------------------------------------------------------------------------

void CMainWindow::StereoChanged(QAction* p_action)
{
    int i = p_action->data().toInt();
    EStereoMode esm = static_cast<EStereoMode>(i);
    Project->GetGraphics()->GetPrimaryView()->SetStereoMode(esm);
    Project->RepaintProject();
}

//------------------------------------------------------------------------------

void CMainWindow::StereoSwapEyes(void)
{
    Project->GetGraphics()->GetPrimaryView()->StereoSwapEyes(actionStereoSwapEyes->isChecked());
    Project->RepaintProject();
}

//------------------------------------------------------------------------------

void CMainWindow::DepthCueing(void)
{
    Project->GetGraphics()->GetPrimaryView()->SetDepthCueing(actionDepthCueing->isChecked());
    Project->RepaintProject();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



