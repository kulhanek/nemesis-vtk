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

#include <ErrorSystem.hpp>
#include <SmallString.hpp>
#include <SmallTime.hpp>
#include <XMLElement.hpp>

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <QPushButton>

#include "ErrorConsoleWorkPanel.moc"
#include "ErrorConsoleWorkPanel.hpp"
#include "GlobalWorkPanelsModule.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* ErrorConsoleWorkPanelCB(void* p_data);

CExtUUID        ErrorConsoleWorkPanelID(
                    "{ERROR_CONSOLE_WORK_PANEL:7dc0630c-19e5-48bc-9130-904cc8640900}",
                    "Error Console");

CPluginObject   ErrorConsoleWorkPanelObject(&GlobalWorkPanelsPlugin,
                    ErrorConsoleWorkPanelID,WORK_PANEL_CAT,
                    ":/images/GlobalWorkPanels/ErrorConsole.svg",
                    ErrorConsoleWorkPanelCB);

// -----------------------------------------------------------------------------

CErrorConsoleWorkPanel* p_s1_window = NULL;

// -----------------------------------------------------------------------------

QObject* ErrorConsoleWorkPanelCB(void* p_data)
{
    if( p_s1_window != NULL ) return(p_s1_window);
    p_s1_window = new CErrorConsoleWorkPanel;
    return(p_s1_window);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CErrorConsoleWorkPanel::CErrorConsoleWorkPanel(void)
    : CWorkPanel(&ErrorConsoleWorkPanelObject,NULL,EWPR_WINDOW)
{
    WidgetUI.setupUi(this);

    // ask for messages from error system
    RegisterEventForServer(ErrorSystem_Changed, &ErrorSystem);

    // add my buttons (refresh, remove all) to buttonbox
    QPushButton* refresh = WidgetUI.buttonBox->addButton("Refresh",QDialogButtonBox::ActionRole);
    QPushButton* removeAll = WidgetUI.buttonBox->addButton("Remove all",QDialogButtonBox::ActionRole);

    // Connect actions to buttons and checkbox
    connect(refresh, SIGNAL(clicked()), this, SLOT(RefreshList()));
    connect(removeAll, SIGNAL(clicked()), this, SLOT(ClearErrors()));

    connect(WidgetUI.checkBox, SIGNAL(stateChanged(int)), this, SLOT(AutoUpdate(int)));
    connect(WidgetUI.checkBox, SIGNAL(toggled(bool)), refresh, SLOT(setDisabled(bool)));

    // Fill list
    Model = new QStandardItemModel(this);
    WidgetUI.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    WidgetUI.tableView->setModel(Model);
    WidgetUI.tableView->setShowGrid(false);
    WidgetUI.tableView->resizeColumnsToContents();
    // create headers
    QHeaderView* headerHorizontal = WidgetUI.tableView->horizontalHeader();
    QHeaderView* headerVertical = WidgetUI.tableView->verticalHeader();
    headerVertical->setVisible(false);
    headerHorizontal->setStretchLastSection ( true );
    headerHorizontal->setDefaultAlignment(Qt::AlignLeft);
    QStringList header = QStringList();
    header << "Error decsription" << "File" << "Function" << "Line" << "Time";
    Model->setHorizontalHeaderLabels(header);
    // refresh new list
    RefreshList();

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CErrorConsoleWorkPanel::~CErrorConsoleWorkPanel()
{
    SaveWorkPanelSetup();
    p_s1_window = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CErrorConsoleWorkPanel::RefreshList(void)
{
    Model->setRowCount(0);

    // check element
    if( ErrorSystem.GetRootElement() == NULL ) return;

    CXMLElement* p_ele = ErrorSystem.GetRootElement()->GetFirstChildElement("ERROR");


    //    Error element consist of following attributes:
    //     1) ("text",error);
    //     2) ("fce",fce);
    //     3) ("file",file);
    //     4) ("line",line);
    //     5) ("time",time);

    // update model
    while( p_ele != NULL ) {
        CSmallString            text, fce, file, line;
        CSmallTimeAndDate       time;
        p_ele->GetAttribute("text",text);
        p_ele->GetAttribute("file",file);
        p_ele->GetAttribute("fce",fce);
        p_ele->GetAttribute("line",line);
        p_ele->GetAttribute("time",time);

        QList<QStandardItem*> list;

        QStandardItem* errorItem = new QStandardItem(QString(text));
        errorItem->setEditable(false);
        list << errorItem;

        errorItem = new QStandardItem(QString(file));
        errorItem->setEditable(false);
        list << errorItem;
        errorItem = new QStandardItem(QString(fce));
        errorItem->setEditable(false);
        list << errorItem;
        errorItem = new QStandardItem(QString(line));
        errorItem->setEditable(false);
        list << errorItem;
        errorItem = new QStandardItem(QString(time.GetSDateAndTime()));
        errorItem->setEditable(false);
        list << errorItem;

        Model->insertRow(0, list);

        p_ele = p_ele->GetNextSiblingElement("ERROR");
    }
}


//------------------------------------------------------------------------------

void CErrorConsoleWorkPanel::ClearErrors(void)
{
    ErrorSystem.RemoveAllErrors();
    RefreshList();
}

//------------------------------------------------------------------------------

void CErrorConsoleWorkPanel::AutoUpdate(int status)
{
    if (status == Qt::Checked) {
        ErrorSystem.EnableEvents(true);
    } else {
        ErrorSystem.EnableEvents(false);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CErrorConsoleWorkPanel::ProcessEvent(const CEventIdentifier& event,
        CEventManager* Sender,void* p_data)
{
    RefreshList();
    return(true);
}

//------------------------------------------------------------------------------

void CErrorConsoleWorkPanel::LoadWorkPanelSpecificData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) return;
    bool auto_update = false;
    p_ele->GetAttribute("auto_update",auto_update);
    WidgetUI.checkBox->setChecked(auto_update);

    CWorkPanel::LoadWorkPanelSpecificData(p_ele);
}

//------------------------------------------------------------------------------

void CErrorConsoleWorkPanel::SaveWorkPanelSpecificData(CXMLElement* p_ele)
{
    if( p_ele == NULL ) return;
    p_ele->SetAttribute("auto_update",WidgetUI.checkBox->isChecked());

    CWorkPanel::SaveWorkPanelSpecificData(p_ele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

