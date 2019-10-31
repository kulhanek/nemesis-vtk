#ifndef GeoPropertySetupH
#define GeoPropertySetupH
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
#include <GraphicsSetup.hpp>
#include <SmallColor.hpp>

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CGeoPropertySetup : public CGraphicsSetup {
public:
// constructors and destructors ------------------------------------------------
    CGeoPropertySetup(CProObject* p_owner);
    ~CGeoPropertySetup(void);

// input/output methods --------------------------------------------------------
    virtual void LoadData(CXMLElement* p_ele);
    virtual void SaveData(CXMLElement* p_ele);

// section of public data ------------------------------------------------------
public:
    int     LineStippleFactor;
    int     LineStipplePattern;
    int     LineWidth;
    CColor  LineColor;

    QString LabelFontName;
    int     LabelFontSize;
    CColor  LabelColor;

    CColor  LabelQuotationColor;
    int     LabelQuotationLineWidth;
    int     LabelQuotationStippleFactor;
    int     LabelQuotationStipplePattern;

    CColor  PointColor;
    int     PointWidth;
    double  PointSize;

    CColor  PointQuotationColor;
    int     PointQuotationWidth;
    int     PointQuotationStippleFactor;
    int     PointQuotationStipplePattern;

    double  AngleArcScope;

    bool    DepthTest;
    bool    ShowUnit;

    //CFTGLFont       FTGLFont;

    void UpdateFTGLFont(void);
};

//------------------------------------------------------------------------------

#endif
