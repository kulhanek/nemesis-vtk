#ifndef GraphicsObjectH
#define GraphicsObjectH
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
#include <ObjectManipulator.hpp>
#include <GraphicsSetup.hpp>
#include <ErrorSystem.hpp>

//------------------------------------------------------------------------------

class CXMLElement;
class CGraphicsObjectList;
class CGraphicsProfileList;
class CGraphics;
class CGraphicsProfileObject;
class CGraphicsViewList;

/// for private objects

extern CExtUUID PrivateObjectID;

//------------------------------------------------------------------------------

/// base class for graphics object

class NEMESIS_CORE_PACKAGE CGraphicsObject : public CProObject,
                                             public CObjectManipulator {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CGraphicsObject(CPluginObject*        p_objectinfo,
                    CGraphicsObjectList*  p_glist);

    virtual ~CGraphicsObject(void);

    virtual void RemoveFromBaseList(CHistoryNode* p_history=NULL);

// methods with changes registered into history list ---------------------------
    /// delete object
    bool DeleteWH(void);

    /// toggle show/hide
    bool ToggleShowHideFlagWH(void);

    /// add object to the list
    bool AddObjectWH(CProObject* p_object);

    /// selected objects to the list
    bool AddSelectedObjectsWH(void);

    /// remove object from the list
    bool RemoveObjectWH(CProObject* p_object);

    /// remove selected objects from the list
    bool RemoveSelectedObjectsWH(void);

    /// remove all objects from the list
    bool RemoveAllObjectsWH(void);

    /// overloaded method as EHCL_GRAPHICS
    bool SetFlagsWH(const CProObjectFlags& flags);

    /// emit parent OnGraphicsProfileChanged
    virtual void SetFlags(const CProObjectFlags& flags,CHistoryNode* p_history=NULL);

// objects methods -------------------------------------------------------------
    /// add object to the list
    // it adds only those objects that are permitted by CanBeObjectAdded
    virtual void AddObject(CProObject* p_object,CHistoryNode* p_history=NULL);

    /// selected objects to the list
    // it adds only those objects that are permitted by CanBeObjectAdded
    void AddSelectedObjects(CHistoryNode* p_history=NULL);

    /// remove object from the list
    virtual void RemoveObject(CProObject* p_object,CHistoryNode* p_history=NULL);

    /// remove selected objects from the list
    void RemoveSelectedObjects(CHistoryNode* p_history=NULL);

    /// is object in the list
    bool ContainsObject(CProObject* p_object);

// setup methods ---------------------------------------------------------------
    /// can be object added to the list of objects?
    virtual bool CanBeObjectAdded(CProObject* p_object);

    /// register allowed object type
    void RegisterAllowedObjectType(const CExtUUID& type);

    /// is allowed object type?
    bool IsAllowedObjectType(const CExtUUID& type);

    /// get begin iterator
    std::vector<CExtUUID>::iterator BeginAllowedObjectTypes(void);

    /// get end iterator
    std::vector<CExtUUID>::iterator EndAllowedObjectTypes(void);

// setup methods ---------------------------------------------------------------
public:
    /// set as common setup
    void SetGlobalSetup(void);

    /// set as project common setup
    void SetProjectSetup(void);

    /// do we have project setup?
    bool HasProjectSetup(void);

    /// remove project individual setup
    void RemoveProjectSetup(void);

    /// set as object individual setup
    void SetObjectSetup(void);

    /// remove object individual setup
    void RemoveObjectSetup(void);

    /// do we have object setup
    bool HasObjectSetup(void);

    /// update setup
    void UpdateSetupLink(void);

    /// emit OnSetupChanged signal and update object state
    virtual void SetupChanged(void);

    /// access to object setup
    CGraphicsSetup* GetSetup(void) const;

    /// type
    template<class type>
    type* GetSetup(void) const;

    /// return type of object or other type responsible for setup
    virtual const CExtUUID& GetGraphicsType(void);

signals:
    /// emmited when setup contents or mode changed
    void OnSetupChanged(void);

// registered objects  ---------------------------------------------------------
public:
    virtual void RemoveFromRegistered(CProObject* p_object,
                                      CHistoryNode* p_history);

// executive methods -----------------------------------------------------------
    /// draw object
    virtual void Draw(void);

// local/global coordinate system ----------------------------------------------
    /// apply global view transformation matrix
    void ApplyGlobalViewTransformation(void);

    /// apply face transformation matrix
    void ApplyFaceTransformation(void);

// information methods ---------------------------------------------------------
    /// get object list
    CGraphicsObjectList*    GetObjects(void);

    /// get profile list
    CGraphicsProfileList*   GetProfiles(void);

    /// get view list
    CGraphicsViewList*      GetViews(void);

    /// get graphics
    CGraphics*              GetGraphics(void);

// input/output methods --------------------------------------------------------
    /// load object data
    virtual void LoadData(CXMLElement* p_ele);

    /// link object with given role - for delayed load
    virtual void LinkObject(const QString& role,CProObject* p_object);

    /// save object data
    virtual void SaveData(CXMLElement* p_ele);

// -----------------------------------------------------------------------------
signals:
    /// emmited when graphics object contents is changed
    void OnGraphicsObjectContentsChanged(void);

protected:
    /// this is called every time when the object list is changed
    virtual void ObjectListObjectAdded(CProObject* p_obj);

    /// this is called every time when the object list is changed
    virtual void ObjectListObjectRemoved(CProObject* p_obj);

    /// this is called every time when the object list is changed
    virtual void ObjectListChanged(void);

// section of private data ----------------------------------------------------
private:
    EGraphicsSetupMode      SetupMode;          // setup type
    CGraphicsSetup*         Setup;              // object setup
    std::vector<CExtUUID>   AllowedObjectTypes; // object types that can be added into Objects

protected:
    QList<CProObject*>      Objects;

    friend class CGraphicsObjectModel;
};

//------------------------------------------------------------------------------

template<class type>
type* CGraphicsObject::GetSetup(void) const
{
    if( Setup == NULL ){
        LOGIC_ERROR("Setup is NULL");
    }
    return( dynamic_cast<type*>(Setup) );
}

//------------------------------------------------------------------------------

// macro usage for object Cxxxx
// include <xxxx.hpp>
// include <GraphicsProfileObject.hpp>
// designer should be CxxxxDesigner

#define GODESIGNER_ENTRY_POINT(name) \
QObject* name##DesignerCB(void* p_data) \
{ \
    CProObject* p_obj = static_cast<CProObject*>(p_data); \
    CGraphicsProfileObject* p_gpo = dynamic_cast<CGraphicsProfileObject*>(p_obj); \
    C##name* p_gobj; \
    if( p_gpo == NULL ){ \
        p_gobj = static_cast<C##name*>(p_obj); \
    } else { \
        p_gobj = static_cast<C##name*>(p_gpo->GetObject()); \
    } \
    QObject* p_object = new C##name##Designer(p_gobj); \
    return(p_object); \
}

//------------------------------------------------------------------------------

#endif
