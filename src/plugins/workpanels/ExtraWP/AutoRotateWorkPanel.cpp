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

#include <QtGui>

#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <Graphics.hpp>
#include <GraphicsView.hpp>

#include "ExtraToolsModule.hpp"

#include "AutoRotateWorkPanel.hpp"
#include "AutoRotateWorkPanel.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* AutoRotateWorkPanelCB(void* p_data);

CExtUUID        AutoRotateWorkPanelID(
                    "{AUTO_ROTATE_WP:751acdb4-1309-41a3-a4fe-92b53fa38180}",
                    "Autorotate");

CPluginObject   AutoRotateWorkPanelObject(&ExtraToolsPlugin,
                    AutoRotateWorkPanelID,WORK_PANEL_CAT,
                    ":/images/ExtraTools/AutoRotateWorkPanel.svg",
                    AutoRotateWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* AutoRotateWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CAutoRotateWorkPanel requires active project");
        return(NULL);
    }

    QObject* p_build_wp = new CAutoRotateWorkPanel(p_project);
    return(p_build_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAutoRotateWorkPanel::CAutoRotateWorkPanel(CProject* p_project)
    : CWorkPanel(&AutoRotateWorkPanelObject,p_project,EWPR_TOOL)
{
    WidgetUI.setupUi(this);

    // set link objects
    WidgetUI.projectLO->setObject(p_project);

    WidgetUI.viewLO->setProject(p_project);
    WidgetUI.viewLO->setObjectBaseMIMEType("graphics.views");
    WidgetUI.viewLO->showRemoveButton(true);
    WidgetUI.viewLO->setObject(p_project->GetGraphics()->GetPrimaryView());

    RotateTimer = new QTimer(this);

    // signals
    // ------------
    connect(WidgetUI.viewLO,SIGNAL(OnObjectChanged()),
            this,SLOT(ViewChanged()));
    // ------------
    connect(WidgetUI.playPB,SIGNAL(clicked(bool)),
            this,SLOT(Play()));
    // ------------
    connect(WidgetUI.stopPB,SIGNAL(clicked(bool)),
            this,SLOT(Stop()));
    // ------------
    connect(WidgetUI.stopPB,SIGNAL(clicked(bool)),
            this,SLOT(Stop()));
    // ------------
    connect(WidgetUI.timePerTickSB,SIGNAL(valueChanged(int)),
            this,SLOT(UpdateTimer()));
    // ------------
    connect(RotateTimer,SIGNAL(timeout()),
            this,SLOT(RotateTick()));

    // set timing
    ViewChanged();
    UpdateTimer();

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CAutoRotateWorkPanel::~CAutoRotateWorkPanel()
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CAutoRotateWorkPanel::WindowDockStatusChanged(bool docked)
{
    if( docked ){
        WidgetUI.projectNameW->hide();
    } else {
        WidgetUI.projectNameW->show();
    }
}

//------------------------------------------------------------------------------

void CAutoRotateWorkPanel::ViewChanged(void)
{
    if( WidgetUI.viewLO->getObject() == NULL ){
        RotateTimer->stop();
        WidgetUI.playPB->setEnabled(false);
        WidgetUI.stopPB->setEnabled(false);
    } else {
        RotateTimer->stop();
        WidgetUI.playPB->setEnabled(true);
        WidgetUI.stopPB->setEnabled(false);
    }
}

//------------------------------------------------------------------------------

void CAutoRotateWorkPanel::Play(void)
{
    RotateTimer->start();
    WidgetUI.playPB->setEnabled(false);
    WidgetUI.stopPB->setEnabled(true);
}

//------------------------------------------------------------------------------

void CAutoRotateWorkPanel::Stop(void)
{
    RotateTimer->stop();
    WidgetUI.playPB->setEnabled(true);
    WidgetUI.stopPB->setEnabled(false);
}

//------------------------------------------------------------------------------

void CAutoRotateWorkPanel::UpdateTimer(void)
{
    RotateTimer->setInterval(WidgetUI.timePerTickSB->value());
}

//------------------------------------------------------------------------------

void CAutoRotateWorkPanel::RotateTick(void)
{
    CGraphicsView* p_view = WidgetUI.viewLO->getObject<CGraphicsView*>();
    if( p_view == NULL ) return;

    CPoint rot;
    rot.x = WidgetUI.rotXSB->value();
    rot.y = WidgetUI.rotYSB->value();
    rot.z = WidgetUI.rotZSB->value();
    rot *= M_PI/180.0;

    p_view->Rotate(rot,p_view);
    p_view->Repaint(EDL_USER_DRAW);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



