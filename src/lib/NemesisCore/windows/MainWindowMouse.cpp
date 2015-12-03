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
#include <SelectionRequest.hpp>
#include <SelectionList.hpp>
#include <ObjectSelection.hpp>
#include <PointSelection.hpp>
#include <MouseDriverSetup.hpp>
#include <MainWindow.hpp>
#include <Graphics.hpp>
#include <GraphicsView.hpp>
#include <QMenu>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ConnectMouseMenu(void)
{
    BIND_ACTION(actionSRTMode);
    BIND_ACTION(actionSelectMode);
    BIND_ACTION(actionRotateMode);
    BIND_ACTION(actionTranslateMode);
    BIND_ACTION(actionScaleMode);

    BIND_ACTION(actionSelectRotationCentre);
    BIND_ACTION(actionAtomManipulation);
    BIND_ACTION(actionDirManipulation);
    BIND_ACTION(actionMoleculeManipulation);
    BIND_ACTION(actionObjectManipulation);
    BIND_ACTION(actionObjectProperties);

    //====================================
    // create groups
    MouseModeGroup = new QActionGroup(this);
    MouseModeGroup->addAction(actionSRTMode);
    MouseModeGroup->addAction(actionSelectMode);
    MouseModeGroup->addAction(actionRotateMode);
    MouseModeGroup->addAction(actionTranslateMode);
    MouseModeGroup->addAction(actionScaleMode);

    //====================================
    connect(MouseModeGroup, SIGNAL(triggered(QAction*)),
            this, SLOT(MouseModeChanged(QAction*)));

    //-----------------
    connect(actionAtomManipulation,SIGNAL(triggered(bool)),
            this,SLOT(AtomManipModeChanged(bool)));
    //-----------------
    connect(actionDirManipulation,SIGNAL(triggered(bool)),
            this,SLOT(DirManipModeChanged(bool)));
    //-----------------
    connect(actionMoleculeManipulation,SIGNAL(triggered(bool)),
            this,SLOT(MolManipModeChanged(bool)));
    //-----------------
    connect(actionObjectManipulation,SIGNAL(triggered(bool)),
            this,SLOT(ObjManipModeChanged(bool)));

    //-----------------
    connect(actionObjectProperties,SIGNAL(triggered(bool)),
            this,SLOT(ObjectProperties(void)));
    //-----------------
    connect(actionSelectRotationCentre,SIGNAL(triggered(bool)),
            this,SLOT(SelectRotationCentre(void)));

    //-----------------
    connect(MouseDriverSetup,SIGNAL(OnMouseSetupChanged(void)),
            this,SLOT(UpdateMouseMenu(void)));
    //-----------------
    connect(Project->GetMouseHandler(),SIGNAL(OnDriverStatusChanged(void)),
            this,SLOT(UpdateMouseMenu(void)));

    actionObjectProperties->setChecked(false);
    actionSelectRotationCentre->setChecked(false);

    UpdateMouseMenu();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::UpdateMouseMenu(void)
{
    CMouseHandler* p_mh = Project->GetMouseHandler();

    //------------------------------------------------
    switch( CMouseDriverSetup::MouseMode ){
        case EMM_SRT:
            actionSRTMode->setChecked(true);
            break;
        case EMM_SELECT:
            actionSelectMode->setChecked(true);
            break;
        case EMM_ROTATE:
            actionRotateMode->setChecked(true);
            break;
        case EMM_TRANSLATE:
            actionTranslateMode->setChecked(true);
            break;
        case EMM_SCALE:
            actionScaleMode->setChecked(true);
            break;
    }

    //------------------------------------------------
    if( p_mh != NULL ){
        switch(p_mh->GetSecondaryDriver()){
            case EMD_ATOM_MANIP:
                actionAtomManipulation->setChecked(true);
                actionDirManipulation->setChecked(false);
                actionMoleculeManipulation->setChecked(false);
                actionObjectManipulation->setChecked(false);
                break;
            case EMD_DIR_MANIP:
                actionAtomManipulation->setChecked(false);
                actionDirManipulation->setChecked(true);
                actionMoleculeManipulation->setChecked(false);
                actionObjectManipulation->setChecked(false);
                break;
            case EMD_MOL_MANIP:
                actionAtomManipulation->setChecked(false);
                actionDirManipulation->setChecked(false);
                actionMoleculeManipulation->setChecked(true);
                actionObjectManipulation->setChecked(false);
                break;
            case EMD_OBJ_MANIP:
                actionAtomManipulation->setChecked(false);
                actionDirManipulation->setChecked(false);
                actionMoleculeManipulation->setChecked(false);
                actionObjectManipulation->setChecked(true);
                break;
            default:
                actionAtomManipulation->setChecked(false);
                actionDirManipulation->setChecked(false);
                actionMoleculeManipulation->setChecked(false);
                actionObjectManipulation->setChecked(false);
                break;
        }
    } else {
        actionAtomManipulation->setChecked(false);
        actionDirManipulation->setChecked(false);
        actionMoleculeManipulation->setChecked(false);
        actionObjectManipulation->setChecked(false);
    }

//------------------------------------------------
  // do not update here
  //  actionObjectProperties->setChecked(false);
  //  actionSelectRotationCentre->setChecked(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::MouseModeChanged(QAction* p_act)
{
    if( actionSRTMode->isChecked() ){
        CMouseDriverSetup::MouseMode = EMM_SRT;
        MouseDriverSetup->EmitOnMouseSetupChanged();
        return;
    }
    if( actionSelectMode->isChecked() ){
        CMouseDriverSetup::MouseMode = EMM_SELECT;
        MouseDriverSetup->EmitOnMouseSetupChanged();
        return;
    }
    if( actionRotateMode->isChecked() ){
        CMouseDriverSetup::MouseMode = EMM_ROTATE;
        MouseDriverSetup->EmitOnMouseSetupChanged();
        return;
    }
    if( actionTranslateMode->isChecked() ){
        CMouseDriverSetup::MouseMode = EMM_TRANSLATE;
        MouseDriverSetup->EmitOnMouseSetupChanged();
        return;
    }
    if( actionScaleMode->isChecked() ){
        CMouseDriverSetup::MouseMode = EMM_SCALE;
        MouseDriverSetup->EmitOnMouseSetupChanged();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::AtomManipModeChanged(bool checked)
{
    if( checked ){
        Project->GetMouseHandler()->SetSecondaryDriver(EMD_ATOM_MANIP);
    } else {
        Project->GetMouseHandler()->SetSecondaryDriver(EMD_NONE);
    }
}

//------------------------------------------------------------------------------

void CMainWindow::DirManipModeChanged(bool checked)
{
    if( checked ){
        Project->GetMouseHandler()->SetSecondaryDriver(EMD_DIR_MANIP);
    } else {
        Project->GetMouseHandler()->SetSecondaryDriver(EMD_NONE);
    }
}

//------------------------------------------------------------------------------

void CMainWindow::MolManipModeChanged(bool checked)
{
    if( checked ){
        Project->GetMouseHandler()->SetSecondaryDriver(EMD_MOL_MANIP);
    } else {
        Project->GetMouseHandler()->SetSecondaryDriver(EMD_NONE);
    }
}

//------------------------------------------------------------------------------

void CMainWindow::ObjManipModeChanged(bool checked)
{
    if( checked ){
        Project->GetMouseHandler()->SetSecondaryDriver(EMD_OBJ_MANIP);
    } else {
        Project->GetMouseHandler()->SetSecondaryDriver(EMD_NONE);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ObjectProperties(void)
{
    if( Project == NULL ) {
        SelRequest->SetRequest(NULL);
        return;
    }
    if( actionObjectProperties->isChecked() ) {
        SelRequest->SetRequest(Project->GetSelection(),
                               &SH_Object,"to get object properties");
        SelRequest->disconnect();
        connect(SelRequest,SIGNAL(OnSelectionCompleted(void)),
                this,SLOT(ObjectPropertiesObjectSelected(void)));
        connect(SelRequest,SIGNAL(OnDetached(void)),
                this,SLOT(ObjectPropertiesDetached(void)));
        connect(SelRequest,SIGNAL(OnRequestChanging(void)),
                this,SLOT(ObjectPropertiesDetached(void)));
    } else {
        SelRequest->SetRequest(NULL);
    }
}

//------------------------------------------------------------------------------

void CMainWindow::ObjectPropertiesObjectSelected(void)
{
    CSelObject obj = Project->GetSelection()->PopSelectedSelObject();
    CProObject* p_obj = obj.GetObject();

    if( p_obj == NULL ) {
        if( obj.GetSubID() == SEL_ESC_KEY ) {
            actionObjectProperties->setChecked(false);
            SelRequest->SetRequest(NULL);
            SelRequest->disconnect();       // important
        }
        return;
    }

    p_obj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CMainWindow::ObjectPropertiesDetached(void)
{
    actionObjectProperties->setChecked(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::SelectRotationCentre(void)
{
    if( Project == NULL ) {
        SelRequest->SetRequest(NULL);
        return;
    }
    if( actionSelectRotationCentre->isChecked() ) {
        SelRequest->SetRequest(Project->GetSelection(),
                               &SH_Point_ESC,"to get rotation centre");
        SelRequest->disconnect();
        connect(SelRequest,SIGNAL(OnSelectionCompleted(void)),
                this,SLOT(RotationCentreSelected(void)));
        connect(SelRequest,SIGNAL(OnDetached(void)),
                this,SLOT(RotationCentreSelectionDetached(void)));
        connect(SelRequest,SIGNAL(OnRequestChanging(void)),
                this,SLOT(RotationCentreSelectionDetached(void)));
    } else {
        SelRequest->SetRequest(NULL);
    }
}

//------------------------------------------------------------------------------

void CMainWindow::RotationCentreSelected(void)
{
    CSelObject obj = Project->GetSelection()->PopSelectedSelObject();
    CProObject* p_obj = obj.GetObject();

    if( p_obj == NULL ) {
        if( obj.GetSubID() == SEL_ESC_KEY ) {
            actionSelectRotationCentre->setChecked(false);
            SelRequest->SetRequest(NULL);
            SelRequest->disconnect();       // important
        }
        return;
    }
    CPoint centre = p_obj->GetGeoDescriptor().GetPoint1();
    CGraphicsView* p_pw = GetProject()->GetGraphics()->GetPrimaryView();
    if( p_pw != NULL ){
        p_pw->SetRotationCentre(centre);
    }
}

//------------------------------------------------------------------------------

void CMainWindow::RotationCentreSelectionDetached(void)
{
    actionSelectRotationCentre->setChecked(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


