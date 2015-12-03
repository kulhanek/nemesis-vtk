#ifndef RegisteredObjectH
#define RegisteredObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Jiri Klusak, anim4tor@mail.muni.cz
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
#include <Project.hpp>
#include <ProjectList.hpp>
#include <Graphics.hpp>
#include <GraphicsObjectList.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

class CRegObjectSignalSender;

//------------------------------------------------------------------------------

class CRegObjectCore {
public:
    /// unregister object
    virtual void UnregisterObject(bool destroyed=false);
};

//------------------------------------------------------------------------------

template<class Object>
class CRegisteredObject : public CRegObjectCore {
public:
// constructors and destructors -----------------------------------------------
    CRegisteredObject(void);
    ~CRegisteredObject(void);

// executive methods ----------------------------------------------------------
    /// register object
    void RegisterObject(CProObject* p_pobj);

    /// unregister object
    virtual void UnregisterObject(bool destroyed=false);

// informational methods ------------------------------------------------------
    /// get Sender sender
    CRegObjectSignalSender* GetSender(void);

    /// get registered object
    Object* GetObject(void);

    /// is object registered and connected?
    bool IsConnected(void);

// member access operator -----------------------------------------------------
    /// smart member access operator
    Object* operator->(void);

// section of private data ----------------------------------------------------
private:
    Object*                 RegisteredObject;
    CRegObjectSignalSender* Sender;
};

// -----------------------------------------------------------------------------
// following class emulates Qt Senders for CRegisteredObject template class
// this is because current moc-compiler of Qt does not support template classes
// with Senders and slots

class CRegObjectSignalSender : public QObject {
    Q_OBJECT
public:
// constructors ---------------------------------------------------------------
    CRegObjectSignalSender(CRegObjectCore* p_regobj);

// Senders --------------------------------------------------------------------
signals:
    /// emit on registered
    void OnRegistered(CRegObjectCore* p_obj);

    /// emit on unregistered
    void OnUnregistered(CRegObjectCore* p_obj,bool destroyed);

// public (but you should not use them) methods --------------------------------
public:
    /// connect object and Senders
    void RegisterObject(CProObject* p_obj);

    /// emit OnUnregistered
    void UnregisterObject(bool destroyed);

public slots:
    /// emit Sender on removed
    void ObjectDestroyed(void);

// section of private data ----------------------------------------------------
private:
    CRegObjectCore* RegObject;
    CProObject*     Object;
};

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

template<class Object>
CRegisteredObject<Object>::CRegisteredObject(void)
{
    RegisteredObject = NULL;
    Sender = new CRegObjectSignalSender(this);
}

//------------------------------------------------------------------------------

template<class Object>
CRegisteredObject<Object>::~CRegisteredObject(void)
{
    delete Sender;
}

//------------------------------------------------------------------------------

template<class Object>
void CRegisteredObject<Object>::RegisterObject(CProObject* p_pobj)
{
    if( RegisteredObject == p_pobj ) return; // already registered

    if( RegisteredObject != NULL ) {
        // unregister previous object
        UnregisterObject();
    }

    // register new project
    RegisteredObject = dynamic_cast<Object*>(p_pobj);
    if( RegisteredObject == NULL ) return;

    Sender->RegisterObject(RegisteredObject);
}

//------------------------------------------------------------------------------

template<class Object>
void CRegisteredObject<Object>::UnregisterObject(bool destroyed)
{
    if( RegisteredObject == NULL ) return; // already unregistered

    if( destroyed) RegisteredObject = NULL;
    Sender->UnregisterObject(destroyed);
    RegisteredObject = NULL;
}

//------------------------------------------------------------------------------

template<class Object>
CRegObjectSignalSender* CRegisteredObject<Object>::GetSender(void)
{
    return(Sender);
}

//------------------------------------------------------------------------------

template<class Object>
Object* CRegisteredObject<Object>::GetObject(void)
{
    return(RegisteredObject);
}

//------------------------------------------------------------------------------

template<class Object>
bool CRegisteredObject<Object>::IsConnected(void)
{
    return( RegisteredObject != NULL );
}

//------------------------------------------------------------------------------

template<class Object>
Object* CRegisteredObject<Object>::operator->(void)
{
    return(RegisteredObject);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

#endif

