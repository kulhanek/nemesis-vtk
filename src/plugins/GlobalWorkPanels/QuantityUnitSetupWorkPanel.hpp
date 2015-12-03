#ifndef QuantityUnitSetupWorkPanelH
#define QuantityUnitSetupWorkPanelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

#include <QtGui>
#include <WorkPanel.hpp>
#include "ui_QuantityUnitSetupWorkPanel.h"

// -----------------------------------------------------------------------------

class CPhysicalQuantity;
class CContainerModel;
class CXMLElement;

// -----------------------------------------------------------------------------

class CQuantityUnitSetupWorkPanel : public CWorkPanel {
    Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CQuantityUnitSetupWorkPanel(void);
    ~CQuantityUnitSetupWorkPanel();

// section of private data ----------------------------------------------------
private:
    int                             ActualQuantity;
    int                             ActualUnit;
    Ui::QuantityUnitSetupWorkPanel  WidgetUI;
    CContainerModel*                QuantitiesModel;
    CContainerModel*                UnitModel;

    void ApplyOrDiscardQuestion(void);

private slots:
    void ButtonBoxClicked(QAbstractButton* p_button);
    void ChangeQuantity(const QModelIndex & );
    void ChangeUnit(const QModelIndex &);
    void ChangeUnit(int);
    void ChangeUnit(const QString& text);
    void NewUnit(void);
    void ApplyUnit(void);
    void RemoveUnit(void);
    void ActivateUnit(void);
};

// -----------------------------------------------------------------------------

#endif
