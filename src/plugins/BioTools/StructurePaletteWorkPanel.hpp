#ifndef StructurePaletteWorkPanelH
#define StructurePaletteWorkPanelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include "ui_StructurePaletteWorkPanel.h"
#include <FragmentPalette.hpp>
#include <QDialog>
#include <HistoryList.hpp>

// -----------------------------------------------------------------------------

enum EPaletteAction { // supported actions
    EBA_NONE,
    EBA_ADD_STRUCTURE
};

// -----------------------------------------------------------------------------

class CStructurePaletteWorkPanel : public CWorkPanel {
Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CStructurePaletteWorkPanel(CProject* p_project);
    ~CStructurePaletteWorkPanel();

// section of private data ----------------------------------------------------
private:
    Ui::StructurePaletteWorkPanel   WidgetUI;
    CFragmentPalette                PentoseFragments;
    CFragmentPalette                HexoseFragments;
    QButtonGroup*                   ActionGroup;
    QAbstractButton*                ActiveButton;
    CSelectionRequest*              SelRequest;
    EPaletteAction                  Action;

    // load fragment palette
    bool LoadFragments(void);

    // return active fragment
    CFragment* GetActiveFragment(void);

    // set action
    void SetAction(EPaletteAction new_action);

private slots:
    void ActionChange(QAbstractButton* p_button);
    void SelectionCompleted(void);
    void SelectionDetached(void);
    void ProjectLockChanged(EHistoryChangeMessage message);
};

// -----------------------------------------------------------------------------

#endif
