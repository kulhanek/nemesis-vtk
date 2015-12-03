#ifndef CheminfoProjectH
#define CheminfoProjectH
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

#include <Project.hpp>

#include "CheminfoProjectMainHeader.hpp"
#include "CheminfoProjectInfoList.hpp"
#include "CheminfoProjectDatabase.hpp"
#include "CheminfoProjectDatabaseStructure.hpp"
#include "CheminfoProjectTransfer.hpp"

//------------------------------------------------------------------------------

class CCheminfoProjectWindow;
class CCheminfoProjectTransfer;
class CCheminfoProjectInfoList;
class CCheminfoProjectStructureListProject;

//------------------------------------------------------------------------------

class CCheminfoProject : public CProject {
public:
// constructor and destructor -------------------------------------------------
    CCheminfoProject(CExtComObject* p_owner);
    ~CCheminfoProject(void);

    virtual void            CreateMainWindow(void);
    virtual CMainWindow*    GetMainWindow(void);
    virtual void            NewData(void);

    /// Access into relational database with Project database
    CCheminfoProjectDatabase*             GetDatabase(void);
    /// Access into filesystem where is saved Structure database
    CCheminfoProjectDatabaseStructure*    GetDatabaseStructure(void);
    /// Aontrol transfer structures into new project
    CCheminfoProjectTransfer*             GetTransfer(void);
    /// Information about each saved project
    CCheminfoProjectInfoList*             GetProjectInfoList(void);
    /// Information about each saved structure hold in relationship with project
    CCheminfoProjectStructureListProject* GetStructureListProject(void);
    /// Receptor can be saved for assigment to the results
    QString                               GetReceptorFile(void);

    void                                  SetReceptorFile(QString recfile);

// section of private data ----------------------------------------------------
private:
    CCheminfoProjectWindow*               MainWindow;
    CCheminfoProjectDatabase*             Database;
    CCheminfoProjectDatabaseStructure*    DatabaseStructure;
    CCheminfoProjectTransfer*             Transfer;
    CCheminfoProjectInfoList*             ProjectInfoList;
    CCheminfoProjectStructureListProject* StructureListProject;
    QString                               ReceptorFile;
};

//------------------------------------------------------------------------------

#endif
