#ifndef GOColorModeH
#define GOColorModeH
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
#include <ElementColors.hpp>
#include <QMap>

//------------------------------------------------------------------------------

class CGraphicsObject;
class CAtom;

//------------------------------------------------------------------------------

enum EGraphicsObjectColorMode {
    EGOCM_NOT_FOUND     = -1,
    EGOCM_ATOM_SYMBOL   = 0,        // color by atom symbol
    EGOCM_USER_COLOR    = 1         // color by user defined color
};

//------------------------------------------------------------------------------

/// manage color modes for graphics objects (standard model, fast line model, etc.)

class NEMESIS_CORE_PACKAGE CGOColorMode : public CProObject {
Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    CGOColorMode(CGraphicsObject* p_object,const QString& name);
    CGOColorMode(CGraphicsObject* p_object,const QString& name,bool no_index);

// methods with changes registered into history list ---------------------------
    /// set color mode
    bool SetColorModeWH(int index);

    /// set user color
    bool SetUserColorWH(const CElementColors& color);

// executive functions ---------------------------------------------------------
    /// set color mode by index
    void SetColorMode(int index,CHistoryNode* p_history=NULL);

    /// set user color
    void SetUserColor(const CElementColors& color,CHistoryNode* p_history=NULL);

    /// disable specific color mode
    void DisableColorMode(EGraphicsObjectColorMode mode);

// informative methods ---------------------------------------------------------
    /// get number of color modes
    int GetNumberOfColorModes(void);

    /// return color mode name by its index
    const QString GetColorModeName(int index);

    /// return color mode index
    int GetColorMode(void);

    /// return user color setup
    const CElementColors& GetUserColor(void);

    /// is color mode enabled
    bool IsColorModeEnabled(EGraphicsObjectColorMode mode);

    /// return color by proton number
    CColor* GetColor(CAtom* p_atom);

    /// return color by proton number
    CElementColors* GetElementColor(CAtom* p_atom);

// input/output methods --------------------------------------------------------
    /// load object data
    virtual void LoadData(CXMLElement* p_ele);

    /// save object data
    virtual void SaveData(CXMLElement* p_ele);

// section of private data -----------------------------------------------------
private:
    QMap<EGraphicsObjectColorMode,QString>  Modes;  // allowed modes
    EGraphicsObjectColorMode                Mode;

    // user color mode
    CElementColors  UserColor;

    /// return mode by index
    EGraphicsObjectColorMode GetColorMode(int index);

    /// return current color mode
    EGraphicsObjectColorMode GetCurrentColorMode(void);

    /// set color mode by mode
    void SetColorMode(EGraphicsObjectColorMode mode,CHistoryNode* p_history=NULL);

    friend class CGOColorModeChangeModeHI;
};

//------------------------------------------------------------------------------

#endif
