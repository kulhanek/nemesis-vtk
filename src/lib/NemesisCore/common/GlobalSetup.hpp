#ifndef GlobalSetupH
#define GlobalSetupH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2013        Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010        Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2009        Petr Kulhanek, kulhanek@chemi.muni.cz,
//                              Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 2008        Petr Kulhanek, kulhanek@enzim.hu,
//                              Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004   Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <FileName.hpp>
#include <deque>
#include <ExtUUID.hpp>
#include <iostream>
#include <map>

//------------------------------------------------------------------------------

class CProject;

//------------------------------------------------------------------------------

/// last open file mode
enum ELastOpenFileMode {
    ELOFM_USE_FIXED_PATH,
    ELOFM_USE_SINGLE_PATH,
    ELOFM_USE_PATH_PER_MODULE
};

//------------------------------------------------------------------------------

/// central storage for path used by program

class NEMESIS_CORE_PACKAGE CGlobalSetup : public QObject {
Q_OBJECT
public:
// setup methods --------------------------------------------------------------
    /// initialize all path - root setup has to be located relatively to libNemesisCore
    bool Init(void);

    /// save user setup
    bool Save(void);

    /// print info about path setup to standard output
    void PrintSetup(std::ostream& vout = std::cout);

// access methods -------------------------------------------------------------
    /// return path to the global system setup
    const CFileName& GetSystemSetupPath(void) const;

    /// return path to plugin locations
    const CFileName& GetPluginsLocation(void) const;

    /// return path to plugin setup
    const CFileName& GetPluginsSetup(void) const;

    /// return path to nemesis documentation
    const CFileName& GetDocPath(void) const;

    /// return path to the global system graphics setup file name
    const CFileName& GetSystemGraphicsSetupPath(void) const;

    /// return path to the system desktop setup directory
    const CFileName& GetSystemDesktopPath(void) const;

    /// return path to the system share folder
    const CFileName& GetSystemSharePath(void) const;

    /// return path to the system desktop setup directory
    const CFileName GetSystemDesktopPath(CProject* p_project,const CFileName& file) const;

    /// return path to the system work panel setup directory
    CFileName GetWorkPanelSetupPath(const CExtUUID& type) const;

    /// return path to FTGL fonts
    const CFileName& GetFontPath(void) const;

// user setup -----------------------------------------------------------------
    /// return path to the user setup directory
    const CFileName& GetUserSetupPath(void) const;

    /// get the path of directory containg the last opened file
    const CFileName  GetLastOpenFilePath(void) const;

    /// get the path of directory containg the last opened file
    const CFileName  GetLastOpenFilePath(const CUUID& module) const;

    /// set the path of directory containg the last opened file
    void             SetLastOpenFilePath(const CFileName& path,const CUUID& module);

    /// set the path of directory containg the last opened file
    void             SetLastOpenFilePathFromFile(const QString& file,const CUUID& module);

    /// is last open file path used?
    ELastOpenFileMode GetLastOpenFileMode(void) const;

    /// is last open file path used?
    void             SetLastOpenFileMode(ELastOpenFileMode set);

    /// get the path of directory containg files
    const CFileName& GetFixedLastOpenFilePath(void) const;

    /// set the path of directory containg files
    void             SetFixedLastOpenFilePath(const CFileName& path);

    /// get map of last file paths for modules
    std::map<CUUID,CFileName>& GetLastFileModulePaths(void);

    /// clear all model paths
    void ClearLastFileModelPaths(void);

    /// return path to the user graphics setup directory
    const CFileName& GetUserGraphicsSetupPath(void) const;

    /// return path to the user desktop setup directory
    const CFileName& GetUserDesktopPath(void) const;

    /// return path to the user desktop setup directory
    const CFileName  GetUserDesktopPath(CProject* p_project,const CFileName& file) const;

// section of private data ----------------------------------------------------
private:
    // user setup
    CFileName               AppDir;                     // path to nemesis application
    CFileName               PackageDir;                 // path to the whole nemesis package
    CFileName               UserSetup;                  // path to user setup
    CFileName               UserGraphicsSetupPath;      // path to user graphics setup
    CFileName               UserDesktopPath;            // path to user desktop setup

    // user file paths
    ELastOpenFileMode           LastOpenFileMode;           // last open file mode
    CFileName                   FixedLastOpenFilePath;      // fixed file repository
    CFileName                   LastOpenFilePath;           // path to the last open file
    std::map<CUUID,CFileName>   LastOpenFilePathPerModule;  // path to the last open file per module

    // global setup
    CFileName   SystemSetup;                // path to system setup
    CFileName   PluginsLocation;            // path to plugins
    CFileName   PluginsSetup;               // path to plugin setup
    CFileName   SystemGraphicsSetupName;    // path to graphics setup
    CFileName   SystemDesktopPath;          // path to desktop setup
    CFileName   DocPath;                    // nemesis documentation
    CFileName   SharePath;                  // share path
    CFileName   FontDir;                    // fonts for FTGL
};

//------------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE class CGlobalSetup*     GlobalSetup;

//---------------------------------------------------------------------------

#endif
