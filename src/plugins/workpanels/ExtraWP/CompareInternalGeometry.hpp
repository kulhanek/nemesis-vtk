#ifndef CompareInternalGeometryH
#define CompareInternalGeometryH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <WorkPanel.hpp>
#include "ui_CompareInternalGeometry.h"
#include <QFileDialog>
#include <QStandardItemModel>

//------------------------------------------------------------------------------

class CAtom;
class CBond;

//------------------------------------------------------------------------------

/// print internal geometry of selected structure

class CCompareInternalGeometry : public CWorkPanel {
Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CCompareInternalGeometry(CProject* p_project);
    ~CCompareInternalGeometry(void);

// section of private data ----------------------------------------------------
private:
    Ui::CompareInternalGeometry     WidgetUI;
    QStandardItemModel*             BondsModel;
    QStandardItemModel*             AnglesModel;
    QStandardItemModel*             TorsionsModel;

    bool CompareAtoms(CAtom* p_a1,CAtom* p_a2);
    bool CompareBonds(CBond* p_b1,CBond* p_b2);

    bool CompareBonds(void);
    bool CompareAngles(void);
    bool CompareTorsions(void);

private slots:
    void CompareInternalCoordinates(void);
    void ClearAllCoordinates(void);
    void HighlightingToggled(bool set);
    void TabChanged(void);
    void HighlightNone(bool repaint=false);
    void BondsSelectionChanged(void);
    void AnglesSelectionChanged(void);
    void TorsionsSelectionChanged(void);
};

//------------------------------------------------------------------------------

#endif
