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

#include <ProjectList.hpp>
#include <Project.hpp>
#include <HistoryList.hpp>
#include <SelectionList.hpp>
#include <Structure.hpp>
#include <XMLPrinter.hpp>
#include <XMLParser.hpp>
#include <QMimeData>
#include <QClipboard>

#include "LeapProjectWindow.hpp"
#include "LeapProject.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CLeapProjectWindow::ConnectEditMenu(void)
{
    connect(WidgetUI.actionUndo,SIGNAL(triggered(bool)),
            this,SLOT(UndoChange(void)));
    //-----------------
    connect(WidgetUI.actionRedo,SIGNAL(triggered(bool)),
            this,SLOT(RedoChange(void)));

    //-----------------
    HistoryList = Project->GetHistory();
    connect(HistoryList,SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(UpdateEditMenu()));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CLeapProjectWindow::UpdateEditMenu(void)
{
//    CLockLevels curr_levels = HistoryList->GetLockModeLevels();
//    CLockLevels min_levels = HistoryList->GetMinLockModeLevels();

//    if( HistoryList->IsUndoActive() ) {
//        WidgetUI.actionUndo->setEnabled(true);
//        QString text("Undo: ");
//        text += HistoryList->GetUndoDescr(0);
//        WidgetUI.actionUndo->setText(text);
//    } else {
//        WidgetUI.actionUndo->setText(tr("Undo"));
//        WidgetUI.actionUndo->setEnabled(false);
//    }
//    if( HistoryList->IsRedoActive()) {
//        WidgetUI.actionRedo->setEnabled(true);
//        QString text("Redo: ");
//        text += HistoryList->GetRedoDescr(0);
//        WidgetUI.actionRedo->setText(text);
//    } else {
//        WidgetUI.actionRedo->setText(tr("Redo"));
//        WidgetUI.actionRedo->setEnabled(false);
//    }

//    bool op_enabled = Project->GetActiveStructure() != NULL;

//    WidgetUI.actionCutStructure->setEnabled(op_enabled && (! curr_levels.testFlag(EHCL_TOPOLOGY)));
//    WidgetUI.actionCopyStructure->setEnabled(op_enabled);

//    const QMimeData* p_mimeData = QApplication::clipboard()->mimeData();
//    if( p_mimeData == NULL ) {
//        WidgetUI.actionPasteStructure->setEnabled(false);
//    } else {
//        WidgetUI.actionPasteStructure->setEnabled(
//            p_mimeData->hasFormat("application/nemesis") && op_enabled && (! curr_levels.testFlag(EHCL_TOPOLOGY)));
//    }

//    WidgetUI.actionDeleteStructure->setEnabled(op_enabled && (! curr_levels.testFlag(EHCL_TOPOLOGY)));

//    WidgetUI.actionLockTopology->setChecked(curr_levels.testFlag(EHCL_TOPOLOGY));
//    WidgetUI.actionLockGeometry->setChecked(curr_levels.testFlag(EHCL_GEOMETRY));
//    WidgetUI.actionLockGraphics->setChecked(curr_levels.testFlag(EHCL_GRAPHICS));
//    WidgetUI.actionLockDescription->setChecked(curr_levels.testFlag(EHCL_DESCRIPTION));

//    LockGroup->setEnabled(true);
//    WidgetUI.actionLockTopology->setEnabled(! min_levels.testFlag(EHCL_TOPOLOGY));
//    WidgetUI.actionLockGeometry->setEnabled(! min_levels.testFlag(EHCL_GEOMETRY));
//    WidgetUI.actionLockGraphics->setEnabled(! min_levels.testFlag(EHCL_GRAPHICS));
//    WidgetUI.actionLockDescription->setEnabled(! min_levels.testFlag(EHCL_DESCRIPTION));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CLeapProjectWindow::UndoChange(void)
{
    Project->GetHistory()->Undo();
}

//------------------------------------------------------------------------------

void CLeapProjectWindow::RedoChange(void)
{
    Project->GetHistory()->Redo();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




