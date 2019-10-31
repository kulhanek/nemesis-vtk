// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <GraphicsProfileList.hpp>
#include <TemplIterator.hpp>
#include <ErrorSystem.hpp>
#include <PluginDatabase.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <XMLElement.hpp>
#include <Graphics.hpp>
#include <GLSelection.hpp>
#include <QGLPixelBuffer>
#include <GraphicsProfileHistory.hpp>
#include <GraphicsProfileListHistory.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        GraphicsProfileListID(
                    "{GRAPHICS_PROFILE_LIST:b2a17719-cc6c-4850-97fe-e216b56387ba}",
                    "Profiles");

CPluginObject   GraphicsProfileListObject(&NemesisCorePlugin,
                    GraphicsProfileListID,GRAPHICS_CAT,
                    ":/images/NemesisCore/graphics/ProfileList.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsProfileList::CGraphicsProfileList(CGraphics* p_owner,bool no_index)
    : CProObject(&GraphicsProfileListObject,p_owner,p_owner->GetProject(),no_index)
{
    ConFlags |= EECOF_SUB_CONTAINERS;
    ConFlags |= EECOF_DYNAMICAL_CONTENTS;

    SetFlag(EPOF_RO_NAME,true);

    DataManipulation = false;

    ActiveProfile = NULL;
    MasterObject = NULL;
}

//------------------------------------------------------------------------------

CGraphicsProfileList::~CGraphicsProfileList(void)
{
    foreach(QObject* p_qobj,children()){
        if( p_qobj ){
            p_qobj->setParent(NULL);
            delete p_qobj;
        }
    }
    EmitOnGraphicsProfileListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsProfile* CGraphicsProfileList::CreateProfileWH(
                                                const QString& name,
                                                const QString& descr)
{
    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("create a new graphics profile"));
    if( p_history == NULL ) return (NULL);

    CGraphicsProfile* p_gp = CreateProfile(name,descr,p_history);

    EndChangeWH();
    return(p_gp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsProfile* CGraphicsProfileList::CreateProfile(
                                                const QString& name,
                                                const QString& descr,
                                                CHistoryNode* p_history)
{
    CGraphicsProfile* p_pro;

    QString pro_name = name;
    if( pro_name.isEmpty() ){
        pro_name = GenerateName(tr("Profile %1"));
    }

    try {
        p_pro = new CGraphicsProfile(this);
    } catch(...) {
        ES_ERROR("unable to allocate CGraphicsProfile");
        return(NULL);
    }

    p_pro->SetName(pro_name);
    p_pro->SetDescription(descr);

    // register change to history
    if( p_history != NULL ){
        CGraphicsProfileHI* p_hdata = new CGraphicsProfileHI(p_pro,EHID_FORWARD);
        p_history->Register(p_hdata);
    }

    // notify list models
    emit OnGraphicsProfileListChanged();
    emit OnChildContainerAdded(this,p_pro);

    return(p_pro);
}

//------------------------------------------------------------------------------

CGraphicsProfile* CGraphicsProfileList::CreateProfile(CXMLElement* p_data,
                                                      CHistoryNode* p_history)
{
    if( p_data == NULL ) return(NULL);

    CGraphicsProfile* p_pro = new CGraphicsProfile(this);

    // load data
    p_pro->LoadData(p_data);

    // register change to history
    if( p_history != NULL ){
        CGraphicsProfileHI* p_hdata = new CGraphicsProfileHI(p_pro,EHID_FORWARD);
        p_history->Register(p_hdata);
    }

    // notify list models
    emit OnGraphicsProfileListChanged();
    emit OnChildContainerAdded(this,p_pro);

    return(p_pro);
}

//------------------------------------------------------------------------------

void CGraphicsProfileList::SetActiveProfile(CGraphicsProfile* p_profile,
                                            CHistoryNode* p_history)
{
    if( ActiveProfile == p_profile ) return;

    if( p_history != NULL ) {
        CHistoryItem* p_item = new CSetActiveProfileHI(this,p_profile,ActiveProfile,EHID_FORWARD);
        p_history->Register(p_item);
    }

    ActiveProfile = p_profile;

    // notify list models
    emit OnGraphicsProfileListChanged();
}

//------------------------------------------------------------------------------

void CGraphicsProfileList::ClearProfiles(void)
{
    foreach(QObject* p_object,children()){
        delete p_object;
    }
    ActiveProfile = NULL;
    emit OnGraphicsProfileListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphics* CGraphicsProfileList::GetGraphics(void) const
{
    return(dynamic_cast<CGraphics*>(parent()));
}

//------------------------------------------------------------------------------

CGraphicsProfile* CGraphicsProfileList::GetActiveProfile(void) const
{
    return(ActiveProfile);
}

//------------------------------------------------------------------------------

bool CGraphicsProfileList::GetDataManipulationMode(void) const
{
    return(DataManipulation);
}

//------------------------------------------------------------------------------

CProObject* CGraphicsProfileList::GetSelectionMasterObject(void) const
{
    return(MasterObject);
}

//------------------------------------------------------------------------------

void CGraphicsProfileList::SetSelectionMasterObject(CProObject* p_object)
{
    MasterObject = p_object;
}

//------------------------------------------------------------------------------

void CGraphicsProfileList::SetDataManipulationMode(bool set)
{
    DataManipulation = set;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsProfileList::LoadData(CXMLElement* p_ele)
{
    // check input data --------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "profiles" ) {
        LOGIC_ERROR("p_ele is not 'profiles'");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    // profiles ----------------------------------
    CXMLElement* p_prele;
    p_prele = p_ele->GetFirstChildElement("profile");

    while( p_prele != NULL ) {
        CreateProfile(p_prele);
        p_prele = p_prele->GetNextSiblingElement("profile");
    }

    // active profile ----------------------------
    int active_profile = -1;
    p_ele->GetAttribute("active_profile",active_profile);

    // all profiles are constructed - use direct linking
    if( active_profile > 0 ){
        CGraphicsProfile* p_profile = static_cast<CGraphicsProfile*>(GetProject()->FindObject(active_profile));
        SetActiveProfile(p_profile);
    }
}

//------------------------------------------------------------------------------

void CGraphicsProfileList::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "profiles" ) {
        LOGIC_ERROR("p_ele is not 'profiles'");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    if( ActiveProfile != NULL ) {
        p_ele->SetAttribute("active_profile",ActiveProfile->GetIndex());
    } else {
        p_ele->SetAttribute("active_profile","-1");
    }

    // profiles -------------------------------------
    foreach(QObject* p_qobj,children()) {
        CGraphicsProfile* p_gp = static_cast<CGraphicsProfile*>(p_qobj);
        CXMLElement* p_prele = p_ele->CreateChildElement("profile");
        p_gp->SaveData(p_prele);
    }
}

//------------------------------------------------------------------------------

void CGraphicsProfileList::EmitOnGraphicsProfileListChanged(void)
{
    emit OnGraphicsProfileListChanged();
}

//------------------------------------------------------------------------------

void CGraphicsProfileList::EmmitGraphicsProfileRemovedSignal(CGraphicsProfile* p_gp)
{
    if( p_gp == ActiveProfile ){
        ActiveProfile = NULL;
    }
    emit OnGraphicsProfileListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
