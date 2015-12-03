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

#include <PhysicalQuantity.hpp>
#include <PhysicalQuantities.hpp>

#include "CheminfoProjectModule.hpp"
#include "CheminfoProjectStructureList.hpp"
#include "CheminfoStructureListModel.hpp"
#include "CheminfoStructureListModel.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CheminfoStructureListModelCB(void* p_data);

//------------------------------------------------------------------------------

CExtUUID        CheminfoStructureListModelID(
                    "{CHEMINFO_STRUCTURE_LIST_MODEL:a9176ccf-bdff-488f-b1c5-d10bdd418dcb}",
                    "Cheminfo Structures");

CPluginObject   CheminfoStructureListModelObject(&CheminfoProjectPlugin,
                    CheminfoStructureListModelID,MODEL_CAT,
                    ":../images/CheminfoProject/12.cheminfo/CheminfoStructureList.svg",
                    CheminfoStructureListModelCB);

// -----------------------------------------------------------------------------

QObject* CheminfoStructureListModelCB(void* p_data)
{
    CCheminfoProjectStructureList* p_con = static_cast<CCheminfoProjectStructureList*>(p_data);
    if( p_con == NULL ) return(NULL);

    CCheminfoStructureListModel* p_obj = new CCheminfoStructureListModel(NULL);
    p_obj->SetRootObject(p_con);

    return(p_obj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoStructureListModel::CCheminfoStructureListModel(QObject* p_parent)
    : CContainerModel(&CheminfoStructureListModelObject,p_parent)
{
    RootObject = NULL;
}

//------------------------------------------------------------------------------

void CCheminfoStructureListModel::SetRootObject(CCheminfoProjectStructureList* p_data)
{
    if( RootObject != NULL ) {
        RootObject->disconnect(this);
    }

    RootObject = p_data;
    if( RootObject != NULL ) {
        connect(RootObject,SIGNAL(destroyed(QObject*)),
                this,SLOT(RootObjectDeleted(void)));

        connect(RootObject,SIGNAL(OnCheminfoProjectStructureListChanged(void)),
                this,SLOT(ListChanged()));

        NumInfo = 6;
        NumResults = RootObject->GetProjectInfo()->GetNumberOfResults();
        NumColumns = NumInfo + NumResults;
    }
    // FIXME
    beginResetModel();
    endResetModel();
}

//------------------------------------------------------------------------------

QObject*  CCheminfoStructureListModel::GetItem(const QModelIndex& index) const
{
    if( ! index.isValid() ) return(NULL);
    CCheminfoProjectStructureList* p_obj = static_cast<CCheminfoProjectStructureList*>(index.internalPointer());
    return(p_obj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QVariant CCheminfoStructureListModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
    if( orientation != Qt::Horizontal ) return( QVariant() );
   // if( role == Qt::TextAlignmentRole ) return(Qt::AlignHCenter);
    if( role != Qt::DisplayRole ) return( QVariant() );

    switch(section) {
        case 0:
            return(tr("ID"));
        case 1:
            return(tr("Flag"));
        case 2:
            return(tr("InChiKey"));
        case 3:
            return(tr("Number atoms"));
        case 4:
            return(tr("Partial charge"));
        case 5: {
            QString col = tr("Molar mass [%1]");
            col = col.arg(PQ_MASS->GetUnitName());
            return(col);
        }    
//        case 6: {
//            QString col = tr("R1 E [%1]");
//            col = col.arg(PQ_ENERGY->GetUnitName());
//            return(col);
//        }
       default: {
            if (((NumInfo - 1 ) < section) && (section < NumColumns) ) {
                QString col = tr("R%1 E [%2]");
                col = col.arg(section - NumInfo + 1);
                col = col.arg(PQ_ENERGY->GetUnitName());
                return(col);
            }  else {
                return("");
            }
        }
//            return("");
    }

}

//------------------------------------------------------------------------------

int CCheminfoStructureListModel::columnCount(const QModelIndex &parent) const
{
    return(NumColumns);
}

//------------------------------------------------------------------------------

int CCheminfoStructureListModel::rowCount(const QModelIndex &parent) const
{
    if( RootObject == 0 ) return(0);
    return( RootObject->children().count() );
}

//------------------------------------------------------------------------------

QModelIndex CCheminfoStructureListModel::index(int row, int column,
                                  const QModelIndex &parent) const
{
    if( RootObject == NULL ) return( QModelIndex() );

    if( ! hasIndex(row, column, parent) ) return( QModelIndex() );
    if( parent.isValid() )  return( QModelIndex() );

    QModelIndex index = createIndex(row, column, RootObject->children()[row]);

    return(index);
}

//------------------------------------------------------------------------------

QModelIndex CCheminfoStructureListModel::parent(const QModelIndex &index) const
{
    // parent is always invalid
    return( QModelIndex() );
}

//------------------------------------------------------------------------------

QVariant CCheminfoStructureListModel::data(const QModelIndex &index, int role) const
{
    if( RootObject == NULL ) return( QVariant() );
    if( ! index.isValid() ) return( QVariant() );

    CCheminfoProjectStructure* p_obj = static_cast<CCheminfoProjectStructure*>(index.internalPointer());

    switch(role) {
    case Qt::DisplayRole: {
        switch( index.column() ) {
        case 0:
            return(p_obj->GetID().GetBuffer());
        case 1:
            return(p_obj->GetFlag());
        case 2: {
            QString inchi;
            inchi += p_obj->GetInChiKey().GetBuffer();
            return(inchi);
        }
        case 3:
            return(p_obj->GetNumberOfAtoms());
        case 4:
            return(p_obj->GetPartialCharge() );
        case 5:
            return(PQ_MASS->GetRealValueText(p_obj->GetMolarMass() ) );
//        case 6:
//            return(PQ_ENERGY->GetRealValueText(p_obj->GetResult1() ) );
        default: {
          //  return( QVariant() );
                if (((NumInfo - 1 ) < index.column()) && (index.column() < NumColumns ) ) {
                    // vector of results is count from 0
                    return ( PQ_ENERGY->GetRealValueText(p_obj->GetResults()->at( index.column( ) - NumInfo ) ) );
                }  else {
                    return( QVariant() );
                }
            }
        }
        break;

    }

    case Qt::TextAlignmentRole:
        switch( index.column() ) {
            case 0:
                return(Qt::AlignJustify);
            case 1:
                return(Qt::AlignJustify);
            case 2:
                return(Qt::AlignJustify);
            case 3:
                return(Qt::AlignJustify);
            case 4:
                return(Qt::AlignJustify);
            case 5:
                return(Qt::AlignJustify);
//            case 6:
//                return(Qt::AlignRight);
            default: {
                if (((NumInfo - 1 ) < index.column()) && (index.column() < NumColumns ) ) {
                    return(Qt::AlignJustify);
                }  else {
                    return( QVariant() );
                }
            }
        }
        break;
    }
    return( QVariant() );
}

//------------------------------------------------------------------------------

Qt::ItemFlags CCheminfoStructureListModel::flags(const QModelIndex &index) const
{
    if( !index.isValid() ) return(0);
    return(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoStructureListModel::RootObjectDeleted(void)
{
    RootObject = NULL;
    // FIXME
    beginResetModel();
    endResetModel();
}

//------------------------------------------------------------------------------

void CCheminfoStructureListModel::ListChanged(void)
{
    // FIXME
    beginResetModel();
    endResetModel();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


