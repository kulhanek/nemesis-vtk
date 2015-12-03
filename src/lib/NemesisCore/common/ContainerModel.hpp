#ifndef ContainerModelH
#define ContainerModelH
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
#include <QAbstractItemModel>
#include <ComObject.hpp>

// -----------------------------------------------------------------------------

// IMPORTANT NOTE
// order of base objects is important, it allows to use static_cast on objects
// returned by QObject::children()

// -----------------------------------------------------------------------------

/// base class of all models

class NEMESIS_CORE_PACKAGE CContainerModel : public QAbstractItemModel, public CComObject {
public:
// constructors and destructors -----------------------------------------------
    CContainerModel(CPluginObject* p_pluginobject,QObject* p_parent);

// informational methods ------------------------------------------------------
    /// return object item from model index
    virtual QObject* GetItem(const QModelIndex& index) const;
};

// -----------------------------------------------------------------------------

#endif
