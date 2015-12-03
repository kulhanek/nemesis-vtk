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

#include <FCPhysicalQuantity.hpp>
#include <ErrorSystem.hpp>
#include <XMLIterator.hpp>
#include <PhysicalQuantities.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>

#include "FCPhysicalQuantity.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        FCPhysicalQuantityID(
                    "{FC_PHYSICAL_QUANTITY:c8169592-251b-4e19-965b-be9b983ceb32}",
                    "Force constant physical quantity");

CPluginObject   FCPhysicalQuantityObject(&NemesisCorePlugin,
                   FCPhysicalQuantityID,GENERIC_CAT,NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFCPhysicalQuantity::CFCPhysicalQuantity(CExtComObject* p_parent,
                                             const QString& quantity_name,
                                             CPhysicalQuantity* p_right)
    : CPhysicalQuantity(&FCPhysicalQuantityObject,p_parent)
{
    setObjectName(quantity_name);
    RightPQ = p_right;

    connect(PQ_ENERGY,SIGNAL(OnUnitChanged(void)),
            this,SLOT(SubUnitChanged(void)));

    connect(RightPQ,SIGNAL(OnUnitChanged(void)),
            this,SLOT(SubUnitChanged(void)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CFCPhysicalQuantity::GetQuantityName(void) const
{
    return(objectName());
}

//------------------------------------------------------------------------------

const QString CFCPhysicalQuantity::GetUnitName(void) const
{
    QString unit_name;
    unit_name = PQ_ENERGY->GetUnitName() + "/" + RightPQ->GetUnitName() + "^2";
    return(unit_name);
}

//------------------------------------------------------------------------------

char CFCPhysicalQuantity::GetUnitFormat(void) const
{
    return(PQ_ENERGY->GetUnitFormat());
}

//------------------------------------------------------------------------------

int CFCPhysicalQuantity::GetUnitDecimals(void) const
{
    return(PQ_ENERGY->GetUnitDecimals());
}

//------------------------------------------------------------------------------

double CFCPhysicalQuantity::GetUnitOffset(void) const
{
    // combined PQ should not have any offset
    return(0.0);
}

//------------------------------------------------------------------------------

double CFCPhysicalQuantity::GetUnitScale(void) const
{
    double scale = 1.0;
    scale = PQ_ENERGY->GetUnitScale() / ( RightPQ->GetUnitScale() * RightPQ->GetUnitScale() );
    return(scale);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFCPhysicalQuantity::SubUnitChanged(void)
{
    emit OnUnitChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



