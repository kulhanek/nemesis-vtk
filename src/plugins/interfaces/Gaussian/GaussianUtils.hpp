#ifndef GaussianUtilsH
#define GaussianUtilsH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <istream>
#include <vector>
#include <Point.hpp>

//------------------------------------------------------------------------------

class CStructure;

//------------------------------------------------------------------------------

/// gaussian atom

class CGAtom {
public:
    int     Z;      // proton number
    CPoint  Pos;    // position
};

//------------------------------------------------------------------------------

/// gaussian utilities

class CGaussianUtils {
public:

    /// is normal termination?
    static bool IsNormalTermination(const QString& file_name);

    /// read method - cmd lines
    static bool ReadMethod(std::istream& sin,QString& method);

    /// read geometry
    static bool ReadGeometry(std::istream& sin,int& lineno,std::vector<CGAtom>& atoms,
                             bool skipfirstline=false);

    /// get energy
    static bool ReadEnergy(std::istream& sin,int& lineno,double& energy,std::string& type);

    /// parse energy line
    static bool ParseEnergy(std::string& line,double& energy,std::string& type);

    /// is geometry
    static bool IsGeometry(const std::string& line);

    /// is energy
    static bool IsEnergy(const std::string& line);

    /// convert geometry to CStructure
    static bool ConvertToStructure(std::vector<CGAtom>& atoms,CStructure* p_str);

    /// import the first structure found

};

//------------------------------------------------------------------------------

#endif
