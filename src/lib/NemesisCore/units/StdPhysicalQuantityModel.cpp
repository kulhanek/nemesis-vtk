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

#include <StdPhysicalQuantityModel.hpp>
#include <StdPhysicalQuantity.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>

#include "StdPhysicalQuantityModel.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* StdPhysicalQuantityModelCB(void* p_data);

CExtUUID        StdPhysicalQuantityModelID(
                    "{STD_PHYSICAL_QUANTITY_MODEL:a6098d98-c63c-40ae-a54b-20b14260e7d4}",
                    "Standard physical quantity model");

CPluginObject   StdPhysicalQuantityModelObject(&NemesisCorePlugin,
                    StdPhysicalQuantityModelID,MODEL_CAT,
                    StdPhysicalQuantityModelCB);

// -----------------------------------------------------------------------------

QObject* StdPhysicalQuantityModelCB(void* p_data)
{
    CStdPhysicalQuantity* p_con = static_cast<CStdPhysicalQuantity*>(p_data);
    if( p_con == NULL ) return(NULL);

    CStdPhysicalQuantityModel* p_obj = new CStdPhysicalQuantityModel(NULL);
    p_obj->SetPhysicalQuntity(p_con);

    return(p_obj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CStdPhysicalQuantityModel::CStdPhysicalQuantityModel(QObject* p_parent)
    : CContainerModel(&StdPhysicalQuantityModelObject,p_parent)
{
    StdPhysicalQuantity = NULL;
}

//------------------------------------------------------------------------------

void CStdPhysicalQuantityModel::SetPhysicalQuntity(CStdPhysicalQuantity* p_data)
{
    StdPhysicalQuantity = p_data;

    if( StdPhysicalQuantity != NULL ) {
        StdPhysicalQuantity->disconnect(this);
    }

    StdPhysicalQuantity = p_data;

    if( StdPhysicalQuantity != NULL ) {
        connect(StdPhysicalQuantity,SIGNAL(OnUnitChanged(void)),
                this,SLOT(DataChanged(void)));
    }

    // FIXME
    beginResetModel();
    endResetModel();
}

//------------------------------------------------------------------------------

QModelIndex  CStdPhysicalQuantityModel::GetIndex(int unit_index) const
{
    if( StdPhysicalQuantity == NULL ) return( QModelIndex() );
    if( (unit_index < 0 ) || (unit_index >= StdPhysicalQuantity->GetNumberOfUnits()) ) return( QModelIndex() );
    return(createIndex(unit_index,0));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CStdPhysicalQuantityModel::columnCount(const QModelIndex &parent) const
{
    return(6);
}

//------------------------------------------------------------------------------

int CStdPhysicalQuantityModel::rowCount(const QModelIndex &parent) const
{
    if( StdPhysicalQuantity == NULL ) return(0);
    return( StdPhysicalQuantity->GetNumberOfUnits() );
}

//------------------------------------------------------------------------------

QModelIndex CStdPhysicalQuantityModel::index(int row, int column,
        const QModelIndex &parent) const
{
    if( StdPhysicalQuantity == NULL ) return( QModelIndex() );

    if( ! hasIndex(row, column, parent) ) return( QModelIndex() );
    if( parent.isValid() )  return( QModelIndex() );

    QModelIndex index = createIndex(row, column);

    return(index);
}

//------------------------------------------------------------------------------

QModelIndex CStdPhysicalQuantityModel::parent(const QModelIndex &index) const
{
    // parent is always invalid
    return( QModelIndex() );
}

//------------------------------------------------------------------------------

QVariant CStdPhysicalQuantityModel::data(const QModelIndex &index, int role) const
{
    if( StdPhysicalQuantity == NULL ) return( QVariant() );
    if( ! index.isValid() ) return( QVariant() );
    if( role != Qt::DisplayRole) return( QVariant() );
    if( index.row() >= StdPhysicalQuantity->GetNumberOfUnits() ) return( QVariant() );

    QString unitName;
    char format;
    int decimals;
    double offset;
    double scale;
    bool active;

    StdPhysicalQuantity->GetUnit(index.row(),unitName,format,decimals,offset,scale,active);

    switch(index.column ()) {
        default:
        case 0:
            if( active == true ) return("#");
            return("");
        case 1:
            return(unitName);
        case 2:
            return(QChar(format));
        case 3:
            return(decimals);
        case 4:
            return(scale);
        case 5:
            return(offset);
    }

    return("test");
}

//------------------------------------------------------------------------------

QVariant CStdPhysicalQuantityModel::headerData(int section, Qt::Orientation orientation,
        int role) const
{
    if( role != Qt::DisplayRole ) return( QVariant() );
    if( orientation != Qt::Horizontal ) return( QVariant() );

    switch(section) {
    default:
    case 0:
        return("");
    case 1:
        return("Unit");
    case 2:
        return("Format");
    case 3:
        return("Decimals");
    case 4:
        return("Scale");
    case 5:
        return("Offset");
    }
}

//------------------------------------------------------------------------------

void CStdPhysicalQuantityModel::DataChanged(void)
{
    // FIXME
    beginResetModel();
    endResetModel();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



