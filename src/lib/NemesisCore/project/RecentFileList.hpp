#ifndef RecentFileListH
#define RecentFileListH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012        Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <RecentFile.hpp>

//------------------------------------------------------------------------------

class CProject;
class CRecentFile;

//------------------------------------------------------------------------------

/// central storage for path used by program

class NEMESIS_CORE_PACKAGE CRecentFileList : public QObject {
Q_OBJECT
public:
        CRecentFileList(QObject* p_parent);

// setup methods --------------------------------------------------------------
    /// load recent file list
    bool Load(void);

    /// save user recent file list
    bool Save(void);

// recent files ----------------------------------------------------------------
    /// get i-th recent file name
    bool GetFileName(unsigned int index, QString& name, QString& importer) const;

    /// get i-th recent file name
    bool GetFileName(unsigned int index, CFileName& name) const;

    /// get i-th recent file name
    bool OpenFile(unsigned int index,CProject* p_project);

    /// push new recent file name
    void PushProjectFile(const CFileName& file);

    /// push recent file name
    void PushFile(const CFileName& file,const CExtUUID& processor);

    /// remove recent file name
    void RemoveFile(const CFileName& file);

    /// push new recent file name
    void ClearList(void);

signals:
    /// emmited when recent file list is updates
    void OnRecentFileListUpdate(void);

// section of private data ----------------------------------------------------
private:
    std::deque<CRecentFilePtr>   RecentFiles;         // recently opened projects
};

//------------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE class CRecentFileList*     RecentFiles;

//---------------------------------------------------------------------------

#endif
