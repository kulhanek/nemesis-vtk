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

#include <QuantityLineEdit.hpp>
#include <ErrorSystem.hpp>
#include <PhysicalQuantity.hpp>

#include "QuantityLineEdit.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QQuantityLineEdit::QQuantityLineEdit(QWidget* p_parent)
    : QLineEdit(p_parent)
{
    PQ = NULL;
    InternalValue = 0;
    ValidData = true;
    setReadOnly(true);
    setEnabled(false);
}

//------------------------------------------------------------------------------

void QQuantityLineEdit::setPhysicalQuantity(CPhysicalQuantity* p_pq)
{
    if( PQ ){
        // unregister previous events
        PQ->disconnect(this);
    }
    PQ = p_pq;

    if( PQ ) {
        // register new events
        connect(PQ,SIGNAL(destroyed(QObject*)),
                this,SLOT(physicalQuantityDeleted(void)));
        connect(PQ,SIGNAL(OnUnitChanged()),
                this,SLOT(unitChanged()));
        setEnabled(true);
        setInternalValue(InternalValue);
    } else {
        setEnabled(false);
        setText("");
    }
}

//------------------------------------------------------------------------------

void QQuantityLineEdit::physicalQuantityDeleted(void)
{
    PQ = NULL;
    bool old_value = blockSignals(true);
    setText("");
    setEnabled(false);
    blockSignals(old_value);
}

//------------------------------------------------------------------------------

void QQuantityLineEdit::setInternalValue(double value)
{
    InternalValue = value;
    if( (PQ != NULL) & ValidData ) {
        QString text;
        text = PQ->GetRealValueText(InternalValue) + " " + PQ->GetUnitName();
        setText(text);
    } else {
        setText("");
    }
}

//------------------------------------------------------------------------------

void QQuantityLineEdit::setValidData(bool value)
{
    ValidData = value;
    setInternalValue(InternalValue);
}

//------------------------------------------------------------------------------

void QQuantityLineEdit::unitChanged(void)
{
    setInternalValue(InternalValue);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
