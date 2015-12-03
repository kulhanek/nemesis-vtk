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

#include <CombPhysicalQuantity.hpp>
#include <ErrorSystem.hpp>
#include <XMLIterator.hpp>
#include <PhysicalQuantities.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>

#include "CombPhysicalQuantity.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        CombPhysicalQuantityID(
                    "{COMB_PHYSICAL_QUANTITY:50a8a0ac-e23c-4edb-8b41-cca8a7f05830}",
                    "Combined physical quantity");

CPluginObject   CombPhysicalQuantityObject(&NemesisCorePlugin,
                   CombPhysicalQuantityID,GENERIC_CAT,NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCombPhysicalQuantity::CCombPhysicalQuantity(CExtComObject* p_parent,
                                             const QString& quantity_name,
                                             CPhysicalQuantity* p_left,
                                             CPhysicalQuantity* p_right,
                                             ECombPhysicalQuantityOp oper)
    : CPhysicalQuantity(&CombPhysicalQuantityObject,p_parent)
{
    setObjectName(quantity_name);

    LeftPQ = p_left;
    RightPQ = p_right;
    Operation = oper;

    connect(LeftPQ,SIGNAL(OnUnitChanged(void)),
            this,SLOT(SubUnitChanged(void)));

    connect(RightPQ,SIGNAL(OnUnitChanged(void)),
            this,SLOT(SubUnitChanged(void)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CCombPhysicalQuantity::GetQuantityName(void) const
{
    return(objectName());
}

//------------------------------------------------------------------------------

const QString CCombPhysicalQuantity::GetUnitName(void) const
{
    QString unit_name;
    switch(Operation){
        case ECPQO_MULT:
            if( LeftPQ == RightPQ ){
                unit_name = LeftPQ->GetUnitName() + "^2";
            } else {
                unit_name = LeftPQ->GetUnitName() + "*" + RightPQ->GetUnitName();
            }
        break;
        case ECPQO_DIV:
            unit_name = LeftPQ->GetUnitName() + "/" + RightPQ->GetUnitName();
        break;
    }

    return(unit_name);
}

//------------------------------------------------------------------------------

char CCombPhysicalQuantity::GetUnitFormat(void) const
{
    return(LeftPQ->GetUnitFormat());
}

//------------------------------------------------------------------------------

int CCombPhysicalQuantity::GetUnitDecimals(void) const
{
    return(LeftPQ->GetUnitDecimals());
}

//------------------------------------------------------------------------------

double CCombPhysicalQuantity::GetUnitOffset(void) const
{
    // combined PQ should not have any offset
    return(0.0);
}

//------------------------------------------------------------------------------

double CCombPhysicalQuantity::GetUnitScale(void) const
{
    double scale = 1.0;
    switch(Operation){
        case ECPQO_MULT:
            scale = LeftPQ->GetUnitScale() * RightPQ->GetUnitScale();
        break;
        case ECPQO_DIV:
            scale = LeftPQ->GetUnitScale() / RightPQ->GetUnitScale();
        break;
    }
    return(scale);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCombPhysicalQuantity::SubUnitChanged(void)
{
    emit OnUnitChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



