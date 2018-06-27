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

#include "TrajectoryProjectWindow.hpp"
#include <ProjectList.hpp>
#include <Project.hpp>
#include <list>
#include <PluginDatabase.hpp>
#include <PluginObject.hpp>
#include <Trajectory.hpp>
#include <TrajectoryList.hpp>
#include <QMessageBox>

#include "TrajectoryProjectWindow.hpp"
#include "TrajectoryProject.hpp"

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectoryProjectWindow::ConnectTrajectoryMenu(void)
{
    CONNECT_ACTION(ActiveTrajectoryInfo);
    CONNECT_ACTION(AllTrajectoriesInfo);

    CONNECT_ACTION(FirstSnapshot);
    CONNECT_ACTION(PrevSnapshot);
    CONNECT_ACTION(NextSnapshot);
    CONNECT_ACTION(LastSnapshot);

    CONNECT_ACTION(FirstSegment);
    CONNECT_ACTION(PrevSegment);
    CONNECT_ACTION(NextSegment);
    CONNECT_ACTION(LastSegment);

    CONNECT_ACTION(Play);
    CONNECT_ACTION(Pause);
    CONNECT_ACTION(Stop);

    CONNECT_ACTION(ManageTrajectories);
    CONNECT_ACTION(OpenInBuildProject);

    BIND_MENU(menuImportTrajectory);

    // bind toolbuttons
    WidgetUI.firstSnapshotPB->setDefaultAction(actionFirstSnapshot);
    WidgetUI.prevSnapshotPB->setDefaultAction(actionPrevSnapshot);
    WidgetUI.playPB->setDefaultAction(actionPlay);
    WidgetUI.pausePB->setDefaultAction(actionPause);
    WidgetUI.stopPB->setDefaultAction(actionStop);   
    WidgetUI.nextSnapshotPB->setDefaultAction(actionNextSnapshot);
    WidgetUI.lastSnapshotPB->setDefaultAction(actionLastSnapshot);

    UpdateTrajectoryMenu();
    SetupImportTrajectoryMenu();

    // connect signals
    connect(Project->GetTrajectories(),SIGNAL(OnTrajectoryListChanged(void)),
            this,SLOT(UpdateTrajectoryMenu(void)));
    connect(Project->GetTrajectories(),SIGNAL(OnSnapshotChanged(void)),
            this,SLOT(UpdateTrajectoryMenu(void)));

    // slider
    connect(WidgetUI.snapshotSL,SIGNAL(valueChanged(int)),
            this,SLOT(SliderValueChanged(int)));
    connect(WidgetUI.snapshotSB,SIGNAL(valueChanged(int)),
            this,SLOT(SpinBoxValueChanged(int)));
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::UpdateTrajectoryMenu(void)
{
    if( Project == NULL ) return;
    CTrajectory* p_traj = Project->GetTrajectories()->GetActiveTrajectory();
    if( p_traj == NULL ){
        actionFirstSnapshot->setEnabled(false);
        actionPrevSnapshot->setEnabled(false);
        actionNextSnapshot->setEnabled(false);
        actionLastSnapshot->setEnabled(false);

        actionFirstSegment->setEnabled(false);
        actionPrevSegment->setEnabled(false);
        actionNextSegment->setEnabled(false);
        actionLastSegment->setEnabled(false);

        actionPlay->setEnabled(false);
        actionPause->setEnabled(false);
        actionStop->setEnabled(false);

        WidgetUI.snapshotSB->setValue(0);
        WidgetUI.snapshotSB->setEnabled(false);

        WidgetUI.numOfSnapshotsSB->setValue(0);
        WidgetUI.numOfSnapshotsSB->setEnabled(false);

        WidgetUI.snapshotSL->setEnabled(false);
        WidgetUI.snapshotSL->setMinimum(0);
        WidgetUI.snapshotSL->setMaximum(0);
        return;
    }

    int nsnaps = p_traj->GetNumberOfSnapshots();

    bool set = nsnaps != 0;
    bool first = set && p_traj->GetCurrentSnapshotIndex() > 1;
    bool last = set && p_traj->GetCurrentSnapshotIndex() != nsnaps;

    if( set ) {
        if( SliderUpdating == false ){
            SliderUpdating = true;
            WidgetUI.snapshotSL->setMinimum(1);
            WidgetUI.snapshotSL->setMaximum(nsnaps);
            WidgetUI.snapshotSL->setValue(p_traj->GetCurrentSnapshotIndex());
            WidgetUI.snapshotSL->setEnabled(set);
            SliderUpdating = false;
        }

        if( SpinBoxUpdating == false ){
            SpinBoxUpdating = true;
            WidgetUI.snapshotSB->setMinimum(1);
            WidgetUI.snapshotSB->setMaximum(nsnaps);
            WidgetUI.snapshotSB->setValue(p_traj->GetCurrentSnapshotIndex());
            WidgetUI.snapshotSB->setEnabled(set);
            SpinBoxUpdating = false;
        }

        WidgetUI.numOfSnapshotsSB->setMinimum(1);
        WidgetUI.numOfSnapshotsSB->setMaximum(nsnaps);
        WidgetUI.numOfSnapshotsSB->setValue(nsnaps);
        WidgetUI.numOfSnapshotsSB->setEnabled(set);

    } else {
        if( SliderUpdating == false ){
            SliderUpdating = true;
            WidgetUI.snapshotSL->setMinimum(0);
            WidgetUI.snapshotSL->setMaximum(0);
            WidgetUI.snapshotSL->setValue(0);
            WidgetUI.snapshotSL->setEnabled(false);
            SliderUpdating = false;
        }

        if( SpinBoxUpdating == false ){
            SpinBoxUpdating = true;
            WidgetUI.snapshotSB->setMinimum(0);
            WidgetUI.snapshotSB->setMaximum(0);
            WidgetUI.snapshotSB->setValue(0);
            WidgetUI.snapshotSB->setEnabled(false);
            SpinBoxUpdating = false;
        }

        WidgetUI.numOfSnapshotsSB->setMinimum(0);
        WidgetUI.numOfSnapshotsSB->setMaximum(0);
        WidgetUI.numOfSnapshotsSB->setValue(0);
        WidgetUI.numOfSnapshotsSB->setEnabled(false);
    }

    actionFirstSnapshot->setEnabled(first);
    actionPrevSnapshot->setEnabled(first);
    actionNextSnapshot->setEnabled(last);
    actionLastSnapshot->setEnabled(last);

    bool first_seg = p_traj->GetCurrentSegmentIndex() > 1;
    bool last_seg = p_traj->GetCurrentSegmentIndex() != p_traj->GetNumberOfSegments();

    actionFirstSegment->setEnabled(first_seg);
    actionPrevSegment->setEnabled(first_seg);
    actionNextSegment->setEnabled(last_seg);
    actionLastSegment->setEnabled(last_seg);

    actionPlay->setEnabled(set & (!p_traj->IsPlaying() || p_traj->IsPaused()));
    actionPause->setEnabled(set & p_traj->IsPlaying() && (!p_traj->IsPaused()));
    actionStop->setEnabled(set & p_traj->IsPlaying());
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::SliderValueChanged(int pos)
{
    if( SliderUpdating == true ) return;

    CTrajectory* p_traj = Project->GetTrajectories()->GetActiveTrajectory();
    if( p_traj == NULL ) return;

    SliderUpdating = true;
    p_traj->MoveToSnapshot(pos);
    SliderUpdating = false;
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::SpinBoxValueChanged(int pos)
{
    if( SpinBoxUpdating == true ) return;

    CTrajectory* p_traj = Project->GetTrajectories()->GetActiveTrajectory();
    if( p_traj == NULL ) return;

    SpinBoxUpdating = true;
    p_traj->MoveToSnapshot(pos);
    SpinBoxUpdating = false;
}

//------------------------------------------------------------------------------

void  CTrajectoryProjectWindow::SetupImportTrajectoryMenu(void)
{
    list<CPluginObject*>    impitems;

    CSimpleIteratorC<CPluginObject>    I(PluginDatabase.GetObjectList());
    CPluginObject*                     p_obj;
    QActionGroup*                      p_actGroup = new QActionGroup(this);
    while( (p_obj = I.Current()) != NULL ) {
        if (p_obj->GetCategoryUUID() == IMPORT_TRAJECTORY_CAT ) {
            impitems.push_back(p_obj);
        }
        I++;
    }

    // sort items
    impitems.sort(SortPluginObjectByName);

    // build menut items
    list<CPluginObject*>::iterator iit = impitems.begin();
    list<CPluginObject*>::iterator iie = impitems.end();

    while( iit != iie ){
        CPluginObject*  p_obj = *iit;
        CreateTrajImpexAction(p_actGroup,menuImportTrajectory,p_obj);
        iit++;
    }

    // any items?
    if( impitems.size() == 0 ) {
        menuImportTrajectory->addAction(tr("- not available -"));
    }

    connect(p_actGroup,SIGNAL(triggered(QAction *)),this,SLOT(ImportTrajectoryAction(QAction*)));
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::CreateTrajImpexAction(QActionGroup* p_actGroup,QMenu* p_menu,
                                                     CPluginObject* p_pod)
{
    QMenu* p_finmenu = p_menu;

    // do we have category?
    QString category;
    if( PluginDatabase.FindObjectConfigValue(p_pod->GetObjectUUID(),"_category",category,true) == true ){
        QList<QMenu*> menus = p_menu->findChildren<QMenu*>();
        bool found = false;
        foreach(QMenu* p_submenu,menus){
            if( p_submenu->title() == category ){
                p_finmenu = p_submenu;
                found = true;
                break;
            }
        }
        if( found == false ){
            p_finmenu = p_menu->addMenu(category);
        }
    }

    QAction* p_act = p_finmenu->addAction(p_pod->GetName());
    p_act->setProperty("trajectory.uuid",QVariant(QString(p_pod->GetObjectUUID().GetFullStringForm())));
    p_actGroup->addAction(p_act);
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::ImportTrajectoryAction(QAction* p_act)
{
    CExtUUID uuid;
    // impex object
    QVariant type = p_act->property("trajectory.uuid");
    uuid.LoadFromString(type.toString().toLatin1().constData());

    Project->setProperty("impex.direct",false);

    // open dialog
    ExecuteObject(uuid,Project);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectoryProjectWindow::ActiveTrajectoryInfo(void)
{
    CTrajectory* p_trj = Project->GetActiveTrajectory();
    if( p_trj == NULL ) return;
    p_trj->OpenObjectDesigner();
}


//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::AllTrajectoriesInfo(void)
{
    // we have trajectory project - GetTrajecotries() should not return NULL
    Project->GetTrajectories()->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::FirstSnapshot(void)
{
    CTrajectory* p_traj = Project->GetTrajectories()->GetActiveTrajectory();
    if( p_traj == NULL ) return;
    p_traj->FirstSnapshot();
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::PrevSnapshot(void)
{
    CTrajectory* p_traj = Project->GetTrajectories()->GetActiveTrajectory();
    if( p_traj == NULL ) return;
    p_traj->PrevSnapshot();
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::NextSnapshot(void)
{
    CTrajectory* p_traj = Project->GetTrajectories()->GetActiveTrajectory();
    if( p_traj == NULL ) return;
    p_traj->NextSnapshot();
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::LastSnapshot(void)
{
    CTrajectory* p_traj = Project->GetTrajectories()->GetActiveTrajectory();
    if( p_traj == NULL ) return;
    p_traj->LastSnapshot();
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::FirstSegment(void)
{
    CTrajectory* p_traj = Project->GetTrajectories()->GetActiveTrajectory();
    if( p_traj == NULL ) return;
    p_traj->FirstSnapshot();
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::PrevSegment(void)
{
    CTrajectory* p_traj = Project->GetTrajectories()->GetActiveTrajectory();
    if( p_traj == NULL ) return;
    p_traj->PrevSegment();
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::NextSegment(void)
{
    CTrajectory* p_traj = Project->GetTrajectories()->GetActiveTrajectory();
    if( p_traj == NULL ) return;
    p_traj->NextSegment();
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::LastSegment(void)
{
    CTrajectory* p_traj = Project->GetTrajectories()->GetActiveTrajectory();
    if( p_traj == NULL ) return;
    p_traj->LastSegment();
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::Play(void)
{
    CTrajectory* p_traj = Project->GetTrajectories()->GetActiveTrajectory();
    if( p_traj == NULL ) return;
    p_traj->Play();
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::Pause(void)
{
    CTrajectory* p_traj = Project->GetTrajectories()->GetActiveTrajectory();
    if( p_traj == NULL ) return;
    p_traj->Pause();
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::Stop(void)
{
    CTrajectory* p_traj = Project->GetTrajectories()->GetActiveTrajectory();
    if( p_traj == NULL ) return;
    p_traj->Stop();
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::ManageTrajectories(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{TRAJECTORY_LIST_WORK_PANEL:ff57f787-40f2-4d62-94c8-ce52bfe3257d}");
    OpenToolPanel(mp_uuid,true);
}

//------------------------------------------------------------------------------

void CTrajectoryProjectWindow::OpenInBuildProject(void)
{
    // create build project
    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString("{BUILD_PROJECT:b64d16f0-b73f-4747-9a13-212ab9a15d38}");
    CProject* p_project = Projects->NewProject(mp_uuid);
    if( p_project == NULL ){
        ES_ERROR("unable to create new build project");
        QMessageBox::critical(NULL, tr("Build Structure"),
                              tr("An error occurred during project opening!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        return;
    }
    p_project->ShowProject();
    QApplication::processEvents();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




