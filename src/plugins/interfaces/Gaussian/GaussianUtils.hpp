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
class CPhysicalQuantity;

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
    static bool ReadMethod(std::istream& sin,int& lineno,QString& method);

    /// read SCD definition
    static bool ReadSCDDef(std::istream& sin,int& lineno,QString& scd,QString& mod,double& cvscalefac,CPhysicalQuantity* &p_pq);

    /// read SCD value
    static bool ReadSCDValue(std::istream& sin,int& lineno,const QString& mod,double& value);

    /// find geometry or optimization end
    static bool FindGeometryOrOptEnd(std::istream& sin,int& lineno);

    /// find geometry
    static bool FindGeometry(std::istream& sin,int& lineno);

    /// read geometry
    static bool ReadGeometry(std::istream& sin,int& lineno,std::vector<CGAtom>& atoms);

    /// get energy
    static bool ReadEnergy(std::istream& sin,int& lineno,double& energy,std::string& type);

    /// parse energy line
    static bool ParseEnergy(std::string& line,double& energy,std::string& type);

    /// is geometry
    static bool IsGeometry(const std::string& line);

    /// is opt end
    static bool IsOptEnd(const std::string& line);

    /// is energy
    static bool IsEnergy(const std::string& line);

    /// convert geometry to CStructure
    static bool ConvertToStructure(std::vector<CGAtom>& atoms,CStructure* p_str);
};

//------------------------------------------------------------------------------

#endif
