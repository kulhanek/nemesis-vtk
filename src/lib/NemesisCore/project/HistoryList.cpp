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

#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <SelectionList.hpp>
#include <Project.hpp>
#include <XMLDocument.hpp>
#include <XMLPrinter.hpp>
#include <XMLElement.hpp>

#include <HistoryList.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        HistoryListID(
                    "{HISTORY_LIST:dfaa255c-6858-46b1-aa60-0517acaf21b1}",
                    "History");

CPluginObject   HistoryListObject(&NemesisCorePlugin,
                    HistoryListID,GENERIC_CAT,
                    ":/images/NemesisCore/project/HistoryItem.svg",
                    NULL);

//------------------------------------------------------------------------------

CIndexCounter   CHistoryList::ProjectCounter;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CHistoryList::CHistoryList(CProObject* p_parent,bool no_index)
    : CProObject(&HistoryListObject,p_parent,p_parent->GetProject(),no_index)
{
    NumOfMaxChanges=20;
    NumOfRedo=0;
    LockModeLevels = CLockLevels();
    MinLockModeLevels = CLockLevels();
    CurrentChangeLevel = EHCL_NONE;
    RedoStatus = false;

    EnableDebug = false;
    ProjectID = ProjectCounter.GetIndex();
    ActionID = 0;
}

//---------------------------------------------------------------------------

CHistoryList::~CHistoryList(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CHistoryList::SetDepthOfBuffer(int depth)
{
    if( NumOfMaxChanges <= depth ) {
        NumOfMaxChanges = depth;     // only increase limit
        return;
    }

    // set new limit
    NumOfMaxChanges = depth;

    // we need to shrink buffer
    while( children().count() <= NumOfMaxChanges ) {
        QObject* p_qobj = children().first();
        if( p_qobj == NULL ) break;
        delete p_qobj;
    }

    // rise event
    emit OnHistoryChanged(EHCM_BUFFER);
}

// -----------------------------------------------------------------------------

void CHistoryList::SetLockModeLevels(const CLockLevels& levels)
{
    LockModeLevels = levels;

    // apply min locks to lock levels
    LockModeLevels |= MinLockModeLevels;

    emit OnHistoryChanged(EHCM_LOCK_LEVEL);
}

// -----------------------------------------------------------------------------

void CHistoryList::SetMinLockModeLevels(const CLockLevels& levels)
{
    MinLockModeLevels = levels;

    // apply min locks to lock levels
    LockModeLevels |= MinLockModeLevels;

    emit OnHistoryChanged(EHCM_LOCK_LEVEL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool  CHistoryList::IsUndoActive(void)
{
    if( children().count() == NumOfRedo ) return(false);
    if( IsLocked(EHCL_HISTORY) ) return(false);

    // analyse item lock level
    if( RedoStatus == false ) {
        NumOfRedo = 0;
    }
    int undotop = children().count() - NumOfRedo;
    CHistoryNode* p_history = dynamic_cast<CHistoryNode*>(children().at(undotop-1));
    return( p_history->IsChangeAllowed(LockModeLevels) );
}

//------------------------------------------------------------------------------

bool  CHistoryList::IsRedoActive(void)
{
    if( NumOfRedo == 0 ) return(false);
    if( IsLocked(EHCL_HISTORY) ) return(false);

    int redotop = children().count() - NumOfRedo;
    CHistoryNode* p_history = dynamic_cast<CHistoryNode*>(children().at(redotop));
    return( p_history->IsChangeAllowed(LockModeLevels) );
}

//------------------------------------------------------------------------------

int CHistoryList::GetNumberOfUndoAvailable(void)
{
    if( IsUndoActive() == false ) return(0);
    return( children().count() - NumOfRedo );
}

//------------------------------------------------------------------------------

int CHistoryList::GetNumberOfRedoAvailable(void)
{
    if( IsRedoActive() == false ) return(0);
    return( NumOfRedo );
}

//------------------------------------------------------------------------------

int CHistoryList::GetMaxNumberOfUndoAvailable(void)
{
    return( children().count() - NumOfRedo );
}

//------------------------------------------------------------------------------

int CHistoryList::GetMaxNumberOfRedoAvailable(void)
{
    if( RedoStatus == false ) return(0);
    return( NumOfRedo );
}

//---------------------------------------------------------------------------

bool CHistoryList::IsLocked(EHistoryChangeLevel level)
{
    return( LockModeLevels.testFlag(level) );
}

//---------------------------------------------------------------------------

const CLockLevels& CHistoryList::GetLockModeLevels(void) const
{
    return(LockModeLevels);
}

//---------------------------------------------------------------------------

const CLockLevels& CHistoryList::GetMinLockModeLevels(void) const
{
    return(MinLockModeLevels);
}

//---------------------------------------------------------------------------

QString CHistoryList::GetUndoDescr(int i,bool short_ver)
{
    // anything in the list?
    if( IsUndoActive() == false ) return("");
    // check for overflow
    if( i >= children().count() - NumOfRedo ) return("");

    // get item from the list
    CHistoryNode* p_hist;

    p_hist = dynamic_cast<CHistoryNode*>(children().at(children().count() - NumOfRedo - 1 - i));
    if( p_hist == NULL ) return("");

    return(p_hist->GetShortDescription());
}

//---------------------------------------------------------------------------

QString CHistoryList::GetRedoDescr(int i,bool short_ver)
{
    // anything in the list?
    if( IsRedoActive() == false ) return("");

    // check for overflow
    if( i >= NumOfRedo ) return("");

    CHistoryNode*   p_hist;
    p_hist = dynamic_cast<CHistoryNode*>(children().at(children().count() - NumOfRedo + i));

    return(p_hist->GetShortDescription());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CHistoryList::BeginChange(EHistoryChangeLevel lockmodelevel)
{
    CurrentChangeLevel = lockmodelevel;
    return( ! IsLocked(CurrentChangeLevel) );
}

//---------------------------------------------------------------------------

void CHistoryList::RegisterChange(CHistoryNode* p_node)
{
    if( p_node == NULL ) return;
    p_node->SetChangeLevel(CurrentChangeLevel);

    if( RegHistories.NumOfMembers() > 0 ) {
        RegHistories.GetLast()->Register(p_node);
    }

    RegHistories.InsertToEnd(p_node);

    if( RedoStatus != false ) {  // is redo active?
        // if yes that what we should do with redo part?

        for(int i=0; i<NumOfRedo; i++) {
            QObject* p_qobj = children().last();
            if( p_qobj == NULL ) break;
            delete p_qobj;
        }
        RedoStatus = false;
        NumOfRedo = 0;
    }

    // it registers change
    if( RegHistories.NumOfMembers() == 1 ) { // is it first change?
        p_node->setParent(this);        // register
        while( children().count() > NumOfMaxChanges ) { // do we need shrink buffer?
            QObject* p_qobj = children().first();
            if( p_qobj == NULL ) break;
            delete p_qobj;
        }
    }
}

//---------------------------------------------------------------------------

void CHistoryList::EndChange(void)
{
    RegHistories.RemoveFromEnd();
    if( RegHistories.NumOfMembers() > 0 ){
        CurrentChangeLevel = RegHistories.GetLast()->GetChangeLevel();
    }
    if( RegHistories.NumOfMembers() == 0 ) {
        WriteDebugData();
        emit OnHistoryChanged(EHCM_BUFFER);
    }
}

//---------------------------------------------------------------------------

bool CHistoryList::Undo(int i)
{
    // test for lock level
    if( IsLocked(EHCL_HISTORY) ) return(false); // everything is fully locked

    // was everything undoed?
    if( NumOfRedo == children().count() ) return(false);

    // if there are any selected objects - unslected them
    if( GetProject()->GetSelection()->NumOfSelectedObjects() > 0 ){
        GetProject()->GetSelection()->ResetSelection();
    }

    // analyse item lock level
    if( RedoStatus == false ) {
        NumOfRedo = 0;
    }

    // correct overflow of i
    if( i >= children().count() - NumOfRedo ) {
        i = children().count() - NumOfRedo -1;
    }

    // undo i changes
    bool changed = false;
    for(int j=0; j<=i; j++) {

        if( RedoStatus == false ) {
            RedoStatus = true;
            NumOfRedo = 0;
        }

        int undotop = children().count() - NumOfRedo;

        CHistoryNode* p_history = dynamic_cast<CHistoryNode*>(children().at(undotop-1));
        if( ! p_history->IsChangeAllowed(LockModeLevels) ){
            if( changed ) emit OnHistoryChanged(EHCM_REDO);
            return(false);
        }
        if( p_history != NULL ) p_history->MakeChange();

        NumOfRedo++;
        changed = true;
    }

    emit OnHistoryChanged(EHCM_UNDO);

    return(true);
}

// -----------------------------------------------------------------------------

bool CHistoryList::Redo(int i)
{
    // test for lock level
    if( IsLocked(EHCL_HISTORY) ) return(false); // everything is fully locked
    if( NumOfRedo == 0 ) return(false);

    // if there are any selected objects - unslected them
    if( GetProject()->GetSelection()->NumOfSelectedObjects() > 0 ){
        GetProject()->GetSelection()->ResetSelection();
    }

    // correct overflow of i
    if( i >= NumOfRedo ) {
        i = NumOfRedo - 1;
    }

    bool changed = false;
    for(int j=0; j<=i; j++) {
        int redotop = children().count() - NumOfRedo;

        CHistoryNode* p_history = dynamic_cast<CHistoryNode*>(children().at(redotop));
        if( ! p_history->IsChangeAllowed(LockModeLevels) ){
            if( changed ) emit OnHistoryChanged(EHCM_REDO);
            return(false);
        }

        if( p_history != NULL ) p_history->MakeChange();

        NumOfRedo--;
        changed = true;
    }

    emit OnHistoryChanged(EHCM_REDO);
    return(true);
}

//------------------------------------------------------------------------------

void CHistoryList::ClearHistory(void)
{
    // we cannot delete list if the list is locked
    if( IsLocked(EHCL_HISTORY) ) return;

    foreach(QObject* p_qobj,children()) {
        delete p_qobj;
    }

    NumOfRedo = 0;
    RedoStatus = false;

    emit OnHistoryChanged(EHCM_BUFFER);
}

//------------------------------------------------------------------------------

void CHistoryList::LoadActionAndExecute(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }
    if( p_ele->GetName() ){
        INVALID_ARGUMENT("p_ele must be <item>")
    }

    // top item is always CHistoryNode
    BeginChange(EHCL_COMPOSITE);

        CHistoryNode* p_node = new CHistoryNode(GetProject());
        RegisterChange(p_node); // register node

        // load node data
        p_node->LoadData(p_ele);

        // reverse action direction
        p_node->ReverseDirection();

        // execute action
        p_node->MakeChange();

    EndChange();    // inform about the change
}

//------------------------------------------------------------------------------

void CHistoryList::SaveLastAction(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }
    if( p_ele->GetName() ){
        INVALID_ARGUMENT("p_ele must be <item>")
    }

    QObject* p_qobj = children().last();
    if( p_qobj == NULL ) return; // no data

    CHistoryItem* p_item = static_cast<CHistoryItem*>(p_qobj);
    p_item->SaveData(p_ele); // save data
}

//------------------------------------------------------------------------------

void CHistoryList::WriteDebugData(void)
{
    if( ! EnableDebug ) return;
    QObject* p_qobj = children().last();
    if( p_qobj == NULL ) return;

    ActionID = ActionCounter.GetIndex();
    CHistoryItem* p_item = static_cast<CHistoryItem*>(p_qobj);

    CXMLDocument xml_doc;

    xml_doc.FastSetAttribute = true;

    xml_doc.CreateChildDeclaration();
    xml_doc.CreateChildComment("history change action");
    CXMLElement* p_ele = xml_doc.CreateChildElement("item");
    p_item->SaveData(p_ele);

    QString tmp("%1_%2.xml");
    tmp = tmp.arg(ProjectID).arg(ActionID);

    CXMLPrinter xml_printer;
   // xml_printer.SetOutputFormat(EXF_BXML);
    xml_printer.SetPrintedXMLNode(&xml_doc);
    xml_printer.Print(tmp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

