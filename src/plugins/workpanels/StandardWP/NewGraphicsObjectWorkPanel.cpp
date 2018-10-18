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
#include <QPushButton>
#include <QMessageBox>

#include <Project.hpp>
#include <ProjectList.hpp>
#include <Graphics.hpp>
#include <GraphicsObjectList.hpp>
#include <GraphicsProfile.hpp>
#include <Structure.hpp>
#include <WorkPanelList.hpp>
#include <StructureList.hpp>
#include <XMLElement.hpp>

#include "NewGraphicsObjectWorkPanel.hpp"
#include "StandardWPModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* NewGraphicsObjectWorkPanelCB(void* p_data);

CExtUUID        NewGraphicsObjectWorkPanelID(
                    "{NEW_GRAPHICS_OBJECT_WORK_PANEL:b0568e3a-c78b-44f3-aee5-1e4739cc4be9}",
                    "New Graphics Object");

CPluginObject   NewGraphicsObjectWorkPanelObject(&StandardWPPlugin,
                    NewGraphicsObjectWorkPanelID,WORK_PANEL_CAT,
                    ":/images/StandardWP/GraphicsObjectAdd.svg",
                    NewGraphicsObjectWorkPanelCB);

// -----------------------------------------------------------------------------

QObject* NewGraphicsObjectWorkPanelCB(void* p_data)
{
    CProObject* p_po = static_cast<CProObject*>(p_data);
    if( p_po == NULL ){
        ES_ERROR("Active project required!");
        return(NULL);
    }

    // only one instance is allowed
    QObject* p_wp = WorkPanels->Find(NewGraphicsObjectWorkPanelID,p_po->GetProject());
    if( p_wp == NULL ) {
        p_wp = new CNewGraphicsObjectWorkPanel(p_po);
    }

    return(p_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNewGraphicsObjectWorkPanel::CNewGraphicsObjectWorkPanel(CProObject* p_scope)
    : CWorkPanel(&NewGraphicsObjectWorkPanelObject,p_scope->GetProject(),EWPR_DIALOG)
{
    Scope = p_scope;

    WidgetUI.setupUi(this);

    if( Scope == GetProject()->GetGraphics()->GetObjects() ){
        WidgetUI.mainContainerLA->setText(tr("all objects"));
    } else {
        WidgetUI.mainContainerLA->setText(tr("profile objects"));
    }

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
            if (p_object->GetCategoryUUID() == GRAPHICS_OBJECT_CAT ) {
                // check if item is allowed
                QString user = "YES";
                PluginDatabase.FindObjectConfigValue(p_object->GetObjectUUID(),"_user",user,true);
                if( user == "YES" ){
                    QStandardItem* item = new QStandardItem(p_object->GetIcon(),
                                                            QString(p_object->GetName()));
                    item->setData(QString(p_object->GetObjectUUID().GetFullStringForm()));
                    Model->appendRow(item);
                }
            }
            J++;
        }
        I++;
    }

    Model->sort(0);
    WidgetUI.listView->setModel(Model);

    // add objects
    WidgetUI.addObjectsCB->addItem(tr("None"));
    WidgetUI.addObjectsCB->addItem(tr("Selected objects"));
    WidgetUI.addObjectsCB->addItem(tr("Structure"));
    WidgetUI.addObjectsCB->addItem(tr("All structures"));

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

CNewGraphicsObjectWorkPanel::~CNewGraphicsObjectWorkPanel()
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CNewGraphicsObjectWorkPanel::ListItemChanged(const QModelIndex & index)
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

void CNewGraphicsObjectWorkPanel::NameEdited(const QString& text)
{
    WidgetUI.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(text.length() != 0);
}

//------------------------------------------------------------------------------

void CNewGraphicsObjectWorkPanel::CreateObject(void)
{
    CreateObject(WidgetUI.listView->currentIndex());
}

//------------------------------------------------------------------------------

void CNewGraphicsObjectWorkPanel::CreateObject(const QModelIndex & index)
{
    QStandardItem*  p_item = Model->itemFromIndex(index);
    QVariant        data = p_item->data();

    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString((const char*) data.toString().toLatin1().constData());

    CGraphicsObject* p_object = NULL;

    if( GetProject()->BeginChangeWH(EHCL_GRAPHICS,"New graphics object") == NULL ) {
        ES_ERROR("it is not allowed to add a new graphics object");
        QMessageBox::critical(NULL, tr("New Graphics Object"),
                              tr("It is not allowed to add a new graphics object!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        close();
        return;
    }

        if( dynamic_cast<CGraphicsObjectList*>(Scope) != NULL ){
            CGraphicsObjectList* p_list = dynamic_cast<CGraphicsObjectList*>(Scope);
            p_object = p_list->CreateObjectWH(mp_uuid);
        }

        if( dynamic_cast<CGraphicsProfile*>(Scope) != NULL ){
            CGraphicsProfile* p_profile = dynamic_cast<CGraphicsProfile*>(Scope);
            p_object = p_profile->CreateObjectWH(mp_uuid);
        }

        if( p_object == NULL ) {
            ES_ERROR("unable to create a new graphics object");
            QMessageBox::critical(NULL, tr("New Graphics Object"),
                                  tr("An error occurred during a graphics objects creating!"),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
            return;
        }

        // populate object with ...
        switch(WidgetUI.addObjectsCB->currentIndex()){
            case 0:
                // nothing to do
                break;
            case 1:
                p_object->AddSelectedObjectsWH();
                break;
            case 2:
                p_object->AddObjectWH(GetProject()->GetActiveStructure());
                break;
            case 3:
                p_object->AddObjectWH(GetProject()->GetStructures());
                break;
        }

    GetProject()->EndChangeWH();

    if( WidgetUI.openObjectDesignerCB->isChecked() ){
        p_object->OpenObjectDesigner();
    }

    close();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CNewGraphicsObjectWorkPanel::LoadWorkPanelSpecificData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }

    bool checked = true;
    p_ele->GetAttribute("od",checked);
    WidgetUI.openObjectDesignerCB->setChecked(checked);

    int index = 0;
    p_ele->GetAttribute("ao",index);
    WidgetUI.addObjectsCB->setCurrentIndex(index);

    CWorkPanel::LoadWorkPanelSpecificData(p_ele);
}

//------------------------------------------------------------------------------

void CNewGraphicsObjectWorkPanel::SaveWorkPanelSpecificData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }

    p_ele->SetAttribute("od",WidgetUI.openObjectDesignerCB->isChecked());
    p_ele->SetAttribute("ao",WidgetUI.addObjectsCB->currentIndex());

    CWorkPanel::SaveWorkPanelSpecificData(p_ele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================





