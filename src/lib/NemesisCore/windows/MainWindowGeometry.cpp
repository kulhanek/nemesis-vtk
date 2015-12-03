// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <Structure.hpp>
#include <AtomList.hpp>
#include <OptimizerSetupList.hpp>
#include <Optimizer.hpp>
#include <RestraintList.hpp>
#include <MainWindow.hpp>
#include <StructureList.hpp>
#include <QMenu>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ConnectGeometryMenu(void)
{
    CONNECT_ACTION(GeometryWP);
    CONNECT_ACTION(Optimize);
    CONNECT_ACTION(OptimizerSetup);
    CONNECT_ACTION(GeoManipWP);
    CONNECT_ACTION(RestraintManagerWP);

    CONNECT_ACTION(NewRestraint);
    CONNECT_ACTION(DeleteInvalidRestraints);
    CONNECT_ACTION(DeleteSelectedRestraints);
    CONNECT_ACTION(DeleteAllRestraints);

    CONNECT_ACTION(AlignPrincipalAxes);
    CONNECT_ACTION(COMToOrigin);
    CONNECT_ACTION(COMToBoxCenter);
    CONNECT_ACTION(RotateAroundXAxisClockwise);
    CONNECT_ACTION(RotateAroundYAxisClockwise);
    CONNECT_ACTION(RotateAroundZAxisClockwise);
    CONNECT_ACTION(RotateAroundXAxisAnticlockwise);
    CONNECT_ACTION(RotateAroundYAxisAnticlockwise);
    CONNECT_ACTION(RotateAroundZAxisAnticlockwise);
    CONNECT_ACTION(MirrorInXYPlane);
    CONNECT_ACTION(MirrorInXZPlane);
    CONNECT_ACTION(MirrorInYZPlane);
    CONNECT_ACTION(FreezeAllAtoms);
    CONNECT_ACTION(FreezeHeavyAtoms);
    CONNECT_ACTION(FreezeSelectedAtoms);
    CONNECT_ACTION(UnfreezeAllAtoms);
    CONNECT_ACTION(UnfreezeSelectedAtoms);
    CONNECT_ACTION(ImageByMolecules);
    CONNECT_ACTION(ImageByResidues);
    CONNECT_ACTION(ImageByAtoms);

    BIND_ACTION(actionEnableRestraints);

    BIND_ACTION(actionManipulateAllAtoms);
    BIND_ACTION(actionManipulateSelectedAtoms);
    BIND_ACTION(actionManipulateMolecules);

    BIND_ACTION(actionAroundOrigin);
    BIND_ACTION(actionFamiliarShape);

    // create group
    GeoScopeGroup = new QActionGroup(this);
    GeoScopeGroup->addAction(actionManipulateAllAtoms);
    GeoScopeGroup->addAction(actionManipulateSelectedAtoms);
    GeoScopeGroup->addAction(actionManipulateMolecules);
    actionManipulateAllAtoms->setChecked(true);

    connect(actionEnableRestraints,SIGNAL(triggered(bool)),
            this,SLOT(EnableRestraints(bool)));
    //-----------------
    connect(HistoryList,SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(UpdateGeometryMenu()));
    //-----------------
    connect(Project->GetStructures(),SIGNAL(OnStructureListChanged(void)),
            this,SLOT(UpdateGeometryMenu()));

    // update menu
    UpdateGeometryMenu();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::UpdateGeometryMenu(void)
{
    bool set = true;

    CLockLevels curr_levels = Project->GetHistory()->GetLockModeLevels();
    set &= ! curr_levels.testFlag(EHCL_GEOMETRY);

    actionOptimize->setEnabled(set);

    actionRestraintManagerWP->setEnabled(true);
    actionEnableRestraints->setEnabled(set);

    actionNewRestraint->setEnabled(set);
    actionDeleteInvalidRestraints->setEnabled(set);
    actionDeleteSelectedRestraints->setEnabled(set);
    actionDeleteAllRestraints->setEnabled(set);

    actionFreezeAllAtoms->setEnabled(set);
    actionFreezeHeavyAtoms->setEnabled(set);
    actionFreezeSelectedAtoms->setEnabled(set);
    actionUnfreezeAllAtoms->setEnabled(set);
    actionUnfreezeSelectedAtoms->setEnabled(set);

    actionAlignPrincipalAxes->setEnabled(set);

    actionCOMToOrigin->setEnabled(set);
    actionCOMToBoxCenter->setEnabled(set);

    actionRotateAroundXAxisClockwise->setEnabled(set);
    actionRotateAroundYAxisClockwise->setEnabled(set);
    actionRotateAroundZAxisClockwise->setEnabled(set);

    actionRotateAroundXAxisAnticlockwise->setEnabled(set);
    actionRotateAroundYAxisAnticlockwise->setEnabled(set);
    actionRotateAroundZAxisAnticlockwise->setEnabled(set);

    actionMirrorInXYPlane->setEnabled(set);
    actionMirrorInXZPlane->setEnabled(set);
    actionMirrorInYZPlane->setEnabled(set);

    actionImageByMolecules->setEnabled(set);
    actionImageByResidues->setEnabled(set);
    actionImageByAtoms->setEnabled(set);

    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ){
        actionCOMToBoxCenter->setEnabled(false);
        actionEnableRestraints->setChecked(false);
        actionImageByMolecules->setEnabled(false);
        actionImageByResidues->setEnabled(false);
        actionImageByAtoms->setEnabled(false);
        return;
    }

    // update some items when menu is going to be shown
    bool enabled = p_str->PBCInfo.IsPBCEnabled() && p_str->PBCInfo.IsValid();
    actionCOMToBoxCenter->setEnabled(enabled);

    actionImageByMolecules->setEnabled(enabled);
    actionImageByResidues->setEnabled(enabled);
    actionImageByAtoms->setEnabled(enabled);

    actionEnableRestraints->setChecked(p_str->GetRestraints()->IsEnabled());
}

//------------------------------------------------------------------------------

EGeometryScope CMainWindow::GetGeometryScope(void)
{
    if( GeoScopeGroup->checkedAction() == actionManipulateAllAtoms ) return(EGS_ALL_ATOMS);
    if( GeoScopeGroup->checkedAction() == actionManipulateSelectedAtoms ) return(EGS_SELECTED_ATOMS);
    if( GeoScopeGroup->checkedAction() == actionManipulateMolecules ) return(EGS_SELECTED_FRAGMENTS);
    return(EGS_ALL_ATOMS);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::GeometryWP(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{GEOMETRY_WORK_PANEL:63a0f336-31f4-4509-84b7-4218e5a84e9d}");
    OpenToolPanel(mp_uuid,true);
}

//------------------------------------------------------------------------------

void CMainWindow::GeoManipWP(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{GEO_MANIP_WORK_PANEL:b2865e28-8b2c-4e40-bb45-e9a705f8f7d6}");
    OpenToolPanel(mp_uuid,true);
}

//------------------------------------------------------------------------------

void CMainWindow::Optimize(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;

    COptimizer* p_opt = OptimizerSetups->CreateDefaultOptimizerJob(p_str);
    if( p_opt == NULL ) return;

    p_opt->SubmitJob();
}

//------------------------------------------------------------------------------

void CMainWindow::OptimizerSetup(void)
{
    OptimizerSetups->OpenDefaultOptimizerSetupDesigner();
}

//------------------------------------------------------------------------------

void CMainWindow::RestraintManagerWP(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{RESTRAINTS_MANAGER_WP:e65b89fd-9724-4a33-ab70-e9acccca6d7a}");
    OpenToolPanel(mp_uuid,true);
}

//------------------------------------------------------------------------------

void CMainWindow::EnableRestraints(bool checked)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetRestraints()->EnableRestraintsWH(checked);
}

//------------------------------------------------------------------------------

void CMainWindow::NewRestraint(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    CRestraint* p_rst = p_str->GetRestraints()->CreateRestraintWH();
    if( p_rst != NULL ){
        p_rst->OpenObjectDesigner();
    }
}

//------------------------------------------------------------------------------

void CMainWindow::DeleteInvalidRestraints(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetRestraints()->DeleteInvalidRestraintsWH();
}

//------------------------------------------------------------------------------

void CMainWindow::DeleteSelectedRestraints(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetRestraints()->DeleteSelectedRestraintsWH();
}

//------------------------------------------------------------------------------

void CMainWindow::DeleteAllRestraints(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetRestraints()->DeleteAllRestraintsWH();
}

//------------------------------------------------------------------------------

void CMainWindow::AlignPrincipalAxes(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->AlignPrincipalAxesWH(EAT_AXIS_Z,GetGeometryScope());
}

//------------------------------------------------------------------------------

void CMainWindow::COMToOrigin(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->MoveCOMToWH(EGO_ORIGIN,GetGeometryScope());
}

//------------------------------------------------------------------------------

void CMainWindow::COGToOrigin(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->MoveCOGToWH(EGO_ORIGIN,GetGeometryScope());
}

//------------------------------------------------------------------------------

void CMainWindow::COMToBoxCenter(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->MoveCOMToWH(EGO_BOX_CENTER,GetGeometryScope());
}

//------------------------------------------------------------------------------

void CMainWindow::COGToBoxCenter(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->MoveCOGToWH(EGO_BOX_CENTER,GetGeometryScope());
}


//------------------------------------------------------------------------------

void CMainWindow::RotateAroundXAxisClockwise(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->RotateByWH(EAT_AXIS_X,M_PI/2,GetGeometryScope());
}

//------------------------------------------------------------------------------

void CMainWindow::RotateAroundYAxisClockwise(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->RotateByWH(EAT_AXIS_Y,M_PI/2,GetGeometryScope());
}

//------------------------------------------------------------------------------

void CMainWindow::RotateAroundZAxisClockwise(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->RotateByWH(EAT_AXIS_Z,M_PI/2,GetGeometryScope());
}

//------------------------------------------------------------------------------

void CMainWindow::RotateAroundXAxisAnticlockwise(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->RotateByWH(EAT_AXIS_X,-M_PI/2,GetGeometryScope());
}

//------------------------------------------------------------------------------

void CMainWindow::RotateAroundYAxisAnticlockwise(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->RotateByWH(EAT_AXIS_Y,-M_PI/2,GetGeometryScope());
}

//------------------------------------------------------------------------------

void CMainWindow::RotateAroundZAxisAnticlockwise(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->RotateByWH(EAT_AXIS_Z,-M_PI/2,GetGeometryScope());
}

//------------------------------------------------------------------------------

void CMainWindow::MirrorInXYPlane(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->MirrorInPlaneWH(EPT_PLANE_XY,GetGeometryScope());
}

//------------------------------------------------------------------------------

void CMainWindow::MirrorInXZPlane(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->MirrorInPlaneWH(EPT_PLANE_XZ,GetGeometryScope());
}

//------------------------------------------------------------------------------

void CMainWindow::MirrorInYZPlane(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->MirrorInPlaneWH(EPT_PLANE_YZ,GetGeometryScope());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::FreezeAllAtoms(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->FreezeAllAtomsWH();
}

//------------------------------------------------------------------------------

void CMainWindow::FreezeHeavyAtoms(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->FreezeHeavyAtomsWH();
}

//------------------------------------------------------------------------------

void CMainWindow::FreezeSelectedAtoms(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->FreezeSelectedAtomsWH();
}

//------------------------------------------------------------------------------

void CMainWindow::UnfreezeAllAtoms(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->UnfreezeAllAtomsWH();
}

//------------------------------------------------------------------------------

void CMainWindow::UnfreezeSelectedAtoms(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->UnfreezeSelectedAtomsWH();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ImageByAtoms(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->ImageCoordinatesWH(EIM_BY_ATOMS,
                                          actionAroundOrigin->isChecked(),
                                          actionFamiliarShape->isChecked(),
                                          GetGeometryScope());
}

//------------------------------------------------------------------------------

void CMainWindow::ImageByResidues(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->ImageCoordinatesWH(EIM_BY_RESIDUES,
                                          actionAroundOrigin->isChecked(),
                                          actionFamiliarShape->isChecked(),
                                          GetGeometryScope());
}

//------------------------------------------------------------------------------

void CMainWindow::ImageByMolecules(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;
    p_str->GetAtoms()->ImageCoordinatesWH(EIM_BY_MOLECULES,
                                          actionAroundOrigin->isChecked(),
                                          actionFamiliarShape->isChecked(),
                                          GetGeometryScope());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



