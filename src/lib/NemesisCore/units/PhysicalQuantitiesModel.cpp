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

#include <PhysicalQuantitiesModel.hpp>
#include <PhysicalQuantities.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* PhysicalQuantitiesModelCB(void* p_data);

CExtUUID        PhysicalQuantitiesModelID(
                    "{PHYSICAL_QUANTITIES_MODEL:9cb0f309-6005-4210-97f4-1177f6d6463b}",
                    "Physical quantities model");

CPluginObject   PhysicalQuantitiesModelObject(&NemesisCorePlugin,
                    PhysicalQuantitiesModelID,MODEL_CAT,
                    PhysicalQuantitiesModelCB);

// -----------------------------------------------------------------------------

QObject* PhysicalQuantitiesModelCB(void* p_data)
{
    CPhysicalQuantities* p_con = static_cast<CPhysicalQuantities*>(p_data);
    if( p_con == NULL ) return(NULL);

    CPhysicalQuantitiesModel* p_obj = new CPhysicalQuantitiesModel(NULL);
    p_obj->SetPhysicalQuntities(p_con);

    return(p_obj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPhysicalQuantitiesModel::CPhysicalQuantitiesModel(QObject* p_parent)
    : CContainerModel(&PhysicalQuantitiesModelObject,p_parent)
{
    PhysicalQuantities = NULL;
}

//------------------------------------------------------------------------------

void CPhysicalQuantitiesModel::SetPhysicalQuntities(CPhysicalQuantities* p_data)
{
    PhysicalQuantities = p_data;
    // FIXME
    beginResetModel();
    endResetModel();
}

//------------------------------------------------------------------------------

QModelIndex  CPhysicalQuantitiesModel::GetIndex(int qt_index) const
{
    if( PhysicalQuantities == NULL ) return( QModelIndex() );
    if( (qt_index < 0 ) || (qt_index >= PhysicalQuantities->NumOfQuantities()) ) return( QModelIndex() );
    return(createIndex(qt_index,0));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CPhysicalQuantitiesModel::columnCount(const QModelIndex &parent) const
{
    return(1);
}

//------------------------------------------------------------------------------

int CPhysicalQuantitiesModel::rowCount(const QModelIndex &parent) const
{
    if( PhysicalQuantities == 0 ) return(0);
    return( PhysicalQuantities->children().count() );
}

//------------------------------------------------------------------------------

QModelIndex CPhysicalQuantitiesModel::index(int row, int column,
        const QModelIndex &parent) const
{
    if( PhysicalQuantities == NULL ) return( QModelIndex() );

    if( ! hasIndex(row, column, parent) ) return( QModelIndex() );
    if( parent.isValid() )  return( QModelIndex() );

    const QObjectList& list = PhysicalQuantities->children();
    QModelIndex index = createIndex(row, column, list[row]);

    return(index);
}

//------------------------------------------------------------------------------

QModelIndex CPhysicalQuantitiesModel::parent(const QModelIndex &index) const
{
    // parent is always invalid
    return( QModelIndex() );
}

//------------------------------------------------------------------------------

QVariant CPhysicalQuantitiesModel::data(const QModelIndex &index, int role) const
{
    if( PhysicalQuantities == 0 ) return( QVariant() );
    if( ! index.isValid() ) return( QVariant() );
    if( role != Qt::DisplayRole) return( QVariant() );

    QObjectList list = PhysicalQuantities->children();

    if( index.row() >= list.count() ) return( QVariant() );

    return( list[index.row()]->objectName() );
}

//------------------------------------------------------------------------------

QVariant CPhysicalQuantitiesModel::headerData(int section, Qt::Orientation orientation,
        int role) const
{
    if( role != Qt::DisplayRole ) return( QVariant() );
    if( orientation != Qt::Horizontal ) return( QVariant() );

    return(tr("Quantity"));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



