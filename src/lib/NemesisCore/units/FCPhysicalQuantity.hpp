#ifndef FCPhysicalQuantityH
#define FCPhysicalQuantityH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011        Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <PhysicalQuantity.hpp>
#include <ExtComObject.hpp>
#include <QString>
#include <SmallString.hpp>
#include <XMLElement.hpp>

// -----------------------------------------------------------------------------

/// implementation of force constant physical quantity

class NEMESIS_CORE_PACKAGE CFCPhysicalQuantity : public CPhysicalQuantity {
    Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CFCPhysicalQuantity(CExtComObject* p_parent,
                          const QString& quantity_name,
                          CPhysicalQuantity* p_right);

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

// section of private data ----------------------------------------------------
private:
    CPhysicalQuantity*      RightPQ;

private slots:
    void SubUnitChanged(void);
};

// -----------------------------------------------------------------------------

#endif
