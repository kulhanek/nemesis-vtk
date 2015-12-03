#ifndef ProObjectH
#define ProObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ExtComObject.hpp>
#include <GeoDescriptor.hpp>
#include <XMLDocument.hpp>
#include <ObjMetrics.hpp>
#include <QFlags>
#include <HistoryNode.hpp>

//------------------------------------------------------------------------------

class CProject;
class CXMLElement;
class CObjectDesigner;
class CHistoryNode;

//------------------------------------------------------------------------------

// these flags are object specific

enum EProObjectFlag {
    // flags that are not saved -----------
    // technical flags          = 0x00000001    // first technical flag
    EPOF_READ_ONLY              = 0x00000001,   // read only object
    EPOF_RO_NAME                = 0x00000002,   // name of object cannot be changed
    EPOF_RO_DESCRIPTION         = 0x00000004,   // description of object cannot be changed
    EPOF_TMP_NAME               = 0x00000008,   // temporary name
    EPOF_SELECTED               = 0x00000010,   // object is selected/highlighted
    EPOF_MANIP_FLAG             = 0x00000020,   // molecule spanning flag
    EPOF_PROC_FLAG              = 0x00000040,   // molecule spanning flag
    EPOF_PROJECT_CHANGED        = 0x00000080,   // project was changed
    EPOF_PRIMARY_VIEW           = 0x00000100,   // primary view

    // flags that are saved ---------------
    // saved flags              = 0x00001000    // first saved flag
    EPOF_VISIBLE                = 0x00001000,   // object is visible
    EPOF_FREEZED                = 0x00002000,   // object is freezed (used by atoms)
    EPOF_ENABLED                = 0x00004000    // object is enabled
    // user flags               = 0x00010000    // first user flag

    // flags that are not saved -----------
    // user technical flags     = 0x10000000    // first user technical flag
};

#define EPOF_SAVE_MASK            0x0FFFF000

#define SET_FLAG(flags,flag,set) \
if( set ){ \
    flags |= (flag); \
} else { \
    flags &= ~(flag); \
}

typedef QFlags<EProObjectFlag> CProObjectFlags;

//------------------------------------------------------------------------------

// for OnStatusChanged signal

enum EStatusChanged {
    ESC_NAME,           // object name was changed
    ESC_DESCRIPTION,    // object description was changed
    ESC_FLAG,           // object flags were changed
    ESC_PARENT,         // object parentship was changed
    ESC_OTHER           // other object status was changed
};

extern void ProObjectRegisterMetaObject(void);

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CProObject : public CExtComObject {
    Q_OBJECT
public:
// constructors ----------------------------------------------------------------
    CProObject(CPluginObject* p_objectinfo,
               CExtComObject* p_owner,
               CProject* p_project);

    CProObject(CPluginObject* p_objectinfo,
               CExtComObject* p_owner,
               CProject* p_project,
               bool no_index);

    CProObject(CPluginObject* p_objectinfo,
               CExtComObject* p_owner,
               CProject* p_project,
               const QString& name);

    virtual ~CProObject(void);

// general history support -----------------------------------------------------
    /// begin composite change with record to history list
    CHistoryNode* BeginChangeWH(EHistoryChangeLevel level,
                                const QString& short_descr,
                                const QString& long_descr = QString());

    /// end composite change with record to history list
    void EndChangeWH(void);

    // shortcuts for History methods
    /// begin change recording
    bool BeginChange(EHistoryChangeLevel lockmodelevel);

    /// record change
    void RegisterChange(CHistoryNode *p_node);

    /// close recording
    void EndChange(void);

    /// is this subsystem locked
    bool IsHistoryLocked(EHistoryChangeLevel level);

// name and description of object with event support ---------------------------
    /// change name with change recorded to history list
    bool SetNameWH(const QString& name);

    /// change description with change recorded to history list
    bool SetDescriptionWH(const QString& descrip);

    /// change flgs with change recorded to history list
    bool SetFlagsWH(const CProObjectFlags& flags);

// description -----------------------------------------------------------------
    /// set name
    virtual void SetName(const QString& name,CHistoryNode* p_history=NULL);

    /// set description
    virtual void SetDescription(const QString& descrip,CHistoryNode* p_history=NULL);

    /// get description
    const QString&  GetDescription(void);

// object id methods -----------------------------------------------------------
    /// set ID of this object
    virtual  void SetIndex(int index);

    /// get ID of this object
    virtual  int  GetIndex(void) const;

    /// return project specific MIME type
    const QString GetProjectMIMEType(const QString& baseid);

// flags support ---------------------------------------------------------------
    /// set object flags
    virtual void SetFlags(const CProObjectFlags& flags,CHistoryNode* p_history=NULL);

    /// set object flag
    virtual void SetFlag(EProObjectFlag flag,bool set);

    template <typename T1>
    inline void SetFlag(T1 flag,bool set);

    /// is flag set?
    bool IsFlagSet(EProObjectFlag flag);

    /// is flag set?
    template <typename T1>
    inline bool IsFlagSet(T1 flag);

    /// get object flags
    const CProObjectFlags& GetFlags(void) const;

// executive methods -----------------------------------------------------------
    /// register object - server side
    void  RegisterObject(CProObject* p_ro);

    /// unregister object - server side
    void  UnregisterObject(CProObject* p_ro);

    /// remove first object from list - client side
    CProObject* RemoveFirstFromROList(void);

    /// unregister all registered objects - client side
    void  UnregisterAllRegisteredObjects(CHistoryNode* p_history=NULL);

    /// remove object from registered objects - server side
    virtual void RemoveFromRegistered(CProObject* p_object,
                                      CHistoryNode* p_history);

    /// return model for registered objects
    CContainerModel* GetRegObjectsContainerModel(QObject* p_parent);

    /// return list of registered objects
    const QList<CProObject*>& GetROList(void) const;

    /// inform about designer status
    virtual void UpdateDesignerRegistration(void);

// informational methods -------------------------------------------------------
    /// get associated project
    CProject* GetProject(void) const;

    /// return geometry descriptor
    virtual CGeoDescriptor  GetGeoDescriptor(void);

    /// get object metrics
    virtual void GetObjectMetrics(CObjMetrics& metrics);

// input/output methods --------------------------------------------------------
    /// load data
    virtual void LoadData(CXMLElement* p_ele);

    /// link object with given role - for delayed load
    virtual void LinkObject(const QString& role,CProObject* p_object);

    /// save data
    virtual void SaveData(CXMLElement* p_ele);

// signals ---------------------------------------------------------------------
signals:
    /// signal emmited when ProObjectStatus is changed
    void OnStatusChanged(EStatusChanged type);

    /// signal emmited when new object is registered
    void OnProObjectRegistered(CProObject* p_obj);

    /// signal emmited when object is unregistered
    void OnProObjectUnregistered(CProObject* p_obj);

// section of private data -----------------------------------------------------
private:
    CProject*           Project;            // project that owns object
    QList<CProObject*>  RegisteredObjects;  // registered objects
    int                 Index;
    QString             Description;
    CProObjectFlags     Flags;

    friend class CObjectDesigner;
};

//------------------------------------------------------------------------------

template <typename T1>
inline void CProObject::SetFlag(T1 flag,bool set)
{
    SetFlag(static_cast<EProObjectFlag>(flag),set);
}

//------------------------------------------------------------------------------

template <typename T1>
inline bool CProObject::IsFlagSet(T1 flag)
{
    return( IsFlagSet(static_cast<EProObjectFlag>(flag)) );
}

//------------------------------------------------------------------------------

#endif
