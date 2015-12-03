#ifndef DockingProjectH
#define DockingProjectH
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

#include "DockingProjectMainHeader.hpp"
#include <Project.hpp>
#include <QFile>
#include <QFileInfo>

//------------------------------------------------------------------------------

class CDockingProjectWindow;
class CTrajectoryList;
class CPDBQTTrajSegment;
class CDockingProjectLigandImportJob;
class CDockingProjectReceptorImportJob;

//------------------------------------------------------------------------------

class CDockingProject : public CProject {
public:
    // constructor and destructor ----------------------------------------------
    CDockingProject(CExtComObject* p_owner);
    ~CDockingProject(void);

    virtual void            CreateMainWindow(void);
    virtual CMainWindow*    GetMainWindow(void);
    virtual void            NewData(void);
    void                    CreateResults(void);
    // metods for work with structures and their info --------------------------
    virtual CTrajectoryList*GetTrajectories(void);
    CStructure*             GetReceptorStructure(void);
    CStructure*             GetLigandStructure(void);
    QFileInfo               GetLigandFileName(void);
    QFileInfo               GetReceptorFileName(void);

    /// Import structures = set structures + set file names
    CDockingProjectReceptorImportJob* LoadReceptorStructure(const QString &file_ligand);
    CDockingProjectLigandImportJob* LoadLigandStructure(const QString &file_ligand);
    /// Load results into first empty result structure and trajectory
    bool                    LoadResultsStructure(const QString &file_results);
    /// Set structures
    void                    SetReceptorStructure(CStructure* str_receptor);
    void                    SetLigandStructure(CStructure* str_ligand);
    /// Set structures file names
    void                    SetLigandFileName(QFileInfo ligand_name);
    void                    SetReceptorFileName(QFileInfo  receptor_name);

    // section of private data -------------------------------------------------
private:
    CDockingProjectWindow*  MainWindow;
    // variables for structures and their info ---------------------------------
    CStructure*             Receptor;
    CStructure*             Ligand;
    CStructure*             Results;
    CTrajectoryList*        Trajectories;
    CTrajectory*            ResultTraj;
    QFileInfo               LigandFile;
    QFileInfo               ReceptorFile;
};

//------------------------------------------------------------------------------

#endif
