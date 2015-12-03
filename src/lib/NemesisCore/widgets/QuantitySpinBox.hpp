#ifndef QuantitySpinBoxH
#define QuantitySpinBoxH
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
#include <QDoubleSpinBox>

// -----------------------------------------------------------------------------

class CPhysicalQuantity;

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE QQuantitySpinBox : public QDoubleSpinBox {
    Q_OBJECT
public:
// constructor and destructors -------------------------------------------------
    QQuantitySpinBox(QWidget* p_parent);

// setup methods ---------------------------------------------------------------
    /// set related quantity
    void setPhysicalQuantity(CPhysicalQuantity* p_pq);

    /// get internal value
    double getInternalValue(void);

    /// set internal value
    void setInternalValue(double value);

// section of private data ----------------------------------------------------
private:
    CPhysicalQuantity*      PQ;
    double                  InternalValue;

private slots:
    void unitChanged(void);
    void realValueChanged(double value);
    void physicalQuantityDeleted(void);
};

// -----------------------------------------------------------------------------

#endif
