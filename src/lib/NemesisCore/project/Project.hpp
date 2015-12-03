#ifndef ProjectH
#define ProjectH
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
#include <IndexCounter.hpp>
#include <FileName.hpp>
#include <ProObject.hpp>
#include <MouseHandler.hpp>
#include <QMainWindow>
#include <QUuid>

//------------------------------------------------------------------------------

class CProObject;
class CStructure;
class CGraph;
class CXMLElement;
class CHistoryList;
class CSelectionList;
class CGraphics;
class CProjectWidget;
class CJobList;
class CGraphicsObject;
class CGraphicsProfile;
class CProjectDesktop;
class CStructureList;
class CPropertyList;
class CMainWindow;
class CTrajectoryList;
class CTrajectory;

//------------------------------------------------------------------------------

/// this is used for notifications displayed in ProjectStatusBar as text
enum ETextNotificationType {
    ETNT_CLEAR,         // remove notification label
    ETNT_TEXT,          // text notification (black)
    ETNT_WARNING,       // warning text (blue)
    ETNT_ERROR          // error text (red)
};

// times are in miliseconds
#define ETNT_ERROR_DELAY    5000
#define ETNT_WARNING_DELAY  4000

//------------------------------------------------------------------------------

/// this is used for notifications displayed in ProjectStatusBar as progress bar
enum EProgressNotificationType {
    EPNT_START,         // start progress bar
    EPNT_PROGRESS,      // update progress bar
    EPNT_END            // end progress bar
};

//------------------------------------------------------------------------------

/// project flags
enum EProjecFlag {
    // user flags               = 0x00010000    // first user flag
    EPF_PERSISTENT_DESIGNERS    = 0x00010000,   // keep object designer persistent
    EPF_PERSISTENT_DESKTOP      = 0x00020000,   // keep project desktop persistent
    EPF_ALLOW_IMP_TO_EMPTY_STR  = 0x00040000    // allow import to empty structure
                                                // even if the Topology subsystem is locked
};


extern void ProjectRegisterMetaObject(void);

//------------------------------------------------------------------------------

/// link object record
class CProObjectLink {
public:
    CProObjectLink(void);
public:
    CProObject* Requestor;
    int         ObjectID;
    QString     Role;
};

//------------------------------------------------------------------------------

/*
===================================== Indexes ==================================
invalid id                      <= 0
Project:                        1
NemesisCore static objects:     2-50
Derived project static objects: 51-100
Dynamic objects:                101 <=

WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING

*** dynamic objects (ID > 100) cannot be created in the project constructor ***

WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
*/

// list of static indexes is provided in StaticIndexes.hpp

//------------------------------------------------------------------------------

///  base class for all projects

class NEMESIS_CORE_PACKAGE CProject : public CProObject {
Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    CProject(CPluginObject* p_objectinfo,CExtComObject* p_parent);
    ~CProject(void);

// main methods ----------------------------------------------------------------
    /// save project
    bool SaveProject(void);

    /// save project as
    bool SaveProjectAs(const CFileName& fullname);

    /// close project
    void CloseProject(void);

    /// create main project window
    virtual void CreateMainWindow(void);

    /// show project, e.g. CreateMainWindow() + BringToFront() + TryToOpenObjectDesigners() + some other stuff
    void ShowProject(void);

    /// bring project to front if possible
    void BringToFront(void);

    /// clear project
    /**
        it destroys all sub-objects, only top classes are be kept
        if redefined, then CProject::ResetProject MUST be called first
    */
    virtual void ClearProject(void);

// information methods ---------------------------------------------------------
    /// return full name of project including path
    const CFileName GetFullName(void) const;

    /// return name of project without path
    //this is inherited from CExtObject
    //const CSmallString& GetName(void) const;

    /// return path to the project file
    const CFileName& GetPath(void) const;

    /// get all structures
    CStructureList* GetStructures(void);

    /// return active structure
    CStructure* GetActiveStructure(void);

    /// get all trajectories - by default it returns NULL
    virtual CTrajectoryList* GetTrajectories(void);

    /// return active trajectory
    CTrajectory* GetActiveTrajectory(void);

    /// get history list
    CHistoryList* GetHistory(void);

    /// get selection list
    CSelectionList* GetSelection(void);

    /// get graphics
    CGraphics* GetGraphics(void);

    /// get jobs
    CJobList* GetJobs(void);

    /// get properties
    CPropertyList* GetProperties(void);

    /// get mouse handler
    CMouseHandler* GetMouseHandler(void);

    /// get project desktop
    CProjectDesktop* GetDesktop(void);

    /// get main window reference
    virtual CMainWindow* GetMainWindow(void);

    /// get project ID
    const QUuid& GetProjectID(void) const;

// notification ----------------------------------------------------------------
public slots:
    /// notify about error, warning or text to StatusBar, time is in msec
    void TextNotification(ETextNotificationType type,const QString& text=QString(),int time=-1);

    /// start progress notification
    void StartProgressNotification(int length);

    /// update progress notification
    void ProgressNotification(int progress,const QString& text=QString());

    /// end progress notification
    void EndProgressNotification(void);

signals:
    /// send text notification signal
    void OnTextNotification(ETextNotificationType type,const QString& text,int time);

    /// send progress notification signal
    void OnProgressNotification(EProgressNotificationType type,int progress,const QString& text);

public:
// basic data operations -------------------------------------------------------
    /// populate with initial data
    virtual void NewData(void);

    /// load data
    virtual void LoadData(CXMLElement* p_ele);

    /// save data
    virtual void SaveData(CXMLElement* p_ele);

// object index management -----------------------------------------------------
    /// find object
    CProObject* FindObject(int object_id);

    template<class type>
    type FindObject(int object_id);

    /// get base object index
    int GetBaseObjectIndex(void);

    /// get free object index
    /*! it is the next free index from IndexCounter
    */
    int GetFreeObjectIndex(void);

    /// set top object index
    /*! it assures that the next object index will be higher by one than object_id if
        object_id >= topindex, otherwise nothing happens
    */
    void SetTopObjectIndex(int object_id);

    /// begin link object procedure
    void BeginLinkProcedure(int base_index=-1);

    /// request to link object
    void RequestToLinkObject(CProObject* p_requestor,CXMLElement* p_ele,const QString& attrname,const QString& role);

    /// request to link object
    void RequestToLinkObject(CProObject* p_requestor,CXMLElement* p_ele,const QString& role);

    /// request to link object
    void RequestToLinkObject(CProObject* p_requestor,int object_id,const QString& role);

    /// end link object procedure
    void EndLinkProcedure(void);

    /// shedule object designer opening
    void ScheduleDesignerOpening(CProObject* p_obj);

    /// try to open object designers for sheduled objects
    void OpenScheduledObjectDesigners(void);

    /// close all opened object designers
    void CloseOpenedObjectDesigners(void);

public slots:
    /// repaint project
    void RepaintProject(void);

// section of protected data ---------------------------------------------------
private:
    CFileName               ProjectPath;
    QUuid                   ProjectID;      // unique project ID for drag&drop
// project subsystems ----------------------------
    CHistoryList*           History;
    CSelectionList*         Selection;
    CGraphics*              Graphics;
    CJobList*               Jobs;
    CPropertyList*          Properties;
    CMouseHandler*          MouseHandler;
    CProjectDesktop*        Desktop;
    CStructureList*         Structures;

// object management -----------------------------
    CIndexCounter           IndexCounter;
    QHash<int,CProObject*>  ObjectMap;
    int                     BaseIndex;
    QList<CProObjectLink>   Links;
    // if >0, it disables Index request in CProObject construktor
    int                     LinkingEnabled;
    // objects that have opened designers
    QList<CProObject*>      OpenedDesigners;
    // scheduled designers, they should be opened
    QList<CProObject*>      ScheduledDesigners;

private slots:
    void ProjectHistoryChanged(void);

protected:
// helper methods --------------------------------------------------------------
    /// helper methods
    CGraphicsObject* AddGraphicsObject(const CUUID& uuid, CGraphicsProfile* p_profile);

    friend class CProjectList;
    friend class CProObject;
};

//------------------------------------------------------------------------------

template<class type>
type CProject::FindObject(int object_id)
{
    return( dynamic_cast<type>(ObjectMap[object_id]) );
}

//------------------------------------------------------------------------------

#endif
