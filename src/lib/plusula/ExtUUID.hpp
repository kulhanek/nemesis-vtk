#ifndef ExtUUIDH
#define ExtUUIDH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010      Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008-2009 Petr Kulhanek, kulhanek@enzim.hu,
//                            Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <PluSuLaMainHeader.hpp>
#include <UUID.hpp>
#include <QString>

//------------------------------------------------------------------------------

class CXMLElement;

//------------------------------------------------------------------------------

class PLUSULA_PACKAGE CExtUUID : public CUUID {
public:
// constructor and destructors -------------------------------------------------
    CExtUUID(void);

    CExtUUID(const QString& extuuid);

    CExtUUID(const QString& extuuid,
             const QString& name);

    CExtUUID(const QString& extuuid,
             const QString& name,
             const QString& description);

// setup operations ------------------------------------------------------------
    /// load from {CLASS_NAME:uuid}
    bool            LoadFromString(const QString& string);

    /// return in form {CLASS_NAME:uuid}
    const QString   GetFullStringForm(void) const;

    /// return in form {CLASS_NAME#uuid}
    /**
        important for accessing files on filesystems, especialy on windows
    */
    const QString   GetMaskedStringForm(void) const;

// description operations ------------------------------------------------------
    /// class name
    const QString&  GetUUIDName(void) const;

    /// object name
    const QString&  GetName(void) const;

    /// object description
    const QString&  GetDescription(void) const;

// input/output ----------------------------------------------------------------
    /// set it from XMLElement attribute
    void SetValue(CXMLElement* p_ele,const CSmallString& attrname) const;

    /// get it from XMLElement attribute
    bool GetValue(CXMLElement* p_ele,const CSmallString& attrname);

// section of private data -----------------------------------------------------
private:
    QString         UUIDName;
    QString         Name;
    QString         Description;
};

// -----------------------------------------------------------------------------

#endif
