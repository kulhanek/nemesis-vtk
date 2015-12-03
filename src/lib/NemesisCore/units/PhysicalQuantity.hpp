#ifndef PhysicalQuantityH
#define PhysicalQuantityH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011        Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <ExtComObject.hpp>
#include <QString>
#include <SmallString.hpp>
#include <XMLElement.hpp>

// -----------------------------------------------------------------------------

/// implementation of physical quantity and its units

class NEMESIS_CORE_PACKAGE CPhysicalQuantity : public CExtComObject {
Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CPhysicalQuantity(CPluginObject* p_pobject,CExtComObject* p_parent);

// informational methods ------------------------------------------------------
    /// return name of quantity
    virtual const QString   GetQuantityName(void) const;

    /// return name of unit
    virtual const QString   GetUnitName(void) const;

    /// return format of unit
    virtual char            GetUnitFormat(void) const;

    /// return number of decimals
    virtual int             GetUnitDecimals(void) const;

    /// return offset of unit
    virtual double          GetUnitOffset(void) const;

    /// return scale of unit
    virtual double          GetUnitScale(void) const;

// conversion methods ---------------------------------------------------------
    /// return real value as a string, format is provided by user
    QString GetRealValueText(double internalvalue,
                             char format, int decimals) const;

    /// return real value as a string, format is taken from configuration
    QString GetRealValueText(double internalvalue) const;

    /// return real value
    double  GetRealValue(double internalvalue) const;

    /// return internal value
    double  GetInternalValue(double realvalue) const;

    /// convert text to internal value
    bool    GetInternalValue(const QString& realvalue,double& value) const;

// public signals -------------------------------------------------------------
signals:
    /// signal is emmited when unit of quantity was changed
    void OnUnitChanged(void);
};

// -----------------------------------------------------------------------------

#endif
