#ifndef CPODesignerCheminfoProjectStructureListH
#define CPODesignerCheminfoProjectStructureListH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <QWidget>
#include <HistoryList.hpp>

#include "ui_PODesignerCheminfoProjectStructureList.h"

//------------------------------------------------------------------------------

class CCheminfoProjectStructureList;
class CContainerModel;
class QSortFilterProxyModel;
class CWorkPanel;

//------------------------------------------------------------------------------

class CPODesignerCheminfoProjectStructureList : public QWidget {
Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    CPODesignerCheminfoProjectStructureList(QWidget* p_owner, CCheminfoProjectStructureList *p_object, QWidget* p_widget);

// executive methods -----------------------------------------------------------
    /// initialize visualization of general properties
    void InitValues(void);

    /// update object general properties according to visual setup
    void ApplyValues(void);

// section of private data ----------------------------------------------------
private:
    Ui::PODesignerCheminfoProjectStructureList  WidgetUI;
    CCheminfoProjectStructureList*              List;
    CContainerModel*                            ObjectsModel;
    QSortFilterProxyModel*                      SortFilter;
    QWidget*                                    Widget;

private slots:
    void ObjectTVReset(void);
    void ObjectTVClicked(const QModelIndex& index);
    void ObjectTVDblClicked(const QModelIndex& index);
    void LoadStructures(void);
    void NewStructure(void);
    void ActivateStructure(void);
    void RemoveStructure(void);
    void ProjectLockChanged(EHistoryChangeMessage message);
};

//------------------------------------------------------------------------------

#endif
