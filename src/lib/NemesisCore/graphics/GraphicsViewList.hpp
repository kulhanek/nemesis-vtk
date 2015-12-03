#ifndef GraphicsViewListH
#define GraphicsViewListH
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

#include <NemesisCoreMainHeader.hpp>
#include <ProObject.hpp>
#include <GraphicsView.hpp>
#include <QMap>
#include <QThread>
#include <Transformation.hpp>

//------------------------------------------------------------------------------

class CGraphics;
class CHistoryItem;
class CXMLElement;
class CGraphicsView;

//------------------------------------------------------------------------------

// NOTE #1: primary view cannot be deleted
// NOTE #2: no history recording is supported for views

//------------------------------------------------------------------------------

/// list of all graphics views

class NEMESIS_CORE_PACKAGE CGraphicsViewList : public CProObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CGraphicsViewList(CGraphics* p_owner,bool no_index);

// executive methods without change registration to history list ---------------
    /// create new graphics view
    CGraphicsView* CreateView(const QString& name = QString(),
                              const QString& descr = QString());

    /// create new graphics view
    CGraphicsView* CreateView(CXMLElement* p_data);

    /// clear all dynamic graphics views
    void ClearViews(void);

    /// close all dynamic graphics views
    void CloseAllViews(void);

// executive methods -----------------------------------------------------------
    /// repaint all views
    void RepaintAllViews(void);

    /// repaint views associated with primary view
    void RepaintAssociatedViews(CGraphicsView* p_req);

    /// sync all associated views
    void SynchroniseAssociatedViews(CGraphicsView* p_req);

    /// try to attach openGL canvases
    void TryToAttachAllShadowViews(void);

// information methods ---------------------------------------------------------
    /// return graphics node
    CGraphics*              GetGraphics(void);

    /// return graphics profile list node
    CGraphicsProfileList*   GetProfiles(void);

    /// return primary view
    CGraphicsView*          GetPrimaryView(void);

// current view management -----------------------------------------------------
    /// current view for this thread
    CGraphicsView* GetCurrentView(void);

    /// register current view for this thread
    void RegisterCurrentView(CGraphicsView* p_view);

// local/global coordinate system ----------------------------------------------
    /// apply global view transformation matrix
    void ApplyGlobalViewTransformation(void);

    /// apply face transformation matrix
    void ApplyFaceTransformation(void);

    /// get current view Transformation matrix
    const CTransformation GetCurrentViewTrans(void);

// input/output methods --------------------------------------------------------
    /// load all graphics objects
    virtual void LoadData(CXMLElement* p_ele);

    /// save all graphics object
    virtual void SaveData(CXMLElement* p_ele);

// -----------------------------------------------------------------------------
signals:
    /// emmited when graphics object list is changed
    void OnGraphicsViewListChanged(void);

public:
    /// emit OnGraphicsViewListChanged signal
    void EmitOnGraphicsViewListChanged(void);

// section of private methods --------------------------------------------------
private:
    QMap<QThread*,CGraphicsView*>    ActiveViews;

    /// create new graphics view
    CGraphicsView* CreateView(const QString& name,const QString& descr,bool no_index);
};

//------------------------------------------------------------------------------

#endif
