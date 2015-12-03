#ifndef Mol2ImportJobH
#define Mol2ImportJobH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <ImportJob.hpp>
#include <HistoryList.hpp>
#include <fstream>

//------------------------------------------------------------------------------

class CStructure;
class CAtom;

//------------------------------------------------------------------------------

class CMol2ImportJob : public CImportJob {
    Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CMol2ImportJob(CProject* p_project);

// section of private data ----------------------------------------------------
private:
    CHistoryNode*           History;
    std::ifstream           sin;
    CLockLevels             BackupLockLevels;
    std::map<int,CAtom*>    IndexMap;
    int                     MaxTicks;
    int                     Quantum;
    int                     Tick;
    QObject*                OldMoleculeParent;
    QObject*                OldHistoryParent;
    int                     LineNumber;
    std::string             Line;
    int                     NumOfAtoms;
    int                     NumOfBonds;

    /// can we submit the job? - executed from main thread during JobSubmit
    virtual bool JobAboutToBeSubmitted(void);

    /// initialize job - executed from main thread
    virtual bool InitializeJob(void);

    /// job main execution point - executed from job thread
    virtual bool ExecuteJob(void);

    /// finalize job - executed from main thread
    virtual bool FinalizeJob(void);

    /// helper methods
    void ReadHead(void);
    void ReadAtoms(void);
    void ReadBonds(void);
    void ReadCrystal(void);
    int  GuessZ(const QString& atname,const QString& attype);

signals:
    void OnStartProgressNotification(int length);
    void OnProgressNotification(int progress,const QString& text);
};

//------------------------------------------------------------------------------

#endif
