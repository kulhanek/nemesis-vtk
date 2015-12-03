// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QMessageBox>
#include <QPushButton>
#include <QMenu>
#include <QApplication>
#include <MainWindow.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::ConnectEditMenu(void)
{
    BIND_ACTION(actionLockStructures);
    BIND_ACTION(actionLockTopology);
    BIND_ACTION(actionLockGeometry);
    BIND_ACTION(actionLockGraphics);
    BIND_ACTION(actionLockDescription);
    BIND_ACTION(actionLockProperties);

    CONNECT_ACTION(Undo);
    CONNECT_ACTION(Redo);

    CONNECT_ACTION(CopyActiveStructure);
    CONNECT_ACTION(PasteStructure);
    CONNECT_ACTION(PasteStructureDoNotOverlap);

    CONNECT_ACTION(ClearHistory);
    CONNECT_ACTION(ClearAllData);

    LockGroup = new QActionGroup(this);
    LockGroup->addAction(actionLockStructures);
    LockGroup->addAction(actionLockTopology);
    LockGroup->addAction(actionLockGeometry);
    LockGroup->addAction(actionLockGraphics);
    LockGroup->addAction(actionLockDescription);
    LockGroup->addAction(actionLockProperties);
    LockGroup->setEnabled(true);
    LockGroup->setExclusive(false);

    //-----------------
    connect(LockGroup,SIGNAL(triggered(QAction*)),
            this,SLOT(LockChanged(QAction*)));
    //-----------------
    connect(HistoryList,SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(UpdateEditMenu()));
    //-----------------
    connect(QApplication::clipboard(),SIGNAL(dataChanged()),
            this,SLOT(UpdateEditMenu()));


    UpdateEditMenu();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::UpdateEditMenu(void)
{
    CLockLevels curr_levels = HistoryList->GetLockModeLevels();
    CLockLevels min_levels = HistoryList->GetMinLockModeLevels();

    if( HistoryList->IsUndoActive() ) {
        actionUndo->setEnabled(true);
        QString text("Undo: ");
        text += HistoryList->GetUndoDescr(0);
        actionUndo->setText(text);
    } else {
        actionUndo->setText(tr("Undo"));
        actionUndo->setEnabled(false);
    }
    if( HistoryList->IsRedoActive()) {
        actionRedo->setEnabled(true);
        QString text("Redo: ");
        text += HistoryList->GetRedoDescr(0);
        actionRedo->setText(text);
    } else {
        actionRedo->setText(tr("Redo"));
        actionRedo->setEnabled(false);
    }

    actionClearHistory->setEnabled(HistoryList->IsUndoActive() || HistoryList->IsRedoActive());

    bool op_enabled = Project->GetActiveStructure() != NULL;

    actionCopyActiveStructure->setEnabled(op_enabled);

    const QMimeData* p_mimeData = QApplication::clipboard()->mimeData();
    if( p_mimeData == NULL ) {
        actionPasteStructure->setEnabled(false);
        actionPasteStructureDoNotOverlap->setEnabled(false);
    } else {
        bool set = p_mimeData->hasFormat("application/nemesis") && op_enabled && (! curr_levels.testFlag(EHCL_TOPOLOGY));
        actionPasteStructure->setEnabled(set);
        actionPasteStructureDoNotOverlap->setEnabled(set);
    }

    actionLockStructures->setChecked(curr_levels.testFlag(EHCL_STRUCTURES));
    actionLockTopology->setChecked(curr_levels.testFlag(EHCL_TOPOLOGY));
    actionLockGeometry->setChecked(curr_levels.testFlag(EHCL_GEOMETRY));
    actionLockGraphics->setChecked(curr_levels.testFlag(EHCL_GRAPHICS));
    actionLockDescription->setChecked(curr_levels.testFlag(EHCL_DESCRIPTION));
    actionLockProperties->setChecked(curr_levels.testFlag(EHCL_PROPERTY));

    actionLockStructures->setEnabled(! min_levels.testFlag(EHCL_STRUCTURES));
    actionLockTopology->setEnabled(! min_levels.testFlag(EHCL_TOPOLOGY));
    actionLockGeometry->setEnabled(! min_levels.testFlag(EHCL_GEOMETRY));
    actionLockGraphics->setEnabled(! min_levels.testFlag(EHCL_GRAPHICS));
    actionLockDescription->setEnabled(! min_levels.testFlag(EHCL_DESCRIPTION));
    actionLockProperties->setEnabled(! min_levels.testFlag(EHCL_PROPERTY));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::Undo(void)
{
    HistoryList->Undo();
}

//------------------------------------------------------------------------------

void CMainWindow::Redo(void)
{
    HistoryList->Redo();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMainWindow::CopyActiveStructure(void)
{
    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;

    CXMLDocument xml_document;
    CXMLElement* p_mele = xml_document.CreateChildElement("structure");
    p_str->Copy(p_mele,false);


    CXMLPrinter printer;
    printer.SetPrintedXMLNode(&xml_document);

    unsigned int    length;
    char*           char_data;
    char_data = (char*)printer.Print(length);

    if( char_data == NULL ) {
        ES_ERROR("unable to print structure data to XML stream");
        return;
    }

    QByteArray byte_array(char_data,length);

    QMimeData* p_mime_data = new QMimeData;
    p_mime_data->setData("application/nemesis",byte_array);

    QApplication::clipboard()->setMimeData(p_mime_data);
}

//------------------------------------------------------------------------------

void CMainWindow::PasteStructure(void)
{
    const QMimeData* p_mimeData = QApplication::clipboard()->mimeData();
    if( p_mimeData == NULL ) return;
    if( ! p_mimeData->hasFormat("application/nemesis") ) return;

    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;

    CXMLDocument xml_document;

    CXMLParser xml_parser;
    xml_parser.SetOutputXMLNode(&xml_document);

    QByteArray byte_array = p_mimeData->data("application/nemesis");

    if( xml_parser.Parse(byte_array.data(),byte_array.length()) == false ) {
        ES_ERROR("unable to parse structure data from XML stream");
        return;
    }

    CXMLElement* p_mele = xml_document.GetFirstChildElement("structure");
    if( p_mele == NULL ) {
        ES_ERROR("unable to get structure element");
        return;
    }

    p_str->PasteWH(p_mele,false);
}

//------------------------------------------------------------------------------

void CMainWindow::PasteStructureDoNotOverlap(void)
{
    const QMimeData* p_mimeData = QApplication::clipboard()->mimeData();
    if( p_mimeData == NULL ) return;
    if( ! p_mimeData->hasFormat("application/nemesis") ) return;

    CStructure* p_str = Project->GetActiveStructure();
    if( p_str == NULL ) return;

    CXMLDocument xml_document;

    CXMLParser xml_parser;
    xml_parser.SetOutputXMLNode(&xml_document);

    QByteArray byte_array = p_mimeData->data("application/nemesis");

    if( xml_parser.Parse(byte_array.data(),byte_array.length()) == false ) {
        ES_ERROR("unable to parse structure data from XML stream");
        return;
    }

    CXMLElement* p_mele = xml_document.GetFirstChildElement("structure");
    if( p_mele == NULL ) {
        ES_ERROR("unable to get molecule element");
        return;
    }

    p_str->PasteWH(p_mele,true);
}

//------------------------------------------------------------------------------

void CMainWindow::ClearHistory(void)
{
    HistoryList->ClearHistory();
}

//------------------------------------------------------------------------------

void CMainWindow::ClearAllData(void)
{
    QMessageBox* p_mbox = new QMessageBox(this);
    p_mbox->setWindowTitle(tr("Warning"));
    p_mbox->setText(tr("All project data will be destroyed! This action cannot be undone!"));
    QAbstractButton* p_clearbtn = p_mbox->addButton(tr("Clear"), QMessageBox::NoRole);
    QAbstractButton* p_reinitialize = p_mbox->addButton(tr("Re-initialize"), QMessageBox::NoRole);
    QPushButton* p_cancel = p_mbox->addButton(tr("Cancel"), QMessageBox::NoRole);
    p_cancel->setDefault(true);

    p_mbox->exec();

    if( p_mbox->clickedButton() == p_clearbtn ){
        Project->ClearProject();
    }

    if( p_mbox->clickedButton() == p_reinitialize ){
        Project->ClearProject();
        Project->NewData();
    }

    delete p_mbox;
}

//------------------------------------------------------------------------------

void CMainWindow::LockChanged(QAction* p_act)
{
    if( p_act == actionLockStructures ) {
        CLockLevels locks = HistoryList->GetLockModeLevels();
        SET_FLAG(locks,EHCL_STRUCTURES,p_act->isChecked());
        HistoryList->SetLockModeLevels(locks);
    }
    if( p_act == actionLockTopology ) {
        CLockLevels locks = HistoryList->GetLockModeLevels();
        SET_FLAG(locks,EHCL_TOPOLOGY,p_act->isChecked());
        HistoryList->SetLockModeLevels(locks);
    }
    if( p_act == actionLockGeometry ) {
        CLockLevels locks = HistoryList->GetLockModeLevels();
        SET_FLAG(locks,EHCL_GEOMETRY,p_act->isChecked());
        HistoryList->SetLockModeLevels(locks);
    }
    if( p_act == actionLockGraphics ) {
        CLockLevels locks = HistoryList->GetLockModeLevels();
        SET_FLAG(locks,EHCL_GRAPHICS,p_act->isChecked());
        HistoryList->SetLockModeLevels(locks);
    }
    if( p_act == actionLockDescription ) {
        CLockLevels locks = HistoryList->GetLockModeLevels();
        SET_FLAG(locks,EHCL_DESCRIPTION,p_act->isChecked());
        HistoryList->SetLockModeLevels(locks);
    }
    if( p_act == actionLockProperties ) {
        CLockLevels locks = HistoryList->GetLockModeLevels();
        SET_FLAG(locks,EHCL_PROPERTY,p_act->isChecked());
        HistoryList->SetLockModeLevels(locks);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




