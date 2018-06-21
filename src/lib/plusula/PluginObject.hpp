#ifndef PluginObjectH
#define PluginObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010      Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008-2009 Petr Kulhanek, kulhanek@enzim.hu,
//                            Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <PluSuLaMainHeader.hpp>
#include <ExtUUID.hpp>
#include <ComObject.hpp>
#include <QIcon>
#include <QObject>

//------------------------------------------------------------------------------

class CXMLElement;
class CPluginModule;
class CComObject;
class CPluginObject;

//------------------------------------------------------------------------------

typedef QObject* (*TCreateObject)(void* p_data);

//------------------------------------------------------------------------------

class PLUSULA_PACKAGE CPluginObject {
public:
// constructor and destructor --------------------------------------------------
    CPluginObject(CPluginModule*        p_module,
                  const CExtUUID&       objectuuid,
                  const CExtUUID&       categoryuuid,
                  TCreateObject         createobject);

    CPluginObject(CPluginModule*        p_module,
                  const CExtUUID&       objectuuid,
                  const CExtUUID&       categoryuuid,
                  const QString&        picture_name,
                  TCreateObject         createobject);

    CPluginObject(CPluginModule*        p_module,
                  const CExtUUID&       objectuuid,
                  const CExtUUID&       categoryuuid,
                  const QString&        picture_name,
                  const QStringList&    attributes,
                  TCreateObject         createobject);

    CPluginObject(CPluginModule*        p_module,
                  const CExtUUID&       objectuuid,
                  const CExtUUID&       categoryuuid,
                  const QStringList&    attributes,
                  TCreateObject         createobject);

    ~CPluginObject(void);

// initialization methods ------------------------------------------------------
    void InitializeObject(CXMLElement* p_objectconfig);

// description of object -------------------------------------------------------
    /// get name of object
    const QString&  GetName(void) const;

    /// get description of object
    const QString&  GetDescription(void) const;

    /// get categoy UUID of object
    const CExtUUID& GetCategoryUUID(void) const;

    /// get object UUID of object
    const CExtUUID& GetObjectUUID(void) const;

    /// number of opened objects
    int             GetObjectCount(void) const;

    /// return picture name associated with object
    const QString&  GetPictureName(void) const;

    /// return icon associated with object
    const QIcon     GetIcon(void) const;

    /// has attribute?
    bool HasAttribute(const QString& attrname);

    /// get attribute value
    const QString GetAttributeValue(const QString& attrname);

// setup management ------------------------------------------------------------
    /// find config value
    bool FindObjectConfigValue(const QString& key, CUUID& value) const;

    /// find config value
    bool FindObjectConfigValue(const QString& key, CExtUUID& value) const;

    /// find config value
    bool FindObjectConfigValue(const QString& key, QString& value) const;

// object creation -------------------------------------------------------------
    QObject*        CreateObject(void* p_data);

// information methods ---------------------------------------------------------
    CPluginModule*  GetPluginModule(void) const;
    CXMLElement*    GetObjectConfig(void) const;

// section of private data -----------------------------------------------------
private:
    CXMLElement*    ObjectConfig;
    CPluginModule*  PluginModule;

    const CExtUUID& ObjectUUID;
    const CExtUUID& CategoryUUID;
    int             ObjectCounter;
    TCreateObject   ObjectCallback;

    // description
    QString         PictureName;
    QStringList     Attributes;

    void CreateObject(void);
    void DestroyObject(void);

    friend class CComObject;
    friend class CPluginDatabase;
};

//------------------------------------------------------------------------------
#endif
