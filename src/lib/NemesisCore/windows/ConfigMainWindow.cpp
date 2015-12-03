// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Tomas Marek, xmarek11@stud.fit.vutbr.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <QMessageBox>
#include <QCloseEvent>
#include <ErrorSystem.hpp>
#include <QSet>
#include <QMenuBar>
#include <ConfigMainWindow.hpp>
#include <MainWindow.hpp>
#include <CategoryUUID.hpp>
#include <NemesisCoreModule.hpp>
#include <Project.hpp>
#include <ProjectDesktop.hpp>
#include <GlobalDesktop.hpp>

#include "ConfigMainWindow.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        ConfigMainWindowID(
                    "{CONFIG_MAIN_WINDOW:4446ec31-b703-4a60-8b40-20b9e0b3ddf3}",
                    "Config Main Window");

CPluginObject   ConfigMainWindowObject(&NemesisCorePlugin,
                    ConfigMainWindowID,WORK_PANEL_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CConfigMainWindow::CConfigMainWindow(CMainWindow* p_parent)
    : CWorkPanel(&ConfigMainWindowObject,NULL,EWPR_DIALOG)
{
    Parent = p_parent;
    WidgetUI.setupUi(this);

    // project type
    WidgetUI.projectTypeLA->setText(Parent->GetProject()->GetType().GetName());

    // ---------------------------------
    ActionsModel = new QStandardItemModel(this);
    WidgetUI.treeViewActions->setModel(ActionsModel);
    UpdateActionsList();

    // ---------------------------------
    ToolBarsModel = new QStandardItemModel(this);
    WidgetUI.listViewToolBars->setModel(ToolBarsModel);
    UpdateToolBarsList();

    // ---------------------------------
    ToolBarActionsModel = new QStandardItemModel(this);
    WidgetUI.listViewActions->setModel(ToolBarActionsModel);

    // ---------------------------------
    connect(WidgetUI.treeViewActions, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(ActionListClicked(const QModelIndex&)));
    // -------------
    connect(WidgetUI.addToolBarButton, SIGNAL(clicked()),
            this, SLOT(AddToolBar()));
    // -------------
    connect(WidgetUI.removeToolBarButton, SIGNAL(clicked()),
            this, SLOT(RemoveToolBar()));
    // -------------
    connect(WidgetUI.listViewToolBars, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(ToolBarListClicked(const QModelIndex&)));

    connect(WidgetUI.addActionButton, SIGNAL(clicked()),
            this, SLOT(AddAction()));
    // -------------
    connect(WidgetUI.removeActionButton, SIGNAL(clicked()),
            this, SLOT(RemoveAction()));
    // -------------
    connect(WidgetUI.upActionButton, SIGNAL(clicked()),
            this, SLOT(MoveUpAction()));
    // -------------
    connect(WidgetUI.downActionButton, SIGNAL(clicked()),
            this, SLOT(MoveDownAction()));
    // -------------
    connect(WidgetUI.addSeparatorButton, SIGNAL(clicked()),
            this, SLOT(AddSeparator()));
    // -------------
    connect(WidgetUI.listViewActions, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(ToolBarActionListClicked(const QModelIndex&)));
    // -------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            this, SLOT(ButtonBoxClicked(QAbstractButton *)));
    // -------------
    connect(WidgetUI.setShortcutTB, SIGNAL(clicked()),
            this, SLOT(SetShortcut()));
    // -------------
    connect(WidgetUI.resetShortcutTB, SIGNAL(clicked()),
            this, SLOT(ResetShortcut()));

    // event filter for shortcuts
    WidgetUI.shortcutLE->installEventFilter(this);

    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CConfigMainWindow::~CConfigMainWindow(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CConfigMainWindow::ButtonBoxClicked(QAbstractButton *button)
{
    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Reset ) {
        Parent->LoadToolBars(); // load user configuration
        Parent->LoadShortcuts();
        UpdateToolBarsList();
        UpdateToolBarActionList();
        UpdateMenuItemsShortcuts();
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::RestoreDefaults ) {
        Parent->LoadToolBars(true); // load system default configuration
        Parent->LoadShortcuts(true);
        UpdateToolBarsList();
        UpdateToolBarActionList();
        UpdateMenuItemsShortcuts();
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Cancel ) {
        Parent->LoadToolBars(); // load previous configuration
        Parent->LoadShortcuts();
        close();
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Apply ) {
        if( WidgetUI.saveSetupCB->isChecked() ){
            // save new configuration
            Parent->SaveToolBars();
            Parent->SaveShortcuts();
            CProjectDesktop* p_pdsk = Parent->GetProject()->GetDesktop();
            p_pdsk->SaveMainWindowSetup();

            // propagate changes to all projects of the same type
            GlobalDesktop->NotifyAllMainWindows(Parent->GetProject()->GetType().GetFullStringForm());
        }
        close();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CConfigMainWindow::UpdateActionsList(void)
{
    ActionsModel->clear();

    QStringList header;
    header << "Actions list";
    ActionsModel->setHorizontalHeaderLabels(header);
    ActiveAction = NULL;

    QStandardItem*  p_rooti = ActionsModel->invisibleRootItem();
    UpdateMenuItems(Parent->menuBar(),p_rooti);

    WidgetUI.treeViewActions->collapseAll();

    WidgetUI.addActionButton->setEnabled(false);
    WidgetUI.removeActionButton->setEnabled(false);
    WidgetUI.upActionButton->setEnabled(false);
    WidgetUI.downActionButton->setEnabled(false);
    WidgetUI.addSeparatorButton->setEnabled(false);
}

//------------------------------------------------------------------------------

void CConfigMainWindow::UpdateMenuItems(QWidget* p_widget,QStandardItem* p_root)
{
    foreach(QAction* p_act, p_widget->actions()){
        if( p_act->isSeparator() ) continue;

        QStandardItem* p_actitem = new QStandardItem(p_act->icon(),
                                            p_act->text() + "    " + p_act->shortcut().toString());
        p_actitem->setEditable(false);

        if( p_act->menu() ){
            UpdateMenuItems(p_act->menu(),p_actitem);
        } else {
            QVariant tb;
            tb.setValue<void*>((void*)p_act);
            p_actitem->setData(tb);
        }
        p_root->appendRow(p_actitem);
    }
}

//------------------------------------------------------------------------------

void CConfigMainWindow::UpdateMenuItemsShortcuts(void)
{
    QStandardItem*  p_rooti = ActionsModel->invisibleRootItem();
    UpdateMenuItemsShortcuts(p_rooti);
}

//------------------------------------------------------------------------------

void CConfigMainWindow::UpdateMenuItemsShortcuts(QStandardItem* p_item)
{
    for(int i=0; i < p_item->rowCount(); i++){
        QStandardItem* p_sitem = p_item->child(i);
        QAction* p_act = static_cast<QAction*>(p_sitem->data().value<void*>());
        if( p_act ){
            p_sitem->setText(p_act->text() + "    " + p_act->shortcut().toString());
        } else {
            UpdateMenuItemsShortcuts(p_sitem);
        }
    }
}

//------------------------------------------------------------------------------

void CConfigMainWindow::ActionListClicked(const QModelIndex & index)
{
    QStandardItem* p_item = ActionsModel->itemFromIndex(index);
    ActiveAction = static_cast<QAction*>(p_item->data().value<void*>());

    if (ActiveAction == NULL) {
        WidgetUI.addActionButton->setEnabled(false);
        WidgetUI.setShortcutTB->setEnabled(false);
        WidgetUI.resetShortcutTB->setEnabled(false);
        WidgetUI.shortcutLE->setEnabled(false);
        return;
    }

    WidgetUI.setShortcutTB->setEnabled(true);
    WidgetUI.resetShortcutTB->setEnabled(true);
    WidgetUI.shortcutLE->setEnabled(true);

    WidgetUI.shortcutLE->setText(ActiveAction->shortcut().toString());

    if( ActiveToolBar != NULL ) {
        WidgetUI.addActionButton->setEnabled(ActiveToolBar->actions().contains(ActiveAction) == false);
    } else {
        WidgetUI.addActionButton->setEnabled(false);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CConfigMainWindow::UpdateToolBarsList(void)
{
    ActiveToolBar = NULL;

    ToolBarsModel->clear();

    QList<QToolBar*>    tlist = Parent->findChildren<QToolBar*>();
    QStandardItem*      p_rooti = ToolBarsModel->invisibleRootItem();

    foreach(QToolBar* p_tb,tlist) {
        if( p_tb->objectName().isEmpty() ) continue;

        QStandardItem* p_tbi = new QStandardItem(p_tb->objectName());
        p_tbi->setEditable(false);
        QVariant tb;
        tb.setValue<void*>((void*)p_tb);
        p_tbi->setData(tb);
        p_rooti->appendRow(p_tbi);
    }
    p_rooti->sortChildren(0);

    // list is recreated so always disable this button
    WidgetUI.removeToolBarButton->setEnabled(false);
    WidgetUI.addActionButton->setEnabled(false);
}

//------------------------------------------------------------------------------

void CConfigMainWindow::AddToolBar(void)
{
    QString NEW_TOOLBAR_NAME = tr("ToolBar%1");
    QString toolbar_name;

    int     i = 1;
    bool    name_ok;
    do {
        name_ok = true;
        toolbar_name = NEW_TOOLBAR_NAME.arg(i);
        i++;
        foreach(QToolBar* p_qobj,Parent->findChildren<QToolBar*>()) {
            if( p_qobj->objectName() == toolbar_name ) {
                name_ok = false;
                break;
            }
        }

    } while( ! name_ok );

    QToolBar*       p_tb = new QToolBar(Parent);
    p_tb->setObjectName(toolbar_name);
    p_tb->setWindowTitle(toolbar_name);

    Parent->addToolBar(p_tb);

    QStandardItem*  p_rooti = ToolBarsModel->invisibleRootItem();
    QStandardItem*  p_tbi = new QStandardItem(p_tb->objectName());
    p_tbi->setEditable(false);
    QVariant        tb;

    tb.setValue<void*>((void*)p_tb);
    p_tbi->setData(tb);
    p_rooti->appendRow(p_tbi);

    p_rooti->sortChildren(0);

    QModelIndex             idx = ToolBarsModel->indexFromItem(p_tbi);
    QItemSelection          selection(idx,idx);
    QItemSelectionModel*    p_selmodel = WidgetUI.listViewToolBars->selectionModel();

    p_selmodel->select(selection,QItemSelectionModel::ClearAndSelect);

    p_tb->show();

    ToolBarListClicked(idx);
}

//------------------------------------------------------------------------------

void CConfigMainWindow::RemoveToolBar(void)
{
    if( ActiveToolBar != NULL ) {
        delete ActiveToolBar;
        ActiveToolBar = NULL;
    }
    UpdateToolBarsList();
    UpdateToolBarActionList();
}

//------------------------------------------------------------------------------

void CConfigMainWindow::ToolBarListClicked(const QModelIndex & index)
{
    QStandardItem* p_item = ToolBarsModel->itemFromIndex(index);
    ActiveToolBar = (QToolBar*)p_item->data().value<void*>();

    WidgetUI.removeToolBarButton->setEnabled(true);
    WidgetUI.addActionButton->setEnabled(ActiveAction != NULL);

    UpdateToolBarActionList();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CConfigMainWindow::UpdateToolBarActionList(void)
{
    ActiveToolBarAction = NULL;

    ToolBarActionsModel->clear();

    if( ActiveToolBar == NULL ) {
        // list is recreated so always disable these buttons
        WidgetUI.addActionButton->setEnabled(false);
        WidgetUI.removeActionButton->setEnabled(false);
        WidgetUI.upActionButton->setEnabled(false);
        WidgetUI.downActionButton->setEnabled(false);
        WidgetUI.addSeparatorButton->setEnabled(false);
        return;
    }

    QList<QAction*>     tlist = ActiveToolBar->actions();
    QStandardItem*      p_rooti = ToolBarActionsModel->invisibleRootItem();

    foreach(QAction* p_act,tlist) {
        if( p_act->text().isEmpty() && !p_act->isSeparator()) continue;

        QStandardItem* p_acti = new QStandardItem(p_act->icon(),p_act->text());
        p_acti->setEditable(false);
        QVariant tb;
        tb.setValue<void*>((void*)p_act);
        p_acti->setData(tb);
        p_rooti->appendRow(p_acti);
    }

    // list is recreated so always disable these buttons
    if( ActiveAction ){
        WidgetUI.addActionButton->setEnabled(ActiveToolBar->actions().contains(ActiveAction) == false);
    } else {
        WidgetUI.addActionButton->setEnabled(false);
    }
    WidgetUI.removeActionButton->setEnabled(false);
    WidgetUI.upActionButton->setEnabled(false);
    WidgetUI.downActionButton->setEnabled(false);
    WidgetUI.addSeparatorButton->setEnabled(true);
}

//------------------------------------------------------------------------------

void CConfigMainWindow::ToolBarActionListClicked(const QModelIndex & index)
{
    QStandardItem* p_item = ToolBarActionsModel->itemFromIndex(index);
    ActiveToolBarAction = (QAction*)p_item->data().value<void*>();

    if( ActiveAction ){
        WidgetUI.addActionButton->setEnabled(ActiveToolBar->actions().contains(ActiveAction) == false);
    } else {
        WidgetUI.addActionButton->setEnabled(false);
    }
    WidgetUI.removeActionButton->setEnabled(true);

    WidgetUI.upActionButton->setEnabled(index.row() > 0);
    WidgetUI.downActionButton->setEnabled(index.row() < ToolBarActionsModel->rowCount() - 1);
}

//------------------------------------------------------------------------------

void CConfigMainWindow::AddAction(void)
{
    if( ActiveAction == NULL ) return;

    ActiveToolBar->addAction(ActiveAction);

    QStandardItem*  p_rooti = ToolBarActionsModel->invisibleRootItem();
    QStandardItem*  p_acti = new QStandardItem(ActiveAction->icon(),ActiveAction->text());
    p_acti->setEditable(false);
    QVariant        tb;

    tb.setValue<void*>((void*)ActiveAction);
    p_acti->setData(tb);
    p_rooti->appendRow(p_acti);

    QModelIndex             idx = ToolBarActionsModel->indexFromItem(p_acti);
    QItemSelection          selection(idx,idx);
    QItemSelectionModel*    p_selmodel = WidgetUI.listViewActions->selectionModel();

    p_selmodel->select(selection,QItemSelectionModel::ClearAndSelect);

    ToolBarActionListClicked(idx);
}

//------------------------------------------------------------------------------

void CConfigMainWindow::RemoveAction(void)
{
    ActiveToolBar->removeAction(ActiveToolBarAction);
    ActiveToolBarAction = NULL;
    UpdateToolBarActionList();
}

//------------------------------------------------------------------------------

void CConfigMainWindow::MoveUpAction(void)
{
    QItemSelectionModel*    p_selmodel = WidgetUI.listViewActions->selectionModel();
    QModelIndex             idx = p_selmodel->selectedRows()[0];
    if( idx.row() <= 0 )    return;

    QStandardItem*  p_item = ToolBarActionsModel->itemFromIndex(idx);
    QStandardItem*  p_sitem = p_item->clone();

    QStandardItem*  p_rooti = ToolBarActionsModel->invisibleRootItem();
    p_rooti->removeRow(idx.row());
    p_rooti->insertRow(idx.row()-1,p_sitem);

    idx = ToolBarActionsModel->indexFromItem(p_sitem);
    QItemSelection          selection(idx,idx);
    p_selmodel->select(selection,QItemSelectionModel::ClearAndSelect);

    WidgetUI.upActionButton->setEnabled(idx.row() > 0);
    WidgetUI.downActionButton->setEnabled(idx.row() < ToolBarActionsModel->rowCount() - 1);

    ActiveToolBar->clear();
    for(int i=0; i < ToolBarActionsModel->rowCount(); i++) {
        p_item = ToolBarActionsModel->item(i);
        QVariant tb = p_item->data();
        QAction* p_act = (QAction*)tb.value<void*>();
        ActiveToolBar->addAction(p_act);
    }
}

//------------------------------------------------------------------------------

void CConfigMainWindow::MoveDownAction(void)
{
    QItemSelectionModel*    p_selmodel = WidgetUI.listViewActions->selectionModel();
    QModelIndex             idx = p_selmodel->selectedRows()[0];
    if( idx.row() >= ToolBarActionsModel->rowCount() )    return;

    QStandardItem*  p_item = ToolBarActionsModel->itemFromIndex(idx);
    QStandardItem*  p_sitem = p_item->clone();

    QStandardItem*  p_rooti = ToolBarActionsModel->invisibleRootItem();
    p_rooti->removeRow(idx.row());
    p_rooti->insertRow(idx.row()+1,p_sitem);

    idx = ToolBarActionsModel->indexFromItem(p_sitem);
    QItemSelection          selection(idx,idx);
    p_selmodel->select(selection,QItemSelectionModel::ClearAndSelect);

    WidgetUI.upActionButton->setEnabled(idx.row() > 0);
    WidgetUI.downActionButton->setEnabled(idx.row() < ToolBarActionsModel->rowCount() - 1);

    ActiveToolBar->clear();
    for(int i=0; i < ToolBarActionsModel->rowCount(); i++) {
        p_item = ToolBarActionsModel->item(i);
        QVariant tb = p_item->data();
        QAction* p_act = (QAction*)tb.value<void*>();
        ActiveToolBar->addAction(p_act);
    }
}

//------------------------------------------------------------------------------

void CConfigMainWindow::AddSeparator(void)
{
    QAction* p_sep = ActiveToolBar->addSeparator();
    p_sep->setText("-- Separator --");

    QStandardItem*  p_rooti = ToolBarActionsModel->invisibleRootItem();
    QStandardItem*  p_acti = new QStandardItem("-- Separator --");
    p_acti->setEditable(false);
    QVariant        tb;

    tb.setValue<void*>((void*)p_sep);
    p_acti->setData(tb);
    p_rooti->appendRow(p_acti);

    QModelIndex             idx = ToolBarActionsModel->indexFromItem(p_acti);
    QItemSelection          selection(idx,idx);
    QItemSelectionModel*    p_selmodel = WidgetUI.listViewActions->selectionModel();

    p_selmodel->select(selection,QItemSelectionModel::ClearAndSelect);

    ToolBarActionListClicked(idx);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CConfigMainWindow::SetShortcut(void)
{
    if( WidgetUI.shortcutLE->text().isEmpty() ) return;

    // if no action is selected
    if( ! ActiveAction ) {
        WidgetUI.shortcutLE->setText(tr("Select action first..."));
        return;
    }

    // check if shortcut exists
    foreach(QMenu* p_menu, Parent->findChildren<QMenu*>()) {
        foreach (QAction* p_action, p_menu->actions()) {
            if( QKeySequence(WidgetUI.shortcutLE->text()).toString()
                == p_action->shortcut().toString() ) {
                WidgetUI.shortcutLE->setText(tr("Shortcut allready exists..."));
                return;
            }
        }
    }

    // set shortcut
    ActiveAction->setShortcut(QKeySequence(WidgetUI.shortcutLE->text()));

    // update action list
    UpdateMenuItemsShortcuts();
}

//------------------------------------------------------------------------------

void CConfigMainWindow::ResetShortcut(void)
{
    // reset active action and clear text
    WidgetUI.shortcutLE->setText("");

    // remove shortcut
    if(ActiveAction) {
        ActiveAction->setShortcut(QKeySequence(""));
    }

    // update action list
    UpdateMenuItemsShortcuts();
}

//------------------------------------------------------------------------------

/**
 * Shortcuts event filter. Reimplemented from QObject::eventFilter().
 * @param object object
 * @param event event
 * @return true when event identified, false when not
 */
bool CConfigMainWindow::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        CConfigMainWindow::HandleKeyEvent(keyEvent);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------

/**
 * Handler for dynamical adding of shortcuts.
 * @param event event
 */
void CConfigMainWindow::HandleKeyEvent(QKeyEvent *event)
{
    int current_key = event->key();

    // when currently pressed key is meta, conroll, shift or alt - shortcut is not finished
    if (current_key == Qt::Key_Control || current_key == Qt::Key_Shift || current_key == Qt::Key_Meta || current_key == Qt::Key_Alt){
        return;
    }

    // parse
    QKeyEvent *key_event = dynamic_cast<QKeyEvent *>(event);
    int accel = key_event->key() + key_event->modifiers();

    // build
    QKeySequence seq(accel);

    // use
    WidgetUI.shortcutLE->setText(seq.toString());
}
