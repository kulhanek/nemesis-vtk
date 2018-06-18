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

#include <PhysicalQuantity.hpp>
#include <ErrorSystem.hpp>
#include <XMLIterator.hpp>
#include <PhysicalQuantities.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPhysicalQuantity::CPhysicalQuantity(CPluginObject* p_pobject,CExtComObject* p_parent)
    : CExtComObject(p_pobject,p_parent)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CPhysicalQuantity::GetQuantityName(void) const
{
    return(objectName());
}

//------------------------------------------------------------------------------

const QString CPhysicalQuantity::GetUnitName(void) const
{
    return(QString());
}

//------------------------------------------------------------------------------

char CPhysicalQuantity::GetUnitFormat(void) const
{
    return('f');
}

//------------------------------------------------------------------------------

int CPhysicalQuantity::GetUnitDecimals(void) const
{
    return(3);
}

//------------------------------------------------------------------------------

double CPhysicalQuantity::GetUnitOffset(void) const
{
    return(0.0);
}

//------------------------------------------------------------------------------

double CPhysicalQuantity::GetUnitScale(void) const
{
    return(1.0);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QString CPhysicalQuantity::GetRealValueText(double internalvalue,
                                            char format,int decimals) const
{
    QString text;
    text.setNum(GetRealValue(internalvalue),format,decimals);
    return(text);
}

//------------------------------------------------------------------------------

QString CPhysicalQuantity::GetRealValueText(double internalvalue) const
{
    return(GetRealValueText(internalvalue,GetUnitFormat(),GetUnitDecimals()));
}

//------------------------------------------------------------------------------

double CPhysicalQuantity::GetRealValue(double internalvalue) const
{
    return((internalvalue * GetUnitScale()) + GetUnitOffset());
}

//------------------------------------------------------------------------------

double CPhysicalQuantity::GetInternalValue(double realvalue) const
{
    return((realvalue - GetUnitOffset()) / GetUnitScale());
}

//------------------------------------------------------------------------------

bool CPhysicalQuantity::GetInternalValue(const QString& realvalue,double& value) const
{
    bool ok;
    value = GetInternalValue(realvalue.toDouble(&ok));
    return(ok);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



