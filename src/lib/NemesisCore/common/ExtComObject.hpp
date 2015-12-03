#ifndef ExtComObjectH
#define ExtComObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010        Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QObject>
#include <ComObject.hpp>
#include <XMLDocument.hpp>

// -----------------------------------------------------------------------------

class CContainerModel;
class CExtComObjectDesigner;

// -----------------------------------------------------------------------------

// IMPORTANT NOTE
// order of base classes is important, it allows to use static_cast on objects
// returned by QObject::children()

// -----------------------------------------------------------------------------

enum EExtComObjectFlag {
    EECOF_SUB_CONTAINERS        = 0x00000001,   // container has subcontainers
    EECOF_DYNAMICAL_CONTENTS    = 0x00000002,   // container can change its contents (subcontainers name, description, etc...)
    EECOF_HIDDEN                = 0x00000004    // container should be hidden
};

// -----------------------------------------------------------------------------

/// extended object

class NEMESIS_CORE_PACKAGE CExtComObject : public QObject, public CComObject  {
    Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    CExtComObject(CPluginObject* p_objectinfo,QObject* p_parent);

    /// destroy object
    ~CExtComObject(void);

// container operations --------------------------------------------------------
    /// has children?
    virtual bool HasChildren(void);

    /// open specialized model for this container
    virtual CContainerModel* GetContainerModel(QObject* p_parent,const QString& type="model");

    /// has sub containers?
    bool HasSubContainers(void) const;

    /// has dynamical contents
    bool HasDynamicalContents(void) const;

// name ------------------------------------------------------------------------
    /// set name
    void SetName(const QString& name);

    /// get name
    // virtualization is used to generate names for derived objects - used in CBond
    // for saving data CExtComObject::GetName is always used
    virtual const QString GetName(void) const;

    /// generate name according to template
    const QString GenerateName(const QString& tmp);

// object designer ------------------------------------------------------------
    /// close object designer - if it is opened
    void CloseObjectDesigner(bool erase_open_flag=false);

    /// save object designer setup
    void SaveObjectDesignerSetup(void);

    /// return designer data element
    CXMLElement* GetDesignerData(bool create=false);

    /// is object designer opened?
    bool IsObjectDesignerOpened(void);

    /// inform about designer status
    virtual void UpdateDesignerRegistration(void);

public slots:
    /// open object designer
    CExtComObjectDesigner* OpenObjectDesigner(void);

// signals ---------------------------------------------------------------------
signals:
    // these signals should be used only by TreeModel
    // they should be emitted by containers with dynamical contents

    /// emmited when new child object is added to the list
    void OnChildContainerAdded(QObject* p_parent,QObject* p_obj);

    /// emmited just after child object is removed from the list
    void OnChildContainerRemoved(QObject* p_obj);

    /// emmited when child object was changed (e.g. name, etc)
    void OnChildContainerChanged(QObject* p_obj);

public:
    /// emit signal
    void EmitChildContainerRemoved(QObject* p_obj);
    void EmitChildContainerChanged(QObject* p_obj);

public:
    QFlags<EExtComObjectFlag>   ConFlags;

// section of private data -----------------------------------------------------
protected:
    // DesignerData should be used only for Designers in CProObject derived classes
    // see CProject flag EPF_PERSISTENT_DESIGNERS and its implementation in
    // CProObject::SaveData
    CXMLDocument                DesignerData;
    CExtComObjectDesigner*      ObjectDesigner;

    friend class CExtComObjectDesigner;
};

// -----------------------------------------------------------------------------

#endif
