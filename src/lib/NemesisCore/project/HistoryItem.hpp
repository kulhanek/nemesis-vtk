#ifndef HistoryItemH
#define HistoryItemH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QObject>
#include <ComObject.hpp>
#include <CategoryUUID.hpp>

//------------------------------------------------------------------------------

class CProject;
class CXMLElement;

//------------------------------------------------------------------------------

/// direction of change
enum EHistoryItemDirection {
    EHID_FORWARD,       // change is executed in forward direction
    EHID_BACKWARD       // change is executed in backward direction
};

//------------------------------------------------------------------------------

// IMPORTANT NOTE
// order of base classes is important, it allows to use static_cast on objects
// returned by QObject::children()

/// base class for all elementary changes

class NEMESIS_CORE_PACKAGE CHistoryItem : public QObject, public CComObject {
public:
// constructors and destructors ------------------------------------------------
    CHistoryItem(CPluginObject* p_objectinfo,
                 CProject* p_project,
                 EHistoryItemDirection create_direction);
    CHistoryItem(CPluginObject* p_objectinfo,
                 CProject* p_project);
    virtual ~CHistoryItem(void);

// executive methods -----------------------------------------------------------
    /// execute change in correct direction
    void MakeChange(void);

    /// reverse the change direction
    virtual void ReverseDirection(void);

    /// perform the change in the forward direction
    virtual void Forward(void)=0;

    /// perform the change in the backward direction
    virtual void Backward(void)=0;

// information methods ---------------------------------------------------------
    /// get current applied direction of the change
    EHistoryItemDirection GetCurrentDirection(void) const;

    /// get project
    CProject* GetProject(void) const;

// input/output methods --------------------------------------------------------
    /// load data
    virtual void LoadData(CXMLElement* p_ele);

    /// save data
    virtual void SaveData(CXMLElement* p_ele);

// section of private data -----------------------------------------------------
private:
    EHistoryItemDirection   CurrentDirection;
    CProject*               Project;
};

//------------------------------------------------------------------------------

#define REGISTER_HISTORY_OBJECT(module,name,uuid) \
QObject* name##CB(void* p_data) \
{ \
    return(new C##name(static_cast<CProject*>(p_data))); \
}\
CExtUUID      name##ID(uuid,"History item"); \
CPluginObject name##Object(&module, \
                         name##ID,HISTORY_CAT, \
                         ":/images/NemesisCore/project/HistoryItem.svg", \
                         name##CB); \
C##name::C##name(CProject* p_project) \
    : CHistoryItem(&name##Object,p_project) \
{ \
}

//------------------------------------------------------------------------------

#endif
