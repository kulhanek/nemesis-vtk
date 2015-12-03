#ifndef CheminfoProjectTransferH
#define CheminfoProjectTransferH
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

#include "CheminfoProject.hpp"

//------------------------------------------------------------------------------

class CCheminfoProject;
class CCheminfoProjectStructure;
class CJob;

//------------------------------------------------------------------------------

class CCheminfoProjectTransfer : public CProObject {
    Q_OBJECT
public:
    // constructors and destructors -----------------------------------------------
    /// Constructor
    CCheminfoProjectTransfer(CProject* p_project);
    /// Destructor
    ~CCheminfoProjectTransfer(void);

    // section of public data ----------------------------------------------------
    /// Method for check if file is exist in filesystem
    bool                ExistsThisFile(QString file_name);

    /// Choosen structure will be load with results and receptor into New Project
    /// default is new project Docking project
    void                LoadStructureToNewProject(CCheminfoProjectStructure *p_str);


    // section of private data ----------------------------------------------------
private:

    CCheminfoProject*   CheminfoProject;

    QString             ReceptorFile;

    CProject*           NewProject;
    CStructureList*     StructureList;

    /// Complete path of files
    QString             LigandFile;
    QString             ResultsFile;

    /// Method can add results into project
    CStructure*         CreateResults(CProject* p_project);

private slots:
    /// After finish jobs which load files into project will be open new project
    void                FinishLoadToNewProject(void);

};

//------------------------------------------------------------------------------
#endif
