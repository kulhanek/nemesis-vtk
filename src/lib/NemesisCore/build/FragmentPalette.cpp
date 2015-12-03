// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <FragmentPalette.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <XMLParser.hpp>
#include <XMLPrinter.hpp>
#include <XMLIterator.hpp>
#include <Fragment.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFragmentPalette::CFragmentPalette(void)
{

}

//------------------------------------------------------------------------------

CFragmentPalette::~CFragmentPalette(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CFragmentPalette::Load(const CSmallString& name)
{
    // ------------
    Clear();

    // ------------
    CXMLDocument   xml_doc;
    CXMLParser     xml_parser;

    xml_parser.SetOutputXMLNode(&xml_doc);

    if( xml_parser.Parse(name) == false ) {
        CSmallString error;
        error << "unable to parse structure palette " << name;
        ES_ERROR(error);
        return(false);
    }

    CXMLElement* p_el = xml_doc.GetFirstChildElement("structures");
    return( Load(p_el) );
}

//------------------------------------------------------------------------------

bool CFragmentPalette::Save(const CSmallString& name)
{
    CXMLDocument   xml_doc;

    xml_doc.CreateChildDeclaration();
    xml_doc.CreateChildComment("NEMESIS Structure Palette");

    CXMLElement* p_el = xml_doc.CreateChildElement("structures");
    if( Save(p_el) == false ) {
        CSmallString error;
        error << "unable to save structure palette" << name;
        ES_ERROR(error);
        return(false);
    }

    CXMLPrinter     xml_printer;
    xml_printer.SetPrintedXMLNode(&xml_doc);

    if( xml_printer.Print(name) == false ) {
        CSmallString error;
        error << "unable to save structure palette " << name;
        ES_ERROR(error);
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CFragmentPalette::Load(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        ES_ERROR("p_element is NULL");
        return(false);
    }

    bool result = true;

    CXMLIterator    I(p_ele);
    CXMLElement*    p_strele;

    while( (p_strele = I.GetNextChildElement("structure")) != NULL ) {
        // -----------
        CFragment* p_str = new CFragment();
        try{
            p_str->Load(p_strele);
        } catch(...) {
            delete p_str;
            result = false;
            continue;
        }
        // -----------
        Fragments.InsertToEnd(p_str,0,true);
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CFragmentPalette::Save(CXMLElement* p_element)
{
    //TODO:
    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CFragmentPalette::GetNumberOfFragments(void)
{
    return(Fragments.NumOfMembers());
}

//------------------------------------------------------------------------------

CFragment* CFragmentPalette::GetFragment(int index)
{
    if( (index < 0) || (index >= Fragments.NumOfMembers()) ) return(NULL);
    return(Fragments[index]);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFragmentPalette::Clear(void)
{
    Fragments.RemoveAll();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

