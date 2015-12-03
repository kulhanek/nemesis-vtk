#ifndef RecentFileH
#define RecentFileH
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
#include <ExtUUID.hpp>
#include <XMLDocument.hpp>
#include <boost/shared_ptr.hpp>

//------------------------------------------------------------------------------

class CProject;

//------------------------------------------------------------------------------

/// recent file item
class NEMESIS_CORE_PACKAGE CRecentFile : public QObject {
public:
// constructors ----------------------------------------------------------------
        CRecentFile(void);
        CRecentFile(const CFileName& name);
        CRecentFile(const CFileName& name,const CExtUUID& processor);

// executive methods -----------------------------------------------------------
    /// open the file
    bool OpenFile(CProject* p_project);

    /// get file name
    const CFileName& GetName(void) const;

    /// get file processor
    const CExtUUID& GetProcessor(void) const;

// i/o methods -----------------------------------------------------------------
    /// load item
    bool Load(CXMLElement* p_ele);

    /// save item
    void Save(CXMLElement* p_ele);

// section of private data -----------------------------------------------------
private:
    CFileName       FileName;       // name of file
    CExtUUID        Processor;      // what will be used to open the file
};

//------------------------------------------------------------------------------

typedef boost::shared_ptr<CRecentFile> CRecentFilePtr;

//------------------------------------------------------------------------------

#endif
