
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <DesktopSystem.hpp>
#include <ExtUUID.hpp>
#include <ErrorSystem.hpp>
#include <PluginDatabase.hpp>
#include <GlobalSetup.hpp>
#include <WorkPanel.hpp>
#include <Project.hpp>
#include <FileSystem.hpp>
#include <UUID.hpp>
#include <XMLElement.hpp>
#include <XMLParser.hpp>
#include <MainWindow.hpp>
#include <FileSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ConnectToolsMenu(void)
{    
    BIND_MENU(menuTools);

    CFileName name = GlobalSetup->GetSystemDesktopPath(Project,"tools_menu.xml");

    if( ! CFileSystem::IsFile(name) ){
        // menu setup does not exist
        delete menuTools;
        menuTools = NULL;
        return;
    }

    CXMLDocument xml_doc;
    CXMLParser   xml_parser;
    xml_parser.SetOutputXMLNode(&xml_doc);

    if( xml_parser.Parse(name) == false ) {
        ES_ERROR("Unable to parse tools menu items");
        return;
    }

    CXMLElement* p_ele = xml_doc.GetFirstChildElement("tools");
    if ( p_ele == NULL ) return;    // no tools
    QActionGroup* p_agroup = new QActionGroup(this);

    CreateToolActions(menuTools,p_agroup,p_ele);

    connect(p_agroup, SIGNAL(triggered(QAction*)),
            this, SLOT(ToolsMenuActionTriggered(QAction*)));
}

//------------------------------------------------------------------------------

void CMainWindow::CreateToolActions(QMenu* p_parent,QActionGroup* p_agroup,CXMLElement* p_ele)
{
    CXMLElement* p_tele = p_ele->GetFirstChildElement();

    while(p_tele  != NULL) {
        // separator -------------------
        if( p_tele->GetName() == "separator" ){
            menuTools->addSeparator();
        }
        // WP --------------------------
        if( p_tele->GetName() == "workpanel" ){
            QString suuid;
            p_tele->GetAttribute("uuid", suuid);

            CPluginObject* p_po;
            if( (p_po = PluginDatabase.FindPluginObject(CExtUUID(suuid))) != NULL ){
                QAction* p_action = new QAction(p_parent);
                p_action->setObjectName(suuid);
                p_action->setData(QVariant(suuid));
                p_action->setText(p_po->GetName());
                p_action->setToolTip(p_po->GetDescription());
                p_action->setIcon(p_po->GetIcon());
                p_parent->addAction(p_action);
                p_agroup->addAction(p_action);
            } else {
                CSmallString warning;
                warning << "Tool " << suuid << "does not have record in the plugin database";
                ES_WARNING(warning);
            }
        }
        // submenu --------------------------
        if( p_tele->GetName() == "menu" ){
            QString name;
            p_tele->GetAttribute("name",name);
            QMenu* p_menu = new QMenu(p_parent);
            p_menu->setTitle(name);
            p_parent->addMenu(p_menu);
            CreateToolActions(p_menu,p_agroup,p_tele);
        }
        p_tele = p_tele->GetNextSiblingElement();
    }
}

//------------------------------------------------------------------------------

void CMainWindow::ToolsMenuActionTriggered(QAction* p_a)
{
    if(p_a == NULL) return;
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString(p_a->data().toString());
    OpenToolPanel(mp_uuid, true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
