#ifndef GraphicsPropertyH
#define GraphicsPropertyH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <NemesisCoreMainHeader.hpp>
#include <Property.hpp>
#include <GraphicsObjectEnd.hpp>

// -----------------------------------------------------------------------------

class CPropertyList;

// -----------------------------------------------------------------------------

///  Property definition class

class NEMESIS_CORE_PACKAGE CGraphicsProperty : public CProperty, public CGraphicsObjectEnd {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    /// constructor.
    CGraphicsProperty(CPluginObject* p_po,CPropertyList *p_bl);

    /// update link object status - emit OnGraphicsLinkUpdated
    virtual void LinkObjectUpdated(void);

// input/output methods --------------------------------------------------------
    /// load atom data
    virtual void LoadData(CXMLElement* p_ele);

    /// save atom data
    virtual void SaveData(CXMLElement* p_ele);

// signals ---------------------------------------------------------------------
signals:
    void OnGraphicsLinkUpdated(void);
};

// -----------------------------------------------------------------------------

#endif

