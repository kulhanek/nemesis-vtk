// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <CategoryUUID.hpp>
#include "OpenBabelImpexFormats.hpp"
#include "OpenBabelModule.hpp"
#include <openbabel/obconversion.h>
#include <vector>

//------------------------------------------------------------------------------

using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* OpenBabelImpexFormatsCB(void* p_data);

CExtUUID        OpenBabelImpexFormatsID(
                    "{OPEN_BABEL_IMPEX_FORMATS:f6a0a457-fa8e-4218-9af0-ed9416b2cb44}",
                    "OpenBabel import/export formats");

CPluginObject   OpenBabelImpexFormatsObject(&OpenBabelPlugin,
                    OpenBabelImpexFormatsID,IMPEX_FORMATS_CAT,
                    ":/images/OpenBabel/Openbabel.png",
                    OpenBabelImpexFormatsCB);

// -----------------------------------------------------------------------------

QObject* OpenBabelImpexFormatsCB(void* p_data)
{
    QObject* p_obj = new COpenBabelImpexFormats();
    return(p_obj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

COpenBabelImpexFormats::COpenBabelImpexFormats(void)
    : CImpexFormats(&OpenBabelImpexFormatsObject)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QStringList COpenBabelImpexFormats::GetImportFormats(bool extensions)
{
    // get list of supported babel formats -------
    OBConversion co;
    vector<string> formats = co.GetSupportedInputFormat();

    // parse formats list ------------------------
    QStringList filters;
    filters << "All files (*.*)";
    filters << "Common formats (*.pdb *.xyz)";

    for(unsigned int i = 0; i < formats.size(); i++) {
        CSmallString formatString, extension, type;
        formatString = formats.at(i).c_str();
        int pos = formatString.FindSubString("--");
        if( pos != -1 ) {
            extension = formatString.GetSubString(0, pos-1);
            if( pos+3 <= (int)formatString.GetLength()-1 ){
                type = formatString.GetSubStringFromTo(pos+3,formatString.GetLength()-1);
            } else {
                type = formatString;
            }
            CSmallString format;
            if( extensions ){
                format << type << " (*." << extension << ")";
            } else {
                format << type;
            }
            filters << format.GetBuffer();
        }
    }
    filters.removeDuplicates();
    return(filters);
}

//------------------------------------------------------------------------------

const QStringList COpenBabelImpexFormats::GetExportFormats(bool extensions)
{
    OpenBabel::OBConversion     co;
    std::vector< std::string>   formats = co.GetSupportedOutputFormat();

    // parse formats list ------------------------
    QStringList filters;
    filters << "Common formats (*.pdb *.xyz)";

    for(unsigned int i = 0; i < formats.size(); i++) {
        CSmallString formatString, extension, type;
        formatString = formats.at(i).c_str();
        int pos = formatString.FindSubString("--");
        if( pos != -1 ) {
            extension = formatString.GetSubString(0, pos-1);
            if( pos+3 <= (int)formatString.GetLength()-1 ){
                type =  formatString.GetSubStringFromTo(pos+3,formatString.GetLength()-1);
            } else {
                type = formatString;
            }
            CSmallString format;
            if( extensions ){
                format << type << " (*." << extension << ")";
            } else {
                format << type;
            }
            filters << format.GetBuffer();
        }
    }
    filters.removeDuplicates();
    return(filters);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



