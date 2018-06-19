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

#include <QtGui>

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <DesktopSystem.hpp>
#include <TreeModel.hpp>
#include <XMLElement.hpp>
#include <RootList.hpp>
#include <ContainerModel.hpp>
#include <ProObject.hpp>

#include "ObjectInspectorWorkPanel.hpp"
#include "AdminWPModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* ObjectInspectorWorkPanelCB(void* p_data);

CExtUUID        ObjectInspectorWorkPanelID(
                    "{OBJECT_INSPECTOR_WORK_PANEL:743b3270-c31e-4dfb-8ed4-583cbbf6f785}",
                    "Object Inspector");

CPluginObject   ObjectInspectorWorkPanelObject(&AdminWPPlugin,
                    ObjectInspectorWorkPanelID,WORK_PANEL_CAT,
                    ":images/AdminWP/ObjectInspector.svg",
                    ObjectInspectorWorkPanelCB);

// -----------------------------------------------------------------------------

CObjectInspectorWorkPanel* p_object_insp_wp = NULL;

// -----------------------------------------------------------------------------

QObject* ObjectInspectorWorkPanelCB(void* p_data)
{
    if (p_object_insp_wp != NULL) return(p_object_insp_wp);
    p_object_insp_wp = new CObjectInspectorWorkPanel;
    return(p_object_insp_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CObjectInspectorWorkPanel::CObjectInspectorWorkPanel(void)
    : CWorkPanel(&ObjectInspectorWorkPanelObject,NULL,EWPR_WINDOW)
{
    ConModel = NULL;

    WidgetUI.setupUi(this);

    // build models
    TreeModel = new CTreeModel(this);
    TreeModel->SetRootObject(RootList);

    // connect views with models
    WidgetUI.treeView->setModel(TreeModel);
    WidgetUI.treeView->resizeColumnToContents(0);

    // WidgetUI.listView->setModel(ListModel);
    ConModel = RootList->GetContainerModel(this);
    WidgetUI.listView->setModel(ConModel);
    WidgetUI.listView->resizeColumnToContents(0);

    WidgetUI.listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    WidgetUI.listView->setDragDropMode(QAbstractItemView::DragOnly);
    WidgetUI.listView->setDragEnabled(true);

    // connect signals with slots            
    connect(WidgetUI.treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(TreeViewSelectionChanged(void)));

    connect(WidgetUI.listView,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(listViewDoubleClicked(const QModelIndex&)));

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CObjectInspectorWorkPanel::~CObjectInspectorWorkPanel()
{
    SaveWorkPanelSetup();
    p_object_insp_wp = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CObjectInspectorWorkPanel::TreeViewSelectionChanged(void)
{
    if( ConModel != NULL ) delete ConModel;
    ConModel = NULL;

    QModelIndexList selected_rows = WidgetUI.treeView->selectionModel()->selectedRows();
    if( selected_rows.count() != 1 )  return;

    ConModel = TreeModel->GetContainerModel(selected_rows.at(0));
    UpdateNumberOfObjects();

    if( ConModel == NULL ) return;

    connect(ConModel,SIGNAL(modelReset(void)),
            this,SLOT(UpdateNumberOfObjects(void)));

    WidgetUI.listView->setModel(ConModel);

    for(int i=0; i < ConModel->columnCount(); i++) {
        WidgetUI.listView->resizeColumnToContents(i);
    }
}

//------------------------------------------------------------------------------

void CObjectInspectorWorkPanel::listViewDoubleClicked(const QModelIndex& index)
{
    if( ConModel == NULL ) return;

    QObject* p_object = ConModel->GetItem(index);
    if( p_object == NULL ) return;

    // is it container?
    CExtComObject* p_extobj = dynamic_cast<CExtComObject*>(p_object);
    QModelIndex tree_index = TreeModel->FindItemIndex(p_extobj);
    if( tree_index.isValid() ) {
        WidgetUI.treeView->expand(tree_index.parent());
        QItemSelection          selection(tree_index,tree_index);
        QItemSelectionModel*    p_selmodel = WidgetUI.treeView->selectionModel();
        p_selmodel->select(selection,QItemSelectionModel::ClearAndSelect);
        TreeViewSelectionChanged();
        return;
    }

    // is it project object?
    CProObject* p_proobj = dynamic_cast<CProObject*>(p_object);
    if( p_proobj != NULL ) {
        p_proobj->OpenObjectDesigner();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CObjectInspectorWorkPanel::UpdateNumberOfObjects(void)
{
    QString text = tr("Number of objects: %1");
    int num = 0;
    if( ConModel != NULL ) {
        num = ConModel->rowCount();
    }
    text = text.arg(num);
    WidgetUI.labelNumOfObjects->setText(text);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
