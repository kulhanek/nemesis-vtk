#ifndef StdPhysicalQuantityH
#define StdPhysicalQuantityH
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
#include <PhysicalQuantity.hpp>
#include <ExtComObject.hpp>
#include <QString>
#include <SmallString.hpp>
#include <XMLElement.hpp>

// -----------------------------------------------------------------------------

/// implementation of physical quantity and its units

class NEMESIS_CORE_PACKAGE CStdPhysicalQuantity : public CPhysicalQuantity {
    Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CStdPhysicalQuantity(CExtComObject* p_parent,
                      const QString& quantity_name,
                      const QString& main_unit,
                      char  main_format,
                      int   decimals);

    CStdPhysicalQuantity(CExtComObject* p_parent,
                      const  QString& quantity_name,
                      const  QString& main_unit,
                      char   main_format,
                      int    decimals,
                      double offset,
                      double scale);

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

// settings methods------------------------------------------------------------
    /// units indexing: 0 - main internal unit; (from 1 to GetNumberOfUnits(void)) - user units
    /// activate unit with index (current active unit is reactivated if index == -1)
    bool  ActivateUnit(int index=-1);

    /// return the number of available units
    int  GetNumberOfUnits(void);

    /// create new unit
    bool  NewUnit(const QString& unitName, char format, int decimals,
                  double offset, double scale, bool active);

    /// remove unit
    void  RemoveUnit(int index);

    /// get unit setup
    bool  GetUnit(int index,
                  QString& unitName, char& format, int& decimals,
                  double& offset, double& scale, bool& active);

    /// set unit setup
    void  SetUnit(int index,
                  const QString& unitName, char format, int decimals,
                  double offset, double scale, bool active);

    /// set unit setup
    void  SetUnit(int index,
                  const QString& unitName, char format, int decimals,
                  double offset, double scale);

// section of private data ----------------------------------------------------
private:
    // main setup
    QString     MainUnitName;
    char        MainFormat;
    int         MainDecimals;
    double      MainOffset;
    double      MainScale;

    // current setup
    QString    UnitName;
    char       Format;
    int        Decimals;
    double     Offset;
    double     Scale;

    // return quantity setup from the user setup stored in CPhysicalQuantities
    CXMLElement* GetQuantityXMLSetup(void);

    friend class CPhysicalQuantities;
};

// -----------------------------------------------------------------------------

#endif
