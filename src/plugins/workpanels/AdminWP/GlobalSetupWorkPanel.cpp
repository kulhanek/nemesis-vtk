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

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <GlobalDesktop.hpp>
#include <PluginDatabase.hpp>
#include <GlobalSetup.hpp>
#include <MainWindow.hpp>
#include <Project.hpp>
#include <MouseDriverSetup.hpp>
#include <QMessageBox>
#include <QStandardItemModel>
#include <GraphicsSetupProfile.hpp>
#include <QFileDialog>

#include "GlobalSetupWorkPanel.hpp"
#include "AdminWPModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GlobalSetupWorkPanelCB(void* p_data);

CExtUUID        GlobalSetupWorkPanelID(
                    "{GLOBAL_SETUP_WORK_PANEL:6b9610e6-1c81-4b1c-9c2b-60f6af9a45be}",
                    "Global setup");

CPluginObject   GlobalSetupWorkPanelObject(&AdminWPPlugin,
                    GlobalSetupWorkPanelID,WORK_PANEL_CAT,
                    ":images/AdminWP/GlobalConfig.svg",
                    GlobalSetupWorkPanelCB);

// -----------------------------------------------------------------------------

CGlobalSetupWorkPanel* p_s2_window = NULL;

// -----------------------------------------------------------------------------

QObject* GlobalSetupWorkPanelCB(void* p_data)
{
    if( p_s2_window != NULL ) return(p_s2_window);
    p_s2_window = new CGlobalSetupWorkPanel;
    return(p_s2_window);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGlobalSetupWorkPanel::CGlobalSetupWorkPanel(void)
    : CWorkPanel(&GlobalSetupWorkPanelObject,NULL,EWPR_DIALOG)
{
    WidgetUI.setupUi(this);

    InitStartupTab();
    InitPathsTab();
    InitMouseTab();
    InitGraphicsTab();

    // signals
    connect(WidgetUI.getFixedPathTB, SIGNAL(clicked()),
           this, SLOT(UpdateFixedPath()));

    connect(WidgetUI.clearModulePathsTB, SIGNAL(clicked()),
           this, SLOT(ClearModulePaths()));


    connect(WidgetUI.restoreMouseSystemDefaultsPB, SIGNAL(clicked(bool)),
           this, SLOT(RestoreMouseSystemDefaults(void)));
    connect(WidgetUI.restoreMouseUserDefaultsPB, SIGNAL(clicked(bool)),
           this, SLOT(RestoreMouseUserDefaults(void)));
    connect(WidgetUI.applyUserMouseSetupPB, SIGNAL(clicked(bool)),
           this, SLOT(ApplyMouseSetup(void)));

    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton *)),
            SLOT(buttonBoxClicked(QAbstractButton *)));

    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CGlobalSetupWorkPanel::~CGlobalSetupWorkPanel()
{
    SaveWorkPanelSetup();
    p_s2_window = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGlobalSetupWorkPanel::InitStartupTab(void)
{
    CSimpleIteratorC<CPluginObject>   J(PluginDatabase.GetObjectList());
    CPluginObject*                    p_object;
    CUUID                             initial_project_type;
    int                               counter = 0;
    int                               index = -1;

    initial_project_type = GlobalDesktop->GetInitialProjectType();

    // loop over objects(
    while ((p_object = J.Current()) != NULL) {
        if (p_object->GetCategoryUUID() == PROJECT_CAT) {
            WidgetUI.projectsCB->addItem(QString(p_object->GetName()),
                                         QString(p_object->GetObjectUUID().GetStringForm()));
            if( p_object->GetObjectUUID() == initial_project_type ) index = counter;
            counter++;
        }
        J++;
    }

    if(index == -1 ) index = 0;
    WidgetUI.projectsCB->setCurrentIndex(index);

    connect(WidgetUI.userProjectRB, SIGNAL(toggled(bool)),
            WidgetUI.projectsCB, SLOT(setEnabled(bool)));

    switch(GlobalDesktop->GetProjectRestoreMode()) {
        case EDPRM_NEW_ROJECT_WP:
            WidgetUI.newProjectRB->setChecked(true);
            break;
        case EDPRM_USER:
            WidgetUI.userProjectRB->setChecked(true);
            break;
    }

    WidgetUI.projectsCB->setEnabled(WidgetUI.userProjectRB->isChecked());

    WidgetUI.desktopRestoreCB->setChecked(GlobalDesktop->IsWPRestorationEnabled());

    WidgetUI.terminateApplicationAfterLastProjectCB->setChecked(!GlobalDesktop->IsInitialProjectEnabledAfterLastIsClosed());
}

//------------------------------------------------------------------------------

void CGlobalSetupWorkPanel::SaveStartupTab(void)
{
    if( WidgetUI.newProjectRB->isChecked() ) {
        GlobalDesktop->SetProjectRestoreMode(EDPRM_NEW_ROJECT_WP);
    }
    if( WidgetUI.userProjectRB->isChecked() ) {
        GlobalDesktop->SetProjectRestoreMode(EDPRM_USER);
    }

    CUUID  initial_project_type;
    int    index;
    index = WidgetUI.projectsCB->currentIndex();
    if( index >= 0 ) {
        QVariant data = WidgetUI.projectsCB->itemData(index);
        initial_project_type.SetFromStringForm((const char*) data.toString().toLatin1().constData());
    }

    GlobalDesktop->SetInitialProjectType(initial_project_type);
    GlobalDesktop->EnableWPRestoration(WidgetUI.desktopRestoreCB->isChecked());
    GlobalDesktop->EnableInitialProjectAfterLastIsClosed(!WidgetUI.terminateApplicationAfterLastProjectCB->isChecked());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGlobalSetupWorkPanel::InitPathsTab(void)
{
    switch(GlobalSetup->GetLastOpenFileMode()) {
        case ELOFM_USE_FIXED_PATH:
            WidgetUI.fixedPathRB->setChecked(true);
            break;
        case ELOFM_USE_SINGLE_PATH:
            WidgetUI.lastPathRB->setChecked(true);
            break;
        case ELOFM_USE_PATH_PER_MODULE:
            WidgetUI.lastModulePathCB->setChecked(true);
            break;
    }

    WidgetUI.fixedPathLE->setText(QString(GlobalSetup->GetFixedLastOpenFilePath()));
    WidgetUI.dynamicPathLE->setText(QString(GlobalSetup->GetLastOpenFilePath()));

    // list module paths
    ModulePaths = new QStandardItemModel(this);
    WidgetUI.modulePathsTV->setModel(ModulePaths);
    WidgetUI.modulePathsTV->resizeColumnToContents(0);

    // set model heaer
    QStringList header = QStringList();
    header << "Module" << "Path";
    ModulePaths->setHorizontalHeaderLabels(header);

    // root item
    QStandardItem* rootItem = ModulePaths->invisibleRootItem();

    std::map<CUUID,CFileName>::iterator it = GlobalSetup->GetLastFileModulePaths().begin();
    std::map<CUUID,CFileName>::iterator ie = GlobalSetup->GetLastFileModulePaths().end();

    while( it != ie ) {
        QList<QStandardItem*> objectRow;
        QStandardItem* p_item;

        CUUID uuid = it->first;
        CExtUUID ext_uuid;
        PluginDatabase.GetObjectExtType(uuid,ext_uuid);

        p_item = new QStandardItem(ext_uuid.GetName());
        p_item->setEditable(false);
        objectRow << p_item;

        p_item = new QStandardItem(QString(it->second));
        p_item->setEditable(false);
        objectRow << p_item;

        rootItem->appendRow(objectRow);
        it++;
    }
}

//------------------------------------------------------------------------------

void CGlobalSetupWorkPanel::ClearModulePaths(void)
{
    ModulePaths->clear();
    QStringList header = QStringList();
    header << "Module" << "Path";
    ModulePaths->setHorizontalHeaderLabels(header);
    GlobalSetup->ClearLastFileModelPaths();
}

//------------------------------------------------------------------------------

void CGlobalSetupWorkPanel::UpdateFixedPath(void)
{
    QString old_dir = WidgetUI.fixedPathLE->text();
    QString dir = QFileDialog::getExistingDirectory(this, "Get fixed path", old_dir);

    if( dir == NULL ) return;
    WidgetUI.fixedPathLE->setText(dir);
}

//------------------------------------------------------------------------------

void CGlobalSetupWorkPanel::SavePathsTab(void)
{
    if( WidgetUI.fixedPathRB->isChecked() ){
        GlobalSetup->SetLastOpenFileMode(ELOFM_USE_FIXED_PATH);
    }
    if( WidgetUI.lastPathRB->isChecked() ){
        GlobalSetup->SetLastOpenFileMode(ELOFM_USE_SINGLE_PATH);
    }
    if( WidgetUI.lastModulePathCB->isChecked() ){
        GlobalSetup->SetLastOpenFileMode(ELOFM_USE_PATH_PER_MODULE);
    }

    GlobalSetup->SetFixedLastOpenFilePath(CSmallString(WidgetUI.fixedPathLE->text()));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGlobalSetupWorkPanel::InitMouseTab(void)
{
    WidgetUI.RSensitivityXSB->setValue(CMouseDriverSetup::RSensitivity.x);
    WidgetUI.RSensitivityYSB->setValue(CMouseDriverSetup::RSensitivity.y);
    WidgetUI.RSensitivityZSB->setValue(CMouseDriverSetup::RSensitivity.z);

    WidgetUI.MSensitivityXSB->setValue(CMouseDriverSetup::MSensitivity.x);
    WidgetUI.MSensitivityYSB->setValue(CMouseDriverSetup::MSensitivity.y);
    WidgetUI.MSensitivityZSB->setValue(CMouseDriverSetup::MSensitivity.z);

    WidgetUI.WSensitivityYSB->setValue(CMouseDriverSetup::WheelSensitivity);

    WidgetUI.ThreeButtonsCB->setChecked(CMouseDriverSetup::ThreeButtonSimul);

    WidgetUI.DefaultMouseModeCB->setCurrentIndex(CMouseDriverSetup::MouseMode);
}

//------------------------------------------------------------------------------

void CGlobalSetupWorkPanel::SaveMouseTab(void)
{
    CMouseDriverSetup::RSensitivity.x = WidgetUI.RSensitivityXSB->value();
    CMouseDriverSetup::RSensitivity.y = WidgetUI.RSensitivityYSB->value();
    CMouseDriverSetup::RSensitivity.z = WidgetUI.RSensitivityZSB->value();

    CMouseDriverSetup::MSensitivity.x = WidgetUI.MSensitivityXSB->value();
    CMouseDriverSetup::MSensitivity.y = WidgetUI.MSensitivityYSB->value();
    CMouseDriverSetup::MSensitivity.z = WidgetUI.MSensitivityZSB->value();

    CMouseDriverSetup::WheelSensitivity = WidgetUI.WSensitivityYSB->value();

    CMouseDriverSetup::ThreeButtonSimul = WidgetUI.ThreeButtonsCB->isChecked();

    CMouseDriverSetup::MouseMode = static_cast<EMouseMode>(WidgetUI.DefaultMouseModeCB->currentIndex());

    MouseDriverSetup->EmitOnMouseSetupChanged();
}

//------------------------------------------------------------------------------

void CGlobalSetupWorkPanel::RestoreMouseSystemDefaults(void)
{
    MouseDriverSetup->LoadSystemSetup();
    InitMouseTab();
}

//------------------------------------------------------------------------------

void CGlobalSetupWorkPanel::RestoreMouseUserDefaults(void)
{
    if( MouseDriverSetup->LoadUserSetup() == false ){
        QMessageBox::warning(this,tr("Warning"),tr("No user setup or error!"));
    }
    InitMouseTab();
}

//------------------------------------------------------------------------------

void CGlobalSetupWorkPanel::ApplyMouseSetup(void)
{
    SaveMouseTab();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGlobalSetupWorkPanel::InitGraphicsTab(void)
{
    WidgetUI.multisamplingCB->setChecked(GraphicsSetupProfile->MultiSamplingEnabled);
    WidgetUI.quadStereoCB->setChecked(GraphicsSetupProfile->QuadStereoEnabled);
}

//------------------------------------------------------------------------------

void CGlobalSetupWorkPanel::SaveGraphicsTab(void)
{
    GraphicsSetupProfile->MultiSamplingEnabled = WidgetUI.multisamplingCB->isChecked();
    GraphicsSetupProfile->QuadStereoEnabled = WidgetUI.quadStereoCB->isChecked();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGlobalSetupWorkPanel::buttonBoxClicked(QAbstractButton* p_button)
{
    if( WidgetUI.buttonBox->buttonRole(p_button) != QDialogButtonBox::AcceptRole ) {
        return;
    }

    SaveStartupTab();
    SavePathsTab();
    SaveMouseTab();
    SaveGraphicsTab();

    close();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
