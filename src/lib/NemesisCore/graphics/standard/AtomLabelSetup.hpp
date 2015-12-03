#ifndef AtomLabelSetupH
#define AtomLabelSetupH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <GraphicsSetup.hpp>
#include <SmallColor.hpp>
#include <FTGLFont.hpp>

//------------------------------------------------------------------------------

/// atom label setup

class CAtomLabelSetup : public CGraphicsSetup {
public:
// constructors and destructors ------------------------------------------------
    CAtomLabelSetup(CProObject* p_owner);
    ~CAtomLabelSetup(void);

// input/output methods --------------------------------------------------------
    virtual void LoadData(CXMLElement* p_ele);
    virtual void SaveData(CXMLElement* p_ele);

// section of public data ------------------------------------------------------
public:
    QString         LabelFontName;
    int             LabelFontSize;
    CColor          LabelColor;
    int             LineStippleFactor;
    int             LineStipplePattern;
    int             LineWidth;
    CColor          LineColor;
    bool            DepthTest;
    CFTGLFont       FTGLFont;

    void UpdateFTGLFont(void);
};

//------------------------------------------------------------------------------

#endif
