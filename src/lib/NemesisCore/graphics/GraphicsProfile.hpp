#ifndef GraphicsProfileH
#define GraphicsProfileH
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

#include <NemesisCoreMainHeader.hpp>
#include <ProObject.hpp>
#include <Point.hpp>
#include <Manipulator.hpp>

//------------------------------------------------------------------------------

class CGraphicsProfileList;
class CHistoryItem;
class CXMLElement;
class CGraphicsProfileObject;
class CGraphicsObject;
class CGraphicsObjectList;

// -----------------------------------------------------------------------------

enum EMoveDirection {
    EMD_DOWN,
    EMD_UP
};

// -----------------------------------------------------------------------------

/// implementation of graphics profile

class NEMESIS_CORE_PACKAGE CGraphicsProfile : public CProObject {
Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CGraphicsProfile(CGraphicsProfileList* p_bl);

    virtual ~CGraphicsProfile(void);
    virtual bool  RemoveFromBaseList(CHistoryNode* p_history=NULL);

// methods with changes registered into history list --------------------------
    /// activate profile
    bool ActivateProfileWH(void);

    /// delete profile
    bool DeleteProfileWH(void);

    /// create new graphics object with the profile
    CGraphicsObject* CreateObjectWH(
                        const CUUID& objectuuid,
                        const QString& name  = QString(),
                        const QString& descr = QString());

    /// add object to the profile
    bool AddObjectWH(CGraphicsObject* p_object);

    /// remove object from the profile
    bool RemoveObjectWH(CGraphicsObject* p_object);

    /// move object down or up in the profile
    bool MoveObjectWH(CGraphicsObject* p_object,EMoveDirection direction);

// object list manipulation methods -------------------------------------------
    /// add object to the profile
    CGraphicsProfileObject* AddObject(CGraphicsObject* p_object,int position = -1,CHistoryNode* p_history=NULL);

    /// add object to the profile
    CGraphicsProfileObject* AddObject(CXMLElement* p_ele,CHistoryNode* p_history=NULL);

    /// remove object from the list
    bool RemoveObject(CGraphicsObject* p_object,CHistoryNode* p_history=NULL);

    /// move object with the list
    bool MoveObject(CGraphicsObject* p_object,EMoveDirection direction,
                    CHistoryNode* p_history=NULL);

    /// set name
    virtual void SetName(const QString& name,CHistoryNode* p_history=NULL);

    /// set description
    virtual void SetDescription(const QString& descrip,CHistoryNode* p_history=NULL);

// informational methods ------------------------------------------------------
    /// return profile list
    CGraphicsProfileList*   GetProfiles(void) const;

    /// return all object list
    CGraphicsObjectList*    GetObjects(void) const;

    /// return number of graphics objects
    int                     GetNumberOfGraphicsObject(void) const;

    /// return graphics object by index (sorted by priority)
    CGraphicsObject*        GetGraphicsObject(int index) const;

    /// return graphics object by object
    CGraphicsProfileObject* GetGraphicsProfileObject(CGraphicsObject* p_obj) const;

    /// return graphics profile object by index (sorted by priority)
    CGraphicsProfileObject* GetGraphicsProfileObject(int index) const;

    /// calculate scene metrics
    void GetSceneMetrics(CObjMetrics& metrics);

    /// is profile active
    bool IsProfileActive(void) const;

    /// is the object the first object?
    bool IsFirstObject(CGraphicsObject* p_object) const;

    /// is the object the last object?
    bool IsLastObject(CGraphicsObject* p_object) const;

    /// is object a member of profile?
    bool IsInProfile(CGraphicsObject* p_obj);

// executive methods -----------------------------------------------------------
    /// draw profile objects
    void Draw(void);

// registered objects  ---------------------------------------------------------
    /// handle object removal from the profile list
    virtual void RemoveFromRegistered(CProObject* p_object,
                                      CHistoryNode* p_history);

// input/output methods --------------------------------------------------------
    /// load profile objects
    virtual void LoadData(CXMLElement* p_ele);

    /// save profile objects
    virtual void SaveData(CXMLElement* p_ele);

// -----------------------------------------------------------------------------
signals:
    /// emmited when graphics profile is changed
    void OnGraphicsProfileChanged(void);

// slot is required by CGraphicsProfileObject::CGraphicsProfileObject()
public slots:
    /// emit OnGraphicsProfileChanged signal
    void EmitOnGraphicsProfileChanged(void);

// section of private data ----------------------------------------------------
private:
    CGraphicsProfileObject* FirstObject;

    void EmmitGraphicsObjectRemovedSignal(CGraphicsProfileObject* p_go);

    friend class CGraphicsProfileObject;
};

//------------------------------------------------------------------------------
#endif
