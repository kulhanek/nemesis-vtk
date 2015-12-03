#ifndef GraphicsProfileListH
#define GraphicsProfileListH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <NemesisCoreMainHeader.hpp>
#include <ProObject.hpp>
#include <Manipulator.hpp>
#include <GraphicsProfile.hpp>

//------------------------------------------------------------------------------

class CGraphics;
class CHistoryItem;
class CGLWidget;
class CXMLElement;

//------------------------------------------------------------------------------

/// list of all graphics objects and profiles

class NEMESIS_CORE_PACKAGE CGraphicsProfileList : public CProObject {
Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CGraphicsProfileList(CGraphics* p_owner,bool no_index);
    ~CGraphicsProfileList(void);

// profile manipulation with registration to history list ---------------------
    /// create new profile
    CGraphicsProfile* CreateProfileWH(const QString& name = QString(),
                                      const QString& descr = QString());

// profile manipulation without registration to histoy list -------------------
    /// create profile
    CGraphicsProfile* CreateProfile(const QString& name = QString(),
                                    const QString& descr = QString(),
                                    CHistoryNode* p_history=NULL);
    /// create profile
    CGraphicsProfile* CreateProfile(CXMLElement* p_data,
                                    CHistoryNode* p_history=NULL);

    /// set active profile
    void              SetActiveProfile(CGraphicsProfile* profile,
                                       CHistoryNode* p_history=NULL);

    /// clear all dynamics graphics profiles
    void ClearProfiles(void);


// information methods ---------------------------------------------------------
    /// return owner of this list
    CGraphics*          GetGraphics(void) const;

    /// return the active profile
    CGraphicsProfile*   GetActiveProfile(void) const;

    /// return current manipulation mode
    bool GetDataManipulationMode(void) const;

    /// return current master object
    CProObject* GetSelectionMasterObject(void) const;

// setup methods ---------------------------------------------------------------
    /// set master object
    void SetSelectionMasterObject(CProObject* p_object);

    /// disable scene repainting when structure data are manipulated (during import)
    void SetDataManipulationMode(bool set);

// input/output methods --------------------------------------------------------
    /// load all profiles
    virtual void LoadData(CXMLElement* p_ele);

    /// save all profiles
    virtual void SaveData(CXMLElement* p_ele);

// -----------------------------------------------------------------------------
signals:
    /// emmited when graphics profile list is changed
    void OnGraphicsProfileListChanged(void);

public:
    /// emit OnGraphicsProfileListChanged signal + update active profile
    void EmitOnGraphicsProfileListChanged(void);

// section of private data -----------------------------------------------------
private:
    CGraphicsProfile*   ActiveProfile;      // active profile
    bool                DataManipulation;
    CProObject*         MasterObject;

    void EmmitGraphicsProfileRemovedSignal(CGraphicsProfile* p_gp);

    friend class CGraphicsProfile;
};

//------------------------------------------------------------------------------

#endif
