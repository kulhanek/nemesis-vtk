#ifndef StdPhysicalQuantityModelH
#define StdPhysicalQuantityModelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011        Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ContainerModel.hpp>

// -----------------------------------------------------------------------------

class CStdPhysicalQuantity;

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CStdPhysicalQuantityModel :  public CContainerModel {
Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CStdPhysicalQuantityModel(QObject* p_parent);

    /// set base physical quantities list
    void SetPhysicalQuntity(CStdPhysicalQuantity* p_data);

    /// return model item index by quantity unit id
    QModelIndex  GetIndex(int unit_index) const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

// section of private data ----------------------------------------------------
private:
    CStdPhysicalQuantity*    StdPhysicalQuantity;     // data

    QModelIndex index(int row, int column,
                      const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

private slots:
    void DataChanged(void);
};

// -----------------------------------------------------------------------------

#endif
