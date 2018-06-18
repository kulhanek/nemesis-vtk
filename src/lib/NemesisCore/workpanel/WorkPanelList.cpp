// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <WorkPanelList.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <WorkPanel.hpp>
#include <Project.hpp>
#include <XMLIterator.hpp>
#include <XMLElement.hpp>
#include <PluginDatabase.hpp>
#include <GlobalDesktop.hpp>
#include <ProjectDesktop.hpp>
#include <QCoreApplication>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        WorkPanelListID(
                    "{WORK_PANEL_LIST:66c245c2-fc25-498d-9222-76bf13e1b4a1}",
                    "Work panels");

CPluginObject   WorkPanelListObject(&NemesisCorePlugin,
                    WorkPanelListID,GENERIC_CAT,
                    ":/images/NemesisCore/workpanel/WorkPanelList.svg",
                    NULL);

//------------------------------------------------------------------------------

CWorkPanelList* WorkPanels = NULL;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CWorkPanelList::CWorkPanelList(CExtComObject* p_parent)
    : CExtComObject(&WorkPanelListObject,p_parent)
{
    ConFlags |= EECOF_SUB_CONTAINERS;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CWorkPanelList::RestoreWorkPanels(CProject* p_project)
{
    // close all work panels if any
    if( p_project ){
        RemoveAllProjectWorkPanels(p_project);
    } else {
        RemoveAllGlobalWorkPanels();
    }

    // create new set of workpanels
    CXMLElement*    p_root;

    if( p_project ){
        p_root = p_project->GetDesktop()->GetWorkPanelsSetup();
    } else {
        p_root = GlobalDesktop->GetWorkPanelsSetup();
    }

    CXMLIterator    I(p_root);
    CXMLElement*    p_ele;

    while( (p_ele = I.GetNextChildElement("item")) != NULL ) {
        CExtUUID uuid;
        if( uuid.GetValue(p_ele,"uuid") ) {
            bool opened = false;
            p_ele->GetAttribute("open",opened);
            if( opened == true ) {
                PluginDatabase.CreateObject(uuid,p_project);
            }
        }
    }

    // load new workpanel window positions
    foreach(CWorkPanel* p_wp, WorkPanels){
        if( p_wp->GetProject() == p_project ){
            p_wp->LoadWorkPanelSetup();
        }
    }
}

//------------------------------------------------------------------------------

void CWorkPanelList::SaveWorkPanelSetup(CProject* p_project)
{
    foreach(CWorkPanel* p_wp, WorkPanels){
        if( p_wp->GetProject() == p_project ){
            p_wp->SaveWorkPanelSetup();
        }
    }
}

//------------------------------------------------------------------------------

void CWorkPanelList::ShowWorkPanels(CProject* p_project)
{
    foreach(CWorkPanel* p_wp, WorkPanels){
        if( p_wp->GetProject() == p_project ){
            p_wp->Show();
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CWorkPanelList::RemoveAllWorkPanels(bool force)
{
    foreach(CWorkPanel* p_wp, WorkPanels){
        if( force ){
            delete p_wp;
        } else {
            p_wp->deleteLater();
        }
    }
}

//------------------------------------------------------------------------------

void CWorkPanelList::RemoveAllProjectWorkPanels(CProject* p_project)
{
    foreach(CWorkPanel* p_wp, WorkPanels){
        if( p_wp->GetProject() == p_project ){
            p_wp->deleteLater();
        }
    }
}

//------------------------------------------------------------------------------

void CWorkPanelList::RemoveAllGlobalWorkPanels(void)
{
    foreach(CWorkPanel* p_wp, WorkPanels){
        if( p_wp->GetProject() == NULL ){
            p_wp->deleteLater();
        }
    }
}

//------------------------------------------------------------------------------

CWorkPanel* CWorkPanelList::Find(const CUUID& uuid)
{
    foreach(CWorkPanel* p_wp, WorkPanels){
        if( p_wp->GetType() == uuid ) return(p_wp);
    }

    return(NULL);
}

//------------------------------------------------------------------------------

CWorkPanel* CWorkPanelList::Find(const CUUID& uuid,CProject* p_project)
{
    foreach(CWorkPanel* p_wp, WorkPanels){
        if( p_wp->GetType() == uuid ){
            if( p_wp->GetProject() == p_project ) return(p_wp);
        }
    }

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CWorkPanelList::WorkPanelAdded(CWorkPanel* p_wp)
{
    WorkPanels.push_back(p_wp);
    emit OnWorkPanelAdded(p_wp);
}

//------------------------------------------------------------------------------

void CWorkPanelList::WorkPanelRemoved(CWorkPanel* p_wp)
{
    WorkPanels.removeAll(p_wp);
    emit OnWorkPanelRemoved(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



