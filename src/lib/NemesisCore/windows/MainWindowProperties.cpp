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

#include <MainWindow.hpp>
#include <QMenu>
#include <list>
#include <PluginDatabase.hpp>
#include <CategoryUUID.hpp>
#include <PropertyList.hpp>
#include <Project.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ConnectPropertiesMenu(void)
{
    CONNECT_ACTION(NewProperty);
    CONNECT_ACTION(ManagePropertiesWP);
    CONNECT_ACTION(ExportPropertiesWP);

    CONNECT_ACTION(SelectedProperties);
    CONNECT_ACTION(DeleteInvalidProperties);
    CONNECT_ACTION(DeleteAllProperties);

    BIND_MENU(menuImportProperty);

    //-----------------
    connect(HistoryList,SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(UpdatePropertiesMenu()));

    SetupImportPropertiesMenu();
    UpdatePropertiesMenu();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::UpdatePropertiesMenu(void)
{
    CLockLevels curr_levels = HistoryList->GetLockModeLevels();
    bool set = ! curr_levels.testFlag(EHCL_PROPERTY);

    actionNewProperty->setEnabled(set);
    menuImportProperty->setEnabled(set);
}

//------------------------------------------------------------------------------

void  CMainWindow::SetupImportPropertiesMenu(void)
{
    list<CPluginObject*>    impitems;

    CSimpleIteratorC<CPluginObject>    I(PluginDatabase.GetObjectList());
    CPluginObject*                     p_obj;
    QActionGroup*                      p_actGroup = new QActionGroup(this);
    while( (p_obj = I.Current()) != NULL ) {
        if (p_obj->GetCategoryUUID() == IMPORT_PROPERTY_CAT ) {
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
        CreateImportPropertyAction(p_actGroup,menuImportProperty,p_obj);
        iit++;
    }

    // any items?
    if( impitems.size() == 0 ) {
        menuImportProperty->addAction(tr("- not available -"));
    }

    connect(p_actGroup,SIGNAL(triggered(QAction *)),this,SLOT(ImportPropertyAction  (QAction*)));
}

//------------------------------------------------------------------------------

void CMainWindow::CreateImportPropertyAction(QActionGroup* p_actGroup,QMenu* p_menu,CPluginObject* p_pod)
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

    // create action
    QAction* p_act = p_finmenu->addAction(p_pod->GetName());
    p_act->setProperty("property.uuid",QVariant(QString(p_pod->GetObjectUUID().GetFullStringForm())));
    p_actGroup->addAction(p_act);

}

//------------------------------------------------------------------------------

void CMainWindow::ImportPropertyAction(QAction* p_act)
{
    CExtUUID uuid;
    // impex object
    QVariant type = p_act->property("property.uuid");
    uuid.LoadFromString(type.toString().toLatin1().constData());

    // open dialog
    ExecuteObject(uuid,Project);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::NewProperty(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{NEW_PROPERTY_WORK_PANEL:4a0b7079-006c-4c7b-a26a-2fec50413de5}");
    ShowAsDialog(mp_uuid, Project);
}

//------------------------------------------------------------------------------

void CMainWindow::ManagePropertiesWP(void)
{
    CExtUUID mp_uuid;
    mp_uuid.LoadFromString("{PROPERTIES_MANAGER_WORK_PANEL:25843821-dab1-4b5a-a594-d1c0603f0dfd}");
    OpenToolPanel(mp_uuid, true);
}

//------------------------------------------------------------------------------

void CMainWindow::SelectedProperties(void)
{
    Project->GetProperties()->DeleteSelectedPropertiesWH();
}

//------------------------------------------------------------------------------

void CMainWindow::DeleteInvalidProperties(void)
{
    Project->GetProperties()->DeleteInvalidPropertiesWH();
}

//------------------------------------------------------------------------------

void CMainWindow::DeleteAllProperties(void)
{
    Project->GetProperties()->DeleteAllPropertiesWH();
}

//------------------------------------------------------------------------------

void CMainWindow::ExportPropertiesWP(void)
{
//    CExtUUID mp_uuid;
//    mp_uuid.LoadFromString();
//    OpenToolPanel(mp_uuid, true);
// FIXME
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

