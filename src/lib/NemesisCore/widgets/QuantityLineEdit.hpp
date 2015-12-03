#ifndef QuantityLineEditH
#define QuantityLineEditH
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
#include <QLineEdit>

// -----------------------------------------------------------------------------

class CPhysicalQuantity;

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE QQuantityLineEdit : public QLineEdit {
    Q_OBJECT
public:
// constructor and destructors -------------------------------------------------
    QQuantityLineEdit(QWidget* p_parent);

// setup methods ---------------------------------------------------------------
    /// set related quantity
    void setPhysicalQuantity(CPhysicalQuantity* p_pq);

    /// set internal value
    void setInternalValue(double value);

    /// determine if data are valid
    void setValidData(bool value);

// section of private data ----------------------------------------------------
private:
    CPhysicalQuantity*      PQ;
    double                  InternalValue;
    bool                    ValidData;

private slots:
    void unitChanged(void);
    void physicalQuantityDeleted(void);
};

// -----------------------------------------------------------------------------

#endif
