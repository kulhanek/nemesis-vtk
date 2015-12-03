
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

#include <QMessageBox>

#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <Graphics.hpp>
#include <GraphicsProfileList.hpp>

#include "CheminfoProjectModule.hpp"

#include "CheminfoProjectStructureListProject.hpp"
#include "CheminfoProjectStructureListProject.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        CheminfoProjectStructureListProjectID(
                    "{CHEMINFO_STRUCTURE_LIST_PROJECT:b2a0edc4-e0fd-40d0-a6b7-a379b23a1a66}",
                    "Cheminfo Project Structure List Project");

CPluginObject   CheminfoProjectStructureListProjectObject(&CheminfoProjectPlugin,
                    CheminfoProjectStructureListProjectID,CHEMINFO_CAT,
                    ":/images/CheminfoProject/12.cheminfo/CheminfoProjectStructureListProject.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectStructureListProject::CCheminfoProjectStructureListProject(CProject* p_project)
    : CProObject(&CheminfoProjectStructureListProjectObject,p_project,p_project)
{
    ConFlags |= EECOF_SUB_CONTAINERS;
    ConFlags |= EECOF_DYNAMICAL_CONTENTS;
    CheminfoProject = dynamic_cast<CCheminfoProject*>(p_project);
    ActiveStructureList = NULL;
    ProjectInfoList = NULL;

}

//------------------------------------------------------------------------------

CCheminfoProjectStructureListProject::CCheminfoProjectStructureListProject(CProject* p_project,bool no_index)
    : CProObject(&CheminfoProjectStructureListProjectObject,p_project,p_project,no_index)
{
    ConFlags |= EECOF_SUB_CONTAINERS;
    ConFlags |= EECOF_DYNAMICAL_CONTENTS;
    CheminfoProject = dynamic_cast<CCheminfoProject*>(p_project);
    ActiveStructureList = NULL;
    ProjectInfoList = NULL;

}

//------------------------------------------------------------------------------

CCheminfoProjectStructureListProject::~CCheminfoProjectStructureListProject(void)
{
    foreach(QObject* p_qobj,children()) {
        delete p_qobj;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectInfoList* CCheminfoProjectStructureListProject::GetProjectInfoList(void) const
{
    return (ProjectInfoList);
}

//------------------------------------------------------------------------------

CCheminfoProjectStructureList* CCheminfoProjectStructureListProject::GetActiveStructureList(void) const
{
    return(ActiveStructureList);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void  CCheminfoProjectStructureListProject::SetProjectInfoList(CCheminfoProjectInfoList* p_info)
{
    ProjectInfoList = p_info;
}

//------------------------------------------------------------------------------

void CCheminfoProjectStructureListProject::SetActiveStructureList(CCheminfoProjectStructureList* p_strlist)
{
    ActiveStructureList = p_strlist;
    EmitOnCheminfoProjectStructureListProjectChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectStructureListProject::EmitOnCheminfoProjectStructureListProjectChanged(void)
{
    emit OnCheminfoProjectStructureListProjectChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

