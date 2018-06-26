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

#include <SmallString.hpp>
#include <ErrorSystem.hpp>
#include <QMessageBox>
#include <Structure.hpp>
#include <AtomList.hpp>
#include <OpenBabelUtils.hpp>

#include "GaussianUtils.hpp"
#include "openbabel/mol.h"
#include "openbabel/obconversion.h"

//------------------------------------------------------------------------------

using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CGaussianUtils::IsNormalTermination(const QString& file_name)
{
    std::ifstream   sin;

    sin.open(file_name.toLatin1());
    if( !sin ) {
        return(false);
    }

    string line;

    while( sin ){
        getline(sin,line);
        if( line.find("Normal termination of Gaussian") != std::string::npos ) return(true);
    }

    return(false);
}

//------------------------------------------------------------------------------

bool CGaussianUtils::ReadMethod(std::istream& sin,QString& method)
{
    method = "";

    string line;

    // find beginning
    int count = 3;
    while( sin ){
        getline(sin,line);
        if( line.size() < 2 ) continue;
        if( line[1] == '-' ) count--;
        if( count == 0 ) break;
    }
    // not found
    if( count != 0 ) return(false);

    // next lines are method
    count = 0;
    while( sin ){
        getline(sin,line);
        if( line.size() < 2 ) continue;
        if( line[1] == '-' ) return(true);
        if( count > 0 ) method += " ";
        method += QString(line.c_str());
        count++;
    }

    return(false);
}

//------------------------------------------------------------------------------

bool CGaussianUtils::ReadGeometry(std::istream& sin,int& lineno,std::vector<CGAtom>& atoms,bool skipfirstline)
{
    atoms.clear();

    string line;

    // find beginning
    if( ! skipfirstline ) {
        for(;;){
            getline(sin,line);
            if( ! sin ) {
                CSmallString error;
                error << "no geometry header found - end of file";
                ES_WARNING(error);
                return(false);
            }
            lineno++;
            if( IsGeometry(line) == true ) break;
        }
    }

    // skip header
    getline(sin,line);
    lineno++;
    getline(sin,line);
    lineno++;
    getline(sin,line);
    lineno++;
    getline(sin,line);
    lineno++;

    // read the geometry
    for(;;){
        getline(sin,line);
        if( ! sin ) {
            ES_ERROR("premature end of file");
            return(false);
        }
        lineno++;
        if( line.find(" ------") != string::npos ) break; // end of geometry
        stringstream str(line);
        CGAtom atom;
        int    buffer;
        str >> buffer >> atom.Z >> buffer >> atom.Pos.x >> atom.Pos.y >> atom.Pos.z;
        if( ! str ){
            CSmallString error;
            error << "unable to read correctly data with geometry at line " << lineno;
            ES_ERROR(error);
            return(false);
        }
        atoms.push_back(atom);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CGaussianUtils::IsGeometry(const std::string& line)
{
    if( line.find("Input orientation:") != string::npos ) return(true);
    if( line.find("Z-Matrix orientation:") != string::npos ) return(true);
    if( line.find("Standard orientation:") != string::npos ) return(true);
    return(false);
}

//------------------------------------------------------------------------------

bool CGaussianUtils::IsEnergy(const std::string& line)
{
    if( line.find(" SCF Done: ") == 0 ) return(true);
    return(false);
}

//------------------------------------------------------------------------------

bool CGaussianUtils::ReadEnergy(std::istream& sin,int& lineno,double& energy,std::string& type)
{
    string line;

    // find beginning
    for(;;){
        getline(sin,line);
        if( ! sin ) {
            CSmallString error;
            error << "no energy header found - end of file";
            ES_WARNING(error);
            return(false);
        }
        lineno++;
        if( IsEnergy(line) == true ) break;
    }

    // parse energy
    return( ParseEnergy(line,energy,type) );
}

//------------------------------------------------------------------------------

bool CGaussianUtils::ParseEnergy(std::string& line,double& energy,std::string& type)
{
    energy = 0.0;
    string buffer;
    stringstream str(line);
    str >> buffer >> buffer >> type >> buffer >> energy;
    return( ! str.fail() );
}

//------------------------------------------------------------------------------

bool CGaussianUtils::ConvertToStructure(std::vector<CGAtom>& atoms,CStructure* p_str)
{
    try {
        OBMol mol;

        std::vector<CGAtom>::iterator   it = atoms.begin();
        std::vector<CGAtom>::iterator   ie = atoms.end();

        while( it != ie ){
            CGAtom atom = *it;
            // create new atom
            OBAtom* p_obatom = mol.NewAtom();
            p_obatom->SetAtomicNum(atom.Z);
            p_obatom->SetVector(atom.Pos.x,atom.Pos.y,atom.Pos.z);

            it++;
        }

        mol.ConnectTheDots();
        mol.PerceiveBondOrders();

        // convert data
        COpenBabelUtils::OpenBabel2Nemesis(mol,p_str);

    } catch(...){
        ES_ERROR("an exception occured");
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



