#ifndef TreeModelH
#define TreeModelH
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

#include <NemesisCoreMainHeader.hpp>
#include <QStandardItemModel>
#include <ComObject.hpp>

// -----------------------------------------------------------------------------

class CExtComObject;
class CContainerModel;

// -----------------------------------------------------------------------------

/// root of all dynamic objects

class NEMESIS_CORE_PACKAGE CTreeModel : public QStandardItemModel, public CComObject {
Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CTreeModel(QObject* p_parent);

    /// set root object
    void SetRootObject(CExtComObject* p_data);

    /// set header text
    void SetHeaderText(QString text);

    /// get container enumerator
    CContainerModel* GetContainerModel(const QModelIndex& index);

    /// return model index from object
    QModelIndex FindItemIndex(CExtComObject* p_obj);

// section of private data ----------------------------------------------------
private:
    CExtComObject*  RootObject;
    QString         HeaderData;

    void AddSubContainers(QStandardItem* p_parent,CExtComObject* p_data);
    void AddContainer(QStandardItem* p_parent,CExtComObject* p_eobj);
    QStandardItem* FindItem(QStandardItem* p_item,CExtComObject* p_data);

private slots:
    void RootObjectDeleted(void);
    void ChildContainerAdded(QObject* p_parent,QObject* p_obj);
    void ChildContainerRemoved(QObject* p_obj);
    void ChildContainerChanged(QObject* p_obj);
};

// -----------------------------------------------------------------------------

#endif
