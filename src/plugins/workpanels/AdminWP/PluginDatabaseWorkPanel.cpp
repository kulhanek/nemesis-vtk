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

#include <QtGui>

#include <PluginObject.hpp>
#include <PluginModule.hpp>
#include <PluginDatabase.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>

#include "PluginDatabaseWorkPanel.hpp"
#include "GlobalWorkPanelsModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PluginDatabaseWorkPanelCB(void* p_data);

CExtUUID    PluginDatabaseWorkPanelID(
                "{PLUGIN_DATABASE_WORK_PANEL:4428c905-e31c-4d8a-9720-1e6738b70601}",
                "Plugin Database");

CPluginObject   PluginDatabaseWorkPanelObject(&GlobalWorkPanelsPlugin,
                PluginDatabaseWorkPanelID,WORK_PANEL_CAT,
                ":images/GlobalWorkPanels/PluginDatabase.svg",
                PluginDatabaseWorkPanelCB );

// -----------------------------------------------------------------------------

CPluginDatabaseWorkPanel* p_s4_window = NULL;

// -----------------------------------------------------------------------------

QObject* PluginDatabaseWorkPanelCB(void* p_data)
{
    if( p_s4_window != NULL ) return(p_s4_window);
    p_s4_window = new CPluginDatabaseWorkPanel;
    return(p_s4_window);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPluginDatabaseWorkPanel::CPluginDatabaseWorkPanel(void)
    : CWorkPanel(&PluginDatabaseWorkPanelObject,NULL,EWPR_WINDOW)
{
    WidgetUI.setupUi(this);

    // build model for tableview
    Model = new QStandardItemModel(this);
    WidgetUI.treeView->setModel(Model);
    WidgetUI.treeView->resizeColumnToContents(0);
    CreateModel();

    // ask for messages
    RegisterEventForServer(PluginDatabase_Changed, &PluginDatabase);

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CPluginDatabaseWorkPanel::~CPluginDatabaseWorkPanel()
{
    SaveWorkPanelSetup();
    p_s4_window = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPluginDatabaseWorkPanel::CreateModel(void)
{
    // set model heaer
    QStringList header = QStringList();
    header << "Plugin name" << "Category" << "Number of objects";
    Model->setHorizontalHeaderLabels(header);

    // root item
    QStandardItem* rootItem = Model->invisibleRootItem();

    // iterators for modules
    CSimpleIteratorC<CPluginModule> I(PluginDatabase.GetModuleList());
    CPluginModule* p_module;

    // standard items
    QStandardItem *moduleName, *objectName, *objectCat, *objectNum;

    int num_of_objects = 0;
    // loop over modules
    while ((p_module = I.Current()) != NULL) {
        moduleName = new QStandardItem(QString(p_module->GetModuleName()));
        moduleName->setEditable(false);
        rootItem->appendRow(moduleName);
        // append objects
        CSimpleIteratorC<CPluginObject> J(p_module->GetObjectList());
        CPluginObject* p_object;
        // loop over objects
        while ((p_object = J.Current()) != NULL) {
            QList<QStandardItem*> objectRow;

            //objectName->setIcon(*icon);
            objectName = new QStandardItem(p_object->GetIcon(),QString(p_object->GetName()));
            objectName->setEditable(false);
            objectCat = new QStandardItem(QString(p_object->GetCategoryUUID().GetName()));
            objectCat->setEditable(false);
            objectNum = new QStandardItem(QString::number(p_object->GetObjectCount()));
            objectNum->setEditable(false);
            objectRow << objectName << objectCat << objectNum;
            moduleName->appendRow(objectRow);
            num_of_objects++;
            J++;
        }
        I++;
    }
    WidgetUI.numOfObjectsSB->setValue(num_of_objects);
}

//------------------------------------------------------------------------------

bool CPluginDatabaseWorkPanel::ProcessEvent(const CEventIdentifier& event,
        CEventManager* Sender,void* p_data)
{
    // iterators for modules
    CSimpleIteratorC<CPluginModule> I(PluginDatabase.GetModuleList());
    CPluginModule* p_module;
    // loop over modules
    int i = 0, j;
    while ((p_module = I.Current()) != NULL) {
        // loop over objects
        CSimpleIteratorC<CPluginObject> J(p_module->GetObjectList());
        CPluginObject* p_object;
        QModelIndex pluginIndex = Model->index(i, 0, QModelIndex());
        j = 0;
        while ((p_object = J.Current()) != NULL) {
            // check wheather value was changed
            QModelIndex objectIndex = Model->index(j, 2, pluginIndex);
            QStandardItem* item = Model->itemFromIndex ( objectIndex );
            if (item == NULL) {
                ES_ERROR("null item");
                return(false);
            }
            int value = item->text().toInt();
            if (p_object->GetObjectCount() != value) {
                item->setText(QString::number(p_object->GetObjectCount()));
            }
            J++;
            j++;
        }
        I++;
        i++;
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



