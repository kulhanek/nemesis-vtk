// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <NemesisCoreModule.hpp>
#include <GraphicsProfileModel.hpp>
#include <CategoryUUID.hpp>
#include <GraphicsProfile.hpp>
#include <GraphicsProfileObject.hpp>
#include <GraphicsObject.hpp>
#include <GraphicsObjectLink.hpp>
#include <QBrush>
#include <QMimeData>
#include <Project.hpp>

#include "GraphicsProfileModel.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GraphicsProfileModelCB(void* p_data);

//------------------------------------------------------------------------------

CExtUUID        GraphicsProfileModelID(
                    "{GRAPHICS_PROFILE_MODEL:c6f27ac7-90c3-4443-83a0-b4cafe10878e}",
                    "Graphics profile model");

CPluginObject   GraphicsProfileModelObject(&NemesisCorePlugin,
                    GraphicsProfileModelID,MODEL_CAT,
                    GraphicsProfileModelCB);

// -----------------------------------------------------------------------------

QObject* GraphicsProfileModelCB(void* p_data)
{
    CGraphicsProfile* p_con = static_cast<CGraphicsProfile*>(p_data);
    if( p_con == NULL ) return(NULL);

    CGraphicsProfileModel* p_obj = new CGraphicsProfileModel(NULL);
    p_obj->SetRootObject(p_con);

    return(p_obj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsProfileModel::CGraphicsProfileModel(QObject* p_parent)
    : CContainerModel(&GraphicsProfileModelObject,p_parent)
{
    RootObject = NULL;
}

//------------------------------------------------------------------------------

void CGraphicsProfileModel::SetRootObject(CGraphicsProfile* p_data)
{
    if( RootObject != NULL ) {
        RootObject->disconnect(this);
    }

    RootObject = p_data;
    if( RootObject != NULL ) {
        connect(RootObject,SIGNAL(destroyed(QObject*)),
                this,SLOT(RootObjectDeleted(void)));

        connect(RootObject,SIGNAL(OnGraphicsProfileChanged(void)),
                this,SLOT(GraphicsProfileChanged(void)));
    }
    // FIXME
    beginResetModel();
    endResetModel();
}

//------------------------------------------------------------------------------

QObject*  CGraphicsProfileModel::GetItem(const QModelIndex& index) const
{
    if( ! index.isValid() ) return(NULL);
    CGraphicsObject* p_obj = static_cast<CGraphicsObject*>(index.internalPointer());
    return(p_obj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QVariant CGraphicsProfileModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
    if( orientation != Qt::Horizontal ) return( QVariant() );
    if( role == Qt::TextAlignmentRole ) return(Qt::AlignHCenter);
    if( role != Qt::DisplayRole ) return( QVariant() );

    switch(section) {
        case 0:
            return(tr("Name"));
        case 1:
            return(tr("Type"));
        case 2:
            return(tr("Description"));
        case 3:
            return(tr("ID"));
        default:
            return("");
    }
}

//------------------------------------------------------------------------------

int CGraphicsProfileModel::columnCount(const QModelIndex &parent) const
{
    return(4);
}

//------------------------------------------------------------------------------

int CGraphicsProfileModel::rowCount(const QModelIndex &parent) const
{
    if( RootObject == 0 ) return(0);
    return( RootObject->GetNumberOfGraphicsObject() );
}

//------------------------------------------------------------------------------

QModelIndex CGraphicsProfileModel::index(int row, int column,
                                  const QModelIndex &parent) const
{
    if( RootObject == NULL ) return( QModelIndex() );

    if( ! hasIndex(row, column, parent) ) return( QModelIndex() );
    if( parent.isValid() )  return( QModelIndex() );

    QModelIndex index = createIndex(row, column, RootObject->GetGraphicsProfileObject(row) );

    return(index);
}

//------------------------------------------------------------------------------

QModelIndex CGraphicsProfileModel::parent(const QModelIndex &index) const
{
    // parent is always invalid
    return( QModelIndex() );
}

//------------------------------------------------------------------------------

QVariant CGraphicsProfileModel::data(const QModelIndex &index, int role) const
{
    if( RootObject == NULL ) return( QVariant() );
    if( ! index.isValid() ) return( QVariant() );

    CGraphicsProfileObject* p_obj = static_cast<CGraphicsProfileObject*>(index.internalPointer());
    CGraphicsObject* p_gobj = p_obj->GetObject();

    switch(role) {
        case Qt::DisplayRole: {
            switch( index.column() ) {
                case 0:
                    return(p_gobj->GetName());
                case 1:
                    return(p_gobj->GetPluginObject()->GetName());
                case 2:
                    return(p_gobj->GetDescription());
                case 3:
                    return(p_gobj->GetIndex());
                default:
                    return( QVariant() );
            }
            break;
        }

        case Qt::TextAlignmentRole:
            switch( index.column() ) {
                case 0:
                case 1:
                case 2:
                    return(Qt::AlignLeft);
                case 3:
                    return(Qt::AlignRight);
                default:
                    return( QVariant() );
            }
        break;

        case Qt::ForegroundRole: {
                QBrush brush;
                if( p_obj->IsFlagSet(EPOF_VISIBLE) ){
                    brush.setColor(Qt::black);
                } else {
                    brush.setColor(Qt::lightGray);
                }
                return(brush);
            }
            break;

        case Qt::DecorationRole: {
            switch( index.column() ) {
                case 0:{
                    QIcon icon;
                    CGraphicsObjectLink* p_glink = dynamic_cast<CGraphicsObjectLink*>(p_gobj);
                    if( p_glink != NULL ){
                        CProObject* p_pobj = p_glink->GetEndObject();
                        if( p_obj ){
                            icon = p_pobj->GetPluginObject()->GetIcon();
                        }
                    } else {
                        icon = p_gobj->GetPluginObject()->GetIcon();
                    }
                    QPixmap pixmap;
                    if( p_obj->IsFlagSet(EPOF_VISIBLE) ){
                        pixmap = icon.pixmap(64,QIcon::Normal);
                    } else {
                        pixmap = icon.pixmap(64,QIcon::Disabled);
                    }
                    QIcon final(pixmap);
                    return(final);
                }
                default:
                    return( QVariant() );
            }
            break;
        }
    }

    return( QVariant() );
}

//------------------------------------------------------------------------------

Qt::ItemFlags CGraphicsProfileModel::flags(const QModelIndex &index) const
{
    if( !index.isValid() ) return(Qt::ItemIsDropEnabled);
    if( index.column() == 0 ){
        return(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled);
    } else {
        return(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
    }
}

//------------------------------------------------------------------------------

QStringList CGraphicsProfileModel::mimeTypes(void) const
{
    QStringList types;
    types << RootObject->GetProjectMIMEType("graphics.indexes");
    return types;
}

//------------------------------------------------------------------------------

QMimeData* CGraphicsProfileModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData* p_mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach(const QModelIndex &index, indexes) {
        if( index.column() != 0 ) continue;
        CGraphicsObject* p_object = dynamic_cast<CGraphicsObject*>(GetItem(index));
        if( p_object ){
             stream << p_object->GetIndex();
         }
    }

    p_mimeData->setData(RootObject->GetProjectMIMEType("graphics.indexes"), encodedData);
    return(p_mimeData);
}

//------------------------------------------------------------------------------

Qt::DropActions CGraphicsProfileModel::supportedDropActions(void) const
{
    return(Qt::CopyAction);
}

//------------------------------------------------------------------------------

bool CGraphicsProfileModel::dropMimeData(const QMimeData* p_data,
     Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if( action == Qt::IgnoreAction ){
        return(true);
    }

    if( RootObject == NULL ){
        return(false);
    }

    if( ! p_data->hasFormat(RootObject->GetProjectMIMEType("graphics.indexes")) ){
        return(false);
    }

    QByteArray encodedData = p_data->data(RootObject->GetProjectMIMEType("graphics.indexes"));
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    CProject* p_project = RootObject->GetProject();
    CHistoryNode* p_history = p_project->BeginChangeWH(EHCL_GRAPHICS,"add objects to the profile");
    if( p_history == NULL ) return(false);

    while( ! stream.atEnd() ) {
        int index;
        stream >> index;
        CProObject* p_obj = p_project->FindObject(index);
        if( p_obj == NULL ) continue;
        CGraphicsObject* p_gobj = dynamic_cast<CGraphicsObject*>(p_obj);
        if( p_gobj == NULL ) continue;
        RootObject->AddObject(p_gobj,row,p_history);
        if( row >= 0 ) row++;
    }

    p_project->EndChangeWH();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsProfileModel::RootObjectDeleted(void)
{
    RootObject = NULL;
    // FIXME
    beginResetModel();
    endResetModel();
}

//------------------------------------------------------------------------------

void CGraphicsProfileModel::GraphicsProfileChanged(void)
{
    // FIXME
    beginResetModel();
    endResetModel();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


