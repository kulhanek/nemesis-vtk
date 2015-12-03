#ifndef GraphicsObjectSelectionH
#define GraphicsObjectSelectionH
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

#include <SelectionHandler.hpp>
#include <SelectionList.hpp>

// -----------------------------------------------------------------------------

template<class Object>
class CGraphicsObjectSelection : public CSelectionHandler {
public:
    CGraphicsObjectSelection(const QString& object_name);

// handler main method -----------------------------------------------------
    virtual ESelResult RegisterObject(CSelectionList* p_sel,const CSelObject& obj);

// handler description
    virtual const QString GetHandlerDescription(void) const;

private:
    QString ObjectName;
};

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

template<class Object>
CGraphicsObjectSelection<Object>::CGraphicsObjectSelection(const QString& object_name)
    : ObjectName(object_name)
{

}

//------------------------------------------------------------------------------

template<class Object>
const QString CGraphicsObjectSelection<Object>::GetHandlerDescription(void) const
{
    return("select " + ObjectName);
}

//------------------------------------------------------------------------------

template<class Object>
ESelResult CGraphicsObjectSelection<Object>::RegisterObject(CSelectionList* p_sel,const CSelObject& obj)
{
    CProObject* p_obj = obj.GetObject();

    // none object is selected
    if( p_obj == NULL ) return(ESR_NONE_SELECTED);

    // if object is CAtom then select it
    Object* p_go = dynamic_cast<Object*>(p_obj);
    if( p_go != NULL ) {
        p_sel->AddObject(obj);
        return(ESR_SELECTED_OBJECTS_END);
    }

    return(ESR_NONE_SELECTED);
}

//------------------------------------------------------------------------------

#endif

