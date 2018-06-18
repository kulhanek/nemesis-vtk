#ifndef GraphSymmetryWorkPanelH
#define GraphSymmetryWorkPanelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2014 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include "ui_GraphSymmetryWorkPanel.h"
#include <QStandardItemModel>
#include <vector>

//------------------------------------------------------------------------------

/// print internal geometry of selected structure

class CGraphSymmetryWorkPanel : public CWorkPanel {
Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CGraphSymmetryWorkPanel(CProject* p_project);
    ~CGraphSymmetryWorkPanel(void);

// section of private data ----------------------------------------------------
private:
    Ui::GraphSymmetryWorkPanel  WidgetUI;
    QStandardItemModel*         VerticesModel;
    std::vector<unsigned int>   SymmetryClasses;
    bool                        DoNotReset;

private slots:
    void GenerateGraphSymmetry(void);
    void ClearAllVertices(void);
    void HighlightingToggled(bool set);
    void HighlightNone(bool repaint=false);
    void VerticesSelectionChanged(void);
    void ImprintGroups(void);
    void CopyToClipboard(void);
};

//------------------------------------------------------------------------------

#endif
