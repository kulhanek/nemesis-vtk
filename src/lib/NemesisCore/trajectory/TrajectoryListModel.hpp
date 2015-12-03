#ifndef TrajectoryListModelH
#define TrajectoryListModelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ContainerModel.hpp>

// -----------------------------------------------------------------------------

class CTrajectoryList;
class CProObject;

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CTrajectoryListModel : public CContainerModel {
Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CTrajectoryListModel(QObject* p_parent);

    /// set root object
    void SetRootObject(CTrajectoryList* p_data);

    /// return object item from model index
    virtual QObject* GetItem(const QModelIndex& index) const;

// section of private data ----------------------------------------------------
private:
    CTrajectoryList*    RootObject;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /// return supported types
    QStringList mimeTypes(void) const;

    /// return MimeData
    QMimeData*  mimeData(const QModelIndexList &indexes) const;

private slots:
    void RootObjectDeleted(void);
    void ListChanged(void);
};

// -----------------------------------------------------------------------------

#endif
