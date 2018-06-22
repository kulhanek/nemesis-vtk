// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

#include <QtGui>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>

#include <PluginObject.hpp>
#include <PluginModule.hpp>
#include <PluginDatabase.hpp>
#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>

#include <Project.hpp>
#include <ProjectList.hpp>
#include <WorkPanelList.hpp>
#include <NemesisJScript.hpp>

#include "NewProjectWorkPanel.hpp"
#include "AdminWPModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* NewProjectWorkPanelCB(void* p_data);

CExtUUID        NewProjectWorkPanelID(
                    "{NEW_PROJECT_WORK_PANEL:4cf836af-c5ca-4799-b0b7-bd53209d969e}",
                    "New project");

CPluginObject   NewProjectWorkPanelObject(&AdminWPPlugin,
                    NewProjectWorkPanelID,WORK_PANEL_CAT,
                    ":/images/AdminWP/NewProject.svg",
                    NewProjectWorkPanelCB);

CNewProjectWorkPanel* p_new_project_window = NULL;

// -----------------------------------------------------------------------------

QObject* NewProjectWorkPanelCB(void* p_data)
{
    if( p_new_project_window != NULL ) return(p_new_project_window);
    p_new_project_window = new CNewProjectWorkPanel;
    return(p_new_project_window);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNewProjectWorkPanel::CNewProjectWorkPanel(void)
    // EWPR_DIALOG has a bug with save/restore window position
    : CWorkPanel(&NewProjectWorkPanelObject,NULL,EWPR_WINDOW)
{
    WidgetUI.setupUi(this);
    BuildProject = NULL;

    // set ok button inactive
    WidgetUI.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    // set up model and assign it to listView
    Model1 = new QStandardItemModel(this);
    WidgetUI.listView1->setModel(Model1);

    Model2 = new QStandardItemModel(this);
    WidgetUI.listView2->setModel(Model2);

    // fill both models
    CSimpleIteratorC<CPluginModule> I(PluginDatabase.GetModuleList());
    CPluginModule* p_module;

    CExtUUID        buildProjectID("{BUILD_PROJECT:b64d16f0-b73f-4747-9a13-212ab9a15d38}");

    // loop over modules
    while ((p_module = I.Current()) != NULL) {
        CSimpleIteratorC<CPluginObject> J(p_module->GetObjectList());
        CPluginObject* p_object;
        // loop over objects(
        while ((p_object = J.Current()) != NULL) {
            if (p_object->GetCategoryUUID() == PROJECT_CAT ) {
                QStandardItem* p_item = new QStandardItem(p_object->GetIcon(),
                                                        QString(p_object->GetName()));
                p_item->setEditable(false);
                p_item->setData(QString(p_object->GetObjectUUID().GetFullStringForm()));
                if( ! p_object->HasAttribute("incubator") ){
                    Model1->appendRow(p_item);
                } else {
                    Model2->appendRow(p_item);
                }
                if( p_object->GetObjectUUID() == buildProjectID ){
                    BuildProject = p_item;
                }
            }
            J++;
        }
        I++;
    }

    Model1->sort(0);
    Model2->sort(0);

    OpenProjectPB = WidgetUI.buttonBox->addButton(tr("Open ..."),QDialogButtonBox::AcceptRole);

    connect(WidgetUI.tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(CurrentTabChanged(void)));

    // list item activated -> description update
    connect(WidgetUI.listView1->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(ListItemChanged(void)));
    // ok, or double click on item -> project open, dialog close
    connect(WidgetUI.listView1, SIGNAL( doubleClicked( const QModelIndex&)),
            this, SLOT(ProjectNew(void)));

    connect(WidgetUI.listView2->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(ListItemChanged(void)));
    // ok, or double click on item -> project open, dialog close
    connect(WidgetUI.listView2, SIGNAL( doubleClicked( const QModelIndex&)),
            this, SLOT(ProjectNew(void)));

    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBox(QAbstractButton*)));

    // extra cleanup
    connect(this, SIGNAL(destroyed(QObject*)),
            NemesisJScript, SLOT(ShallQuitApplication(void)));

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

void CNewProjectWorkPanel::showEvent(QShowEvent* p_event)
{
    CWorkPanel::showEvent(p_event);

    QModelIndex index = Model1->indexFromItem(BuildProject);
    WidgetUI.listView1->setCurrentIndex(index);
    ListItemChanged();
}

//------------------------------------------------------------------------------

CNewProjectWorkPanel::~CNewProjectWorkPanel()
{
    SaveWorkPanelSetup();
    p_new_project_window = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CNewProjectWorkPanel::CurrentTabChanged(void)
{
    WidgetUI.listView1->selectionModel()->clearSelection();
    WidgetUI.listView2->selectionModel()->clearSelection();
}

//------------------------------------------------------------------------------

void CNewProjectWorkPanel::ListItemChanged(void)
{
    QModelIndexList selected_rows;

    if( WidgetUI.tabWidget->currentIndex() == 0 ){
        selected_rows = WidgetUI.listView1->selectionModel()->selectedRows();
    } else {
        selected_rows = WidgetUI.listView2->selectionModel()->selectedRows();
    }

    if( selected_rows.count() != 1 ){
        WidgetUI.descriptionTE->setHtml("<center>No description available.</center>");
        WidgetUI.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        WidgetUI.buttonBox->button(QDialogButtonBox::Cancel)->setFocus();
        return;
    }

    QStandardItem*  item = NULL;
    if( WidgetUI.tabWidget->currentIndex() == 0 ){
        item = Model1->itemFromIndex(selected_rows.at(0));
    } else {
        item = Model2->itemFromIndex(selected_rows.at(0));
    }

    if( item == NULL ) return; // should not happen

    QVariant data = item->data();

    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString((const char*) data.toString().toLatin1().constData());

    CPluginObject * p_object = PluginDatabase.FindPluginObject(mp_uuid);
    if (p_object == NULL) {
        ES_ERROR("Object not found");
        return;
    }
    QString text;

    if( p_object->GetDescription() != NULL ){
        text = p_object->GetDescription();
    } else {
        text = "<center>No description available.</center>";
    }

    WidgetUI.descriptionTE->setHtml(text);
    WidgetUI.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    WidgetUI.buttonBox->button(QDialogButtonBox::Ok)->setFocus();
}

//------------------------------------------------------------------------------

void CNewProjectWorkPanel::ButtonBox(QAbstractButton* p_button)
{
    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Ok ){
        ProjectNew();
    }
    if( p_button == OpenProjectPB ){
        ProjectOpen();
    }
    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Cancel ){
        // and close
        close();
    }
}

//------------------------------------------------------------------------------

void CNewProjectWorkPanel::ProjectNew(void)
{
    QModelIndexList selected_rows;

    if( WidgetUI.tabWidget->currentIndex() == 0 ){
        selected_rows = WidgetUI.listView1->selectionModel()->selectedRows();
    } else {
        selected_rows = WidgetUI.listView2->selectionModel()->selectedRows();
    }

    if( selected_rows.count() != 1 ){
        return;
    }

    QStandardItem*  item = NULL;
    if( WidgetUI.tabWidget->currentIndex() == 0 ){
        item = Model1->itemFromIndex(selected_rows.at(0));
    } else {
        item = Model2->itemFromIndex(selected_rows.at(0));
    }

    if( item == NULL ) return; // should not happen

    QVariant data = item->data();

    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString((const char*) data.toString().toLatin1().constData());

    class CProject* p_project = Projects->NewProject(mp_uuid);
    if( p_project == NULL ) {
        ES_ERROR("unable to create a new project");
        QMessageBox::critical(NULL, tr("New Project"),
                              tr("An error occurred during a project creating!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        return;
    }

    // then show project window
    // this is necessary for correct update of active window order in UNITY?
    p_project->ShowProject();

    // FIXME
    // solve unity problem with app icons?
    setEnabled(false);
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(close()));
    timer->start(1000);
}

//------------------------------------------------------------------------------

void CNewProjectWorkPanel::ProjectOpen(void)
{
    QString filename = QFileDialog::getOpenFileName(NULL,
                       tr("Open Project"),
                       (const char*)GlobalSetup->GetLastOpenFilePath(GenericProjectID),
                       tr("Nemesis projects (*.npr)"));

    if( filename == NULL ) return; // no file was selected

    // update last open path
    GlobalSetup->SetLastOpenFilePathFromFile(filename,GenericProjectID);

    // load project
    CFileName project_name;
    project_name = CSmallString(filename);
    CProject* p_project = Projects->OpenProject(project_name);

    if( p_project == NULL ){
        ES_ERROR("unable to load project");
        QMessageBox::critical(NULL, tr("Open Project"),
                              tr("An error occurred during project opening!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        return;
    }

    // then show project window
    // this is necessary for correct update of active window order in UNITY?
    p_project->ShowProject();

    // FIXME
    // solve unity problem with app icons?
    setEnabled(false);
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(close()));
    timer->start(1000);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================





