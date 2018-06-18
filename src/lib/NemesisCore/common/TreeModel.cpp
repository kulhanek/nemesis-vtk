// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010        Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <TreeModel.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <ExtComObject.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        TreeModelID(
                    "{TREE_MODEL:e0d26a61-ab41-48bd-b54b-1bd90e94e591}",
                    "Tree model");

CPluginObject   TreeModelObject(&NemesisCorePlugin,
                    TreeModelID,MODEL_CAT,
                    ":images/AdminWorkPanels/error.png",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTreeModel::CTreeModel(QObject* p_parent)
    : QStandardItemModel(p_parent), CComObject(&TreeModelObject)
{
    RootObject = NULL;
    HeaderData = "Containers";
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTreeModel::SetRootObject(CExtComObject* p_data)
{
    if( RootObject != NULL ) {
        RootObject->disconnect(this);
        clear();
    }

    RootObject = p_data;

    QStringList headers;
    headers << HeaderData;
    setHorizontalHeaderLabels(headers);

    if( RootObject != NULL ) {
        // respond to object removal
        connect(RootObject,SIGNAL(destroyed(QObject*)),
                this,SLOT(RootObjectDeleted(void)));
        // populate initial data
        AddSubContainers(invisibleRootItem(),RootObject);
    }
}

//------------------------------------------------------------------------------

void CTreeModel::SetHeaderText(QString text)
{
    HeaderData = text;
    QStringList headers;
    headers << HeaderData;
    setHorizontalHeaderLabels(headers);
}

//------------------------------------------------------------------------------

void CTreeModel::AddSubContainers(QStandardItem* p_parent,CExtComObject* p_data)
{
    foreach(QObject* p_qobj, p_data->children()){
        CExtComObject* p_eobj = dynamic_cast<CExtComObject*>(p_qobj);
        if( p_eobj == NULL ) continue;
        if( p_eobj->ConFlags.testFlag(EECOF_HIDDEN) == true ) continue;
        AddContainer(p_parent,p_eobj);
    }
}

//------------------------------------------------------------------------------

void CTreeModel::AddContainer(QStandardItem* p_parent,CExtComObject* p_eobj)
{
    QStandardItem* p_nitem;
    if( p_eobj->GetPluginObject() ) {
        p_nitem = new QStandardItem(p_eobj->GetPluginObject()->GetIcon(),p_eobj->GetName());
    } else {
        p_nitem = new QStandardItem(p_eobj->GetName());
    }

    QVariant v;
    v.setValue(static_cast<QObject*>(p_eobj));
    p_nitem->setData(v);
    p_parent->appendRow(p_nitem);

    // register events
    p_eobj->disconnect(this);
    connect(p_eobj,SIGNAL(OnChildContainerAdded(QObject*,QObject*)),
            this,SLOT(ChildContainerAdded(QObject*,QObject*)));
    connect(p_eobj,SIGNAL(OnChildContainerRemoved(QObject*)),
            this,SLOT(ChildContainerRemoved(QObject*)));
    connect(p_eobj,SIGNAL(OnChildContainerChanged(QObject*)),
            this,SLOT(ChildContainerChanged(QObject*)));

    if( p_eobj->ConFlags.testFlag(EECOF_SUB_CONTAINERS) == true ){
        AddSubContainers(p_nitem,p_eobj);
    }
}

//------------------------------------------------------------------------------

QStandardItem* CTreeModel::FindItem(QStandardItem* p_item,CExtComObject* p_data)
{
    QVariant v;
    v.setValue(static_cast<QObject*>(p_data));

    for(int i=0; i < p_item->rowCount(); i++){
        QStandardItem* p_citem = p_item->child(i);
        if( p_citem->data() == v ) return(p_citem);
        if( p_citem->hasChildren() ){
            QStandardItem* p_std = FindItem(p_citem,p_data);
            if( p_std != NULL ) return(p_std);
        }
    }

    return(NULL);
}

//------------------------------------------------------------------------------

CContainerModel* CTreeModel::GetContainerModel(const QModelIndex& index)
{
    if( ! index.isValid() ) return(NULL);
    QStandardItem* p_item = itemFromIndex(index);

    QVariant v = p_item->data();
    CExtComObject* p_eobj = dynamic_cast<CExtComObject*>(v.value<QObject*>());
    if( p_eobj == NULL ) return(NULL);

    CContainerModel* p_model = p_eobj->GetContainerModel(this);
    return(p_model);
}

//------------------------------------------------------------------------------

QModelIndex CTreeModel::FindItemIndex(CExtComObject* p_obj)
{
    if( p_obj == NULL ) return(QModelIndex());

    QStandardItem* p_item = FindItem(invisibleRootItem(),p_obj);
    if( p_item == NULL ) return(QModelIndex());

    return( p_item->index() );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTreeModel::RootObjectDeleted(void)
{
    RootObject = NULL;
    clear();

    // restore header
    QStringList headers;
    headers << HeaderData;
    setHorizontalHeaderLabels(headers);
}

//------------------------------------------------------------------------------

void CTreeModel::ChildContainerAdded(QObject* p_parent,QObject* p_obj)
{
    CExtComObject* p_pobj = dynamic_cast<CExtComObject*>(p_parent);
    if( p_pobj == NULL ) return;
    CExtComObject* p_eobj = dynamic_cast<CExtComObject*>(p_obj);
    if( p_eobj == NULL ) return;

    QStandardItem* p_item = FindItem(invisibleRootItem(),p_pobj);
    if( p_item == NULL ) return;

    AddContainer(p_item,p_eobj);
}

//------------------------------------------------------------------------------

void CTreeModel::ChildContainerRemoved(QObject* p_obj)
{
    CExtComObject* p_eobj = dynamic_cast<CExtComObject*>(p_obj);
    if( p_eobj == NULL ) return;
    QStandardItem* p_item = FindItem(invisibleRootItem(),p_eobj);
    if( p_item == NULL ) return;

    QStandardItem* p_parent = p_item->parent();
    if( p_parent ){
        p_item->removeColumn(0);
        p_parent->removeRow(p_item->row());
    }
}

//------------------------------------------------------------------------------

void CTreeModel::ChildContainerChanged(QObject* p_obj)
{
    CExtComObject* p_eobj = dynamic_cast<CExtComObject*>(p_obj);
    if( p_eobj == NULL ) return;
    QStandardItem* p_item = FindItem(invisibleRootItem(),p_eobj);
    if( p_item == NULL ) return;
    p_item->setText(p_eobj->GetName());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



