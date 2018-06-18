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

#include <LineStippleComboBox.hpp>
#include <ErrorSystem.hpp>
#include <DirectoryEnum.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QLineStippleComboBox::QLineStippleComboBox(QWidget* p_parent)
    : QComboBox(p_parent)
{
    setIconSize(QSize(128,16));

    // GLushort = 16 bits
    QVariant data;

    // --------========
    // -x-x-x-x-x-x-x-x
    // 0101010101010101
    data = 0x5555;
    addItem(QIcon(":/images/NemesisCore/patterns/p0001.png"),"",data);

    // --------========
    // -xxx-xxx-xxx-xxx
    // 0111011101110111
    data = 0x7777;
    addItem(QIcon(":/images/NemesisCore/patterns/p0002.png"),"",data);

    // --------========
    // -xx-x-xxx-x-xx-x
    // 0101101110101101
    data = 0x5BAD;
    addItem(QIcon(":/images/NemesisCore/patterns/p0003.png"),"",data);

    // --------========
    // -x-xxxxx-x-xxxxx
    // 0101111101011111
    data = 0x5F5F;
    addItem(QIcon(":/images/NemesisCore/patterns/p0004.png"),"",data);
}

//------------------------------------------------------------------------------

void QLineStippleComboBox::setStipplePattern(int pattern)
{
    QVariant data;
    data = pattern;
    int idx = findData(data);
    setCurrentIndex(idx);
}

//------------------------------------------------------------------------------

int QLineStippleComboBox::getStipplePattern(void) const
{
    QVariant data;
    data = 0x5555;
    if( currentIndex() >= 0 ){
        data = itemData(currentIndex());
    }
    return(data.toInt());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
