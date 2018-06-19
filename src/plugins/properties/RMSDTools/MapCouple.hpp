#ifndef MapCoupleH
#define MapCoupleH
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

#include <ProObject.hpp>

//------------------------------------------------------------------------------

class CAtom;
class CRMSDProperty;

//------------------------------------------------------------------------------

class CMapCouple : public CProObject {
Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    CMapCouple(CRMSDProperty* p_map_list);
    CMapCouple(CRMSDProperty* p_map_list,CAtom* p_at1,CAtom* p_at2);
    ~CMapCouple(void);

    void RemoveFromBaseList(CHistoryNode* p_history=NULL);

// methods with changes registered into history list ---------------------------
    /// delete couple
    bool   DeleteWH(void);

// information methods ---------------------------------------------------------
    /// get template atom
    CAtom*  GetTemplateAtom(void);

    /// get target atom
    CAtom*  GetTargetAtom(void);

    /// contains hydrogen couple
    bool IsHydrogenCouple(void);

    /// return map list
    CRMSDProperty* GetMapList(void);

// input/output methods --------------------------------------------------------
    /// load atom data
    virtual void LoadData(CXMLElement* p_ele);

    /// save atom data
    virtual void SaveData(CXMLElement* p_ele);

// registered objects  ---------------------------------------------------------
    /// handle object removal from the profile list
    virtual void RemoveFromRegistered(CProObject* p_object,
                                      CHistoryNode* p_history);

// section of private data -----------------------------------------------------
private:
    CAtom*  TemplateAtom;
    CAtom*  TargetAtom;
};

//------------------------------------------------------------------------------

#endif
