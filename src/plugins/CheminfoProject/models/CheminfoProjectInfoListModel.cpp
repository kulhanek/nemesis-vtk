// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008-2009 Petr Kulhanek, kulhanek@enzim.hu,
//                            Jakub Stepan, xstepan3@chemi.muni.cz
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


#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <QMimeData>

#include "CheminfoProjectModule.hpp"
#include "CheminfoProjectInfoList.hpp"
#include "CheminfoProjectInfoListModel.hpp"
#include "CheminfoProjectInfoListModel.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CheminfoProjectInfoListModelCB(void* p_data);

//------------------------------------------------------------------------------

CExtUUID        CheminfoProjectInfoListModelID(
                    "{CHEMINFO_PROJECT_LIST_MODEL:d37378c5-a5e6-4a60-a968-c6c22115ac8c}",
                    "Cheminfo Projects");

CPluginObject   CheminfoProjectInfoListModelObject(&CheminfoProjectPlugin,
                    CheminfoProjectInfoListModelID,MODEL_CAT,
                    ":../images/CheminfoProject/12.cheminfo/CheminfoProjectInfoList.svg",
                    CheminfoProjectInfoListModelCB);

// -----------------------------------------------------------------------------

QObject* CheminfoProjectInfoListModelCB(void* p_data)
{
    CCheminfoProjectInfoList* p_con = static_cast<CCheminfoProjectInfoList*>(p_data);
    if( p_con == NULL ) return(NULL);

    CCheminfoProjectInfoListModel* p_obj = new CCheminfoProjectInfoListModel(NULL);
    p_obj->SetRootObject(p_con);

    return(p_obj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectInfoListModel::CCheminfoProjectInfoListModel(QObject* p_parent)
    : CContainerModel(&CheminfoProjectInfoListModelObject,p_parent)
{
    RootObject = NULL;
}

//------------------------------------------------------------------------------

void CCheminfoProjectInfoListModel::SetRootObject(CCheminfoProjectInfoList* p_data)
{
    if( RootObject != NULL ) {
        RootObject->disconnect(this);
    }

    RootObject = p_data;
    if( RootObject != NULL ) {
        connect(RootObject,SIGNAL(destroyed(QObject*)),
                this,SLOT(RootObjectDeleted(void)));

        connect(RootObject,SIGNAL(OnCheminfoProjectInfoListChanged(void)),
                this,SLOT(ListChanged()));
    }
    // FIXME
    beginResetModel();
    endResetModel();
}

//------------------------------------------------------------------------------

QObject*  CCheminfoProjectInfoListModel::GetItem(const QModelIndex& index) const
{
    if( ! index.isValid() ) return(NULL);
    CCheminfoProjectInfoList* p_obj = static_cast<CCheminfoProjectInfoList*>(index.internalPointer());
    return(p_obj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QVariant CCheminfoProjectInfoListModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
    if( orientation != Qt::Horizontal ) return( QVariant() );
    if( role == Qt::TextAlignmentRole ) return(Qt::AlignHCenter);
    if( role != Qt::DisplayRole ) return( QVariant() );

    switch(section) {
        case 0:
            return(tr("ID"));
        case 1:
            return(tr("Name"));
        case 2:
            return(tr("Number of results"));
        case 3:
            return(tr("Operator"));
        case 4:
            return(tr("Description"));
        case 5:
            return(tr("Number of properties"));
        case 6:
            return(tr("Properties"));
        default:
            return("");
    }
}

//------------------------------------------------------------------------------

int CCheminfoProjectInfoListModel::columnCount(const QModelIndex &parent) const
{
    return(7);
}

//------------------------------------------------------------------------------

int CCheminfoProjectInfoListModel::rowCount(const QModelIndex &parent) const
{
    if( RootObject == 0 ) return(0);
    return( RootObject->children().count() );
}

//------------------------------------------------------------------------------

QModelIndex CCheminfoProjectInfoListModel::index(int row, int column,
                                  const QModelIndex &parent) const
{
    if( RootObject == NULL ) return( QModelIndex() );

    if( ! hasIndex(row, column, parent) ) return( QModelIndex() );
    if( parent.isValid() )  return( QModelIndex() );

    QModelIndex index = createIndex(row, column, RootObject->children()[row]);

    return(index);
}

//------------------------------------------------------------------------------

QModelIndex CCheminfoProjectInfoListModel::parent(const QModelIndex &index) const
{
    // parent is always invalid
    return( QModelIndex() );
}

//------------------------------------------------------------------------------

QVariant CCheminfoProjectInfoListModel::data(const QModelIndex &index, int role) const
{
    if( RootObject == NULL ) return( QVariant() );
    if( ! index.isValid() ) return( QVariant() );

    CCheminfoProjectInfo* p_obj = static_cast<CCheminfoProjectInfo*>(index.internalPointer());

    switch(role) {
    case Qt::DisplayRole: {
        switch( index.column() ) {
        case 0:
            return(index.row());
        case 1:
            return(p_obj->GetName().GetBuffer());
        case 2:
            return(p_obj->GetNumberOfResults());
        case 3:
            return(p_obj->GetOperator().GetBuffer());
        case 4:
            return(p_obj->GetDescription().GetBuffer());
        case 5:
            return(p_obj->GetNumberOfProperties());
        case 6:
            return(p_obj->GetProperties().GetBuffer());
        default:
            return( QVariant() );
        }
        break;
    }

    case Qt::TextAlignmentRole:
        switch( index.column() ) {
            case 0:
                return(Qt::AlignRight);
            case 1:
                return(Qt::AlignRight);
            case 2:
                return(Qt::AlignRight);
            case 3:
                return(Qt::AlignRight);
            case 4:
                return(Qt::AlignRight);
            case 5:
                return(Qt::AlignRight);
            case 6:
                return(Qt::AlignRight);
            default:
                return( QVariant() );
        }
        break;
    }
    return( QVariant() );
}

//------------------------------------------------------------------------------

Qt::ItemFlags CCheminfoProjectInfoListModel::flags(const QModelIndex &index) const
{
    if( !index.isValid() ) return(0);
    return(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectInfoListModel::RootObjectDeleted(void)
{
    RootObject = NULL;
    // FIXME
    beginResetModel();
    endResetModel();
}

//------------------------------------------------------------------------------

void CCheminfoProjectInfoListModel::ListChanged(void)
{
    // FIXME
    beginResetModel();
    endResetModel();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


