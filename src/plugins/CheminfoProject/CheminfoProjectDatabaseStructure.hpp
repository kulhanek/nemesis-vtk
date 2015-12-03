#ifndef CheminfoProjectDatabaseStructureH
#define CheminfoProjectDatabaseStructureH
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

#include <ProObject.hpp>

#include <QStringList>
#include <SmallString.hpp>

//------------------------------------------------------------------------------

class CCheminfoProjectDatabaseStructure : public CProObject {
    Q_OBJECT
public:
    // constructors and destructors -----------------------------------------------
    /// constructor
    CCheminfoProjectDatabaseStructure(CProject* p_project);
    /// destructor
    ~CCheminfoProjectDatabaseStructure(void);

    // section of public data ----------------------------------------------------
    bool         AreInitValuesFill(void);
    bool         IsHierarchy(void);
    bool         IsLoaded(void);
    void         Load(void);
    void         Unload(void);
    CSmallString GetPath(void);
    CSmallString GetPrefix(void);
    CSmallString GetLigandType(void);
    CSmallString GetResultsType(void);
    const QStringList GetLigandFormats(void);
    const QStringList GetResultsFormats(void);
    int GetLigandTypeNum(void) const;
    int GetResultsTypeNum(void) const;

    void SetHierarchy(bool dbhierarchy);
    void SetPath(CSmallString dbpath);
    void SetPrefix(CSmallString dbprefix);
    void SetLigandType(CSmallString dbligtype, int typenum);
    void SetResultsType(CSmallString dbrestype, int typenum);

    /// Save database structure setting in to the settings file
    bool SaveDatabaseStructureSettings(void);
    /// Load database structure setting from the settings file
    bool LoadDatabaseStructureSettings(void);

    void EmitOnDatabaseFillChanged(void);
signals:
    void OnDatabaseFillChanged(void);

    // section of private data ----------------------------------------------------
private:
    CSmallString    DBPath;
    CSmallString    DBPrefix;
    CSmallString    DBLigandType;
    CSmallString    DBResultsType;
    int             DBLigandTypeNumber;
    int             DBResultsTypeNumber;
    QStringList     DBLigandFormats;
    QStringList     DBResultsFormats;

    bool            Loaded;
    bool            Hierarchy;
};

//------------------------------------------------------------------------------
#endif
