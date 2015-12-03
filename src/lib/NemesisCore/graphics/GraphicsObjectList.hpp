#ifndef GraphicsObjectListH
#define GraphicsObjectListH
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

#include <NemesisCoreMainHeader.hpp>
#include <ProObject.hpp>
#include <Manipulator.hpp>
#include <GraphicsObject.hpp>

//------------------------------------------------------------------------------

class CGraphics;
class CHistoryItem;
class CXMLElement;
class CGraphicsObjectEnd;

//------------------------------------------------------------------------------

/// list of all graphics objects and profiles

class NEMESIS_CORE_PACKAGE CGraphicsObjectList : public CProObject {
Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CGraphicsObjectList(CGraphics* p_owner,bool no_index);
    ~CGraphicsObjectList(void);

// methods with changes registered into history list --------------------------
    /// create new graphics object
    CGraphicsObject* CreateObjectWH(
                        const CUUID& objectuuid,
                        const QString& name  = QString(),
                        const QString& descr = QString());

    /// hide all objects of given type
    bool HideAllObjectsWH(const CUUID& objectuuid);

    /// show all objects of given type
    bool ShowAllObjectsWH(const CUUID& objectuuid);

// executive methods without change registration to history list --------------
    /// create new graphics object
    CGraphicsObject* CreateObject(const CUUID& objectuuid,
                                const QString& name = QString(),
                                const QString& descr = QString(),
                                CHistoryNode* p_history = NULL);

    /// create new graphics object
    CGraphicsObject* CreateObject(CXMLElement* p_data,
                                CHistoryNode* p_history = NULL);

    /// create link object
    CGraphicsObject* CreateObject(CGraphicsObjectEnd* p_end,
                                CHistoryNode* p_history = NULL);

    /// clear all dynamics graphics objects
    void ClearObjects(void);

// information methods --------------------------------------------------------
    /// return graphics node
    CGraphics*              GetGraphics(void);

    /// return graphics profile list node
    CGraphicsProfileList*   GetProfiles(void);

    /// return primary object of given type
    CGraphicsObject* GetPrimaryObject(const CUUID& objectuuid);

// input/output methods -------------------------------------------------------
    /// load all graphics objects
    virtual void LoadData(CXMLElement* p_ele);

    /// save all graphics object
    virtual void SaveData(CXMLElement* p_ele);

// -----------------------------------------------------------------------------
signals:
    /// emmited when graphics object list is changed
    void OnGraphicsObjectListChanged(void);

public:
    /// emit OnGraphicsObjectListChanged signal
    void EmitOnGraphicsObjectListChanged(void);
};

//------------------------------------------------------------------------------

#endif
