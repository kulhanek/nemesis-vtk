#ifndef StructureDesignerH
#define StructureDesignerH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <ProObjectDesigner.hpp>
#include "ui_StructureDesigner.h"
#include <PBCInfo.hpp>
#include <HistoryList.hpp>
#include <QStandardItemModel>

//------------------------------------------------------------------------------

class CStructure;
class CPODesignerGeneral;
class CPODesignerFlags;
class CPODesignerRefBy;

//------------------------------------------------------------------------------

class CStructureDesigner : public CProObjectDesigner {
Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CStructureDesigner(CStructure* p_str);

    /// initialize visualization of properties
    void InitAllValues(void);

    /// update object properties according to visual setup
    void ApplyAllValues(void);

// section of private data ----------------------------------------------------
private:
    CStructure*             Object;
    Ui::StructureDesigner   WidgetUI;
    CPODesignerGeneral*     General;
    CPODesignerFlags*       Flags;
    CPODesignerRefBy*       RefBy;
    QStandardItemModel*     StatModel;
    CContainerModel*        ResiduesModel;
    CContainerModel*        AtomsModel;
    CContainerModel*        BondsModel;
    CContainerModel*        RestraintsModel;
    CPBCInfo                PBCInfo;

private slots:
    void InitValues(void);
    void ApplyValues(void);

    void OpenProjectDesigner(void);
    void ObjectTVDblClicked(const QModelIndex& index);
    void ButtonBoxClicked(QAbstractButton *);
    void UpdateStatistics(void);

    void FlagGeometryChanged(void);
    void ProjectLockChanged(EHistoryChangeMessage message);

    void SetFromBoundingBox(void);
    void ShowBox(void);
    void MoveCOMToBoxCenter(void);
    void BoundinBoxAroundBoxCenter(void);

    void GenerateInChI(void);
};

//------------------------------------------------------------------------------

#endif
