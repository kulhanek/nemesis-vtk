#ifndef GraphicsSetupH
#define GraphicsSetupH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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
#include <UUID.hpp>

// -----------------------------------------------------------------------------

class CProject;
class CXMLElement;

// -----------------------------------------------------------------------------

enum EGraphicsSetupMode {
    EGSM_GLOBAL_SETUP,
    EGSM_PROJECT_SETUP,
    EGSM_OBJECT_SETUP
};

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CGraphicsSetup : public CProObject {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
            CGraphicsSetup(CPluginObject* p_objectinfo,CProObject* p_owner);
    virtual ~CGraphicsSetup(void);

// setup methods ---------------------------------------------------------------
    /// load user default setup
    void    LoadUserDefault(void);

    /// load system default setup
    void    LoadSystemDefault(void);

    /// emit signal when setup is changed
    void    EmitOnSetupChanged(void);

// information methods ---------------------------------------------------------
    /// get setup mode
    EGraphicsSetupMode  GetSetupMode(void) const;

    /// get associated graphics object ID
    int                 GetObjectID(void) const;

// input/output methods --------------------------------------------------------
    //! load object setup
    void Load(CXMLElement* p_ele);

    //! save object setup
    void Save(CXMLElement* p_ele);

    //! load setup related data
    virtual void LoadData(CXMLElement* p_ele);

    //! save setup related data
    virtual void SaveData(CXMLElement* p_ele);

// signals ---------------------------------------------------------------------
signals:
    /// emmited when setup is changed
    void OnSetupChanged(void);

// section of private data -----------------------------------------------------
protected:
    int         ObjectID;       // object id

    friend class CGraphicsSetupList;
};

// -----------------------------------------------------------------------------

#endif
