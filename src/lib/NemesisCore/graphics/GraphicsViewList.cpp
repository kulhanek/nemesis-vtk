// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <GraphicsViewList.hpp>
#include <ErrorSystem.hpp>
#include <PluginDatabase.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <XMLElement.hpp>
#include <Graphics.hpp>
#include <GL/gl.h>
#include <GL/glu.h>

#include "GraphicsViewList.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        GraphicsViewListID(
                    "{GRAPHICS_VIEW_LIST:55610ded-9278-47ea-80ff-af7a6a879661}",
                    "Views");

CPluginObject   GraphicsViewListObject(&NemesisCorePlugin,
                    GraphicsViewListID,GRAPHICS_CAT,
                    ":/images/NemesisCore/graphics/ViewList.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsViewList::CGraphicsViewList(CGraphics* p_owner,bool no_index)
    : CProObject(&GraphicsViewListObject,p_owner,p_owner->GetProject(),no_index)
{  
    SetFlag(EPOF_RO_NAME,true);

    // create primary view
    CGraphicsView* p_view = CreateView(tr("Primary view"),"",no_index);
    p_view->SetFlag(EPOF_PRIMARY_VIEW,true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsView* CGraphicsViewList::CreateView(const QString& name,
                                             const QString& descr)
{
    CGraphicsView* p_view = CreateView(name,descr,false);
    return(p_view);
}

//------------------------------------------------------------------------------

CGraphicsView* CGraphicsViewList::CreateView(const QString& name,
                                             const QString& descr,bool primary)
{
    // create object
    CGraphicsView* p_gv = new CGraphicsView(this,primary);

    QString obj_name = name;
    if( obj_name.isEmpty() ){
        obj_name = GenerateName(QString("view") + " %1");
    }

    // setup general properties
    p_gv->SetName(obj_name);
    p_gv->SetDescription(descr);

    // notify list models
    emit OnGraphicsViewListChanged();

    return(p_gv);
}

//------------------------------------------------------------------------------

CGraphicsView* CGraphicsViewList::CreateView(CXMLElement* p_data)
{
    if( p_data == NULL ) return(NULL);

    // primary view is already created
    int id = -1;
    p_data->GetAttribute("id",id);
    // try to find it
    CGraphicsView* p_gv = dynamic_cast<CGraphicsView*>(GetProject()->FindObject(id));
    if( p_gv == NULL ){
        // create new one
        p_gv = new CGraphicsView(this,false);
    }

    // load data
    p_gv->LoadData(p_data);

    // notify list models
    emit OnGraphicsViewListChanged();

    return(p_gv);
}

//------------------------------------------------------------------------------

void CGraphicsViewList::ClearViews(void)
{
    CloseAllViews();
    emit OnGraphicsViewListChanged();
}

//------------------------------------------------------------------------------

void CGraphicsViewList::CloseAllViews(void)
{
    foreach(QObject* p_qobj,children()){
        CGraphicsView* p_gv;
        p_gv = static_cast<CGraphicsView*>(p_qobj);
        // do not destroy primary view
        if( p_gv->IsFlagSet(EPOF_PRIMARY_VIEW) == true ) continue;
        p_gv->CloseShadowView();
        delete p_gv;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsViewList::RepaintAllViews(void)
{
    foreach(QObject* p_qobj,children()){
        CGraphicsView* p_gv;
        p_gv = static_cast<CGraphicsView*>(p_qobj);
        if( p_gv->IsOpenGLCanvasAttached() ){
            p_gv->RepaintOnlyThisView();
        }
    }
}

//------------------------------------------------------------------------------

void CGraphicsViewList::RepaintAssociatedViews(CGraphicsView* p_req)
{
    foreach(QObject* p_qobj,children()){
        CGraphicsView* p_gv;
        p_gv = static_cast<CGraphicsView*>(p_qobj);
        if( p_req == p_gv ) continue;
        if( p_gv->IsSynchronizedWithPrimaryView() ){
            if( p_gv->IsOpenGLCanvasAttached() ){
                p_gv->RepaintOnlyThisView();
            }
        }
    }
}

//------------------------------------------------------------------------------

void CGraphicsViewList::SynchroniseAssociatedViews(CGraphicsView* p_req)
{
    foreach(QObject* p_qobj,children()){
        CGraphicsView* p_gv;
        p_gv = static_cast<CGraphicsView*>(p_qobj);
        if( p_req == p_gv ) continue;
        if( p_gv->IsSynchronizedWithPrimaryView() ){
            p_gv->BeginUpdate();
                p_gv->SetPos(p_req->GetPos());
                p_gv->SetCentrum(p_req->GetCentrum());
                p_gv->SetScale(p_req->GetScale());
                p_gv->SetTrans(p_req->GetTrans());
            p_gv->EndUpdate();
        }
    }
}

//------------------------------------------------------------------------------

void CGraphicsViewList::TryToAttachAllShadowViews(void)
{
    foreach(QObject* p_qobj,children()){
        CGraphicsView* p_gv;
        p_gv = static_cast<CGraphicsView*>(p_qobj);
        p_gv->TryToAttachShadowView();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphics*  CGraphicsViewList::GetGraphics(void)
{
    return(dynamic_cast<CGraphics*>(parent()));
}

//------------------------------------------------------------------------------

CGraphicsProfileList*  CGraphicsViewList::GetProfiles(void)
{
    return( GetGraphics()->GetProfiles() );
}

//------------------------------------------------------------------------------

CGraphicsView* CGraphicsViewList::GetPrimaryView(void)
{
    foreach(QObject* p_qobj,children()) {
        CGraphicsView* p_gv;
        p_gv = static_cast<CGraphicsView*>(p_qobj);
        if( p_gv->IsFlagSet(EPOF_PRIMARY_VIEW) == true ) return(p_gv);
    }

    return(NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsView* CGraphicsViewList::GetCurrentView(void)
{
    return( ActiveViews[QThread::currentThread()] );
}

//------------------------------------------------------------------------------

void CGraphicsViewList::RegisterCurrentView(CGraphicsView* p_view)
{
    ActiveViews[QThread::currentThread()] = p_view;
}

//------------------------------------------------------------------------------

void CGraphicsViewList::ApplyGlobalViewTransformation(void)
{
    CGraphicsView* p_view  = GetCurrentView();
    if( p_view == NULL ) return;
    p_view->ApplyGlobalViewTransformation();
}

//------------------------------------------------------------------------------

void CGraphicsViewList::ApplyFaceTransformation(void)
{
    CGraphicsView* p_view  = GetCurrentView();
    if( p_view == NULL ) return;
    p_view->ApplyFaceTransformation();
}

//------------------------------------------------------------------------------

const CTransformation CGraphicsViewList::GetCurrentViewTrans(void)
{
    CGraphicsView* p_view  = GetCurrentView();
    CTransformation trans;
    if( p_view == NULL ) return(trans);
    return(p_view->GetTrans());
}

//------------------------------------------------------------------------------

void CGraphicsViewList::EmitOnGraphicsViewListChanged(void)
{
    emit OnGraphicsViewListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsViewList::LoadData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "views" ) {
        LOGIC_ERROR("p_ele is not 'views'");
    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    CXMLElement* p_gvele;
    p_gvele = p_ele->GetFirstChildElement("view");

    while( p_gvele != NULL ) {
        CreateView(p_gvele);
        p_gvele = p_gvele->GetNextSiblingElement("view");
    }
}

//------------------------------------------------------------------------------

void CGraphicsViewList::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    if( p_ele->GetName() != "views" ) {
        LOGIC_ERROR("p_ele is not 'views'");
    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    foreach(QObject* p_qobj,children()) {
        CGraphicsView* p_gv = static_cast<CGraphicsView*>(p_qobj);
        CXMLElement* p_gvele = p_ele->CreateChildElement("view");
        p_gv->SaveData(p_gvele);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
