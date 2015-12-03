#ifndef DockingProjectDockVinaH
#define DockingProjectDockVinaH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <Project.hpp>

#include "DockingProject.hpp"
#include "DockingProjectDock.hpp"

//------------------------------------------------------------------------------

class CDockingProject;
class CDockingProjectDock;

//------------------------------------------------------------------------------

class CDockingProjectDockVina : public CDockingProjectDock {
    Q_OBJECT
public:
    // constructors and destructors -----------------------------------------------
    /// Constructor
    CDockingProjectDockVina(CProject* p_project);
    /// Destructor
    ~CDockingProjectDockVina(void);

    // section of public data ----------------------------------------------------

    // section of private data ----------------------------------------------------
private:
    int Tick;
    /// This should be redefined according to docking algorithm
    /// Initialize docker for given set of molecules
    virtual bool Initialization(void);

    /// Make dock
    virtual bool Docking(void);

    /// Finalize docking open the results
    virtual bool Finalization(void);

signals:
    // this signals are used due to thread safety - all GUI is done by master thread
    void OnStartDock(int length);
    void OnInitDock(int progress);
    void OnProgressDock(int progress, bool final=false);


};

//------------------------------------------------------------------------------
#endif
