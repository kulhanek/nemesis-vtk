// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Jakub Stepan, xstepan3@chemi.muni.cz
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
#include <XMLElement.hpp>

#include <PluginObject.hpp>
#include <PluginModule.hpp>
#include <PluginDatabase.hpp>
#include <ErrorSystem.hpp>
#include <QPushButton>
#include <QMessageBox>

#include <Project.hpp>
#include <Property.hpp>
#include <PropertyList.hpp>
#include <WorkPanelList.hpp>

#include "NewPropertyWorkPanel.hpp"
#include "StandardWPModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* NewPropertyWorkPanelCB(void* p_data);

CExtUUID        NewPropertyWorkPanelID(
                    "{NEW_PROPERTY_WORK_PANEL:4a0b7079-006c-4c7b-a26a-2fec50413de5}",
                    "New Property Work Panel");

CPluginObject   NewPropertyWorkPanelObject(&StandardWPPlugin,
                    NewPropertyWorkPanelID,WORK_PANEL_CAT,
                    ":/images/StandardWP/PropertyAdd.svg",
                    NewPropertyWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* NewPropertyWorkPanelCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("Active project required.");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(NewPropertyWorkPanelID,p_project);
    if( p_wp == NULL ) {
        p_wp = new CNewPropertyWorkPanel(p_project);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNewPropertyWorkPanel::CNewPropertyWorkPanel(CProject* p_project)
    : CWorkPanel(&NewPropertyWorkPanelObject,p_project,EWPR_DIALOG)
{
    WidgetUI.setupUi(this);

    // set ok button inactive
    WidgetUI.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    // set up model and assign it to listView
    Model = new QStandardItemModel(this);

    CSimpleIteratorC<CPluginModule> I(PluginDatabase.GetModuleList());
    CPluginModule* p_module;

    // loop over modules
    while ((p_module = I.Current()) != NULL) {
        CSimpleIteratorC<CPluginObject> J(p_module->GetObjectList());
        CPluginObject* p_object;
        // loop over objects(
        while ((p_object = J.Current()) != NULL) {
            if (p_object->GetCategoryUUID() == PROPERTY_CAT ) {
                QStandardItem* item = new QStandardItem(p_object->GetIcon(),
                                                        QString(p_object->GetName()));
                item->setData(QString(p_object->GetObjectUUID().GetFullStringForm()));
                Model->appendRow(item);
            }
            J++;
        }
        I++;
    }

    Model->sort(0);
    WidgetUI.listView->setModel(Model);

    // list item activated -> description update
    connect(WidgetUI.listView, SIGNAL(clicked ( const QModelIndex & )),
            this, SLOT(ListItemChanged(const QModelIndex&)));
    // ok, or double click on item -> project open, dialog close
    connect(WidgetUI.listView, SIGNAL( doubleClicked( const QModelIndex & )),
            this, SLOT(CreateObject(const QModelIndex & )));
    connect(WidgetUI.buttonBox, SIGNAL(accepted()),
            this, SLOT(CreateObject()));
    connect(WidgetUI.buttonBox, SIGNAL(rejected()),
            this, SLOT(close()));

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CNewPropertyWorkPanel::~CNewPropertyWorkPanel()
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CNewPropertyWorkPanel::ListItemChanged(const QModelIndex & index)
{
    QStandardItem*  p_item = Model->itemFromIndex(index);
    QVariant        data = p_item->data();

    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString((const char*) data.toString().toLatin1().constData());

    CPluginObject * p_object = PluginDatabase.FindPluginObject(mp_uuid);
    if (p_object == NULL) {
        ES_ERROR("object not found");
        return;
    }
    QString text = tr("<b>Description:</b>");
    if( p_object->GetDescription() != NULL ){
        text += "<br/>";
        text += p_object->GetDescription();
    } else {
        text += "<center><span style=\" font-family:'Courier New,courier'; color:#808080;\">No description available.</span></center>";
    }
    WidgetUI.descriptionLabel->setText(text);

    WidgetUI.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(index.isValid());
}

//------------------------------------------------------------------------------

void CNewPropertyWorkPanel::CreateObject(void)
{
    CreateObject(WidgetUI.listView->currentIndex());
}

//------------------------------------------------------------------------------

void CNewPropertyWorkPanel::CreateObject(const QModelIndex & index)
{
    QStandardItem*  p_item = Model->itemFromIndex(index);
    QVariant        data = p_item->data();

    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString((const char*) data.toString().toLatin1().constData());

    if( GetProject()->BeginChangeWH(EHCL_PROPERTY,"New Property") == false ) {
        ES_ERROR("it is not allowed to add a new property");
        QMessageBox::critical(NULL, tr("New Property"),
                              tr("It is not allowed to add a new property!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        close();
        return;
    }

    CProperty* p_property = GetProject()->GetProperties()->CreatePropertyWH(mp_uuid);

    if( p_property == NULL ) {
        ES_ERROR("unable to create a new property");
        QMessageBox::critical(NULL, tr("New Property"),
                              tr("An error occurred during a property creating!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        return;
    }

    GetProject()->EndChangeWH();

    if( WidgetUI.openObjectDesignerCB->isChecked() ){
        p_property->OpenObjectDesigner();
    }

    close();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CNewPropertyWorkPanel::LoadWorkPanelSpecificData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }

    bool checked = true;
    p_ele->GetAttribute("od",checked);
    WidgetUI.openObjectDesignerCB->setChecked(checked);

    CWorkPanel::LoadWorkPanelSpecificData(p_ele);
}

//------------------------------------------------------------------------------

void CNewPropertyWorkPanel::SaveWorkPanelSpecificData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }

    p_ele->SetAttribute("od",WidgetUI.openObjectDesignerCB->isChecked());

    CWorkPanel::SaveWorkPanelSpecificData(p_ele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================





