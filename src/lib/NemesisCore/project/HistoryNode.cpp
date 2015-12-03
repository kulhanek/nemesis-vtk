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

#include <HistoryNode.hpp>
#include <SimpleIterator.hpp>
#include <QObjectList>
#include <PluginObject.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <PluginDatabase.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>

//------------------------------------------------------------------------------

QObject* HistoryNodeCB(void* p_data);

CExtUUID        HistoryNodeID(
                    "{HISTORY_NODE:97061abf-dc38-46ad-872f-924ec7ba773d}",
                    "History node");

CPluginObject   HistoryNodeObject(&NemesisCorePlugin,
                     HistoryNodeID,HISTORY_CAT,
                    ":/images/NemesisCore/project/HistoryItem.svg",
                     HistoryNodeCB);

// -----------------------------------------------------------------------------

QObject* HistoryNodeCB(void* p_data)
{
    return(new CHistoryNode(static_cast<CProject*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CHistoryNode::CHistoryNode(CProject* p_project)
    : CHistoryItem(&HistoryNodeObject,p_project,EHID_FORWARD)
{
    ChangeLevel = EHCL_CORE_NODE;
}

//------------------------------------------------------------------------------

CHistoryNode::CHistoryNode(CProject* p_project,const QString& short_descr,const QString& long_descr)
    : CHistoryItem(&HistoryNodeObject,p_project,EHID_FORWARD)
{
    ChangeLevel = EHCL_CORE_NODE;
    SetShortDescription(short_descr);
    SetLongDescription(long_descr);
}

//------------------------------------------------------------------------------

CHistoryNode::~CHistoryNode(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CHistoryNode::SetShortDescription(const QString& desc)
{
    ShortDescription = desc;
}

//------------------------------------------------------------------------------

void CHistoryNode::SetLongDescription(const QString& desc)
{
    LongDescription = desc;
}

//------------------------------------------------------------------------------

void CHistoryNode::Register(CHistoryItem* p_data)
{
    p_data->setParent(this);
}

//------------------------------------------------------------------------------

bool CHistoryNode::IsChangeAllowed(const CLockLevels& locks)
{
    if( locks.testFlag(ChangeLevel) ) return(false);

    foreach(QObject* p_qobj,children()){
        CHistoryNode* p_node = dynamic_cast<CHistoryNode*>(p_qobj);
        if( p_node != NULL ){
            if( p_node->IsChangeAllowed(locks) == false ) return(false);
        }
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

EHistoryChangeLevel CHistoryNode::GetChangeLevel(void)
{
    return(ChangeLevel);
}

//------------------------------------------------------------------------------

const QString& CHistoryNode::GetShortDescription(void) const
{
    return(ShortDescription);
}

//------------------------------------------------------------------------------

const QString& CHistoryNode::GetLongDescription(void) const
{
    return(LongDescription);
}

//------------------------------------------------------------------------------

void CHistoryNode::SetChangeLevel(EHistoryChangeLevel level)
{
    ChangeLevel = level;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CHistoryNode::Backward(void)
{
    QListIterator<QObject*> it(children());
    it.toBack();
    while( it.hasPrevious() ){
        QObject* p_qobj = it.previous();
        CHistoryItem* p_item = static_cast<CHistoryItem*>(p_qobj);
        p_item->MakeChange();
    }
}

//------------------------------------------------------------------------------

void CHistoryNode::Forward(void)
{
    foreach(QObject* p_qobj,children()){
        CHistoryItem* p_item = static_cast<CHistoryItem*>(p_qobj);
        p_item->MakeChange();
    }
}

//------------------------------------------------------------------------------

void CHistoryNode::ReverseDirection(void)
{
    CHistoryItem::ReverseDirection();

    foreach(QObject* p_qobj,children()){
        CHistoryItem* p_item = static_cast<CHistoryItem*>(p_qobj);
        p_item->ReverseDirection();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CHistoryNode::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // header ------------------------------------
    p_ele->GetAttribute("sd",ShortDescription);
    p_ele->GetAttribute("ld",LongDescription);

    // load individual items ---------------------
    CXMLElement* p_sele;
    p_sele = p_ele->GetFirstChildElement("item");

    while( p_sele != NULL ) {
        CExtUUID ext_uuid;
        if( ext_uuid.GetValue(p_sele,"uuid") ){
            LOGIC_ERROR("uuid not defined");
        }
        CHistoryItem* p_item = static_cast<CHistoryItem*>(PluginDatabase.CreateObject(ext_uuid,GetProject()));
        if( p_item == NULL ){
            LOGIC_ERROR("unable to create object");
        }
        p_item->LoadData(p_sele);
        p_sele = p_sele->GetNextSiblingElement("item");
    }
}

//------------------------------------------------------------------------------

void CHistoryNode::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // header ------------------------------------
    if( ! ShortDescription.isEmpty() ) p_ele->SetAttribute("sd",ShortDescription);
    if( ! LongDescription.isEmpty() ) p_ele->SetAttribute("ld",LongDescription);

    // save individual items ---------------------
    foreach(QObject* p_qobj,children()){
        CHistoryItem* p_item = static_cast<CHistoryItem*>(p_qobj);
        CXMLElement* p_sele = p_ele->CreateChildElement("item");
        p_item->SaveData(p_sele);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

