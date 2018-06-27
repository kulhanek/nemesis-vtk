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

#include <ProjectList.hpp>
#include <Project.hpp>
#include <QWebEnginePage>

#include "SketchProjectWindow.hpp"
#include "SketchProject.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSketchProjectWindow::ConnectEditMenu(void)
{
    connect(WidgetUI.actionUndo,SIGNAL(triggered(bool)),
            this,SLOT(UndoChange(void)));
    //-----------------
    connect(WidgetUI.actionRedo,SIGNAL(triggered(bool)),
            this,SLOT(RedoChange(void)));
    //-----------------
    connect(WidgetUI.actionSelectAll,SIGNAL(triggered(bool)),
            this,SLOT(SelectAll(void)));
    //-----------------
    connect(WidgetUI.actionClearSelection,SIGNAL(triggered(bool)),
            this,SLOT(ClearSelection(void)));
    //-----------------
    connect(WidgetUI.actionCutStructure,SIGNAL(triggered(bool)),
            this,SLOT(CutStructure(void)));
    //-----------------
    connect(WidgetUI.actionCopyStructure,SIGNAL(triggered(bool)),
            this,SLOT(CopyStructure(void)));
    //-----------------
    connect(WidgetUI.actionPasteStructure,SIGNAL(triggered(bool)),
            this,SLOT(PasteStructure(void)));
    //-----------------
    connect(WidgetUI.actionDeleteAll,SIGNAL(triggered(bool)),
            this,SLOT(DeleteAll(void)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSketchProjectWindow::UndoChange(void)
{
    QWebEnginePage* p_mf = WebView->page();
    p_mf->runJavaScript("ui.onClick_Undo.call($('undo'));");
}

//------------------------------------------------------------------------------

void CSketchProjectWindow::RedoChange(void)
{
    QWebEnginePage* p_mf = WebView->page();
    p_mf->runJavaScript("ui.onClick_Redo.call($('redo'));");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSketchProjectWindow::SelectAll(void)
{
    QWebEnginePage* p_mf = WebView->page();
    p_mf->runJavaScript("ui.selectMode('selector_lasso'); ui.selectAll();");
}

//------------------------------------------------------------------------------

void CSketchProjectWindow::ClearSelection(void)
{
    QWebEnginePage* p_mf = WebView->page();
    p_mf->runJavaScript("ui.clearSelection();");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSketchProjectWindow::CutStructure(void)
{
    QWebEnginePage* p_mf = WebView->page();
    p_mf->runJavaScript("ui.onClick_Cut.call($('cut'));");
}

//------------------------------------------------------------------------------

void CSketchProjectWindow::CopyStructure(void)
{
    QWebEnginePage* p_mf = WebView->page();
    p_mf->runJavaScript("ui.onClick_Copy.call($('copy'));");
}

//------------------------------------------------------------------------------

void CSketchProjectWindow::PasteStructure(void)
{
    QWebEnginePage* p_mf = WebView->page();
    p_mf->runJavaScript("ui.onClick_Paste.call($('paste'));");
}

//------------------------------------------------------------------------------

void CSketchProjectWindow::DeleteAll(void)
{
    QWebEnginePage* p_mf = WebView->page();
    p_mf->runJavaScript("ui.onClick_DeleteAll.call($('delete_all'));");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




