// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011      Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>

#include <CrashCentre.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCrashCentre::CCrashCentre(void)
    : QDialog(NULL)
{
    WidgetUI.setupUi(this);

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
}

//------------------------------------------------------------------------------

CCrashCentre::~CCrashCentre()
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCrashCentre::RefreshList(void)
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

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

