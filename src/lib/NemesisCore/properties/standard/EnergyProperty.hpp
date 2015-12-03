#ifndef EnergyPropertyH
#define EnergyPropertyH
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

#include <NemesisCoreMainHeader.hpp>
#include <Property.hpp>

// -----------------------------------------------------------------------------

class CPropertyList;
// -----------------------------------------------------------------------------

///  Property definition class

class NEMESIS_CORE_PACKAGE CEnergyProperty : public CProperty {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    /// constructor.
    CEnergyProperty(CPropertyList *p_bl);
    
// methods with changes recorded into history list -----------------------------
    /// set energy
    bool SetEnergyWH(double energy);

    /// set method
    bool SetMethodWH(const QString& method);

// executive methods without changes recorded to history list ------------------
    /// set energy
    void SetEnergy(double energy,CHistoryNode* p_history=NULL);

    /// set method
    void SetMethod(const QString& method,CHistoryNode* p_history=NULL);

// informational methods -------------------------------------------------------
    /// get property value - scalar value
    virtual double  GetScalarValue(void);

    /// get energy
    double GetEnergy(void) const;

    /// get method
    const QString& GetMethod(void) const;

// input/output methods --------------------------------------------------------
    /// load atom data
    virtual void LoadData(CXMLElement* p_ele);

    /// save atom data
    virtual void SaveData(CXMLElement* p_ele);

// section of private data ----------------------------------------------------
protected:
    QString Method;     // method to get energy
    double  Energy;     // energy value
};

// -----------------------------------------------------------------------------

#endif

