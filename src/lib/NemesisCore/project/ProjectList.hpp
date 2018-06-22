#ifndef ProjectListH
#define ProjectListH
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
#include <ExtComObject.hpp>

//------------------------------------------------------------------------------

class CProject;
class CFileName;

//------------------------------------------------------------------------------

/// this is used to store last file path for all nemesis projects
extern NEMESIS_CORE_PACKAGE CExtUUID GenericProjectID;

//------------------------------------------------------------------------------

///  list of all projects

class NEMESIS_CORE_PACKAGE CProjectList : public CExtComObject {
    Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CProjectList(QObject* p_parent);

// new/open methods -----------------------------------------------------------
    /// create project uuid
    CProject* NewProject(const CUUID& projectuuid);

    /// create project uuid and set its name
    CProject* NewProject(const CUUID& projectuuid,const QString& pro_name);

    /// open project from filename 'fullname'
    CProject* OpenProject(const CFileName& fullname);

    /// retun number of projects
    int GetNumberOfProjects(void);

// save//close methods --------------------------------------------------------
    /// try to close project
    bool CanCloseProject(CProject* p_proj);

    /// try to save project
    bool ProjectSave(CProject* p_proj);

    /// try to save project as
    bool ProjectSaveAs(CProject* p_proj);

// methods --------------------------------------------------------------------
    /// repaint all project's scenes
    void RepaintAllProjects(void);

    /// remove all projects
    void RemoveAllProjects(void);

// signals --------------------------------------------------------------------
signals:
    /// emitted when new project is added into the list
    void OnProjectAdded(CProject* p_project);

    /// emitted when project is removed from the list
    void OnProjectRemoved(CProject* p_project);

    /// project name changed
    void OnProjectNameChanged(CProject* p_project);

public:
    void EmitProjectRemoved(CProject* p_project);
    void EmitProjectNameChanged(CProject* p_project);

// section of private data ----------------------------------------------------
private:
    friend class CProject;
};

//---------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE CProjectList* Projects;

//---------------------------------------------------------------------------

#endif
