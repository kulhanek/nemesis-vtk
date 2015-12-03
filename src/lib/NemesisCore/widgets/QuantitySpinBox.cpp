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

#include <QuantitySpinBox.hpp>
#include <ErrorSystem.hpp>
#include <PhysicalQuantity.hpp>

#include "QuantitySpinBox.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QQuantitySpinBox::QQuantitySpinBox(QWidget* p_parent)
    : QDoubleSpinBox(p_parent)
{
    PQ = NULL;
    InternalValue = 0;
    setEnabled(false);
    connect(this,SIGNAL(valueChanged(double)),
            this,SLOT(realValueChanged(double)));
}

//------------------------------------------------------------------------------

void QQuantitySpinBox::setPhysicalQuantity(CPhysicalQuantity* p_pq)
{
    if( PQ ){
        // unregister previous events
        PQ->disconnect(this);
    }
    PQ = p_pq;

    if( PQ ){
        // register new events
        connect(PQ,SIGNAL(destroyed(QObject*)),
                this,SLOT(physicalQuantityDeleted(void)));
        connect(PQ,SIGNAL(OnUnitChanged()),
                this,SLOT(unitChanged()));
        setEnabled(true);
        // setup spin box
        setSuffix(" " + PQ->GetUnitName());
        if( PQ->GetUnitDecimals() > 0 ) setDecimals(PQ->GetUnitDecimals());
        setInternalValue(InternalValue);
    } else {
        bool old_value = blockSignals(true);
        InternalValue = 0.0;
        setSuffix("");
        setValue(0.0);
        setEnabled(false);
        blockSignals(old_value);
    }
}

//------------------------------------------------------------------------------

void QQuantitySpinBox::physicalQuantityDeleted(void)
{
    PQ = NULL;
    bool old_value = blockSignals(true);
    setSuffix("");
    setValue(0.0);
    setEnabled(false);
    blockSignals(old_value);
}

//------------------------------------------------------------------------------

double QQuantitySpinBox::getInternalValue(void)
{
    return(InternalValue);
}

//------------------------------------------------------------------------------

void QQuantitySpinBox::setInternalValue(double value)
{
    InternalValue = value;
    if( PQ ) {
        bool old_value = blockSignals(true);
        double real_value = PQ->GetRealValue(InternalValue);
        setMaximum(abs(real_value)+1000);
        setMinimum(-(abs(real_value)+1000));
        setValue(real_value);
        blockSignals(old_value);
    }
}

//------------------------------------------------------------------------------

void QQuantitySpinBox::unitChanged(void)
{
    bool old_value = blockSignals(true);
    setSuffix(" " + PQ->GetUnitName());
    if( PQ->GetUnitDecimals() > 0 ) setDecimals(PQ->GetUnitDecimals());
    setInternalValue(InternalValue);
    blockSignals(old_value);
}

//------------------------------------------------------------------------------

void QQuantitySpinBox::realValueChanged(double value)
{
    if( PQ ){
        InternalValue = PQ->GetInternalValue(value);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
