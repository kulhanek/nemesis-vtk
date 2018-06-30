#ifndef PhysicalQuantitiesH
#define PhysicalQuantitiesH
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
#include <XMLDocument.hpp>

// -----------------------------------------------------------------------------

class CPhysicalQuantity;
class CStdPhysicalQuantity;

// -----------------------------------------------------------------------------
// predefined physical quantities
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_UNITY;
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_TIME;
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_DISTANCE;
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_DIMENSION;
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_ANGLE;
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_PERCENTAGE;
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_ABSOLUTE_ENERGY;
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_ENERGY;
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_TEMPERATURE;
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_CHARGE;
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_MASS;
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_VOLUME;
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_VELOCITY;
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_FREQUENCY;
extern NEMESIS_CORE_PACKAGE CPhysicalQuantity* PQ_IR_INTENSITY;

// -----------------------------------------------------------------------------

/// implementation of units for measurement

class NEMESIS_CORE_PACKAGE CPhysicalQuantities : public CExtComObject {
Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CPhysicalQuantities(CExtComObject* p_parent);

// load configuration ---------------------------------------------------------
    /// load system or user configuration
    bool  LoadConfig(void);

    /// load system configuration
    bool  LoadSystemConfig(void);

    /// save user configuration
    bool  SaveUserConfig(void);

    /// remove user config file
    bool  RemoveUserConfig(void);

// information methods --------------------------------------------------------
    /// return the number of quantities
    int  NumOfQuantities(void);

    /// return the pointer to quantity object, NULL if out-of-range
    CStdPhysicalQuantity*  GetQuantity(int index);

    /// find quantity object via its name
    CStdPhysicalQuantity*  FindQuantity(const QString& name);

signals:
    /// emited when any unit is changed
    void OnUnitChanged(void);

// section of private data ----------------------------------------------------
private:
    CXMLDocument    PhysicalQuantitiesData;

    // this method can be used by CStdPhysicalQuantity only
    CXMLElement*    GetQuantityXMLSetup(const QString& name);

    /// emit OnUnitChanged signal
    void EmitOnUnitChanged(void);

    friend class CStdPhysicalQuantity;
};

// -----------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE CPhysicalQuantities* PhysicalQuantities;

// -----------------------------------------------------------------------------

#endif
